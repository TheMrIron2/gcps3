cmake_minimum_required(VERSION 3.16)

project(gcps3 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(gcps3
    src/core/backend.c
    src/core/config.c
    src/core/log.c
    src/core/runtime.c
    src/gx/gx_frontend.c
    src/gx/gx_state.c
    src/backends/pc/gxnull.c
    src/dvd/dvd_common.c
    src/os/os_common.c
    src/pad/pad_common.c
)

target_include_directories(gcps3
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(gcps3
    PRIVATE
        $<$<C_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra -Wpedantic>
)

add_executable(hello_runtime samples/hello_runtime/main.c)
target_link_libraries(hello_runtime PRIVATE gcps3)

add_executable(gx_clear samples/gx_clear/main.c)
target_link_libraries(gx_clear PRIVATE gcps3)

add_executable(gx_triangle samples/gx_triangle/main.c)
target_link_libraries(gx_triangle PRIVATE gcps3)

add_executable(gx_validation samples/gx_validation/main.c)
target_link_libraries(gx_validation PRIVATE gcps3)

add_executable(gx_vtxdesc samples/gx_vtxdesc/main.c)
target_link_libraries(gx_vtxdesc PRIVATE gcps3)

add_executable(gx_matrix samples/gx_matrix/main.c)
target_link_libraries(gx_matrix PRIVATE gcps3)
