# asr_asl
Provides asl-like callbacks for asr scripts. Doesn't provide a full equivalent for state blocks, you
need to handle versions and pointer paths manually, but does let your just provide a list of
executable names, which will automatically be hooked.

## Comparison with ASL
ASL         | asr_asl      | Notes
:-----------|:-------------|:------
`startup`   | `startup`    |
`shutdown`  |              | ASR cleans up any resources it lets you allocate automatically.
`init`      | `on_launch`  | Renamed to mirror `on_exit`. Can also now reject processes.
`exit`      | `on_exit`    | Renamed to avoid name clash.
`update`    | `update`     |
`start`     | `start`      |
`split`     | `split`      |
`reset`     | `reset`      |
`isLoading` | `is_loading` |
`gameTime`  | `game_time`  |
`onStart`   | `on_start`   |
`onSplit`   |              | ASR doesn't provide split info.
`onReset`   | `on_reset`   |

## Control flow
When the script is first loaded, `startup` is called.

When a matching executable is launched, `on_launch` is called. If you return true, it attaches to
that process. Only one process can be attached to at once, once attached future launches won't call
it. When the attached process exits, `on_exit` is run (and future launches will call `on_launch`
again).

Once attached to a process, the main update loop will start. `update` is called first, if it returns
false, none of the following functions will be called.

During the update loop, if the timer is not running, only `start` will be called. If it returns
true, the timer will start, and it'll fall through to the standard "timer running" functions.

When the timer is running, the update loop starts by calling `is_loading`, The timer will be
paused/unpaused as needed based on it's return value. `game_time` will be called next, and the game
time will be set to whatever it returns (if it's not null). After dealing with game time, it will
call `reset`, and if it returns true it will reset the timer. If `reset` returns false, it will
finally go on to call `split`, and will split if it returns true.

Whenever the timer is started, regardless of manually or automatically, `on_start` will be called.
Similarly, whenever it's reset, `on_reset` will be called.
