#pragma once

#ifdef BOARDFINDERLIBRARY_EXPORTS
#define BOARDFINDERLIBRARY_API __declspec(dllexport)
#else
#define BOARDFINDERLIBRARY_API __declspec(dllimport)
#endif


extern "C" BOARDFINDERLIBRARY_API void addValueToRollingMax(int value, int* rollingMax, int range);
extern "C" BOARDFINDERLIBRARY_API int findMax(int* values, int minVal, int maxVal);
extern "C" BOARDFINDERLIBRARY_API void filterLines(float* lines, int numberOfLines, float* filteredLines, int maxRange, int peakWidth);