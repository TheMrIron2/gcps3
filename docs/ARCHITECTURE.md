# gcps3 Architecture

`gcps3` is a portability runtime for GameCube-style software.

It should be structured as a set of small subsystems with clear boundaries. The project should avoid mixing game code, platform code, renderer code, and PS3-specific implementation details in the same layer.

## Intended Structure

```text
GameCube decomp source / sample program
        ↓
gcps3 public headers
        ↓
gcps3 runtime frontend
        ↓
backend interface
        ↓
PC backend / PS3 backend / future backend
```

## Repository Layout

Suggested initial layout:

```text
gcps3/
  include/
    gcps3/
      config.h
      log.h
      os.h
      dvd.h
      pad.h
      gx.h
      gx_types.h
      backend.h
  src/
    core/
      log.c
      config.c
    os/
      os_common.c
    dvd/
      dvd_common.c
    pad/
      pad_common.c
    gx/
      gx_frontend.c
      gx_state.c
    backends/
      pc/
        pc_main.c
        pc_pad.c
        pc_dvd.c
        gxnull.c
      ps3/
        ps3_main.c
        ps3_pad.c
        ps3_dvd.c
        gxrsx.c
  samples/
    hello_runtime/
    gx_clear/
    textured_cube/
  docs/
    vision.md
    architecture.md
    codex.md
```

This layout may change, but the separation of concerns should remain.

## Runtime Layers

### Core

The core layer provides basic functionality shared by all subsystems.

Initial responsibilities:

* logging
* configuration
* assertions
* simple error codes
* startup and shutdown sequencing

### OS Layer

The OS layer provides a small replacement for commonly used GameCube OS-style functions.

Initial scope:

* `OSInit`
* `OSGetTime`
* `OSReport`
* basic time conversion helpers
* stubs for unsupported functionality

Threading, alarms, and advanced scheduling should be added only when required by samples.

### DVD / Filesystem Layer

The DVD layer maps GameCube-style file access onto host filesystems.

Initial scope:

* set asset root
* open file
* read file
* seek file
* close file
* synchronous reads first
* async read stubs later

The first implementation should use normal host files on PC. The PS3 implementation should later map to PSL1GHT-supported filesystem access.

### PAD Layer

The PAD layer maps GameCube controller concepts onto platform input.

Initial scope:

* controller init
* poll controller state
* buttons
* analog stick
* C-stick
* triggers

The PC backend may use keyboard or SDL later. The PS3 backend should eventually use PSL1GHT controller APIs.

### GX Frontend

The GX frontend is a GameCube-inspired graphics API. It should not directly contain RSX code.

Initial scope:

* init/shutdown
* clear colour
* viewport
* scissor
* depth state
* blend state
* cull state
* basic vertex submission
* basic texture object representation
* simple draw packet representation

The GX frontend should produce backend-neutral state and draw information.

### `gxrsx` Backend

`gxrsx` is the PS3 RSX backend for the GX frontend.

Responsibilities:

* translate GX frontend state into RSX state
* allocate and upload textures
* submit draw commands
* manage render targets
* minimise unnecessary state changes
* avoid CPU-to-RSX synchronisation stalls where possible

`gxrsx` should be implemented only after a null or PC backend exists.

### PC Graphics Backend

A PC backend should exist for fast development.

Early versions may simply log GX calls. Later versions may use OpenGL, Vulkan, or another practical host renderer.

The PC backend is for:

* correctness testing
* API validation
* fast iteration
* comparison against expected output

### SPE Job Layer

The SPE job layer is PS3-specific and should not be implemented before the basic runtime and renderer skeleton exist.

Good future SPE workloads include:

* audio mixing
* ADPCM decode
* texture conversion
* texture swizzling
* matrix batches
* skinning batches

Bad early SPE workloads include:

* high-level GX interpretation
* TEV state compilation
* game logic
* branch-heavy runtime code

## Design Rules

* Keep public APIs small.
* Keep backend interfaces explicit.
* Keep PS3-specific code out of frontend runtime code.
* Build and test on PC before PS3 where practical.
* Prefer stubs over fake completeness.
* Log unsupported features clearly.
* Treat every discovered PSL1GHT limitation as a candidate for documentation, samples, tests, or upstream fixes.
* Avoid commercial game assumptions in the core runtime.
