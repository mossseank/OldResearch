#!/usr/bin/env bash

if [ "$(uname)" == "Darwin" ]; then
	./premake5_mac --file=./luabound.build gmake
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	./premake5_linux --file=./luabound.build gmake
fi
