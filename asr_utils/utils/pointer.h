#ifndef UTILS_POINTER_H
#define UTILS_POINTER_H

#include "utils/pch.h"
#include "utils/process_info.h"

namespace asr_utils {
inline namespace v0 {

/**
 * @brief Class representing a pointer in a remote process.
 * @note Intended to be used in custom struct definitions, so you can recursively follow pointers.
 *
 * @tparam PointerT The pointer type in the remote process.
 * @tparam ValueT The value type it points to.
 */
template <typename PointerT, typename ValueT>
struct RemotePointer {
    PointerT addr;

    /**
     * @brief Dereferences the remote pointer.
     * @note Retains the process endianness.
     *
     * @tparam T The type of value to read. May be a more derived class to read it's extra fields.
     * @param process The process to dereference the pointer in.
     * @return True if read successfully, false on error.
     */
    template <typename T = ValueT, typename = std::enable_if_t<std::is_base_of_v<ValueT, T>>>
    [[nodiscard]] T dereference(ProcessId process) const {
        T val{};
        process_read(process, this->addr, val);
        return val;
    }
};

template <typename T>
using RemotePointer32 = RemotePointer<uint32_t, T>;
template <typename T>
using RemotePointer64 = RemotePointer<uint64_t, T>;

/**
 * @brief Class representing a multi-step pointer path.
 */
struct DeepPointer {
    Address base;
    std::vector<ptrdiff_t> offsets;

    /**
     * @brief Dereferences the pointer path.
     *
     * @param process The process to read the pointer pat in.
     * @return The final dereferenced address, or 0 if any part of the path was invalid.
     */
    [[nodiscard]] Address dereference(const ProcessInfo& process) const;
    [[nodiscard]] Address dereference(ProcessId process) const = delete;
};

}  // namespace v0
}  // namespace asr_utils

#endif /* UTILS_POINTER_H */
