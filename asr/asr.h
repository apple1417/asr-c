#ifndef ASR_H
#define ASR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASR_NO_FLOAT64
#include <assert.h>
#include <limits.h>
static_assert((sizeof(double) * CHAR_BIT) == 64, "double is not a float64");
typedef double float64_t;
#endif

typedef uint64_t Address;
typedef uint64_t NonZeroAddress;
typedef uint64_t ProcessId;

typedef enum
{
    /// The timer is not running.
    NOT_RUNNING = 0,
    /// The timer is running.
    RUNNING = 1,
    /// The timer started but got paused. This is separate from the game
    /// time being paused. Game time may even always be paused.
    PAUSED = 2,
    /// The timer has ended, but didn't get reset yet.
    ENDED = 3,
} TimerState;

/// Gets the state that the timer currently is in.
TimerState timer_get_state(void);

/// Starts the timer.
void timer_start(void);
/// Splits the current segment.
void timer_split(void);
/// Resets the timer.
void timer_reset(void);
/// Sets a custom key value pair. This may be arbitrary information that
/// the auto splitter wants to provide for visualization.
void timer_set_variable(const uint8_t *key_ptr, uintptr_t key_len, const uint8_t *value_ptr, uintptr_t value_len);

/// Sets the game time.
void timer_set_game_time(int64_t secs, int32_t nanos);
/// Pauses the game time. This does not pause the timer, only the
/// automatic flow of time for the game time.
void timer_pause_game_time(void);
/// Resumes the game time. This does not resume the timer, only the
/// automatic flow of time for the game time.
void timer_resume_game_time(void);

/// Attaches to a process based on its name. Returns 0 if the process can't be
/// found.
ProcessId process_attach(const uint8_t *name_ptr, uintptr_t name_len);
/// Detaches from a process.
void process_detach(ProcessId process);
/// Checks whether is a process is still open. You should detach from a
/// process and stop using it if this returns `false`.
bool process_is_open(ProcessId process);
/// Reads memory from a process at the address given. This will write
/// the memory to the buffer given. Returns `false` if this fails.
bool process_read(ProcessId process, Address address, uint8_t *buf_ptr, uintptr_t buf_len);
/// Gets the address of a module in a process.
Address process_get_module_address(ProcessId process, const uint8_t *name_ptr, uintptr_t name_len);
/// Gets the size of a module in a process.
uint64_t process_get_module_size(ProcessId process, const uint8_t *name_ptr, uintptr_t name_len);

/// Sets the tick rate of the runtime. This influences the amount of
/// times the `update` function is called per second.
void runtime_set_tick_rate(float64_t ticks_per_second);
/// Prints a log message for debugging purposes.
void runtime_print_message(const uint8_t *text_ptr, uintptr_t text_len);

/// Adds a new setting that the user can modify. This will return either
/// the specified default value or the value that the user has set.
bool user_settings_add_bool(const uint8_t *key_ptr, uintptr_t key_len, const uint8_t *description_ptr, uintptr_t description_len, bool default_value);

#ifdef __cplusplus
}
#endif

#endif /* ASR_H */
