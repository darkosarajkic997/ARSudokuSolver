import ctypes
import numpy as np
import configparser

config = configparser.ConfigParser()
config.read('config.ini')

board_finder_lib=ctypes.CDLL(config['Resources']['dll'])


def filter_lines(lines,number_of_lines):
    filter_function=board_finder_lib.filterLines
    filter_function.argtypes=[np.ctypeslib.ndpointer(image_data_type),ctypes.c_int32,ctypes.c_int32]
