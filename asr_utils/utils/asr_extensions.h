#ifndef UTILS_ASR_EXTENSIONS_H
#define UTILS_ASR_EXTENSIONS_H

#include "utils/pch.h"

// Various helpers extending ASR functions with C++ types.
// Note that, unlike everything else in this project, all these functions are put into the global
// scope, just like their C versions.

inline namespace asr_utils_v0 {

static_assert(sizeof(std::string::value_type) == sizeof(uint8_t),
              "string value is not same size as uint8");

// ======== String Args ========

/**
 * @brief Sets a custom key value pair, holding arbitrary information that the auto splitter wants
 *        to provide for visualization.
 *
 * @tparam T The type of the value, if not already a string. Should be picked up automatically.
 * @param key The key.
 * @param value The value.
 */
void timer_set_variable(const std::string_view& key, const std::string_view& value);

/**
 * @brief Attaches to a process based on its name
 *
 * @param name The process name.
 * @return The process id, or 0 if it can't be found.
 */
ProcessId process_attach(const std::string_view& name);

/// Gets the address of a module in a process.

/**
 * @brief Gets the address of a module in a process.
 *
 * @param process The process to look through.
 * @param name The module name.
 * @return The module's address, or 0 if it can't be found.
 */
Address process_get_module_address(ProcessId process, const std::string_view& name);

/**
 * @brief Gets the size of a module in a process.
 *
 * @param process The process to look through.
 * @param name The module name.
 * @return The module's size, or 0 if it can't be found.
 */
uint64_t process_get_module_size(ProcessId process, const std::string_view& name);

/**
 * @brief Print a formatted log message.
 *
 * @tparam Args The types of the format args
 * @param fmt The format string.
 * @param args The format string args.
 */
template <typename... Args>
void runtime_print_message(std::format_string<Args...> fmt, Args&&... args) {
    auto str = std::format<Args...>(fmt, std::forward<Args>(args)...);
    ::runtime_print_message(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief Adds a new setting that the user can modify.
 *
 * @param key The setting key.
 * @param description The description to use for the setting.
 * @param default_value The setting's default value.
 * @return The value the user has set (or the specified default).
 */
bool user_settings_add_bool(const std::string_view& key,
                            const std::string_view& description,
                            bool default_value);

// ======== String Return Values ========

/**
 * @brief Gets the path of the given process.
 * @note The path is one which is accessible through the wasi filesystem - `C:\foo\bar.exe` would be
 *       returned as `/mnt/c/foo/bar.exe`
 *
 * @param process The process to get the path of.
 * @return The process's path.
 */
std::string process_get_path(ProcessId process);

/**
 * @brief Gets the name the operating system that the runtime is running on.
 * @note Example values: `windows`, `linux`, `macos`
 *
 * @return The host operating system.
 */
std::string runtime_get_os(void);

/**
 * @brief Gets the name the architecture that the runtime is running on.
 * @note Example values: `x86`, `x86_64`, `arm`, `aarch64`
 *
 * @return the host architecture.
 */
std::string runtime_get_arch(void);

// ======== Memory ========

/**
 * @brief Reads memory from a process at the address given
 *
 * @tparam T The type of the value.
 * @param process The process to read memory of.
 * @param address The address to read memory at.
 * @param val Pointer to where to store the result.
 * @return True on success, false on failure.
 */
template <typename T>
bool process_read(ProcessId process, Address address, T& val) {
    return ::process_read(process, address, reinterpret_cast<uint8_t*>(&val), sizeof(T));
}

}  // namespace asr_utils_v0

#endif /* UTILS_ASR_EXTENSIONS_H */
