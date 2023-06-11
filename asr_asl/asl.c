#include <asr.h>
#include <stdbool.h>
#include <stdint.h>

#include "asl.h"

#define NO_PROCESS 0
static ProcessId game = NO_PROCESS;

static bool try_connect(void) {
    for (const MatchableExecutableName* name = MATCHABLE_EXECUTABLES;
         name->name != NULL && name->len != 0; name++) {
        ProcessId pid = process_attach(name->name, name->len);
        if (pid == 0) {
            continue;
        }

        if (on_launch(pid, name)) {
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
    every_update();

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

__attribute__((weak))
const MatchableExecutableName MATCHABLE_EXECUTABLES[] = {END_MATCHABLE_EXECUTABLES()};

__attribute__((weak)) void startup(void) {}
__attribute__((weak)) void every_update(void) {}
__attribute__((weak)) bool on_launch(ProcessId game, const MatchableExecutableName* name) {
    (void)game;
    (void)name;
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
