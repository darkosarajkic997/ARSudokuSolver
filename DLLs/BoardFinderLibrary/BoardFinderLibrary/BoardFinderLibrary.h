#pragma once

#ifdef BOARDFINDERLIBRARY_EXPORTS
#define BOARDFINDERLIBRARY_API __declspec(dllexport)
#else
#define BOARDFINDERLIBRARY_API __declspec(dllimport)
#endif

#define MAX_ANGLE 180
#define MAX_LINES 500

extern "C" BOARDFINDERLIBRARY_API void addValueToRollingMax(int value, int* rollingMax, int range);
extern "C" BOARDFINDERLIBRARY_API int findMax(int* values, int minVal = 0, int maxVal = MAX_ANGLE);
extern "C" BOARDFINDERLIBRARY_API int selectLinesNearPeak(int* lines, int numberOfLines, int peak, int range, int* selectedLines, int maxLinesSelected = MAX_LINES);
extern "C" BOARDFINDERLIBRARY_API void findTwoBiggestClustersOfLines(int* lines, int numberOfLines, int rollingRange, int peakWidth, int maxLinesSelected, int* horisontalLines, int* verticalLines, int* numberOfHorisontalLines, int* numberOfVerticalLines);
extern "C" BOARDFINDERLIBRARY_API void removeClusterMark(int* clusters, int size, int mark);
extern "C" BOARDFINDERLIBRARY_API int linesDBSCAN(int* lines, int numberOfLines, int* clusters, int minClusterSize = 2, int thetaDelta = 5, int rhoDelta = 10);
extern "C" BOARDFINDERLIBRARY_API void findAverageForClusteredLines(int* lines, int numberOfLines, int* clusters, int numberOfClusters, float* meanClusteredLines);

extern "C" BOARDFINDERLIBRARY_API void getSinTable(float* sinTable, int maxAngle = MAX_ANGLE);
extern "C" BOARDFINDERLIBRARY_API void getCosTable(float* cosTable, int maxAngle = MAX_ANGLE);
extern "C" BOARDFINDERLIBRARY_API int CalculatingRhoThetaMatrix(unsigned __int8* picture, int width, int height, int rho, int theta, int* houghSpaceMatrix);
extern "C" BOARDFINDERLIBRARY_API int FindingLocalMaximums(int* houghSpaceMatrix, int rhoMax, int* lines, int numberOfVoters, int maxNumberOfLines, int thetaMax = MAX_ANGLE);
extern "C" BOARDFINDERLIBRARY_API int houghLineDetector(unsigned __int8* picture, int width, int height, int* lines, float votersThreshold = 0.5, int maxNumberOfLines = MAX_LINES);
