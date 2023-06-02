#include <asr.h>

#include <stdbool.h>
#include <stdint.h>

__attribute__((export_name("update"))) void update(void) {
    static bool first_run = true;
    if (first_run) {
        first_run = false;
        runtime_print_message((uint8_t*)"Hello World", 11);
    }
}
