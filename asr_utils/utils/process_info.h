#ifndef UTILS_PROCESS_INFO_H
#define UTILS_PROCESS_INFO_H

#include "utils/pch.h"

namespace asr_utils {
inline namespace v0 {

struct ProcessInfo {
    using InitFlags = uint32_t;
    static const constexpr auto INIT_FIX_PROTON_MAIN_MODULE_SIZE = (1 << 0);
    static const constexpr auto INIT_DEFAULTS = INIT_FIX_PROTON_MAIN_MODULE_SIZE;

    ProcessId pid{0};
    Address main_module{};
    size_t main_module_size{};
    std::string exe_path{};
    bool is_64_bit{false};
    std::endian endianness{std::endian::little};

    /**
     * @brief Construct a new Process Info object
     *
     * @param pid The process's pid.
     * @param main_module The name of the process's main module. Defaults to the executable name.
     * @param init The initialization flags to use.
     */
    ProcessInfo(ProcessId pid,
                const std::string_view& main_module = "",
                InitFlags init = INIT_DEFAULTS);

    /**
     * @brief Implicitly casts to the associated process id.
     *
     * @return The process id.
     */
    operator ProcessId(void) const;

    ProcessInfo(void) = default;
    ProcessInfo(const ProcessInfo& other) = default;
    ProcessInfo(ProcessInfo&& other) noexcept = default;
    ProcessInfo& operator=(const ProcessInfo& other) = default;
    ProcessInfo& operator=(ProcessInfo&& other) noexcept = default;
    ~ProcessInfo(void) = default;

   private:
    /**
     * @brief Tries to parse a PE header, and fills this object with it's details.
     *
     * @param init The initialization flags to use.
     * @return True if we successfully extracted data, false otherwise.
     */
    bool try_parse_pe(InitFlags init);

    /**
     * @brief Tries to parse an ELF header, and fills this object with it's details.
     *
     * @param init The initialization flags to use.
     * @return True if we successfully extracted data, false otherwise.
     */
    bool try_parse_elf(InitFlags init);
};

}  // namespace v0
}  // namespace asr_utils

#endif /* UTILS_PROCESS_INFO_H */
