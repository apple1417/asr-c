#ifndef UTILS_PROCESS_INFO_H
#define UTILS_PROCESS_INFO_H

#include "utils/pch.h"

namespace asr_utils {
inline namespace v0 {

struct ProcessInfo {
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
     */
    ProcessInfo() = default;
    ProcessInfo(ProcessId pid);

    /**
     * @brief Implicitly casts to the associated process id.
     *
     * @return The process id.
     */
    operator ProcessId() const;
};

}  // namespace v0
}  // namespace asr_utils

#endif /* UTILS_PROCESS_INFO_H */
