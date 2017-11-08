#!/usr/bin/env bash

# This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
#     found in the LICENSE file distributed with the ReboundU source code, or online at 
#     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
#     text of the license itself, must not be removed from the source files.
# Copyright © 2017 Sean Moss

if [ "$(uname)" == "Darwin" ]; then
	./premake5_mac --file=reboundu.build gmake
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	./premake5_linux --file=reboundu.build gmake
else
	echo "Could not detect the operating system. Please use a standard MacOSX or Linux distribution."
fi