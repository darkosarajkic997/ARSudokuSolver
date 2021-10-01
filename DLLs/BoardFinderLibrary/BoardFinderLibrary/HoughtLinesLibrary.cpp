#include "pch.h"
#include"BoardFinderLibrary.h"

#define _USE_MATH_DEFINES
#include<cmath>


void getSinTable(float* sinTable, int maxAngle)
{
	for (int angle = 0; angle < maxAngle; angle++)
		sinTable[angle] = sin(angle * M_PI / 180);
}

void getCosTable(float* cosTable, int maxAngle)
{
	for (int angle = 0; angle < maxAngle; angle++)
		cosTable[angle] = cos(angle * M_PI / 180);
}


int calculateHoughSpaceMatrix(unsigned __int8* picture, int width, int height, int rho, int theta, int* houghSpaceMatrix)
{
	int currentValue, drho;
	float* cosTable, * sinTable;
	int houghSpaceMax = 0;

	cosTable = new float[MAX_ANGLE];
	sinTable = new float[MAX_ANGLE];
	getCosTable(cosTable);
	getSinTable(sinTable);

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			if (picture[y * width + x] > 0)
			{
				for (int dtheta = 0; dtheta < theta; dtheta++)
				{
					drho = int(float(x) * cosTable[dtheta] + float(y) * sinTable[dtheta]) + rho;
					currentValue = ++houghSpaceMatrix[theta * drho + dtheta];
					if (currentValue > houghSpaceMax)
					{
						houghSpaceMax = currentValue;
					}
				}
			}
	delete[] cosTable;
	delete[] sinTable;
	return houghSpaceMax;
}

int findLinesAboveVotersThreshold(int* houghSpaceMatrix, int rhoMax, int* lines, int numberOfVoters, int maxNumberOfLines, int thetaMax)
{

	int lineIndex = 0;
	for (int rho = 0; rho < 2 * rhoMax; rho++)
		for (int theta = 0; theta < thetaMax; theta++)
		{
			if ((houghSpaceMatrix[thetaMax * rho + theta] > numberOfVoters) && (lineIndex < maxNumberOfLines))
			{
				lines[lineIndex * 2 + 1] = theta;
				lines[lineIndex * 2] = rho - rhoMax;
				lineIndex++;
			}
		}
	return lineIndex;
}

int houghLineDetector(unsigned __int8* picture, int width, int height, int* lines, float votersThreshold, int maxNumberOfLines)
{
	int maxHoughSpace, numberOfVoters, numberOfLines;
	int rho = height + width;
	int* houghSpaceMatrix = new int[2 * rho * MAX_ANGLE]{ 0 };
	maxHoughSpace = calculateHoughSpaceMatrix(picture, width, height, rho, MAX_ANGLE, houghSpaceMatrix);
	numberOfVoters = maxHoughSpace * votersThreshold;
	numberOfLines = findLinesAboveVotersThreshold(houghSpaceMatrix, rho, lines, numberOfVoters, maxNumberOfLines);

	delete[] houghSpaceMatrix;

	return numberOfLines;
}
