#ifndef ASR_H
#define ASR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASR_NO_FLOAT64
#include <assert.h>
#include <limits.h>
#define EXPECTED_DOUBLE_SIZE 64
static_assert((sizeof(double) * CHAR_BIT) == EXPECTED_DOUBLE_SIZE, "double is not a float64");

typedef double float64_t;
#endif

typedef uint64_t Address;
typedef uint64_t ProcessId;

typedef uint32_t TimerState;
enum TimeStateValues {
    /// The timer is not running.
    TIMERSTATE_NOT_RUNNING = 0,
    /// The timer is running.
    TIMERSTATE_RUNNING = 1,
    /// The timer started but got paused. This is separate from the game
    /// time being paused. Game time may even always be paused.
    TIMERSTATE_PAUSED = 2,
    /// The timer has ended, but didn't get reset yet.
    TIMERSTATE_ENDED = 3,
};

typedef uint64_t MemoryRangeFlags;
enum MemoryRangeFlagsValues {
    /// The memory range is readable.
    MEMORYRANGEFLAGS_READ = 1 << 1,
    /// The memory range is writable.
    MEMORYRANGEFLAGS_WRITE = 1 << 2,
    /// The memory range is executable.
    MEMORYRANGEFLAGS_EXECUTE = 1 << 3,
    /// The memory range has a file path.
    MEMORYRANGEFLAGS_PATH = 1 << 4,
};

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
void timer_set_variable(const uint8_t* key_ptr,
                        uintptr_t key_len,
                        const uint8_t* value_ptr,
                        uintptr_t value_len);

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
ProcessId process_attach(const uint8_t* name_ptr, uintptr_t name_len);
/// Detaches from a process.
void process_detach(ProcessId process);
/// Checks whether is a process is still open. You should detach from a
/// process and stop using it if this returns `false`.
bool process_is_open(ProcessId process);
/// Reads memory from a process at the address given. This will write
/// the memory to the buffer given. Returns `false` if this fails.
bool process_read(ProcessId process, Address address, uint8_t* buf_ptr, uintptr_t buf_len);

/// Gets the address of a module in a process.
Address process_get_module_address(ProcessId process, const uint8_t* name_ptr, uintptr_t name_len);
/// Gets the size of a module in a process.
uint64_t process_get_module_size(ProcessId process, const uint8_t* name_ptr, uintptr_t name_len);

/// Gets the number of memory ranges in a given process.
uint64_t process_get_memory_range_count(ProcessId process);
/// Gets the start address of a memory range by its index.
Address process_get_memory_range_address(ProcessId process, uint64_t idx);
/// Gets the size of a memory range by its index.
uint64_t process_get_memory_range_size(ProcessId process, uint64_t idx);
/// Gets the flags of a memory range by its index.
MemoryRangeFlags process_get_memory_range_flags(ProcessId process, uint64_t idx);

/// Stores the file system path of the executable in the buffer given. The
/// path is a pa thth that is accessiblerough the WASI file system, so a
/// Windows path of `C:\foo\bar.exe` would be returned as
/// `/mnt/c/foo/bar.exe`. Returns `false` if the buffer is too small. After
/// this call, no matter whether it was successful or not, the
/// `buf_len_ptr` will be set to the required buffer size. The path is
/// guaranteed to be valid UTF-8 and is not nul-terminated.
bool process_get_path(ProcessId process, uint8_t* buf_ptr, uintptr_t* buf_len_ptr);

/// Sets the tick rate of the runtime. This influences the amount of
/// times the `update` function is called per second.
void runtime_set_tick_rate(float64_t ticks_per_second);
/// Prints a log message for debugging purposes.
void runtime_print_message(const uint8_t* text_ptr, uintptr_t text_len);
/// Stores the name of the operating system that the runtime is running
/// on in the buffer given. Returns `false` if the buffer is too small.
/// After this call, no matter whether it was successful or not, the
/// `buf_len_ptr` will be set to the required buffer size. The name is
/// guaranteed to be valid UTF-8 and is not nul-terminated.
/// Example values: `windows`, `linux`, `macos`
bool runtime_get_os(uint8_t* buf_ptr, uintptr_t* buf_len_ptr);
/// Stores the name of the architecture that the runtime is running on
/// in the buffer given. Returns `false` if the buffer is too small.
/// After this call, no matter whether it was successful or not, the
/// `buf_len_ptr` will be set to the required buffer size. The name is
/// guaranteed to be valid UTF-8 and is not nul-terminated.
/// Example values: `x86`, `x86_64`, `arm`, `aarch64`
bool runtime_get_arch(uint8_t* buf_ptr, uintptr_t* buf_len_ptr);

/// Adds a new setting that the user can modify. This will return either
/// the specified default value or the value that the user has set.
bool user_settings_add_bool(const uint8_t* key_ptr,
                            uintptr_t key_len,
                            const uint8_t* description_ptr,
                            uintptr_t description_len,
                            bool default_value);

#ifdef __cplusplus
}
#endif

#endif /* ASR_H */
