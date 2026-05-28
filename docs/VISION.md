# gcps3 Vision

`gcps3` is an experimental GameCube-to-PlayStation 3 portability project.

The project is intended for decompiled GameCube software, GameCube-style sample programs, and eventually selected game ports. It is not a conventional emulator and should not be designed as one.

The core idea is to provide a GameCube-oriented runtime layer that maps familiar GameCube SDK-style systems onto native PS3 services. The renderer backend for PS3 is tentatively named `gxrsx`.

## Goal

The goal of `gcps3` is to make selected GameCube software portable to PS3 by replacing GameCube platform dependencies with native implementations.

At a high level:

```text
GameCube decomp source or sample code
        ↓
gcps3 compatibility headers
        ↓
gcps3 runtime layer
        ↓
native platform backend
        ↓
PS3 / PC / future targets
```

The project should begin with small samples, not commercial games.

## Primary Goals

* Provide GameCube-like runtime APIs for porting decompiled GameCube software.
* Provide compatibility headers for commonly used GameCube SDK-style systems.
* Implement a GX-inspired graphics frontend.
* Implement a PS3 RSX backend named `gxrsx`.
* Use PC backends for fast development and correctness testing where useful.
* Use PS3 SPEs for suitable bulk workloads once the basic runtime exists.
* Improve PSL1GHT through practical pressure testing, reusable examples, documentation, and bug fixes.
* Prefer correctness, clarity, and debuggability before optimisation.

## Non-goals

* Do not port Dolphin to PS3.
* Do not attempt cycle-accurate GameCube emulation.
* Do not attempt binary compatibility with retail GameCube executables in the first phase.
* Do not attempt full GX or TEV coverage at launch.
* Do not use game-specific hacks as the foundation of the runtime.
* Do not optimise before the project has measurable samples and profiling data.

## Initial Success Criteria

The first meaningful milestone is a small sample, not a game.

The initial success condition is:

* A small GameCube-style sample builds on PC.
* The same sample can later build for PS3 using PSL1GHT.
* The sample uses `OS`, `PAD`, `DVD`, and `GX`-style APIs.
* The graphics sample eventually renders a textured cube through the GX frontend.
* The PS3 version eventually renders through `gxrsx`.
* Any PSL1GHT issues discovered during the process are documented or converted into examples, fixes, or regression tests.

## Long-term Success Criteria

A mature version of `gcps3` should make it realistic to port selected GameCube decompilation projects to PS3 with less bespoke platform work.

Long-term goals include:

* stable PS3 builds
* reliable RSX rendering
* a useful subset of GX and TEV
* audio support suitable for real games
* controller support
* filesystem and asset streaming support
* optional SPE acceleration for audio, texture conversion, and math batches
* per-game configuration for resolution and enhancement options
* clear documentation for contributors
