#!/bin/sh

BUILD_DIRECTORY="./build"

meson setup $BUILD_DIRECTORY
ninja -C $BUILD_DIRECTORY

tar czf build.tar.gz $BUILD_DIRECTORY/librb-discord.so rhythmbox-discord-v2.plugin
