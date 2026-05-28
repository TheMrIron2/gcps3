# gcps3

An experimental GameCube-to-PS3 portability project. The goal of gcps3 is to provide devs with tools that assist in porting GameCube and Wii software to PS3.

Major changes need to be made to PSL1GHT and the PS3 toolchain before a serious project is possible. However, the direction of the initial groundwork is to create a layer that can interpret basic GameCube code for PS3.

## Host build

The initial skeleton builds a host-only runtime library and the `hello_runtime` sample.

```sh
cmake -S . -B build
cmake --build build
./build/hello_runtime
```
