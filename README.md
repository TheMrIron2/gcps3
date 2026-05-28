# gcps3

`gcps3` is an experimental GameCube/Wii-to-PlayStation 3 portability runtime.

The project is not an emulator and is not a Dolphin port. Its goal is to provide GameCube/Wii-style runtime APIs that can be implemented on native host and PS3 backends, allowing decompiled source ports and sample programs to be adapted incrementally.

## Current status

The repository currently contains a host-buildable C99 runtime skeleton with:

- core runtime init/shutdown
- logging and configuration
- OS, DVD, and PAD stubs
- a minimal GX-style frontend
- a null PC graphics backend
- immediate-mode triangle packet validation
- minimal vertex descriptor state
- minimal position matrix state

No real rendering backend, PS3 backend, RSX backend, SPE job system, texture support, TEV support, or commercial game support exists yet.

## Objectives

- Provide GameCube/Wii-style APIs for portable source-level ports.
- Keep frontend runtime code backend-neutral.
- Build a PS3 RSX renderer backend named `gxrsx`.
- Use PC backends for fast validation before PS3 hardware work.
- Use SPEs later for suitable bulk workloads such as audio, ADPCM decode, texture conversion, and matrix/animation batches.
- Feed practical findings back into PSL1GHT and PS3 homebrew tooling.

## Host build

```sh
cmake -S . -B build
cmake --build build
./build/hello_runtime
./build/gx_clear
./build/gx_triangle
./build/gx_validation
./build/gx_vtxdesc
./build/gx_matrix