# gcps3

<img width="80%" alt="gcps3-banner" src="https://github.com/user-attachments/assets/2ae8f269-0897-4b13-89d8-2effb7cb09fb" />

`gcps3` is an experimental GameCube / Wii-to-PlayStation 3 portability runtime.

The project is not an emulator and is not a Dolphin port. Its goal is to provide GameCube/Wii-style runtime APIs that can be implemented on native host and PS3 backends, allowing decompiled source ports and sample programs to be adapted incrementally.

## Current status

The repository currently contains a host-buildable C99 runtime skeleton with:

- core runtime init/shutdown
- logging and configuration
- OS, DVD, and PAD stubs
- a minimal GX-style frontend
- authentic GX-style immediate-mode compatibility entry points for early samples
- a null PC graphics backend
- an optional SDL2/OpenGL PC debug visualizer backend named `gxpc`
- immediate-mode triangle packet validation
- minimal vertex descriptor state
- minimal position matrix state
- minimal RGBA8 texture-object state for immediate-mode triangle validation
- minimal depth state capture and gxpc depth-test validation

No final rendering backend, PS3 backend, RSX backend, SPE job system, full texture pipeline, TEV support, or commercial game support exists yet.

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
./build/gx_textured_triangle
./build/gx_textured_cube
./build/gx_compat_triangle
./build/gx_compat_textured_triangle
```

## Optional gxpc debug visualizer

See [docs/BACKENDS.md](docs/BACKENDS.md) for the backend overview.

The default PC graphics backend is `gxnull`, which has no external dependencies and only logs GX packets.

For local development, an optional `gxpc` backend can be enabled with SDL2 and OpenGL:

```sh
cmake -S . -B build-gxpc -DGCPS3_ENABLE_GXPC=ON
cmake --build build-gxpc
./build-gxpc/gx_triangle
```

`gxpc` is only a debug visualizer for early frontend validation. It is not the final renderer, does not replace `gxrsx`, and intentionally does not implement TEV or real GameCube rendering semantics. It currently applies captured position matrices and RGBA8 textures only as visual validation aids.
