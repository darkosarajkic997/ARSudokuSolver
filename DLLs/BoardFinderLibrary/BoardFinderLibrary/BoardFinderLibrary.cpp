#include "pch.h"
#include"BoardFinderLibrary.h"

#define _USE_MATH_DEFINES
#include<cmath>
#include<iostream>


void addValueToRollingMax(int value, int* rollingMax, int range)
{
	int minRange, maxRange;
	minRange = value - range;
	maxRange = value + range;

	if (minRange < 0)
	{
		for (int index = 0; index < maxRange; index++)
		{
			rollingMax[index]++;
		}
		for (int index = MAX_ANGLE + minRange; index < MAX_ANGLE; index++)
		{
			rollingMax[index]++;
		}
	}
	else if (maxRange > MAX_ANGLE)
	{
		for (int index = minRange; index < MAX_ANGLE; index++)
		{
			rollingMax[index]++;
		}
		for (int index = 0; index < maxRange - MAX_ANGLE; index++)
		{
			rollingMax[index]++;
		}
	}
	else
	{
		for (int index = minRange; index < maxRange; index++)
		{
			rollingMax[index]++;
		}
	}
}



int findMax(int* values, int minVal, int maxVal)
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


int selectLinesNearPeak(int* lines, int numberOfLines, int peak, int range, int* selectedLines, int maxLinesSelected)
{
	int index, numberOfLinesSelected;
	int minAngle, maxAngle, theta;
	minAngle = (peak - range);
	maxAngle = (peak + range);

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


void findTwoBiggestClustersOfLines(int* lines, int numberOfLines, int rollingRange, int peakWidth, int maxLinesSelected, int* horisontalLines, int* verticalLines, int* numberOfHorisontalLines, int* numberOfVerticalLines)
{
	int* rollingMax = new int[MAX_ANGLE] {0};
	int lineTheta, firstPeak, secondPeak;

	for (int index = 0; index < numberOfLines; index++)
	{
		lineTheta = lines[2 * index + 1];
		addValueToRollingMax(lineTheta, rollingMax, rollingRange);
	}

	firstPeak = findMax(rollingMax);
	if (firstPeak > MAX_ANGLE - peakWidth)
	{
		secondPeak = findMax(rollingMax, firstPeak - MAX_ANGLE, firstPeak - peakWidth);
	}
	else if (firstPeak < peakWidth)
	{
		secondPeak = findMax(rollingMax, firstPeak + peakWidth, MAX_ANGLE - (firstPeak - peakWidth));
	}
	else
	{
		int leftPeak, rightPeak;
		leftPeak = findMax(rollingMax, 0, firstPeak - peakWidth);
		rightPeak = findMax(rollingMax, firstPeak + peakWidth, MAX_ANGLE);
		secondPeak = (rollingMax[leftPeak] > rollingMax[rightPeak]) ? leftPeak : rightPeak;
	}

	delete[] rollingMax;

	*numberOfHorisontalLines = selectLinesNearPeak(lines, numberOfLines, firstPeak, rollingRange, horisontalLines, maxLinesSelected);
	*numberOfVerticalLines = selectLinesNearPeak(lines, numberOfLines, secondPeak, rollingRange, verticalLines, maxLinesSelected);
}
void removeClusterMark(int* clusters, int size, int mark)
{
	for (int index = 0; index < size; index++)
	{
		if (clusters[index] == mark)
		{
			clusters[index] = -1;
		}
	}
}
int linesDBSCAN(int* lines, int numberOfLines, int* clusters, int minClusterSize, int thetaDelta, int rhoDelta)
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
				if (clusters[remainingIndex] == 0)
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


void findAverageForClusteredLines(int* lines, int numberOfLines, int* clusters, int numberOfClusters, float* meanClusteredLines)
{
	int* numberOfElementsPerCluster = new int[numberOfClusters + 1]{ 0 };
	float* rhoTotalPerCluster = new float[numberOfClusters + 1]{ 0.0 };
	float* thetaTotalPerCluster = new float[numberOfClusters + 1]{ 0.0 };
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
		meanClusteredLines[(index - 1) * 2] = (rhoTotalPerCluster[index] / float(numberOfElementsPerCluster[index]));
		meanClusteredLines[(index - 1) * 2 + 1] = (thetaTotalPerCluster[index] / float(numberOfElementsPerCluster[index]));
	}

	delete[] numberOfElementsPerCluster;
	delete[] rhoTotalPerCluster;
	delete[] thetaTotalPerCluster;
}