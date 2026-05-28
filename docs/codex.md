# Codex Instructions for gcps3

This repository is an experimental GameCube-to-PS3 portability runtime.

Codex should treat the project as a staged runtime and tooling effort. Do not attempt to build a full emulator or a full game port in one step.

## Core Rule

Prefer small, buildable, reviewable changes.

Every change should either:

* improve the project skeleton
* define an interface
* implement a small subsystem
* add a sample
* improve documentation
* fix a build issue

## Project Priorities

1. Keep the repository buildable.
2. Keep interfaces simple.
3. Keep PC and PS3 backend code separated.
4. Keep frontend runtime code backend-neutral.
5. Prefer correctness and clear logging over performance.
6. Add tests or samples for new behaviour where practical.
7. Avoid game-specific hacks in the core runtime.
8. Document assumptions and unsupported features.

## First Implementation Target

Create the initial repository skeleton.

The first target is a host-buildable sample named `hello_runtime`.

The sample should:

* initialise the runtime
* print a log message
* initialise the OS layer
* initialise the PAD layer, even if it is a stub
* initialise the DVD layer, even if it is a stub
* shut down cleanly

Do not implement PS3-specific code in the first step unless the build system already supports it cleanly.

## Second Implementation Target

Add a minimal GX frontend and a null graphics backend.

The sample should be able to call:

* `GXInit`
* `GXSetViewport`
* `GXSetClearColor`
* `GXClear`
* `GXShutdown`

The null backend should log calls rather than render.

## Third Implementation Target

Add a simple graphics sample.

The sample should:

* create a window or backend context only if the selected backend supports it
* clear the screen
* submit a test triangle or textured cube later
* avoid TEV, shaders, SPEs, and game-specific assets

## PS3 Direction

PS3 support should be added gradually.

The PS3 backend should eventually use:

* PSL1GHT build tooling
* PSL1GHT filesystem access
* PSL1GHT controller APIs
* RSX rendering through `gxrsx`
* SPE jobs only after the basic runtime and graphics path are stable

Do not begin by implementing SPE jobs.

Do not begin by implementing full TEV.

Do not begin with a commercial GameCube game.

## Renderer Direction

The renderer should be split into:

* GX frontend
* backend-neutral state representation
* backend implementation

The PS3 renderer backend is named `gxrsx`.

The initial renderer goal is:

```text
GX-style sample
        ↓
GX frontend
        ↓
null or PC backend
        ↓
later gxrsx backend
```

Only after this works should the project attempt:

* textures
* vertex formats
* matrices
* depth/blend/cull state
* TEV subset
* material caching
* shader or fixed-function translation

## SPE Direction

SPE work should be introduced only when there are real workloads and profiling data.

Potential future SPE jobs:

* AX-style audio mixing
* ADPCM decode
* texture conversion
* texture swizzling
* matrix batches
* animation/skinning batches

Avoid using SPEs for:

* high-level GX interpretation
* TEV state compilation
* branch-heavy game logic
* premature optimisation

## Do Not Do Yet

* Do not port Dolphin.
* Do not implement a GameCube binary loader.
* Do not implement full TEV.
* Do not implement full audio.
* Do not implement a game-specific renderer.
* Do not add complex dependencies before the basic samples build.
* Do not optimise before there are profiling numbers.

## Good First Codex Prompt

Read `docs/vision.md`, `docs/architecture.md`, and `docs/codex.md`.

Create the initial `gcps3` repository skeleton with:

* public headers under `include/gcps3`
* source folders under `src`
* backend folders for `pc` and `ps3`
* a minimal logging system
* a minimal runtime init/shutdown path
* a `samples/hello_runtime` sample
* a simple host build path

Do not implement rendering yet. Do not implement PS3-specific functionality yet. Add clear TODO comments where platform-specific implementation will be needed.

Before implementing larger systems such as GX, PAD, DVD, or save handling, check `docs/REFERENCES.md` for relevant prior art and constraints.