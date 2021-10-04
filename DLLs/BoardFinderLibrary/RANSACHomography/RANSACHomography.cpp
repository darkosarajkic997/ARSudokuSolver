#include "pch.h"
#include "RANSACHomography.h"
#define _USE_MATH_DEFINES
#include <cmath>

bool checkIfDetIsZero(float det, float epsilon = 0.00005)
{
	if (det < epsilon)
		return true;
	return false;
}

bool findIntersectionOfTwoIntersectingLines(float rho1, float theta1, float rho2, float theta2, float* x, float* y)
{
	float sinTheta1, sinTheta2, cosTheta1, cosTheta2, detA;
	sinTheta1 = sin(theta1);
	sinTheta2 = sin(theta2);
	cosTheta1 = cos(theta1);
	cosTheta2 = cos(theta2);
	detA = cosTheta1 * sinTheta2 - sinTheta1 * cosTheta1;
	if (checkIfDetIsZero(detA))
	{
		return false;
	}
	*x = (rho1 * sinTheta2 - rho2 * sinTheta1) / detA;
	*y = (-rho1 * cosTheta2 + rho2 * cosTheta1) / detA;

	return true;
}

void findHomograpyFromTwoLinesClusters(float* horizontalLines, int numberOfHorizontalLines, float* verticalLines, int numberOfVerticalLines, float* homographyMatrix)
{
	
}