#include "utils/asr_extensions.h"
#include "utils/pch.h"

inline namespace asr_utils_v0 {

/**
 * @brief Helper macro to expand a string into a pair of pointer and size,
 *
 * @param str The string to expand
 * @returns The pointer and size, as two separate args.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EXPAND_STR(str) reinterpret_cast<const uint8_t*>((str).data()), (str).size()

void timer_set_variable(const std::string_view& key, const std::string_view& value) {
    ::timer_set_variable(EXPAND_STR(key), EXPAND_STR(value));
}
ProcessId process_attach(const std::string_view& name) {
    return ::process_attach(EXPAND_STR(name));
}
Address process_get_module_address(ProcessId process, const std::string_view& name) {
    return ::process_get_module_address(process, EXPAND_STR(name));
}
uint64_t process_get_module_size(ProcessId process, const std::string_view& name) {
    return ::process_get_module_size(process, EXPAND_STR(name));
}
bool user_settings_add_bool(const std::string_view& key,
                            const std::string_view& description,
                            bool default_value) {
    return ::user_settings_add_bool(EXPAND_STR(key), EXPAND_STR(description), default_value);
}

namespace {

/**
 * @brief Helper to extract a stl string from one of the functions which output a C string.
 *
 * @param base_call A function which takes a pointer to the buffer and it's size, returns true if
 *                  the buffer is large enough, and sets the size to the required size.
 * @return The stl string.
 */
std::string extract_string(const std::function<bool(uint8_t* buf, uintptr_t* size)>& base_call) {
    uintptr_t size = 0;
    base_call(nullptr, &size);

    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)
    auto buf = reinterpret_cast<uint8_t*>(malloc(size));
    base_call(buf, &size);

    std::string str{reinterpret_cast<std::string::value_type*>(buf), size};

    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc, cppcoreguidelines-owning-memory)
    free(buf);

    return str;
}

}  // namespace

std::string process_get_path(ProcessId process) {
    return extract_string([process](uint8_t* buf, uintptr_t* size) {
        return ::process_get_path(process, buf, size);
    });
}

std::string runtime_get_os(void) {
    return extract_string(
        [](uint8_t* buf, uintptr_t* size) { return ::runtime_get_os(buf, size); });
}
std::string runtime_get_arch(void) {
    return extract_string(
        [](uint8_t* buf, uintptr_t* size) { return ::runtime_get_arch(buf, size); });
}

}  // namespace asr_utils_v0
