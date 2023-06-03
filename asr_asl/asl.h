#ifndef ASL_H
#define ASL_H

#include <asr.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int64_t secs;
    int32_t nanos;
} Duration;

/**
 * @brief Registers a game executable name to attach to. First registered has highest priority.
 *
 * @param name Pointer to the name. Must remain valid for the lifetime of the program.
 * @param len The length of the name (excluding null terminator).
 * @return True if successfully registered, false on failure.
 */
bool register_executable_name(const uint8_t* name, uintptr_t len);

/*
================================ Callbacks ================================
Implement these functions in your own scripts.

By default, this library contains default weak implementations of all these functions, so that you
only need to define the ones you use.
*/

/**
 * @brief Called once on script startup, intended for initalization.
 */
void startup(void);

/**
 * @brief Called when an instance of the game process is launched (if not already attached).
 *
 * @param game The new game process.
 * @return True if the game process is correct, and can be attached to, false if not to attach.
 */
bool on_launch(ProcessId game);

/**
 * @brief Called when the attached game process exits.
 */
void on_exit(void);

/**
 * @brief Called every update.
 *
 * @param game The attached game process.
 * @return True if to continue on to other function calls. False if to stop for this iteration.
 */
bool update(ProcessId game);

/**
 * @brief Detects when to start the timer (called when attached but not running).
 *
 * @param game The attached game process.
 * @return True if to start the timer.
 */
bool start(ProcessId game);

/**
 * @brief Detects when to split (called when attached and running).
 *
 * @param game The attached game process.
 * @return True if to split.
 */
bool split(ProcessId game);

/**
 * @brief Detects when to reset the timer (called when attached and running).
 *
 * @param game The attached game process.
 * @return True if to reset the timer.
 */
bool reset(ProcessId game);

/**
 * @brief Detects loading screens (called when attached and running).
 *
 * @param game The attached game process.
 * @return True if the game is loading.
 */
bool is_loading(ProcessId game);

/**
 * @brief Syncs game time from the game process (called when attached and running).
 * @note Should typically be used alongside an `is_loading` which always returns `true`.
 *
 * @param game The attached game process.
 * @return A pointer to the duration to sync, or NULL if not to update the game time.
 */
Duration* game_time(ProcessId game);

/**
 * @brief Called whenever the timer is started, whether manually or automatically.
 */
void on_start(void);

/**
 * @brief Called whenever the timer is reset, whether manually or automatically.
 */
void on_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* ASL_H */
