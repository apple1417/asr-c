#ifndef UTILS_VARIABLE_H
#define UTILS_VARIABLE_H

#include <utils/asr_extensions.h>
#include <utils/pch.h>

namespace asr_utils {
inline namespace v0 {

/**
 * @brief A proxy for a timer variable, where writes are automatically synced back to the timer.
 * @note A key of an empty string prevents syncing.
 *
 * @tparam T The type of the variable being stored.
 */
template <typename T>
class Variable {
   private:
    std::string key_internal;
    T value_internal;

    /**
     * @brief Syncs the value stored in this object to the external timer.
     */
    void sync(void) const {
        if (!this->key_internal.empty()) {
            timer_set_variable(this->key_internal, std::move(this->operator std::string()));
        }
    }

   public:
    /**
     * @brief Construct a new variable proxy.
     *
     * @param key The key the variable should use.
     * @param value The variable's inital value.
     */
    Variable(void) = default;
    Variable(const std::string_view& key, const T&& value)
        : key_internal(key), value_internal(value) {}

    /**
     * @brief Stores a new value in this variable.
     *
     * @param value The value to store.
     * @return A reference to the variable.
     */
    Variable<T>& operator=(const T& value) {
        this->value_internal = value;
        this->sync();
        return *this;
    };
    Variable<T>& operator=(T&& value) noexcept {
        this->value_internal = std::move(value);
        this->sync();
        return *this;
    };

    /**
     * @brief Gets the key used by this variable.
     *
     * @return The key.
     */
    [[nodiscard]] const std::string_view& key(void) const { return this->key_val; }

    /**
     * @brief Gets the stored value.
     *
     * @return The stored value.
     */
    [[nodiscard]] const T& value(void) const { return this->value_internal; }
    [[nodiscard]] operator T(void) const { return this->value_internal; }

    /**
     * @brief Converts the stored value into it's string representation.
     *
     * @return The string representation
     */
    template <typename = std::enable_if<std::negation_v<std::is_same<T, std::string>>>>
    [[nodiscard]] operator std::string(void) const;
};

// The basic numeric types which all support std::to_string
template <>
template <>
Variable<int>::operator std::string(void) const;
template <>
template <>
Variable<long>::operator std::string(void) const;
template <>
template <>
Variable<long long>::operator std::string(void) const;
template <>
template <>
Variable<unsigned>::operator std::string(void) const;
template <>
template <>
Variable<unsigned long>::operator std::string(void) const;
template <>
template <>
Variable<unsigned long long>::operator std::string(void) const;
template <>
template <>
Variable<float>::operator std::string(void) const;
template <>
template <>
Variable<double>::operator std::string(void) const;
template <>
template <>
Variable<long double>::operator std::string(void) const;

}  // namespace v0
}  // namespace asr_utils

// Custom formatter to forward variables to their wrapped type.
template <typename T, typename CharT>
struct std::formatter<asr_utils::Variable<T>, CharT> : std::formatter<T, CharT> {
    template <typename Context>
    auto format(const asr_utils::Variable<T> var, Context& ctx) const {
        return std::formatter<T>::format(var.value(), ctx);
    }
};

#endif /* UTILS_VARIABLE_H */
