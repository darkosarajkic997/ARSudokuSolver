#pragma once

#ifdef BOARDFINDERLIBRARY_EXPORTS
#define BOARDFINDERLIBRARY_API __declspec(dllexport)
#else
#define BOARDFINDERLIBRARY_API __declspec(dllimport)
#endif


extern "C" BOARDFINDERLIBRARY_API void addValueToRollingMax(int value, int* rollingMax, int range);
extern "C" BOARDFINDERLIBRARY_API int findMax(int* values, int minVal, int maxVal);
extern "C" BOARDFINDERLIBRARY_API int findTwoBiggestClustersOfLines(float* lines, int numberOfLines, float* filteredLines, int maxRange = 5, int peakWidth = 20, int maxLinesSelected = 250);
extern "C" BOARDFINDERLIBRARY_API int selectLinesNearPeak(float* lines, int numberOfLines, int peak, int range, float* selectedLines, int maxLinesSelected);