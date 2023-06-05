#ifndef UTILS_MEM_WATCHER_H
#define UTILS_MEM_WATCHER_H

#include "utils/pch.h"
#include "utils/process_info.h"
#include "utils/read_mem.h"
#include "utils/variable.h"

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
    Variable<T> current_value{};
    T old_value{};

   public:
    /**
     * @brief Construct a new memory watcher.
     *
     * @param ptr The pointer to watch.
     * @param key The key to use for the memory watcher's variable. Use an empty string to disable.
     * @param process If provided, does an inital update using this process. Not stored.
     */
    MemWatcher(void) = default;
    MemWatcher(DeepPointer&& ptr, const std::string& key = "") : ptr(ptr), current_value(key, {}) {}
    MemWatcher(DeepPointer&& ptr, const ProcessInfo& process) : ptr(ptr) { this->update(process); }
    MemWatcher(DeepPointer&& ptr, const std::string& key, const ProcessInfo& process)
        : ptr(ptr), current_value(key, {}) {
        this->update(process);
    }

    /**
     * @brief Dereferences the pointer path.
     *
     * @param process The process to read the path pointer in.
     * @return The final dereferenced address.
     */
    void update(const ProcessInfo& process) {
        this->old_value = std::move(this->current_value);
        this->current_value = read_mem<T>(process, this->ptr.dereference(process));
    }
    void update(ProcessId process) = delete;

    /**
     * @brief Gets the value the watcher had before the last update.
     *
     * @return The old watcher value.
     */
    [[nodiscard]] const T& old(void) const { return this->old_value; }

    /**
     * @brief Gets the current value of the watcher.
     *
     * @return The current watcher value.
     */
    [[nodiscard]] const T& current(void) const { return this->current_value.value(); }

    /**
     * @brief Gets if the watcher value changed.
     *
     * @return True if the current value is different to the old value, false if they're the same.
     */
    [[nodiscard]] bool changed(void) const { return this->current_value != this->old_value; }
};

}  // namespace v0
}  // namespace asr_utils

#endif /* UTILS_MEM_WATCHER_H */
