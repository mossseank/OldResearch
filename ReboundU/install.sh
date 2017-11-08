#!/usr/bin/env bash

# This file is part of the ReboundU project, and is licensed under the GNU GPL v3 license, the text of which can be 
#     found in the LICENSE file distributed with the ReboundU source code, or online at 
#     <https://opensource.org/licenses/GPL-3.0>. In the event of redistruction of this code, this header, or the 
#     text of the license itself, must not be removed from the source files.
# Copyright © 2017 Sean Moss

echo "Installing files to 'INSTALL/'..."

if [ ! -f "./include/reboundu.h" ]; then
	echo "File directory invalid (reboundu.h), please redownload the repo"
	exit
fi
if [ ! -f "./rebound/rebound.h" ]; then
	echo "File directory invalid (rebound.h), please redownload the repo"
	exit
fi
if [ ! -f "./rebound/bin/librebound.a" ]; then
	echo "Please make sure you build the library first..."
	exit
fi
if [ ! -f "./OUT/libreboundu.a" ]; then
	echo "Please make sure you build the library first..."
	exit
fi

mkdir -p INSTALL
cp ./include/reboundu.h ./INSTALL/
cp ./rebound/rebound.h ./INSTALL/
cp ./rebound/bin/librebound.a ./INSTALL/
cp ./OUT/libreboundu.a ./INSTALL/

echo "Finished" 