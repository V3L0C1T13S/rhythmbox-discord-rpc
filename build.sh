#!/bin/sh

export PATH="/usr/lib64/ccache:$PATH"
export CC=/usr/lib64/ccache/clang CXX=/usr/lib64/ccache/clang++
export CC_LD=lld CXX_LD=lld
BUILD_DIRECTORY="./build"

meson setup $BUILD_DIRECTORY
ninja -C $BUILD_DIRECTORY

tar czf build.tar.gz $BUILD_DIRECTORY/librb-discord.so rhythmbox-discord-v2.plugin
