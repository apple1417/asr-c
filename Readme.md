1. Install clang/cmake/ninja.
2. If you don't have a `<LLVM>\lib\clang\[version]\lib\wasi`, download
   [`libclang_rt.builtins-wasm32-wasi-[version].tar.gz`](https://github.com/WebAssembly/wasi-sdk/releases)
   and extract it there.
3. Download [`wasi-sysroot-[version].tar.gz`](https://github.com/WebAssembly/wasi-sdk/releases)
   (and extract/symlink into this folder if using the standard preset).
4. ```
   cmake . --preset standard
   cmake --build build
   ```
