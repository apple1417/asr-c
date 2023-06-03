#include "utils/sigscan.h"
#include <algorithm>
#include "utils/asr_extensions.h"
#include "utils/pch.h"

namespace asr_utils {
inline namespace v0 {

namespace {

const constexpr auto SIGSCAN_CHUNK_SIZE = 0x1000;

/*
Because we're in another process, we need to read in chunks of memory while sigscanning, we can't
just load the entire range.
This leads to an obvious problem: a match might cross chunk boundaries.

To deal with this, at the end of the chunk we just match prefixes of the pattern. We then at the
start of the next chunk look at the offsets with matching prefixes, then skip that many bytes and
check if the suffix also matches.

    ... | A | A || B | C | ...
=========================================
| A | A | B | C ||              No Match
    | A | A | B || _ |          No Match  idx 3
        | A | A || _ | _ |      Match     idx 2
            | A || _ | _ | _ |  Match     idx 1
=========================================
            | _ || A | B | C |  No Match  idx 1
        | _ | _ || B | C |      Match     idx 2

We index these by number of bytes in the prefix. This wastes the 0 index, but keeping it around
makes the code cleaner.
*/

/**
 * @brief Updates the list of partial matches at the end of a chunk.
 *
 * @param pattern The pattern to search for.
 * @param chunk The chunk of memory to match.
 * @param partial_matches The list of partial matches to update.
 */
void sigscan_find_partial_matches(const uint8_t* bytes,
                                  const uint8_t* mask,
                                  size_t pattern_size,
                                  const uint8_t* chunk,
                                  std::vector<bool>& partial_matches) {
    // +1 to skip the case where all prefix bytes are within this chunk
    for (size_t chunk_offset = (SIGSCAN_CHUNK_SIZE - pattern_size) + 1;
         chunk_offset < SIGSCAN_CHUNK_SIZE; chunk_offset++) {
        auto bytes_in_prefix = SIGSCAN_CHUNK_SIZE - chunk_offset;

        bool match = true;
        for (size_t pattern_idx = 0; pattern_idx < bytes_in_prefix; pattern_idx++) {
            if ((chunk[chunk_offset + pattern_idx] & mask[pattern_idx]) != bytes[pattern_idx]) {
                match = false;
                break;
            }
        }

        partial_matches[bytes_in_prefix] = match;
    }
}

/**
 * @brief Checks if any partial matches from the last chunk finish in this one.
 *
 * @param pattern The pattern to search for.
 * @param chunk The chunk of memory to match.
 * @param partial_matches The list of partial matches from the last chunk.
 * @return The number of bytes before the start of the chunk where the pattern matches, or 0 if
 *         there's no match.
 */
size_t sigscan_finish_partial_matches(const uint8_t* bytes,
                                      const uint8_t* mask,
                                      size_t pattern_size,
                                      const uint8_t* chunk,
                                      const std::vector<bool>& partial_matches) {
    // Start at 1 to skip the case where all suffix bytes are in this chunk
    // Go until pattern size - 1 to skip the case where all prefix bytes are in the last chunk
    for (size_t bytes_in_prefix = 1; bytes_in_prefix < (pattern_size - 1); bytes_in_prefix++) {
        if (!partial_matches[bytes_in_prefix]) {
            continue;
        }

        bool match = true;
        for (size_t chunk_offset = 0; chunk_offset < (pattern_size - bytes_in_prefix);
             chunk_offset++) {
            auto pattern_idx = bytes_in_prefix + chunk_offset;

            if ((chunk[chunk_offset] & mask[pattern_idx]) != bytes[pattern_idx]) {
                match = false;
                break;
            }
        }
        if (match) {
            return bytes_in_prefix;
        }
    }

    return 0;
}

}  // namespace

Address sigscan(ProcessId process,
                const uint8_t* bytes,
                const uint8_t* mask,
                size_t pattern_size,
                Address start,
                size_t size) {
    uint8_t chunk[SIGSCAN_CHUNK_SIZE];
    std::vector<bool> partial_matches(pattern_size, false);

    for (auto chunk_start = start; chunk_start < (start + size);
         chunk_start += SIGSCAN_CHUNK_SIZE) {
        bool success = process_read(process, chunk_start, &chunk[0], SIGSCAN_CHUNK_SIZE);
        if (!success) {
            return 0;
        }

        // Check for matches crossing the start of this chunk
        auto bytes_in_prefix =
            sigscan_finish_partial_matches(bytes, mask, pattern_size, &chunk[0], partial_matches);
        if (bytes_in_prefix != 0) {
            return chunk_start - bytes_in_prefix;
        }

        // Check for matches within this chunk
        auto end_of_chunk = std::min(chunk_start + SIGSCAN_CHUNK_SIZE - pattern_size, start + size);
        for (size_t chunk_offset = 0; chunk_offset < (end_of_chunk - chunk_start); chunk_offset++) {
            bool match = true;
            for (size_t pattern_idx = 0; pattern_idx < pattern_size; pattern_idx++) {
                if ((chunk[chunk_offset + pattern_idx] & mask[pattern_idx]) != bytes[pattern_idx]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                return chunk_start + chunk_offset;
            }
        }

        // Check for matches crossing over the end of this chunk
        sigscan_find_partial_matches(bytes, mask, pattern_size, &chunk[0], partial_matches);
    }

    return 0;
}

}  // namespace v0
}  // namespace asr_utils
