#ifndef UTILS_SIGSCAN_H
#define UTILS_SIGSCAN_H

#include "utils/pch.h"

namespace asr_utils {
inline namespace v0 {

/**
 * @brief Struct holding information about a sigscan pattern.
 */
template <size_t n>
struct Pattern {
    std::array<uint8_t, n> bytes;
    std::array<uint8_t, n> mask;
    ptrdiff_t offset;

    /**
     * @brief Construct a pattern.
     *
     * @param bytes The bytes to match.
     * @param mask The mask over the bytes to match.
     * @param offset The constant offset to add to the found address.
     * @return A sigscan pattern.
     */
    Pattern(const uint8_t (&bytes)[n], const uint8_t (&mask)[n], ptrdiff_t offset = 0)
        : bytes(bytes), mask(mask), offset(offset) {}
    Pattern(const char (&bytes)[n + 1], const char (&mask)[n + 1], ptrdiff_t offset = 0)
        : bytes(reinterpret_cast<const uint8_t*>(bytes)),
          mask(reinterpret_cast<const uint8_t*>(mask)),
          offset(offset) {
        static_assert(sizeof(uint8_t) == sizeof(char), "uint8_t is different size to char");
    }

    /**
     * @brief Constructs a pattern from a hex string, at compile time.
     * @note Spaces are ignored, all other non hex characters get converted into a wildcard.
     * @note The string must contain a whole number of bytes.
     *
     * @tparam m The size of the passed hex string - should be picked up automatically.
     * @param hex The hex string to convert.
     * @param offset The constant offset to add to the found address.
     * @return A sigscan pattern.
     */
    template <size_t m>
    consteval Pattern(const char (&hex)[m], ptrdiff_t offset = 0)
        : bytes(), mask(), offset(offset) {
        size_t idx = 0;
        bool upper_nibble = true;

        for (const auto& character : hex) {
            if (character == '\0') {
                break;
            }
            if (character == ' ') {
                continue;
            }

            uint8_t byte = 0;
            uint8_t mask = 0;

            // NOLINTBEGIN(readability-magic-numbers)
            if ('0' <= character && character <= '9') {
                byte = character - '0';
                mask = 0xF;
            } else if ('A' <= character && character <= 'F') {
                byte = character - 'A' + 0xA;
                mask = 0xF;
            } else if ('a' <= character && character <= 'f') {
                byte = character - 'a' + 0xA;
                mask = 0xF;
            }

            assert((byte & 0xF) == byte);
            assert((mask & 0xF) == mask);
            // NOLINTEND(readability-magic-numbers)

            if (upper_nibble) {
                this->bytes[idx] = byte << 4;
                this->mask[idx] = mask << 4;

                upper_nibble = false;
            } else {
                this->bytes[idx] |= byte;
                this->mask[idx] |= mask;

                idx++;
                upper_nibble = true;
            }
        }

        // Make sure we completely filled the pattern, there are no missing or extra bytes, and
        // we're not halfway through one.
        if (idx != n || !upper_nibble) {
            // To fail compilation, we want to call something non-constexpr
            // While I'm not sure it's strictly guarenteed, pretty good chance we're not allowed to
            // abort at compile time :)
            // Don't want to use assert since that can be switched off (and can't use static_assert
            // since our vars aren't constexpr).
            std::abort();
        }
    }
};

/**
 * @brief Performs a sigscan.
 *
 * @tparam T The type to cast the result to.
 * @param process The process to search through.
 * @param pattern The pattern to search for.
 * @param bytes The bytes to match.
 * @param mask The mask over the bytes to match.
 * @param pattern_size The size of the bytes/mask.
 * @param start The address to start the search at.
 * @param size The length of the region to search.
 * @return The found location, or 0 if not found.
 */
Address sigscan(ProcessId process,
                const uint8_t* bytes,
                const uint8_t* mask,
                size_t pattern_size,
                Address start,
                size_t size);
template <size_t n>
Address sigscan(ProcessId process, const Pattern<n>& pattern, Address start, size_t size) {
    auto addr = sigscan(process, pattern.bytes.data(), pattern.mask.data(), n, start, size);
    return addr == 0 ? addr : addr + pattern.offset;
}

}  // namespace v0
}  // namespace asr_utils

#endif /* UTILS_SIGSCAN_H */
