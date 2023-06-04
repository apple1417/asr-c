#ifndef UTILS_READ_MEM_H
#define UTILS_READ_MEM_H

#include "utils/pch.h"
#include "utils/asr_extensions.h"
#include "utils/process_info.h"

namespace asr_utils {
inline namespace v0 {

/**
 * @brief Swaps the endianness of a value.
 *
 * @tparam T The type of the value.
 * @param val The value.
 * @return The value in the opposite endianness.
 */
template <typename T>
T swap_endianness(T val) {
    union {
        T val;
        uint8_t bytes[sizeof(T)];
    } buf{.val = val};
    std::reverse(std::begin(buf.bytes), std::end(buf.bytes));
    return buf.val;
}

/**
 * @brief Ensures the provided value is in native endianness.
 * @note Swaps the endianness if the process endianness does not match the native endianness.
 *
 * @tparam T The type of the value.
 * @param process The process the value is from.
 * @param val The value.
 * @return
 */
template <typename T>
T fix_endianness(const ProcessInfo& process, T val) {
    return std::endian::native != process.endianness ? swap_endianness(val) : val;
}
template <typename T>
T fix_endianness(ProcessId process, T val) = delete;

/**
 * @brief Reads a value from a process.
 * @note Retains the process endianness.
 *
 * @tparam T The type of the value.
 * @param process The process to read memory of.
 * @param address The address to read memory at.
 * @return The read value, or it's default constructed version if the read fails.
 */
template <typename T>
T read_mem(const ProcessInfo& process, Address address) {
    T val{};
    process_read(process, address, &val);
    return val;
}
template <typename T>
T read_mem(ProcessId process, Address address) = delete;

/**
 * @brief Reads an address from a process, automatically adjusting for pointer size.
 *
 * @param process The process to read memory of.
 * @param address The address to read memory at.
 * @return The address, or 0 if the read fails.
 */
Address read_address(const ProcessInfo& process, Address address);
Address read_address(ProcessId process, Address address) = delete;

/**
 * @brief Reads an x86 assembly pointer offset, and gets the address it points to.
 * @note Used for `mov [<address>], rax` style instructions.
 *
 * @param process The process to read memory of.
 * @param address The address of the offset to read.
 * @return The address it points to, or 0 if the read failed.
 */
Address read_x86_offset(const ProcessInfo& process, Address address);
Address read_x86_offset(ProcessId process, Address address) = delete;
Address read_x86_offset32(ProcessId process, Address address);
Address read_x86_offset64(ProcessId process, Address address);

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
