#include "pch.h"
#include "RANSACHomography.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include<Eigen/Dense>



bool checkIfDetIsZero(float det, float epsilon)
{
	if (det < epsilon)
		return true;
	return false;
}

bool findIntersectionOfTwoIntersectingLines(float rho1, float theta1, float rho2, float theta2, Point *point)
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
	point->x = (rho1 * sinTheta2 - rho2 * sinTheta1) / detA;
	point->y = (-rho1 * cosTheta2 + rho2 * cosTheta1) / detA;

	return true;
}

void findAllIntersectionPoints(float* horizontalLines, int numberOfHorizontalLines, float* verticalLines, int numberOfVerticalLines, Point* points, bool * validPoints)
{
	int pointIndex = 0;
	for (int indexHLine = 0; indexHLine < numberOfHorizontalLines; indexHLine++)
	{
		for (int indexVLine = 0; indexVLine < numberOfVerticalLines; indexVLine++)
		{
			validPoints[pointIndex]=findIntersectionOfTwoIntersectingLines(horizontalLines[indexHLine * 2], horizontalLines[indexHLine * 2 + 1], verticalLines[indexVLine * 2], verticalLines[indexVLine * 2 + 1], &points[pointIndex]);
			pointIndex++;
		}
	}
}
void swapPoints(Point* A, Point* B)
{
	float tmpX, tmpY;
	tmpX = A->x;
	tmpY = A->y;
	A->x = B->x;
	A->y = B->y;
	B->x = tmpX;
	B->y = tmpY;
}

void sortPointsInClockwiseOrder(Point* A, Point* B, Point* C, Point* D)
{
	if (A->y > D->y)
	{
		swapPoints(A, D);
		swapPoints(B, C);
	}
	if (A->x > B->x)
	{
		swapPoints(A, B);
		swapPoints(C, D);
	}

}

void fillEquationMatrix(Eigen::MatrixXf* matrix,Point AOrigin, Point ADestination, int rowIndex)
{
	matrix->coeffRef(rowIndex, 0) = -ADestination.x;
	matrix->coeffRef(rowIndex, 1) = -ADestination.y;
	matrix->coeffRef(rowIndex, 2) = -1;
	matrix->coeffRef(rowIndex, 6) = ADestination.x * AOrigin.x;
	matrix->coeffRef(rowIndex, 7) = ADestination.y * AOrigin.x;
	matrix->coeffRef(rowIndex, 8) = AOrigin.x;
	matrix->coeffRef(rowIndex + 1, 3) = -ADestination.x;
	matrix->coeffRef(rowIndex + 1, 4) = -ADestination.y;
	matrix->coeffRef(rowIndex + 1, 5) = -1;
	matrix->coeffRef(rowIndex + 1, 6) = ADestination.x * AOrigin.x;
	matrix->coeffRef(rowIndex + 1, 7) = ADestination.y * AOrigin.y;
	matrix->coeffRef(rowIndex + 1, 8) = AOrigin.y;
}

void calculateDestinationPoints(Point* A, Point* B, Point* C, Point* D, int distanceX, int distanceY)
{
	A->x = 0;
	A->y = 0;
	B->x = 0;
	B->y = distanceX;
	C->x = distanceX;
	C->y = distanceY;
	D->x = 0;
	D->y = distanceY;
}

Point convertPoint(Point orgPoint, Eigen::MatrixXf* homography)
{
	Point newPoint;
	Eigen::Vector3f point;
	Eigen::Vector3f oldPoint;
	oldPoint << orgPoint.x, orgPoint.y, 1;
	point = homography->colPivHouseholderQr().solve(oldPoint);
	newPoint.x = point(0) / point(2);
	newPoint.y= point(1) / point(2);
	return newPoint;
}



bool RANSACHomography(Point* points, bool* validPoints, int numberOfPoints, Eigen::MatrixXf* homography,Point testPoint, float squareSize, float inliersPercentage, float tolerance)
{
	int inliers = 0;
	float distanceFromTestPointX, distanceFromTestPointY, distanceFromGridX, distanceFromGridY;
	Point tmpPoint;
	int minNumberOfInliers = int(numberOfPoints * inliersPercentage);

	for (int pointIndex = 0; pointIndex < numberOfPoints; pointIndex++)
	{
		if (validPoints[pointIndex])
		{
			tmpPoint = convertPoint(points[pointIndex], homography);
			distanceFromTestPointX = abs(tmpPoint.x - testPoint.x) / squareSize;
			distanceFromTestPointY = abs(tmpPoint.y - testPoint.y) / squareSize;
			distanceFromGridX = abs(round(distanceFromTestPointX) - distanceFromTestPointX);
			distanceFromGridY = abs(round(distanceFromTestPointY) - distanceFromTestPointY);
			if ((distanceFromGridX < distanceFromTestPointX * tolerance) && (distanceFromGridY < distanceFromTestPointY * tolerance))
			{
				inliers++;
				if (inliers > minNumberOfInliers)
				{
					return true;
				}
				else if (minNumberOfInliers - inliers > numberOfPoints - pointIndex)
				{
					return false;
				}
			}
			
		}
	}
	return false;
}

bool calculateHomographyMatrix(Point A, Point B, Point C, Point D, Point* points, bool* validPoints, int numberOfPoints, float* homography, int* sizeX, int* sizeY, float squareSize = 30, float inliersPercentage = 0.5)
{
	Point ADest, BDest, CDest, DDest;
	Eigen::MatrixXf P= Eigen::MatrixXf::Zero(9,9);
	Eigen::VectorXf b= Eigen::VectorXf::Zero(9);
	Eigen::VectorXf homographyVector;
	Eigen::MatrixXf homographyMatrix;
	bool homographyFound=false;
	int distanceX, distanceY;

	P(8, 8) = 1;
	b(8) = 1;

	for (int indexX = 1; indexX < 10; indexX++)
	{
		distanceX = indexX * squareSize;
		for (int indexY = 1; indexY < 10; indexY++)
		{
			distanceY = indexY * squareSize;
			calculateDestinationPoints(&ADest, &BDest, &CDest, &DDest, distanceX, distanceY);
			fillEquationMatrix(&P, A, ADest, 0);
			fillEquationMatrix(&P, B, BDest, 2);
			fillEquationMatrix(&P, C, CDest, 4);
			fillEquationMatrix(&P, D, DDest, 6);
			homographyVector = P.colPivHouseholderQr().solve(b);
			homographyMatrix = homographyVector.reshaped(3, 3).transpose();
			if (RANSACHomography(points, validPoints, numberOfPoints, &homographyMatrix, A, squareSize, inliersPercentage))
			{

			}

		}
	}



	return false;
}
bool findHomogrphyFromFourPoints(int indexA, int indexB, int indexC, int indexD, Point* points, bool* validPoints, int numberOfPoints, float* homographyMatrix,int *sizeX, int *sizeY, float squareSize=30, float inliersPercentage = 0.5)
{
	Point A, B, C, D;
	if (validPoints[indexA] && validPoints[indexB] && validPoints[indexC] && validPoints[indexD])
	{
		A = points[indexA];
		B = points[indexB];
		C = points[indexC];
		D = points[indexD];
		sortPointsInClockwiseOrder(&A, &B, &C, &D);
		if (calculateHomographyMatrix(A, B, C, D,points, validPoints,numberOfPoints, homographyMatrix, sizeX, sizeY, squareSize))
		{
			return true;
		}
	}
	return false;
}


bool findHomograpyFromTwoClustersOfLines(float* horizontalLines, int numberOfHorizontalLines, float* verticalLines, int numberOfVerticalLines, float* homographyMatrix, float inliersPercentage=0.5)
{
	int ACModDelta, numberOfInliers, sizeX, sizeY;
	bool possibleHomography;
	int pointAIndex=0, pointBIndex, pointCIndex, pointDIndex;
	int numberOfIntersections = numberOfHorizontalLines * numberOfVerticalLines;
	Point* intersectionPoints = new Point[numberOfIntersections];
	bool* validIntersections = new bool[numberOfIntersections];
	int squareEdgeSize = 50;

	findAllIntersectionPoints(horizontalLines, numberOfHorizontalLines, verticalLines, numberOfVerticalLines, intersectionPoints, validIntersections);

	while (pointAIndex < numberOfIntersections - numberOfVerticalLines - 1)
	{
		if (pointAIndex % numberOfVerticalLines != numberOfVerticalLines - 1)
		{
			pointCIndex = pointAIndex + numberOfVerticalLines + 1;
			while (pointCIndex < numberOfIntersections)
			{
				ACModDelta = pointCIndex % numberOfVerticalLines - pointAIndex % numberOfVerticalLines;
				if (ACModDelta > 0)
				{
					pointBIndex = pointAIndex + ACModDelta;
					pointDIndex = pointCIndex - ACModDelta;
					if (findHomogrphyFromFourPoints(pointAIndex, pointBIndex, pointCIndex, pointDIndex, intersectionPoints, validIntersections, numberOfIntersections, homographyMatrix, &sizeX, &sizeY, squareEdgeSize))
					{
						return true;
					}

				}
				pointCIndex++;
			}
		}
		pointAIndex++;
	}
	
	delete[] intersectionPoints;
	delete[] validIntersections;

	return false;
}