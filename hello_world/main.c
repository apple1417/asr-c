#include <asr.h>

#include <stdbool.h>
#include <stdint.h>

#define HELLO_WORLD "Hello World"

__attribute__((export_name("update"))) void update(void) {
    static bool first_run = true;
    if (first_run) {
        first_run = false;
        runtime_print_message((const uint8_t*)HELLO_WORLD, sizeof(HELLO_WORLD) - 1);
    }
}
