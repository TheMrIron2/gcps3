# gcps3

An experimental GameCube-to-PS3 portability project. The goal of gcps3 is to provide devs with tools that assist in porting GameCube and Wii software to PS3.

Major changes need to be made to PSL1GHT and the PS3 toolchain before a serious project is possible. However, the direction of the initial groundwork is to create a runtime layer that lets GameCube and Wii source code and decomp projects build against native host and PS3 backends.

## Objectives

- Provide GameCube-like APIs for PS3.
- Hardware-accelerated RSX graphics backend, `gsrsx`.
- Accelerate common tasks such as audio and suitable bulk workloads with the SPEs.
- Make decompiled GameCube / Wii games portable for PS3.

gcps3 should also serve as valuable data for improving PSL1GHT and other PS3 tooling in practical settings.

## Host build

The initial skeleton builds a host-only runtime library and the `hello_runtime` sample.

```sh
cmake -S . -B build
cmake --build build
./build/hello_runtime
```
