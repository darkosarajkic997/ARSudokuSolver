import ctypes
import numpy as np
import configparser

MAX_ANGLE = 180
MAX_LINES = 500

config = configparser.ConfigParser()
config.read('config.ini')

board_finder_lib = ctypes.CDLL(config['Resources']['dll'])


def add_value_to_rolling_max(value, rolling_max, range, max_angle=MAX_ANGLE):
    """Increment value of rolling max for values near angle

    Args:
        value (integer):  Angle
        rolling_max (numpy array, dtype=np.int32): Array of integers that represent rolling max
        range (integer): Range for which to increment rolling max
        max_angle (integer, optional): Maximum index of rollingMax. Defaults to MAX_ANGLE.
    """
    function = board_finder_lib.addValueToRollingMax
    function.argtypes = [ctypes.c_int32, np.ctypeslib.ndpointer(
        ctypes.c_int32), ctypes.c_int32, ctypes.c_int32]
    function(value, rolling_max, range, max_angle)


def find_max(values, min_val=0, max_val=MAX_ANGLE):
    """Finds max value in array on index range from minVal to maxVal

    Args:
        values (numpy array, dtype=np.int32): Array in which to look for maximum
        min_val (integer, optional):  Minimal index. Defaults to 0.
        max_val (integer, optional): Maximal index. Defaults to MAX_ANGLE.

    Returns:
        (integer): Maximum value
    """
    function = board_finder_lib.findMax
    function.argtypes = [np.ctypeslib.ndpointer(
        ctypes.c_int32), ctypes.c_int32, ctypes.c_int32]
    function.restype = ctypes.c_int32
    return function(values, min_val, max_val)


def select_lines_near_peak(lines, number_of_lines, peak, range, selected_lines, max_lines_selected=MAX_LINES):
    """Select all lines from array near peak angle

    Args:
        lines (numpy array, dtype=np.int32): Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho
        number_of_lines (integer): Number of lines in array
        peak (integer):  Angle that represents peak near which lines are selected
        range (integer): Width of range from which to select lines [peak-range, peak+range]
        selected_lines (numpy array, dtype=np.int32): Lines that are selected, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho
        max_lines_selected (integer, optional): Maximum number of lines that could be selected. Defaults to MAX_LINES.

    Returns:
        (integer): Number of selected lines
    """
    function = board_finder_lib.selectLinesNearPeak
    function.argtypes = [np.ctypeslib.ndpointer(
        ctypes.c_int32), ctypes.c_int32, ctypes.c_int32, np.ctypeslib.ndpointer(ctypes.c_int32), ctypes.c_int32]
    function.restype = ctypes.c_int32
    return function(lines, number_of_lines, peak, range, selected_lines, max_lines_selected)


def find_two_biggest_clusters_of_lines(lines, number_of_lines, rolling_range, peak_width, max_lines_selected, horizontal_lines, vertical_lines):
    """Find two group of lines that represent horizontal and vertical lines

    Args:
        lines (numpy array, dtype=np.int32): Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho
        number_of_lines (integer): Number of lines in array
        rolling_range (integer): Range in witch to increment max
        peak_width (integer): Range of angles from witch to select lines near peak angle
        max_lines_selected (integers): Maximum possible number of lines to be selected
        horizontal_lines (numpy array, dtype=np.int32): Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho
        vertical_lines (numpy array, dtype=np.int32): Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho

    Returns:
        (integer),(integer): Number of "horizontal" lines, NUmber of "vertical" lines
    """
    function = board_finder_lib.findTwoBiggestClustersOfLines
    number_of_horizontal_lines=ctypes.c_int(0)
    number_of_vertical_lines=ctypes.c_int(0)
    function.argtypes = [np.ctypeslib.ndpointer(ctypes.c_int32), ctypes.c_int32, ctypes.c_int32,ctypes.c_int32,ctypes.c_int32, np.ctypeslib.ndpointer(ctypes.c_int32),np.ctypeslib.ndpointer(ctypes.c_int32), ctypes.POINTER(ctypes.c_int),ctypes.POINTER(ctypes.c_int)]
    function(lines, number_of_lines, rolling_range, peak_width, max_lines_selected, horizontal_lines, vertical_lines,ctypes.byref(number_of_horizontal_lines),ctypes.byref(number_of_vertical_lines))
    return number_of_horizontal_lines.value, number_of_vertical_lines.value

def remove_cluster_mark(clusters, size, mark):
    """Removes specific mark of cluster from array that contains cluster marks

    Args:
        clusters (numpy array, dtype=np.int32): Array of cluster marks
        size (integer): Size of cluster mark array
        mark (integer): Mark that need to be removed
    """
    function=board_finder_lib.removeClusterMark
    function.argtypes=[np.ctypeslib.ndpointer(ctypes.c_int32), ctypes.c_int32, ctypes.c_int32]
    function(clusters,size,mark)

def lines_DBSCAN(lines, number_of_lines, clusters, min_clusters=2, theta_delta=6,rho_delta=10):
    """DBSCAN algorithm for lines, finds similar lines and remove outliers

    Args:
        lines (numpy array, dtype=np.int32): Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho
        number_of_lines (integer):  Number of lines in array
        clusters (numpy array, dtype=np.int32):  Array of clusters marks for each lines, outliers have cluster mark -1
        min_clusters (integer, optional): Minimum number of similar lines that forms cluster. Defaults to 2.
        theta_delta (integer, optional): Maximum difference in theta that could be between two similar lines. Defaults to 6.
        rho_delta (integer, optional): Maximum difference in rho that could be between two similar lines. Defaults to 10.

    Returns:
        (integer): Number of clusters
    """
    function=board_finder_lib.linesDBSCAN
    function.argtypes=[np.ctypeslib.ndpointer(ctypes.c_int32), ctypes.c_int32,np.ctypeslib.ndpointer(ctypes.c_int32), ctypes.c_int32, ctypes.c_int32, ctypes.c_int32]
    function.restype = ctypes.c_int32
    return function(lines,number_of_lines,clusters,min_clusters,theta_delta,rho_delta)

def find_average_for_clustered_lines(lines,number_of_lines,clusters, number_of_clusters, mean_clustered_lines):
    """Finds average for every cluster and calculates lines that is mean for each cluster

    Args:
        lines (numpy array, dtype=np.int32): Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho
        number_of_lines (integer):  Number of lines in array
        clusters (numpy array, dtype=np.int32):  Array of clusters marks for each lines, outliers have cluster mark -1
        number_of_clusters (integer): Number of clusters
        mean_clustered_lines (numpy array, dtype=np.single): Array of lines where each line is mean for one cluster, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho
    """
    function=board_finder_lib.findAverageForClusteredLines
    function.argtypes=[np.ctypeslib.ndpointer(ctypes.c_int32), ctypes.c_int32,np.ctypeslib.ndpointer(ctypes.c_int32), ctypes.c_int32,np.ctypeslib.ndpointer(ctypes.c_float)]
    function(lines,number_of_lines,clusters,number_of_clusters,mean_clustered_lines)

def calculate_Hough_space_matrix(picture,width,height,rho,theta,hough_space_matrix):
    """Fills Hough space matrix for image that is preprocessed with edge detector and return maximum value from matrix 

    Args:
        picture (numpy array, dtype=np.uint8):  Array that contains pixel values of image, pixel values are 0 and 255
        width (integer):  Width of image in pixels
        height (integer): Height of image in pixels
        rho (integer): Maximum value of rho
        theta (integer): Maximum value of theta
        hough_space_matrix (numpy array, dtype=np.int32): Array that represents Hough space matrix of size [2*rho,theta]

    Returns:
        (integer): Maximum number of voters from Hough space matrix
    """
    function=board_finder_lib.calculateHoughSpaceMatrix
    function.argtypes=[np.ctypeslib.ndpointer(ctypes.c_uint8),ctypes.c_int32,ctypes.c_int32,ctypes.c_int32,ctypes.c_int32,np.ctypeslib.ndpointer(ctypes.c_int32)]
    function.restype = ctypes.c_int32
    return function(picture,width,height,rho,theta,hough_space_matrix)

def find_lines_above_voters_threshold(hough_space_matrix,rho_max,lines,number_of_voters, max_number_of_lines, theta_max=MAX_ANGLE):
    """Finds lines that have number of voters greater than required

    Args:
        hough_space_matrix (numpy array, dtype=np.int32): Array that represents Hough space matrix
        rho_max (integer): Maximum value of rho
        lines (integer): Array of lines that satisfy condition, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho
        number_of_voters (integer): Minimum number of voters that line need to have to be selected
        max_number_of_lines (integer): Maximum possible number of lines to be selected
        theta_max (integer, optional): Maximum value for theta. Defaults to MAX_ANGLE.

    Returns:
        (integer): Number of lines that are selected
    """
    function=board_finder_lib.findLinesAboveVotersThreshold
    function.argtypes=[np.ctypeslib.ndpointer(ctypes.c_int32),ctypes.c_int32,np.ctypeslib.ndpointer(ctypes.c_int32),ctypes.c_int32,ctypes.c_int32,ctypes.c_int32]
    function.restype = ctypes.c_int32
    return function(hough_space_matrix,rho_max,lines,number_of_voters, max_number_of_lines, theta_max)

def hough_line_detector(picture,width, height, lines,votersThreshold=0.5,max_number_of_lines=MAX_LINES):
    """Finds all lines on image that is preprocessed with edge detector using Hough transform

    Args:
        picture (numpy array, dtype=np.uint8): Array that contains pixel values of image, pixel values are 0 and 255
        width (integer):  Width of image
        height (integer): Height of image
        lines (integer): Array of lines that have been found, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho
        votersThreshold (float, optional):  Percentage of maximum number of voters needed for line to be selected. Defaults to 0.5.
        max_number_of_lines (integer, optional): Maximum possible number of lines to be selected. Defaults to MAX_LINES.

    Returns:
        (integer): Number of lines that are selected
    """
    function=board_finder_lib.houghLineDetector
    function.argtypes=[np.ctypeslib.ndpointer(ctypes.c_uint8),ctypes.c_int32,ctypes.c_int32,np.ctypeslib.ndpointer(ctypes.c_int32),ctypes.c_float,ctypes.c_int32]
    function.restype = ctypes.c_int32
    return function(picture,width, height, lines,votersThreshold,max_number_of_lines)



