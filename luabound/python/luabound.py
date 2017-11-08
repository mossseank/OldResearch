#
# Copyright Sean Moss (c) 2017
# Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
#     this repository. If a copy of this license was not included, it can be found at
#     <http://www.gnu.org/licenses/>.
#

"""
This file provides functions for reading in the output files from the luabound tool, and
    populating numpy arrays with the formatted data. It allows easy lookup of parts of the
    formatted output. Because the format information is included in the header of the output
    files, the loading can be done automatically, with nothing more than the file name. Simply
    import the file into your python script, and call the ``load_lbd_file()`` function.

This script is designed to work on the raw output from the luabound tool. If you manually edit
    the contents of the output file before processing it, you run the likely risk of making the
    file unreadable to this code. Also, the script can only understand having the same tag once
    per line (i.e. two '#pa' in the same line, or other tags...) in terms of using the tags as
    indexes into the loaded data. If you do use a tag twice, indexing the data by the tag will
    only ever return the first occurance, but all occurances of the tag will still be loaded
    into memory.

Note: There are no promises made as to how performant the code is. When working with very large
    files, there is a chance that this code will get slow. It does not do any loading tricks to
    make it faster, such as streaming from the hard drive; the entire contents of the files
    are loaded into memory all at once.
"""


import os
import re
import numpy as np


__PARTICLE_OUTPUT_TOKENS = [
    'm', 'r', 'n', 'h', 'a', 'e', 'i', 'O', 'o', 'f',
    'M', 'x', 'y', 'z', 'vx', 'vy', 'vz', 'ax', 'ay', 'az',
    'R', 'Rc', 'ex', 'ey', 'ez', 'ev', 'j', 'jx', 'jy', 'jz', 'jv'
]
__SIM_OUTPUT_TOKENS = [
    'n', 't', 'dt', 'c', 'i', 'G', 'ts', 'w', 'wr'
]
__PUNCTUATION_CHARACTERS = [
    ' ', '\t', ',', ';', ':', '.', '/', '\\'
]
__PUNCTUATION_REGEX = r'[ \t,;:\/\\]+'
__STR_TOKENS = [
    'n', 'i'
]
__VECTOR_TOKENS = [
    'ev', 'jv'
]


class LuaboundFileLoadError(Exception):
    """
    Represents an error that occured while trying to load a Luabound output file.
    """
    def __init__(self, filename, message):
        """
        Creates a new instance with the given error string.

        Args:
            filename (str): The name of the file that has caused the exception.
            message (str): The error message for the exception.
        """
        super(LuaboundFileLoadError, self).__init__('"%s": %s' % (filename, message))
        self.filename = filename


class LbdVector(object):
    """
    Represents a 3-component vector.
    """
    def __init__(self, x=0.0, y=0.0, z=0.0):
        """
        Initializes a new vector with the given x, y, and z components.

        Args:
            x (float): The x-component of the vector.
            y (float): The y-component of the vector.
            z (float): The z-component of the vector.
        """
        self.x = x
        self.y = y
        self.z = z


class LbdList(object):
    """
    Represents a list of values loaded from the file from a list specifier "{}". This object can
        be indexed just like the :py:class:`LuaboundFile` using a format tag, allowing for easy
        access to arrays of values in the list.
    """
    def __init__(self, tagmap, data):
        """
        Args:
            tagmap (dict(str, int)): The map of the output tokens to the column that they represent.
            data (np.array): The extracted objects from the file that make up the list.
        """
        self._tagmap = tagmap
        self._data = data

    @property
    def tagmap(self):
        """
        Returns the tag map for this list.
        """
        return self._tagmap

    def __getitem__(self, key):
        """
        Returns either a list of all values associated with a token, or a list of values associated
            with a single particle from thet list. If the key is an integer, then the list values
            associated with the particle of the passed integer value is returned. If the key is a
            string, then all value associated with the tag of the passed value are returned.

        Returns:
            np.array(...): If the key is an integer, the values associated with the nth particle
            np.array(float): If the key is a string, the float values associated with the tag
            np.array(str): If the key is a string, the float values associated with the tag
            np.array(LbdVector): If the key is a string, the float values associated with the tag

        Raises:
            TypeError: If the key is not an integer or string type, or if it is a string type that
                is not a valid output token.
            IndexError: If the key does not map to a value in the list of values.
        """
        if not (isinstance(key, str) or isinstance(key, int)):
            raise TypeError()
        if isinstance(key, int):
            return self._data[:, key]
        else:
            if not key in self._tagmap:
                raise IndexError()
            return self._data[self._tagmap[key]]


class LbdTagView(object):
    """
    Object returned when all data associated with a tag is extracted from the file data. It allows
        for complex handling of lists from the file, such as extracting all data from a tag within
        a list for all timesteps at once, which would normally require list comprehension or
        complex for loops and slicing.
    """
    def __init__(self, tag, dataview):
        """
        *Note: This class should only be instantiated internally, users of this code should*
            ***NEVER** create an instance of this class themselves.*

        Args:
            tag (str): The tag associated with this view.
            dataview (np.array): The view on the data, pre-sliced.
        """
        self._tag = tag
        self._data = dataview
        self._islist = (tag[0] == 'l')
        self._tagmap = dataview[0].tagmap if self._islist else None

    @property
    def x(self):
        """
        If this object wraps vectors, returns all x components, otherwise raises exception.
        """
        if isinstance(self._data[0], LbdVector):
            return np.array([vec.x for vec in self._data])
        else:
            raise TypeError('The values wrapped by a TagView must be a vector to use "x" member')

    @property
    def y(self):
        """
        If this object wraps vectors, returns all y components, otherwise raises exception.
        """
        if isinstance(self._data[0], LbdVector):
            return np.array([vec.y for vec in self._data])
        else:
            raise TypeError('The values wrapped by a TagView must be a vector to use "y" member')

    @property
    def z(self):
        """
        If this object wraps vectors, returns all z components, otherwise raises exception.
        """
        if isinstance(self._data[0], LbdVector):
            return np.array([vec.z for vec in self._data])
        else:
            raise TypeError('The values wrapped by a TagView must be a vector to use "z" member')

    @property
    def tag(self):
        """
        Gets the tag that this view is associated with.
        """
        return self._tag

    @property
    def data(self):
        """
        Returns the raw view onto the file data associated with the tag.
        """
        return self._data

    def __getitem__(self, key):
        """
        If the view looks at floats, strings, or vectors, the key can only be an integer, and will
            return the value associated with the integer timestep. If the view looks at lists, an
            integer key will return the LbdList object associated with the integer timestep, and a
            string key will return the data associated with the tag embedded in the list, as a 2D
            np.array ordered by timestep, and then all values within each timestep.

        Returns:
            float: If the key is an integer, and the view points to a float type
            str: If the key is an integer, and the view points to a string type
            LbdVector: If the key is an integer, and the view points to a vector type
            LbdList: If the key is an integer, and the view points to a list type
            np.array: If the key is a string, and the view points to a list type

        Raises:
            TypeError: If the key is not an integer or string type, or if it is a string type that
                is not a valid output token.
            IndexError: If the key does not map to a value in the list of values.
        """
        if not self._islist:
            if not isinstance(key, int):
                raise TypeError('The key must be an integer type for non-list tags')
            return self._data[key]
        else:
            if isinstance(key, int):
                return self._data[key]
            elif isinstance(key, str):
                if not key in self._data[0].tagmap:
                    raise IndexError('The tag %s was not in the list' % (key))
                view = np.empty((len(self._data)), dtype=object)
                for i, dat in enumerate(self._data):
                    view[i] = dat[key]
                return view
            else:
                raise TypeError('Keys must be either integers or string for list tags')


class LbdTimestampView(object):
    """
    Object returned when all data associated with a timestep or slice of timesteps is extracted
        from the file data. It is similar to the LbdTagView class, but operates on lines from the
        output file (timesteps) instead. Slicing and indexing works similarly, and it uses the
        same tags from the original file data.
    """
    def __init__(self, tagmap, dataview):
        """
        *Note: This class should only be instantiated internally, users of this code should*
            ***NEVER** create an instance of this class themselves.*

        Args:
            tagmap (dict): The tags from the original file data
            dataview (np.array): The view on the data, pre-sliced.
        """
        self._tagmap = tagmap
        self._data = dataview

    @property
    def data(self):
        """
        Returns the handle to the raw data wrapped by this object.
        """
        return self._data.T

    def __getitem__(self, key):
        """
        Gets the associated tag view if the key is a string, or a specific timstamp if the key
            is an integer

        Returns:
            list: A list of values from a timestamp, of the key is an integer
            LbdTagView: If the key is a string that is a valid tag

        Raises:
            TypeError: If the key is not an integer or string type, or if it is a string type that
                is not a valid output token.
            IndexError: If the key does not map to a value in the list of values.
        """
        if isinstance(key, str):
            if not key in self._tagmap:
                raise IndexError('The tag %s is not in the tagmap' % (key))
            return LbdTagView(key, self._data[self._tagmap[key]])
        elif isinstance(key, int):
            return self._data[:, key]
        else:
            raise TypeError('The key must be either a string or integer')


class LuaboundFile(object):
    """
    Represents the data loaded from a luabound output file.
    """
    def __init__(self, filename, timestamp, outrate, outfmt, tagmap, data):
        """
        Creates a new instance with the given information.
        *Note: This class should only be instantiated internally, users of this code should*
            ***NEVER** create an instance of this class themselves.*

        Args:
            filename (str): The name of the file that this object represents.
            timestamp (str): The time the file was created, formatted as "DD/MM/YY HH:MM:SS".
            outrate (float): The output rate for the file in simulation time.
            outfmt (str): The raw format string used by the file.
            tagmap (dict(str, int)): A mapping of the output tags to the index in the storage
            data (np.array): The full 2D set of data processed from the file
        """
        self._filename = filename
        self._timestamp = timestamp
        self._outrate = outrate
        self._outfmt = outfmt
        self._tagmap = tagmap
        self._data = data

    @property
    def filename(self):
        """
        The name of the file on the disk represented by this object.
        """
        return self._filename

    @property
    def timestamp(self):
        """
        The creation timestamp of this file, formatted as "DD/MM/YY HH:MM:SS".
        """
        return self._timestamp

    @property
    def outrate(self):
        """
        The rate at which the output file was updated, in simulation time.
        """
        return self._outrate

    @property
    def outfmt(self):
        """
        The raw output format string used by the file.
        """
        return self._outfmt

    @property
    def datalen(self):
        """
        The number of lines (timestamps) in the data file.
        """
        return self._data.shape[1]

    def __getitem__(self, key):
        """
        Returns either a single line of data, or all data of a single tag, depending on if the
            key is an integer or string. If the key is an integer, it will return the line from
            the file corresponding to the passed value. If the key is a string, it will attempt to
            look up the column corresponding to the passed string tag.

        Returns:
            np.array(...): If the key is an integer, everything from the corresponding line
            LbdTagView: If the key is a string, a view of all the values associated to that tag

        Raises:
            TypeError: If the key is not an integer or string type, or if it is a string type that
                is not a valid output token.
            IndexError: If the key does not map to a value in the list of values.
        """
        if isinstance(key, int) or isinstance(key, slice):
            return LbdTimestampView(self._tagmap, self._data[:, key])
        elif isinstance(key, str):
            if not key in self._tagmap:
                raise IndexError()
            return LbdTagView(key, self._data[self._tagmap[key]])
        else:
            raise TypeError('The key to the data must be an integer, string, or slice')


def load_lbd_file(filepath):
    """
    Attempts to load the luabound output file at the given path. On success, returns a
    :py:class:`LuaboundFile` object, on failure, raises a :py:class:`LuaboundFileLoadError`
    exception.

    Args:
        filepath (str): The relative path to the luabound output file to load.

    Returns:
        :py:class:`LuaboundFile`: On success, the data loaded from the file, `None` otherwise.

    Raises:
        :py:class:`LuaboundFileLoadError`: In the event that the data could not be loaded for any
            reason, an error is thrown with a message explaining the nature of the load problem.
    """

    # Check if the file exists
    if not os.path.isfile(filepath):
        raise LuaboundFileLoadError(filepath, 'The file could not be found.')

    # Read in the lines from the file
    with open(filepath, 'r') as lbdFile:
        line_count = sum(1 for line in lbdFile if line.rstrip())
        if line_count < 4:
            raise LuaboundFileLoadError(filepath, 'The file does not have enough lines.')
        lbdFile.seek(0) # Rewind read pointer

        # Create the raw line containers
        header_lines = np.empty((4), dtype=object)
        data_lines = np.empty((line_count - 4), dtype=object)
        data_index = 0
        for index, line in enumerate(lbdFile.readlines()):

            # Check for empty lines (for malformed headers)
            if not line.rstrip():
                if index < 4:
                    raise LuaboundFileLoadError(filepath, 'The file has a malformed header.')
                else:
                    continue

            # Add the line to the correct container
            if index < 4:
                header_lines[index] = line.rstrip()
            else:
                data_lines[data_index] = line.rstrip()
                data_index += 1

    # Validate header information
    if header_lines[0][0:11] != '# filename:':
        raise LuaboundFileLoadError(filepath, 'The filename header line is malformed.')
    header_lines[0] = header_lines[0][12:]
    if header_lines[1][0:12] != '# timestamp:':
        raise LuaboundFileLoadError(filepath, 'The timestamp header line is malformed.')
    header_lines[1] = header_lines[1][13:]
    if header_lines[2][0:16] != '# output timing:':
        raise LuaboundFileLoadError(filepath, 'The output timing header line is malformed.')
    try:
        header_lines[2] = float(header_lines[2][17:])
    except ValueError:
        raise LuaboundFileLoadError(filepath, 'The output timing header entry is not a float.')
    if header_lines[3][0:9] != '# format:':
        raise LuaboundFileLoadError(filepath, 'The format header line is malformed.')
    header_lines[3] = header_lines[3][10:]

    # Parse the format string
    format_list = __parse_format(header_lines[0], header_lines[3], False)

    # Create the tagmap
    def _next_tag(tagmap, token):
        if not token in tagmap:
            return token
        token_index = 1
        while '%s%d' % (token, token_index) in tagmap:
            token_index += 1
        return '%s%d' % (token, token_index)
    tag_map = dict()
    _list_index = 0
    for format_entry in format_list:
        if isinstance(format_entry, str):
            tag_map[_next_tag(tag_map, format_entry)] = len(tag_map)
        elif isinstance(format_entry, list):
            tag_map['l%d' % (_list_index)] = len(tag_map)
            _list_index += 1

    # Parse out the data
    file_data = __parse_data(format_list, data_lines)

    # Return the completed object
    return LuaboundFile(
        header_lines[0], header_lines[1], header_lines[2], header_lines[3], tag_map, file_data
    )


def __parse_format(filename, fmtstr, inlist):
    """
    This function parses the format string from the file and returns a list of tokens. The tokens
        are of the forms:
        > int - the number of punctuation tokens to skip (including whitespace)
        > str - if it is a token, returns the token without the "#"
        > list - a list specifier, as a list of ints or strs as described above

    This function is private and should not be called from outside of this module.

    Args:
        fmtstr (str): The raw format string from the file
        inlist (bool): If the raw format string is from a list specifier
    """
    curr_index = 0
    token_list = []
    while curr_index < len(fmtstr):
        sub_str = fmtstr[curr_index:]

        if sub_str[0] == '#': # An output token
            token_type = sub_str[1]
            if len(sub_str) > 3 and sub_str[3].isalpha():
                token_value = sub_str[2:4]
            elif len(sub_str) > 2 and sub_str[2].isalpha():
                token_value = sub_str[2]
            else:
                raise LuaboundFileLoadError(filename, 'Expected value token at %d' % (curr_index))

            if token_type == 's' and token_value in __SIM_OUTPUT_TOKENS:
                token_list.append('%s%s' % (token_type, token_value))
            elif token_type in ['p', 'd', 'a'] and token_value in __PARTICLE_OUTPUT_TOKENS:
                token_list.append('%s%s' % (token_type, token_value))
            else:
                raise LuaboundFileLoadError(filename, 'The token #%s%s is invalid' %\
                    (token_type, token_value))
            curr_index += (len(token_value) + 2)

        elif sub_str[0] == '{': # Beginning of a list specifier
            if inlist:
                raise LuaboundFileLoadError(filename, 'Cannot specify a list in a list (%d)' %\
                    (curr_index))
            list_index = 1
            while list_index < len(sub_str) and sub_str[list_index] != '}':
                list_index += 1
            if list_index == len(sub_str):
                raise LuaboundFileLoadError(filename, 'Reached end of format string with open list')
            if list_index == 1:
                token_list.append(list())
                curr_index += 2
            else:
                list_str = sub_str[1:list_index]
                token_list.append(__parse_format(filename, list_str, True))
                curr_index += (len(list_str) + 2)

        elif sub_str[0] in __PUNCTUATION_CHARACTERS: # Beginning of punctuation list
            punc_count = 1
            while punc_count < len(sub_str) and sub_str[punc_count] in __PUNCTUATION_CHARACTERS:
                punc_count += 1
            token_list.append(punc_count)
            curr_index += punc_count

        else: # Invalid input
            raise LuaboundFileLoadError(filename, 'Invalid format string input at %d' %\
                (curr_index))

    return token_list


def __parse_data(token_list, data_lines):
    """
    This function takes the raw data lines from the file, and parses them as defined in the provided
        token list. It returns an array of the data, parsed into floats, vectors, or lists.

    This function is private and should not be called from outside of the module.

    Args:
        token_list (list): The list of tokens to parse with, as returned from __parse_format().
        data_lines (list): The list of data lines, raw from the file.

    Returns:
        np.array: Set of data from the file, parsed into the data types.
    """
    # Some values to be used in parsing
    list_count = int(sum(1 for token in token_list if isinstance(token, list)))
    nlist_count = int(sum(1 for token in token_list if isinstance(token, str)))
    token_count = list_count + nlist_count
    list_token_count = int(sum(
        sum(1 for token2 in t_list if isinstance(token2, str))
        for t_list in token_list if isinstance(t_list, list)
    ))

    # Prepare object to store output data
    out_data = np.empty((token_count, len(data_lines)), dtype=object)

    # Function for parsing a single string into the correct datatype
    def _parseStrToData(linenum, token, dvalue):
        token_type = token[1:]
        if token_type in __STR_TOKENS:
            return str(dvalue)
        elif token_type in __VECTOR_TOKENS:
            if dvalue[0:2] != '{{':
                raise Exception('Invalid vector value parsed in line %d' % (linenum))
            vector_vals = str(dvalue).strip('{}').split('|')
            return LbdVector(
                float(vector_vals[0]), float(vector_vals[1]), float(vector_vals[2])
            )
        else:
            return float(dvalue)

    # Iterate over the input data lines and process them one at a time
    for index, data_line in enumerate(data_lines):
        split_data = re.split(__PUNCTUATION_REGEX, data_line)

        # Calculate values related to list lengths
        list_value_count = len(split_data) - nlist_count
        particle_count = list_value_count / list_token_count
        if particle_count != int(particle_count):
            raise Exception('Data line %d did not have integer particle count.' % (index))
        particle_count = int(particle_count)

        # Loop over the tokens, and process the data accordingly
        index2 = 0
        data_index = 0
        for token in token_list:
            if isinstance(token, int):
                continue

            # If the token is a single value
            if isinstance(token, str):
                out_data[index2][index] = _parseStrToData(index, token, split_data[data_index])
                data_index += 1

            # If the token is a list
            else:
                token_only_list = [subtoken for subtoken in token if isinstance(subtoken, str)]
                token_only_len = len(token_only_list)
                sub_data = np.empty((token_only_len, particle_count), dtype=object)
                sub_tagmap = dict()
                for subindex, subtoken in enumerate(token_only_list):
                    sub_tagmap[subtoken] = subindex
                    for subdata_index in range(particle_count):
                        data_index2 = data_index + (subdata_index * token_only_len) + subindex
                        sub_data[subindex][subdata_index] = _parseStrToData(
                            index, subtoken, split_data[data_index2]
                        )
                out_data[index2][index] = LbdList(sub_tagmap, sub_data)
                data_index += (particle_count * token_only_len)

            # Move onto the next destination index
            index2 += 1

    return out_data
