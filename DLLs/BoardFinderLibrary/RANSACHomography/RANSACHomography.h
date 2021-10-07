#pragma once
#include<Eigen/Dense>

#ifdef RANSACHOMOGRAPHY_EXPORTS
#define RANSACHOMOGRAPHY_API __declspec(dllexport)
#else
#define RANSACHOMOGRAPHY_API __declspec(dllimport)
#endif

struct Point {
	float x;
	float y;
};

extern "C" RANSACHOMOGRAPHY_API bool checkIfDetIsZero(float det, float epsilon = 0.00005);

extern "C" RANSACHOMOGRAPHY_API bool findIntersectionOfTwoIntersectingLines(float rho1, float theta1, float rho2, float theta2, Point * point);

extern "C" RANSACHOMOGRAPHY_API void findAllIntersectionPoints(float* horizontalLines, int numberOfHorizontalLines, float* verticalLines, int numberOfVerticalLines, Point * points, bool* validPoints);

extern "C" RANSACHOMOGRAPHY_API void swapPoints(Point * A, Point * B);

extern "C" RANSACHOMOGRAPHY_API void sortPointsInClockwiseOrder(Point * A, Point * B, Point * C, Point * D);

extern "C" RANSACHOMOGRAPHY_API void fillEquationMatrix(Eigen::MatrixXf * matrix, Point AOrigin, Point ADestination, int rowIndex);

extern "C" RANSACHOMOGRAPHY_API void calculateDestinationPoints(Point * A, Point * B, Point * C, Point * D, int distanceX, int distanceY);

extern "C" RANSACHOMOGRAPHY_API Point convertPoint(Point orgPoint, Eigen::MatrixXf * homography);

extern "C" RANSACHOMOGRAPHY_API bool RANSACHomography(Point * points, bool* validPoints, int numberOfPoints, Eigen::MatrixXf * homography, Point testPoint, float squareSize = 30, float inliersPercentage = 0.5, float tolerance = 0.05);

extern "C" RANSACHOMOGRAPHY_API bool calculateHomographyMatrix(Point A, Point B, Point C, Point D, Point * points, bool* validPoints, int numberOfPoints, float* homography, int* sizeX, int* sizeY, float squareSize = 30, float inliersPercentage = 0.5);

extern "C" RANSACHOMOGRAPHY_API bool findHomogrphyFromFourPoints(int indexA, int indexB, int indexC, int indexD, Point * points, bool* validPoints, int numberOfPoints, float* homographyMatrix, int* sizeX, int* sizeY, float squareSize = 30, float inliersPercentage = 0.5);

extern "C" RANSACHOMOGRAPHY_API

extern "C" RANSACHOMOGRAPHY_API

