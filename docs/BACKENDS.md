# gcps3 Backends

`gcps3` keeps the GX frontend backend-neutral. Backend code should stay behind the internal backend interface and should not leak platform-specific details into public GX APIs. The public GX layer is beginning to expose authentic GX-style compatibility entry points such as `GX_Begin`, while the frontend still lowers them into the same backend-neutral draw packets used by the temporary validation APIs.

## gxnull

`gxnull` is the default PC backend. It has no external dependencies and logs GX state, draw packets, vertices, descriptors, and matrix snapshots instead of rendering.

Default builds use `gxnull`:

```sh
cmake -S . -B build
cmake --build build
```

## gxpc

gxpc is an optional SDL2/OpenGL development visualizer for quick local checks. It can open a window and draw the current immediate-mode triangle sample, but it is not the final renderer architecture.

For frontend validation, gxpc currently applies the captured 3x4 position matrix from each draw packet to submitted vertices before drawing. It can also upload the current RGBA8 texture object and consume minimal depth-test state for textured immediate-mode triangle and cube samples. These are visual debugging aids only; they are not the final matrix, material, texture, depth, or shader architecture.

## Coordinate convention

gxpc currently treats submitted, matrix-transformed vertex positions as simple OpenGL-style visualizer coordinates.

This means early samples should use coordinates that are already suitable for the debug OpenGL view. gxpc does not currently implement real GameCube viewport, projection, clip-space, depth-range, or screen-space semantics.

This convention is temporary. It exists only so the frontend can be validated visually before the project has a real renderer backend. Future work should define proper GX-style viewport, projection, and transform behaviour before gxrsx relies on these semantics.

Enable it explicitly:

cmake -S . -B build-gxpc -DGCPS3_ENABLE_GXPC=ON
cmake --build build-gxpc
./build-gxpc/gx_triangle
./build-gxpc/gx_textured_triangle
./build-gxpc/gx_textured_cube

When GCPS3_ENABLE_GXPC is off, SDL2 and OpenGL are not required by the build.

## gxrsx

gxrsx is the planned PS3 RSX backend. It should be added only after the GX frontend and PC validation path are stable enough to justify PS3-specific work.

gxpc does not replace gxrsx; it is only a temporary visual debugging aid.
