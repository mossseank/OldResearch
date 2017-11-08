#!/usr/bin/env bash

if [ "$(uname)" == "Darwin" ]; then
	if [ $# -gt 0 ]; then
		./premake5_mac --file=./luabound_plugin.build gmake --pfile=$1
	else 
		./premake5_mac --file=./luabound_plugin.build gmake
	fi
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	if [ $# -gt 0 ]; then
		./premake5_linux --file=./luabound_plugin.build gmake --pfile=$1
	else
		./premake5_linux --file=./luabound_plugin.build gmake
	fi
fi