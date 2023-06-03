#include <asr.h>
#include <stdbool.h>
#include <stdint.h>

#include "asl.h"

#define NO_PROCESS 0
static ProcessId game = NO_PROCESS;

// Since we should typically only get one or two executable names, which are constant for the
// lifespan of the program, implementing a full dynamic array is a bit overkill
// Instead, just use a small hardcoded static array
#ifndef ASL_NUM_EXECUTABLE_NAMES
#define ASL_NUM_EXECUTABLE_NAMES 16
#endif
static struct {
    struct {
        const uint8_t* name;
        uintptr_t len;
    } names[ASL_NUM_EXECUTABLE_NAMES];
    size_t num;
} executable_names;

bool register_executable_name(const uint8_t* name, uintptr_t len) {
    if (executable_names.num >= ASL_NUM_EXECUTABLE_NAMES) {
        return false;
    }

    executable_names.names[executable_names.num].name = name;
    executable_names.names[executable_names.num].len = len;
    executable_names.num++;
    return true;
}

static bool try_connect(void) {
    for (size_t i = 0; i < executable_names.num; i++) {
        ProcessId pid =
            process_attach(executable_names.names[i].name, executable_names.names[i].len);
        if (pid == 0) {
            continue;
        }

        if (on_launch(pid)) {
            game = pid;
            return true;
        }
    }
    return false;
}

static void detach_game(void) {
    process_detach(game);
    game = NO_PROCESS;
    on_exit();
}

static void do_update(TimerState state) {
    if (!update(game)) {
        return;
    }

    switch (state) {
        case TIMERSTATE_NOT_RUNNING: {
            if (!start(game)) {
                break;
            }
            timer_start();
            // Fallthrough
        }
        case TIMERSTATE_RUNNING:
        case TIMERSTATE_PAUSED: {
            static bool last_loading = false;
            bool loading = is_loading(game);
            if (loading != last_loading) {
                if (loading) {
                    timer_pause_game_time();
                } else {
                    timer_resume_game_time();
                }
                last_loading = loading;
            }

            Duration* game_time_ptr = game_time(game);
            if (game_time_ptr != NULL) {
                timer_set_game_time(game_time_ptr->secs, game_time_ptr->nanos);
            }

            if (reset(game)) {
                timer_reset();
            } else if (split(game)) {
                timer_split();
            }
            break;
        }
    }
}

__attribute__((export_name("update"))) void asl_update(void) {
    static bool first_run = true;
    if (first_run) {
        first_run = false;
        startup();
    }

    if (game == NO_PROCESS) {
        if (!try_connect()) {
            return;
        }
    }

    TimerState state = timer_get_state();
    if (process_is_open(game)) {
        do_update(state);
    } else {
        detach_game();
    }

    TimerState new_state = timer_get_state();
    if (state == TIMERSTATE_NOT_RUNNING && new_state != TIMERSTATE_NOT_RUNNING) {
        on_start();
    } else if (state != TIMERSTATE_NOT_RUNNING && new_state == TIMERSTATE_NOT_RUNNING) {
        on_reset();
    }
}

#ifndef ASL_NO_WEAK_DEFAULTS
__attribute__((weak)) void startup(void) {}
__attribute__((weak)) bool on_launch(ProcessId game) {
    (void)game;
    return true;
}
__attribute__((weak)) void on_exit(void) {}
__attribute__((weak)) bool update(ProcessId game) {
    (void)game;
    return true;
}
__attribute__((weak)) bool start(ProcessId game) {
    (void)game;
    return false;
}
__attribute__((weak)) bool split(ProcessId game) {
    (void)game;
    return false;
}
__attribute__((weak)) bool reset(ProcessId game) {
    (void)game;
    return false;
}
__attribute__((weak)) bool is_loading(ProcessId game) {
    (void)game;
    return false;
}
__attribute__((weak)) Duration* game_time(ProcessId game) {
    (void)game;
    return NULL;
}
__attribute__((weak)) void on_start(void) {}
__attribute__((weak)) void on_reset(void) {}
#endif
