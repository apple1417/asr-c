#ifndef ASR_UTILS_MEM_WATCHER_H
#define ASR_UTILS_MEM_WATCHER_H

#include "asr_utils/pch.h"
#include "asr_utils/pointer.h"
#include "asr_utils/process_info.h"
#include "asr_utils/variable.h"

namespace asr_utils {
inline namespace v0 {

template <typename T>
class MemWatcher {
   private:
    DeepPointer ptr{};
    T current_value{};
    T old_value{};
    std::unique_ptr<Variable<T>> var{nullptr};

   public:
    /**
     * @brief Construct a new memory watcher.
     *
     * @param ptr The pointer to watch.
     * @param var A variable to store the watcher's current value in. May be null.
     */
    MemWatcher(void) = default;
    MemWatcher(DeepPointer&& ptr, std::unique_ptr<Variable<T>>&& var = nullptr)
        : ptr(ptr), var(std::move(var)) {}

    /**
     * @brief Updates the stored values.
     *
     * @param process The process to read the pointer in.
     */
    void update(const ProcessInfo& process) {
        this->old_value = std::move(this->current_value);
        this->current_value = read_mem<T>(process, this->ptr.dereference(process));
        if (this->var) {
            *this->var = this->current_value;
        }
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
    [[nodiscard]] const T& current(void) const { return this->current_value; }

    /**
     * @brief Gets if the watcher value changed.
     *
     * @return True if the current value is different to the old value, false if they're the same.
     */
    [[nodiscard]] bool changed(void) const { return this->current_value != this->old_value; }

    /**
     * @brief Copies the current value to the old value, forcing `changed` to return false.
     * @note Does not fetch an updated current value.
     */
    void suppress_changed(void) {
        this->old_value = this->current_value;
    }

    /**
     * @brief Gets the pointer used by this watcher.
     * @note Only valid for the lifetime of the watcher.
     * @note Intended to be used to edit the pointer, rather than needing to create a new watcher.
     *
     * @return The deep pointer.
     */
    [[nodiscard]] DeepPointer& pointer(void) { return this->ptr; }
};

}  // namespace v0
}  // namespace asr_utils

#endif /* ASR_UTILS_MEM_WATCHER_H */
