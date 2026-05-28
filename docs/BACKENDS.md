# gcps3 Backends

`gcps3` keeps the GX frontend backend-neutral. Backend code should stay behind the internal backend interface and should not leak platform-specific details into public GX APIs.

## gxnull

`gxnull` is the default PC backend. It has no external dependencies and logs GX state, draw packets, vertices, descriptors, and matrix snapshots instead of rendering.

Default builds use `gxnull`:

```sh
cmake -S . -B build
cmake --build build
```

## gxpc

`gxpc` is an optional SDL2/OpenGL development visualizer for quick local checks. It can open a window and draw the current immediate-mode triangle sample, but it is not the final renderer architecture.

For frontend validation, `gxpc` currently applies the captured 3x4 position matrix from each draw packet to submitted vertices before drawing. This is a visual debugging aid only; it is not the final matrix, material, or shader architecture.

Enable it explicitly:

```sh
cmake -S . -B build-gxpc -DGCPS3_ENABLE_GXPC=ON
cmake --build build-gxpc
./build-gxpc/gx_triangle
```

When `GCPS3_ENABLE_GXPC` is off, SDL2 and OpenGL are not required by the build.

## gxrsx

`gxrsx` is the planned PS3 RSX backend. It should be added only after the GX frontend and PC validation path are stable enough to justify PS3-specific work.

`gxpc` does not replace `gxrsx`; it is only a temporary visual debugging aid.
