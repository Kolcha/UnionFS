cmake_minimum_required(VERSION 3.8)

project(unityfs C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)

option(USE_FUSE3 "use FUSE3 API instead of FUSE2" NO)

find_package(PkgConfig REQUIRED)

add_executable(unityfs
    attrs.c
    config_file.c
    config_file.h
    context.c
    context.h
    core.c
    core.h
    disk.c
    disk.h
    init.c
    io.c
    links.c
    ordered_set.c
    ordered_set.h
    private.c
    private.h
    readdir.c
    unityfs.h
    xattrs.c
)

if (USE_FUSE3)
    pkg_check_modules(FUSE REQUIRED IMPORTED_TARGET fuse3>=3.2)
    target_compile_definitions(unityfs PRIVATE FUSE_USE_VERSION=32)
    target_sources(unityfs PRIVATE main.c)
else()
    pkg_check_modules(FUSE REQUIRED IMPORTED_TARGET fuse>=2.6)
    target_compile_definitions(unityfs PRIVATE FUSE_USE_VERSION=26)
    target_sources(unityfs PRIVATE main2.c)
endif()

target_link_libraries(unityfs PRIVATE PkgConfig::FUSE)

install(TARGETS unityfs)
