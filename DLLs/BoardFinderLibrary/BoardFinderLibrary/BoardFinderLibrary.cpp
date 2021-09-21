#include "pch.h"
#include"BoardFinderLibrary.h"

#define _USE_MATH_DEFINES
#include<cmath>

#include<iostream>

#define MAX_ANGLE 90

/// <summary>
/// Increment rolling max on range max(0,value-range) to min(value+range,90)  
/// </summary>
/// <param name="value"> 'angle value'</param>
/// <param name="rollingMax"> 'array that holds rolling max'</param>
/// <param name="range"> 'range width'</param>
void addValueToRollingMax(int value, int* rollingMax, int range)
{
	int minRange, maxRange;
	minRange = max(0, value - range);
	maxRange = min(MAX_ANGLE - 1, value + range);

	for (int index = minRange; index < maxRange; index++)
	{
		rollingMax[index]++;
	}
}

/// <summary>
/// Finds max value in array between min and max index
/// </summary>
/// <param name="values"> 'array'</param>
/// <param name="min"> 'min index'</param>
/// <param name="max"> 'max index'</param>
/// <returns>index of maximum element</returns>
int findMax(int* values, int minVal=0, int maxVal=90)
{
	int maxValue = 0;
	int maxIndex = minVal;
	for (int index = minVal; index < maxVal; index++)
	{
		if (maxValue < values[index])
		{
			maxValue = values[index];
			maxIndex = index;
		}
	}
	return maxIndex;
}

/// <summary>
/// Select lines from array that are in around peak in angle range from (peak-range) to (peak+range)
/// </summary>
/// <param name="lines">array of lines</param>
/// <param name="numberOfLines">number of lines from which to select</param>
/// <param name="maxLines">maximum number of selected lines, max number is 500</param>
/// <param name="angle">angle in deg</param>
/// <param name="range">angle range in which to look for lines</param>
/// <param name="selectedLines">selected lines</param>
void selectLines(float* lines,int numberOfLines, int maxLinesSelected, int angle, int range, float* selectedLines)
{
	int index, numberOfLinesSelected;
	float minAngle, maxAngle, theta;
	minAngle = (angle - range) * (M_PI / 180);
	maxAngle= (angle + range) * (M_PI / 180);

	index = 0;
	numberOfLinesSelected = 0;
	while (numberOfLinesSelected < maxLinesSelected && index < numberOfLines)
	{
		theta = lines[index * 2 + 1];
		if (theta >= minAngle && theta <= maxAngle)
		{
			selectedLines[numberOfLinesSelected*2] = lines[index * 2];
			selectedLines[numberOfLinesSelected*2 + 1] = theta;
			numberOfLinesSelected++;
		}
		index++;
	}
}

/// <summary>
/// Divide lines into two "clusters" and remove outliers by angle from their polar coordinate representation
/// </summary>
/// <param name="lines"> 'array of lines given by rho and theta'</param>
/// <param name="numberOfLines"> 'number of lines in array'</param>
/// <param name="filteredLines"> 'filtered lines'</param>
/// <param name="maxRange"> 'range of angles in which to look for similar lines'</param>
/// <param name="peakWidth"> 'range of angles in which not to look for second group of lines'</param>
void filterLines(float* lines, int numberOfLines, float* filteredLines, int maxRange = 5, int peakWidth=20)
{
	int* rollingMax=new int[MAX_ANGLE];
	int lineTheta,firstPeak,secondPeak;

	for (int index = 0; index < numberOfLines; index++)
	{
		lineTheta = int(lines[2 * index + 1] * (180 / M_PI));
		if (lineTheta > MAX_ANGLE)
		{
			lineTheta = MAX_ANGLE - (lineTheta % MAX_ANGLE);
		}
		addValueToRollingMax(lineTheta, rollingMax, maxRange);
	}

	firstPeak = findMax(rollingMax);
	if (firstPeak > MAX_ANGLE - peakWidth)
	{
		secondPeak = findMax(rollingMax, 0, firstPeak - peakWidth);
	}
	else if (firstPeak < MAX_ANGLE)
	{
		secondPeak = findMax(rollingMax, firstPeak+peakWidth, MAX_ANGLE);
	}
	else
	{
		int leftPeak, rightPeak;
		leftPeak = findMax(rollingMax, 0, firstPeak - peakWidth);
		rightPeak = findMax(rollingMax, firstPeak + peakWidth, MAX_ANGLE);
		secondPeak = (rollingMax[leftPeak] > rollingMax[rightPeak]) ? leftPeak : rightPeak;
	}

	delete[] rollingMax;
}