#!/bin/bash


git submodule update --init --recursive || echo "error updating submodules"

# build tgbot-cpp
cd tgbot-cpp
cmake .
make -j4

echo "Finished configuring"
