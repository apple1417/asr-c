# C ASR Helpers
Various C/C++ helpers for interacting with [LiveSplit's Auto Splitting Runtime](https://github.com/LiveSplit/livesplit-core/tree/master/crates/livesplit-auto-splitting).

This repo expects you're using Clang to compile. I've listed the extensions which are used, if you
want to try port it to another compiler. One thing used a lot in general is the assumption that
`char` strings can be transparently cast to utf8 `uint8_t` ones - strictly speaking this isn't
guaranteed.

To get a wasm-capable Clang install, the easiest way I've found is:
1. Install Clang normally
2. Check if it included a file `<LLVM>/lib/clang/[version]/lib/wasi/libclang_rt.builtins-wasm32.a`
3. If not, download [`libclang_rt.builtins-wasm32-wasi-[version].tar.gz`](https://github.com/WebAssembly/wasi-sdk/releases),
   and extract it to that location
4. Download [`wasi-sysroot-[version].tar.gz`](https://github.com/WebAssembly/wasi-sdk/releases), and
   whenever compiling specify it as the sysroot.

To build the hello world project, using the provided CMake files.
1. Make sure you also have CMake and a Ninja installed
2. Copy/symlink the wasi sysroot to `wasi-sysroot` in the repo root dir.
3. ```
   cmake . --preset debug
   cmake --build build
   ```

# Targets

## `asr`
C headers holding the ASR interface - all exported functions and their types.

Note that the actual ASR interface uses a few "non-zero" types - we can't make this guarantee in
C (Clang's nullable extensions only apply to pointers, not integer types). This means you won't get
warned about passing zeros in. The runtime will error gracefully though, there's no undefined
behaviour.

## `asr_asl`
Provides asl-like callbacks for asr scripts. See it's [readme](asr_asl/Readme.md) for more.

Requires the extension `__attribute__((export_name))` to specify exported wasm symbols.

Requires the extension `__attribute__((weak))` so that it can provide default empty implementations,
which are overwritten by those you write in your own scripts. You can define `ASL_NO_WEAK_DEFAULTS`
to avoid this, though you'll have to implement every function yourself then.

## `asr_utils`
A more heavyweight C++ library, containing various more advanced helpers. Again see it's
[readme](asr_utils/Readme.md) for more.

Requires C++20, and specifically within it `std::format`. Shouldn't rely on any implementation
defined behaviour though, just need to make sure your library is new enough.

## `hello_world`
A basic hello world script, using only the `asr` library.

Requires the extension `__attribute__((export_name))` to specify exported wasm symbols.
