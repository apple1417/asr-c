#ifndef UTILS_MEM_WATCHER_H
#define UTILS_MEM_WATCHER_H

#include "utils/pch.h"
#include "utils/process_info.h"
#include "utils/read_mem.h"

namespace asr_utils {
inline namespace v0 {

struct DeepPointer {
    Address base;
    std::vector<ptrdiff_t> offsets;

    /**
     * @brief Dereferences the pointer path.
     *
     * @param process The process to read the path pointer in.
     * @return The final dereferenced address.
     */
    [[nodiscard]] Address dereference(const ProcessInfo& process) const;
    [[nodiscard]] Address dereference(ProcessId process) const = delete;
};

template <typename T>
class MemWatcher {
   private:
    DeepPointer ptr{};
    T old_value{};
    T current_value{};

   public:
    /**
     * @brief Construct a new memory watcher.
     *
     * @param ptr The pointer to watch.
     */
    MemWatcher(void) = default;
    MemWatcher(DeepPointer&& ptr) : ptr(ptr) {}

    /**
     * @brief Dereferences the pointer path.
     *
     * @param process The process to read the path pointer in.
     * @return The final dereferenced address.
     */
    void update(const ProcessInfo& process) {
        old_value = current_value;
        current_value = read_mem<T>(process, this->ptr.dereference(process));
    }
    void update(ProcessId process) = delete;

    /**
     * @brief Gets the value the watcher had before the last update.
     *
     * @return The old watcher value.
     */
    [[nodiscard]] const T& old(void) const { return old_value; }

    /**
     * @brief Gets the current value of the watcher.
     *
     * @return The current watcher value.
     */
    [[nodiscard]] const T& current(void) const { return current_value; }

    /**
     * @brief Gets if the watcher value changed.
     *
     * @return True if the current value is different to the old value, false if they're the same.
     */
    [[nodiscard]] bool changed(void) const { return current_value != old_value; }
};

}  // namespace v0
}  // namespace asr_utils

#endif /* UTILS_MEM_WATCHER_H */
