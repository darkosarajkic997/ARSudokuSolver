#include "pch.h"
#include"BoardFinderLibrary.h"

#define _USE_MATH_DEFINES
#include<cmath>
#include<iostream>

#define MAX_ANGLE 90


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

int findMax(int* values, int minVal = 0, int maxVal = 90)
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


int selectLinesNearPeak(float* lines, int numberOfLines, int peak, int range, float* selectedLines, int maxLinesSelected)
{
	int index, numberOfLinesSelected;
	float minAngle, maxAngle, theta;
	minAngle = (peak - range) * (M_PI / 180);
	maxAngle = (peak + range) * (M_PI / 180);

	index = 0;
	numberOfLinesSelected = 0;
	while (numberOfLinesSelected < maxLinesSelected && index < numberOfLines)
	{
		theta = lines[index * 2 + 1];
		if (theta >= minAngle && theta <= maxAngle)
		{
			selectedLines[numberOfLinesSelected * 2] = lines[index * 2];
			selectedLines[numberOfLinesSelected * 2 + 1] = theta;
			numberOfLinesSelected++;
		}
		index++;
	}

	return numberOfLinesSelected;
}


int findTwoBiggestClustersOfLines(float* lines, int numberOfLines, int maxRange, int peakWidth, int maxLinesSelected, float* horisontalLines, float* verticalLines)
{
	int* rollingMax = new int[MAX_ANGLE];
	int lineTheta, firstPeak, secondPeak, numberOfHorisontalLines, numberOfVerticalLines;
	int maxLinesInCluster = 100;
	int angleDelta = 5;

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
		secondPeak = findMax(rollingMax, firstPeak + peakWidth, MAX_ANGLE);
	}
	else
	{
		int leftPeak, rightPeak;
		leftPeak = findMax(rollingMax, 0, firstPeak - peakWidth);
		rightPeak = findMax(rollingMax, firstPeak + peakWidth, MAX_ANGLE);
		secondPeak = (rollingMax[leftPeak] > rollingMax[rightPeak]) ? leftPeak : rightPeak;
	}

	delete[] rollingMax;

	numberOfHorisontalLines = selectLinesNearPeak(lines, numberOfLines, firstPeak, maxRange, horisontalLines, maxLinesSelected);
	numberOfVerticalLines = selectLinesNearPeak(lines, numberOfLines, secondPeak, maxRange, verticalLines, maxLinesSelected);
}
void removeClusterMark(int* clusters, int size, int mark)
{
	for (int index = 0; index < size; index++)
	{
		if (clusters[index] == mark)
		{
			clusters[index] == -1;
		}
	}
}
int LinesDBSCAN(float* lines, int numberOfLines, int* clusters, float thetaDelta = 2.0, float rhoDelta = 10.0, int minClusterSize = 2)
{
	int index = 0;
	int currentCluster = 0;
	int linesInCurrentCluster;
	float minRho, maxRho, minTheta, maxTheta, currentRho, currentTheta;

	while (index < numberOfLines)
	{
		if (clusters[index] == 0)
		{
			clusters[index] = ++currentCluster;
			minRho = lines[index * 2] - rhoDelta;
			maxRho = minRho + 2 * rhoDelta;
			minTheta = lines[index * 2 + 1] - thetaDelta;
			maxTheta = minTheta + 2 * thetaDelta;
			linesInCurrentCluster = 1;

			for (int remainingIndex = index + 1; remainingIndex < numberOfLines; remainingIndex++)
			{

				currentRho = lines[remainingIndex * 2];
				currentTheta = lines[remainingIndex * 2 + 1];
				if (currentRho > minRho && currentRho < maxRho && currentTheta>minTheta && currentTheta < maxTheta)
				{
					clusters[remainingIndex] = currentCluster;
					minRho = min(minRho, currentRho - rhoDelta);
					maxRho = max(maxRho, maxRho + rhoDelta);
					minTheta = min(minTheta, currentTheta - thetaDelta);
					maxTheta = max(maxTheta, currentTheta + thetaDelta);
					++linesInCurrentCluster;
				}
			}

			if (linesInCurrentCluster < minClusterSize)
			{
				removeClusterMark(clusters, numberOfLines, currentCluster--);
			}
		}
		++index;
	}

	return currentCluster;
}


int findAverageForClusteredLines(float* lines, int numberOfLines, int* clusters, int numberOfClusters)
{
	int* numberOfElementsPerCluster = new int[numberOfClusters + 1]{0};
	float* rhoTotalPerCluster = new float[numberOfClusters + 1]{0};
	float* thetaTotalPerCluster = new float[numberOfClusters + 1]{0};
	float currentRho, currentTheta;
	int cluster;
	
	for (int index = 0; index < numberOfLines; index++)
	{
		currentRho = lines[index * 2];
		currentTheta = lines[index * 2 + 1];
		cluster = clusters[index];
		if (cluster > 0)
		{
			numberOfElementsPerCluster[cluster]++;
			rhoTotalPerCluster[cluster] += currentRho;
			thetaTotalPerCluster[cluster] += currentTheta;
		}
	}

	for (int index = 1; index <= numberOfClusters; index++)
	{
		rhoTotalPerCluster[index] /= numberOfElementsPerCluster[index];
		thetaTotalPerCluster[index] /= numberOfElementsPerCluster[index];
	}


	delete[] numberOfElementsPerCluster;
	delete[] rhoTotalPerCluster;
	delete[] thetaTotalPerCluster;
}