# References and Prior Art

`gcps3` should learn from existing GameCube/Wii reverse-engineering, decompilation, and compatibility-layer projects. These projects are references for architecture and behaviour, not dependencies unless explicitly stated.

## Dolphin

Dolphin is the primary behavioural reference for GameCube and Wii hardware, software behaviour, GX/TEV rendering, audio, filesystem behaviour, and controller behaviour.

`gcps3` is not a Dolphin port and should not attempt to become a conventional emulator. Dolphin is useful as a correctness reference, documentation source, and debugging oracle.

## Aurora

Aurora is a source-level GameCube/Wii compatibility layer for decompilation projects.

It is highly relevant to `gcps3` because it addresses many of the same high-level problems:

- application/runtime abstraction
- GX compatibility
- PAD compatibility
- DVD/disc abstraction
- save compatibility
- controller mapping
- pipeline caching
- resolution scaling and widescreen support
- host-platform portability

Aurora targets modern host graphics APIs through WebGPU/Dawn. `gcps3` cannot use that renderer design directly on PS3, but its frontend/backend separation, runtime structure, and source-port focus are valuable references.

## Dusklight

Dusklight is a reverse-engineered reimplementation of *The Legend of Zelda: Twilight Princess* built on Aurora.

It is useful as a practical example of the source-level compatibility-layer model applied to a real GameCube game. `gcps3` should treat it as proof that this approach can scale beyond samples, while still recognising that PS3 imposes different renderer, memory, tooling, and performance constraints.

## Reference Policy

References should guide architecture and validation, but should not expand the project scope prematurely.

When studying prior art:

- prefer concepts over copying structure blindly
- keep PS3 constraints central
- avoid adding dependencies before they are needed
- keep the null backend and small samples buildable
- preserve `gxrsx` as the eventual PS3 renderer backend