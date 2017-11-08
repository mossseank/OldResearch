#
# Copyright Sean Moss (c) 2017
# Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
#     this repository. If a copy of this license was not included, it can be found at
#     <http://www.gnu.org/licenses/>.
#

"""
This file is simply a script that tests the python parser code included in this directory. In order
    to use this script, you must first build luabound, and run the "example_simulation.lua"
    simulation found in this directory. Otherwise, the tool will complain of not being able to find
    the files.
"""

import sys
import os
sys.path.insert(0, os.path.abspath('./../python'))

from luabound import load_lbd_file

def test():
    testfile = load_lbd_file('../OUT/parser_test.dat')
    print(testfile.filename)
    print(testfile.timestamp)
    print(testfile.outrate)
    print(testfile.outfmt)
    print(testfile.datalen)
    print(testfile[0])
    print(testfile[0].data)
    print(testfile['st'].data)
    print(testfile['l0'])
    print(testfile['l0'][0]['pa'])
    print(testfile['l0']['pa'][5:9])
    print(testfile[0:5])
    print(testfile[0:5].data)
    print(testfile[0:5]['st'].data)
    print(testfile[0::5]['st'].data)
    print(testfile['ajv'].x)

if __name__ == '__main__':
    test()
