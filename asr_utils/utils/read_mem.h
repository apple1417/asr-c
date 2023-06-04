#ifndef UTILS_READ_MEM_H
#define UTILS_READ_MEM_H

#include "utils/pch.h"
#include "utils/asr_extensions.h"

namespace asr_utils {
inline namespace v0 {

/**
 * @brief Reads an object from a process.
 *
 * @tparam T The type of the object.
 * @param process The process to read memory of.
 * @param address The address to read memory at.
 * @return The read object, or it's default constructed version if the read fails.
 */
template <typename T>
T read_mem(ProcessId process, Address address) {
    T val{};
    process_read(process, address, &val);
    return val;
}

/**
 * @brief Reads an assembly offset, and gets the address it points to.
 *
 * @param process The process to read memory of.
 * @param address The address of the offset to read.
 * @return The address it points to, or 0 if the read failed.
 */
Address read_offset32(ProcessId process, Address address);
Address read_offset64(ProcessId process, Address address);

/**
 * @brief Reads a null terminated string.
 *
 * @tparam CharT The string's character type.
 * @tparam Traits The string's traits type.
 * @tparam Allocator The string's allocator type.
 * @param process The process to read memory of.
 * @param address The address of the string to read.
 * @param max_chars The maximum length of the string to read, in characters.
 * @return The string, or an empty string if the read failed.
 */
template <typename CharT = char,
          typename Traits = std::char_traits<CharT>,
          typename Allocator = std::allocator<CharT> >
std::basic_string<CharT, Traits, Allocator> read_string(ProcessId process,
                                                        Address address,
                                                        // NOLINTNEXTLINE(readability-magic-numbers)
                                                        size_t max_chars = 256) {
    CharT buf[max_chars];
    if (!process_read(process, address, buf)) {
        return {};
    }
    return {buf};
}

}  // namespace v0
}  // namespace asr_utils

#endif /* UTILS_READ_MEM_H */
