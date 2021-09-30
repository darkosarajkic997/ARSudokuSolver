#pragma once

#ifdef BOARDFINDERLIBRARY_EXPORTS
#define BOARDFINDERLIBRARY_API __declspec(dllexport)
#else
#define BOARDFINDERLIBRARY_API __declspec(dllimport)
#endif

#define MAX_ANGLE 180
#define MAX_LINES 500

/// <summary>
/// Increment value for rolling max for values near angle
/// </summary>
/// <param name="value"> -> Angle </param>
/// <param name="rollingMax"> -> Array of integers that represent rolling max</param>
/// <param name="range"> -> Range for which to increment rolling max [angle-range,angle+range]</param>
/// <returns></returns>
extern "C" BOARDFINDERLIBRARY_API void addValueToRollingMax(int value, int* rollingMax, int range);
/// <summary>
/// Finds max value in array on index range from minVal to maxVal
/// </summary>
/// <param name="values"> -> Array in which to look for maximum </param>
/// <param name="minVal"> -> Minimal index </param>
/// <param name="maxVal"> -> Maximal index </param>
/// <returns>Maximum value</returns>
extern "C" BOARDFINDERLIBRARY_API int findMax(int* values, int minVal = 0, int maxVal = MAX_ANGLE);
/// <summary>
/// Select all lines from array near peak angle
/// </summary>
/// <param name="lines"> -> Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho</param>
/// <param name="numberOfLines"> -> Number of lines in array</param>
/// <param name="peak"> -> Angle which represents the peak near which lines are selected</param>
/// <param name="range"> -> Width of range from which to select lines [peak-range, peak+range]</param>
/// <param name="selectedLines"> -> Lines that are selected, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho</param>
/// <param name="maxLinesSelected"> -> Maximum number of lines that could be selected</param>
/// <returns>Number of selected lines</returns>
extern "C" BOARDFINDERLIBRARY_API int selectLinesNearPeak(int* lines, int numberOfLines, int peak, int range, int* selectedLines, int maxLinesSelected = MAX_LINES);
/// <summary>
/// Find two group of lines that represent horizontal and vertical lines
/// </summary>
/// <param name="lines"> -> Lines array, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho</param>
/// <param name="numberOfLines"> -> Number of lines</param>
/// <param name="rollingRange"> -> Range in witch to increment max</param>
/// <param name="peakWidth"> -> Range of angles from witch to select lines near peak angle</param>
/// <param name="maxLinesSelected"> -> Maximum possible number of lines to be selected</param>
/// <param name="horizontalLines"> -> Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho</param>
/// <param name="verticalLines"> -> Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho</param>
/// <param name="numberOfHorizontalLines"> -> Number of lines in horizontalLines</param>
/// <param name="numberOfVerticalLines"> -> Number of lines in verticalLines</param>
/// <returns></returns>
extern "C" BOARDFINDERLIBRARY_API void findTwoBiggestClustersOfLines(int* lines, int numberOfLines, int rollingRange, int peakWidth, int maxLinesSelected, int* horizontalLines, int* verticalLines, int* numberOfHorizontalLines, int* numberOfVerticalLines);
/// <summary>
/// Removes specific mark of cluster from array that contains cluster marks 
/// </summary>
/// <param name="clusters"> -> Array of cluster marks</param>
/// <param name="size"> -> Size of cluster mark array</param>
/// <param name="mark"> -> Mark that need to be removed</param>
/// <returns></returns>
extern "C" BOARDFINDERLIBRARY_API void removeClusterMark(int* clusters, int size, int mark);
/// <summary>
/// DBSCAN algorithm for lines, finds similar lines and remove outliers
/// </summary>
/// <param name="lines"> -> Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho</param>
/// <param name="numberOfLines"> -> Number of lines in array</param>
/// <param name="clusters"> -> Array of clusters marks for each lines, outliers have cluster mark -1</param>
/// <param name="minClusterSize"> -> Minimum number of similar lines that forms cluster</param>
/// <param name="thetaDelta"> -> Maximum difference in theta that could be between two similar lines </param>
/// <param name="rhoDelta"> -> Maximum difference in rho that could be between two similar lines</param>
/// <returns> Number of clusters</returns>
extern "C" BOARDFINDERLIBRARY_API int linesDBSCAN(int* lines, int numberOfLines, int* clusters, int minClusterSize = 2, int thetaDelta = 6, int rhoDelta = 10);
/// <summary>
/// Finds average for every cluster and calculates lines that is mean for each cluster
/// </summary>
/// <param name="lines"> -> Array of lines, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho</param>
/// <param name="numberOfLines"> -> Number of lines in array</param>
/// <param name="clusters"> -> Array of clusters marks for each lines, outliers have cluster mark -1</param>
/// <param name="numberOfClusters"> -> Number of clusters</param>
/// <param name="meanClusteredLines"> ->Array of lines where each line is mean for one cluster, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho</param>
/// <returns></returns>
extern "C" BOARDFINDERLIBRARY_API void findAverageForClusteredLines(int* lines, int numberOfLines, int* clusters, int numberOfClusters, float* meanClusteredLines);
//extern "C" BOARDFINDERLIBRARY_API void findhorizontalAndVerticalLines(int* lines, int numberOfLines, int* numberOfhorizontalLines, int* numberOfVerticalLines, float* horizontalLines, float* verticalLines, int rollingRange=10, int peakWidth=30);

/// <summary>
/// Creates table of sine values for each angle in range [0,maxAngle] in increments of 1 degree
/// </summary>
/// <param name="sinTable"> -> Array that contains table of sine values</param>
/// <param name="maxAngle"> -> Maximum value for which to calculate sine </param>
/// <returns></returns>
extern "C" BOARDFINDERLIBRARY_API void getSinTable(float* sinTable, int maxAngle = MAX_ANGLE);
/// <summary>
/// Creates table of cosine values for each angle in range [0,maxAngle] in increments of 1 degree
/// </summary>
/// <param name="cosTable"> -> Array that contains table of cosine values</param>
/// <param name="maxAngle"> -> Maximum value for which to calculate cosine</param>
/// <returns></returns>
extern "C" BOARDFINDERLIBRARY_API void getCosTable(float* cosTable, int maxAngle = MAX_ANGLE);
/// <summary>
/// Fills Hough space matrix for image that is preprocessed with edge detector and return maximum value from matrix 
/// </summary>
/// <param name="picture"> -> Array that contains pixel values of image, pixel values are 0 and 255</param>
/// <param name="width"> -> Width of image in pixels</param>
/// <param name="height"> -> Height of image in pixels </param>
/// <param name="rho"> -> Maximum value of rho</param>
/// <param name="theta"> -> Maximum value of theta</param>
/// <param name="houghSpaceMatrix"> -> Array that represents Hough space matrix of size [2*rho,theta]</param>
/// <returns>Maximum number of voters from Hough space matrix</returns>
extern "C" BOARDFINDERLIBRARY_API int CalculatingRhoThetaMatrix(unsigned __int8* picture, int width, int height, int rho, int theta, int* houghSpaceMatrix);
/// <summary>
/// Finds lines that have number of voters greater than required
/// </summary>
/// <param name="houghSpaceMatrix"> -> Array that represents Hough space matrix</param>
/// <param name="rhoMax"> -> Maximum value of rho</param>
/// <param name="lines"> -> Array of lines that satisfy condition, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho</param>
/// <param name="numberOfVoters"> -> Minimum number of voters that line need to have to be selected</param>
/// <param name="maxNumberOfLines"> -> Maximum possible number of lines to be selected</param>
/// <param name="thetaMax"> -> Maximum value for theta</param>
/// <returns>Number of lines that are selected</returns>
extern "C" BOARDFINDERLIBRARY_API int FindingLocalMaximums(int* houghSpaceMatrix, int rhoMax, int* lines, int numberOfVoters, int maxNumberOfLines, int thetaMax = MAX_ANGLE);
/// <summary>
/// Finds all lines on image that is preprocessed with edge detector using Hough transform
/// </summary>
/// <param name="picture"> -> Array that contains pixel values of image, pixel values are 0 and 255</param>
/// <param name="width"> -> Width of image</param>
/// <param name="height"> -> Height of image</param>
/// <param name="lines"> -> Array of lines that have been found, one line is represented with two adjacent elements in array where first one is theta in degrees and second one is rho </param>
/// <param name="votersThreshold"> -> Percentage of maximum number of voters needed for line to be selected</param>
/// <param name="maxNumberOfLines"> -> Maximum possible number of lines to be selected</param>
/// <returns>Number of lines that are selected</returns>
extern "C" BOARDFINDERLIBRARY_API int houghLineDetector(unsigned __int8* picture, int width, int height, int* lines, float votersThreshold = 0.5, int maxNumberOfLines = MAX_LINES);
