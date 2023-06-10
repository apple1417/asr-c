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
            timer_set_variable(this->key_internal, std::move(this->to_str()));
        }
    }

   public:
    /**
     * @brief Construct a new variable proxy.
     *
     * @param key The key the variable should use.
     * @param value The variable's initial value.
     */
    Variable(void) = default;
    Variable(const std::string_view& key, const T&& value = T{})
        : key_internal(key), value_internal(value) {
        this->sync();
    }

    /**
     * @brief Destroys the variable proxy.
     */
    virtual ~Variable(void) = default;

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
    [[nodiscard]] const std::string_view& key(void) const { return this->key_internal; }

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
    [[nodiscard]] virtual std::string to_str(void) const {
        return std::format("{}", this->value());
    }

    /**
     * @brief Compares the stored value against another.
     * @note Comparing two variables of the same type directly is not supported, since that leads to
     *       an ambiguous overload (compare variables, cast the first, or cast the second?).
     *
     * @param other The other variable to compare against
     * @return The result of the comparison
     */
    auto operator==(const T& other) { return this->value_internal == other; }
    auto operator!=(const T& other) { return this->value_internal != other; }
    auto operator<(const T& other) { return this->value_internal < other; }
    auto operator>(const T& other) { return this->value_internal > other; }
    auto operator<=(const T& other) { return this->value_internal <= other; }
    auto operator>=(const T& other) { return this->value_internal >= other; }
    auto operator<=>(const T& other) { return this->value_internal <=> other; }
};

/**
 * @brief Variable which displays it's value in hex.
 *
 * @tparam T The type of the variable being stored.
 */
template <typename T, typename = std::enable_if<std::is_integral_v<T>>>
class HexVariable : public Variable<T> {
   public:
    using Variable<T>::Variable;
    using Variable<T>::operator=;
    using Variable<T>::operator T;

    [[nodiscard]] std::string to_str(void) const override {
        return std::format("{:#x}", this->value());
    }
};

/**
 * @brief Helper to quickly construct a unique pointer to a standard variable.
 * @note Useful when adding to a mem watcher.
 *
 * @tparam T The type of the variable
 * @param key The key the variable should use.
 * @param value The variable's initial value.
 * @return A unique pointer to the new variable.
 */
template <typename T>
std::unique_ptr<Variable<T>> make_variable(const std::string_view& key, const T&& value = T{}) {
    return std::make_unique<Variable<T>>(key, std::move(value));
}

/**
 * @brief Helper to quickly construct a unique pointer to a hex variable.
 * @note Useful when adding to a mem watcher.
 *
 * @tparam T The type of the variable
 * @param key The key the variable should use.
 * @param value The variable's initial value.
 * @return A unique pointer to the new variable.
 */
template <typename T>
std::unique_ptr<HexVariable<T>> make_hex_variable(const std::string_view& key,
                                                  const T&& value = T{}) {
    return std::make_unique<HexVariable<T>>(key, std::move(value));
}

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
