#ifndef ASL_H
#define ASL_H

#include <asr.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// All symbols in this header are for you to define.
// Default weak implementations are provided, so you only need to implement what you use.

typedef struct {
    int64_t secs;
    int32_t nanos;
} Duration;

typedef struct {
    const uint8_t* name;
    uintptr_t len;  // Excluding null terminator
} MatchableExecutableName;

/**
 * @brief Convenience macro to generate an executable name from a string literal.
 * @note Assumes string literals are castable to utf8 uint8_t strings.
 *
 * @param str The executable name.
 */
#ifdef __cplusplus
#define MATCH_EXECUTABLE(str)                                                 \
    (MatchableExecutableName) {                                               \
        .name = reinterpret_cast<const uint8_t*>(str), .len = sizeof(str) - 1 \
    }
#else
#define MATCH_EXECUTABLE(str)                                 \
    (MatchableExecutableName) {                               \
        .name = (const uint8_t*)(str), .len = sizeof(str) - 1 \
    }
#endif

/**
 * @brief Convenience macro to generate a null executable name, to place at the end of the list.
 *
 * @returns A null executable name.
 */
#define END_MATCHABLE_EXECUTABLES() \
    (MatchableExecutableName) {     \
        .name = NULL, .len = 0      \
    }

/**
 * @brief A list of executable names to match.
 * @note Must contain an entry with a null pointer or a length of 0 to mark the end of the list.
 */
extern const MatchableExecutableName MATCHABLE_EXECUTABLES[];

/**
 * @brief Called once on script startup, intended for initialization.
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
