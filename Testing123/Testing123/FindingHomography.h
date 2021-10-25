#pragma once
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <vector>

# define pi         3.14159265358979323846

using namespace cv;
using namespace std;

class FindingHomography {
	int* rhoThetaMatrixInitilization(int rho, int theta);
	float CalculatingDRho(int x, int y, float dthetaInRadians);
	void IncrementingHoughSpaceMatrixField(int drho, int rho, int* houghSpaceMatrix, int theta, int dtheta);
	void GoingThrewAllPossibleValuesOfDTheta2(int x, int y, int rho, int theta, int* houghSpaceMatrix);
	void GoingThrewAllPossibleValuesOfDTheta(int x, int y, int rho, int theta, int* houghSpaceMatrix);
	void CalculatingRhoThetaMatrix(unsigned __int8* picture, int width, int height, int rho, int theta, int* houghSpaceMatrix);
	int CalculatingMatrixMaximum(int rho, int theta, int* houghSpaceMatrix);
	std::vector<int> FindingLocalMaximums(int rho, int theta, int* houghSpaceMatrix, int matrixMaximum, float cuttingFactor = 0.5);
	void AddALineToOutputArray(int* outputArray, int* arrayIncrement, int* rhoThetaBeforeClipping, int index);
	int TakeTheMiddleLineIndex(int i, int numberOfSimilarLines);
	std::vector<int> Filtering(std::vector<int> rhoThetaBeforeClipping, int rhoDifference = 15, int thetaDifference = 5);
	std::vector<int> doPicture(unsigned __int8* picture, int width, int height);
	cv::Mat CalculateHomography(std::vector<Point2f> a, std::vector<Point2f> b);
	Point2f CalculatePoint(int rho_1, int theta_1, int rho_2, int theta_2);
	void sort(std::vector<int> array);
	std::vector<Point2f> CalculateIntersections(std::vector<int> niz1, std::vector<int> niz2);
	float distance(Point2f p1, Point2f p2);
	int CalculateNumOfInliers(std::vector<Point2f> vec, cv::Mat homography, float tolerance);
	void swap(int* x, int* y);
	void DrawLines(Mat image, std::vector<int> array1);
	void DrawLines2(Mat image, std::vector<int> array1);
	Mat to_Mat(Point2f p);
	int BelongsToGrid(Mat X_warped, Mat P1_warped, float x_distance, float tolerance, float belongs_to_picture_tolerance = 5);
	int CheckMissingVerticalEdges(Mat imageCannied, float x_distance, float left_most_x, float right_most_x, int tolerance = 5);
	int CheckMissingHorizontalEdges(Mat imageCannied, float x_distance, float upper_most_y, float lower_most_y, int tolerance = 5);
	std::vector<Point2f> IntersectionPoints(vector<int> arrayHor, vector<int> arrayVer);
	Mat MoveBoard(std::vector<Point2f> src_array, Mat H, float x_distance, int* gridPosArray, int margin = 6);
	std::vector<float> Lowest_X_Biggest_X(Mat P1, Mat P2, Mat imageCannied, float x_distance, std::vector<float> edgeArray);
	std::vector<float> Lowest_Y_Biggest_Y(Mat P1, Mat P2, Mat imageCannied, float x_distance, std::vector<float> edgeArray);
	std::vector<Point2f> FindSudokuBoardCorners(Mat warpedImage, Mat movedHomography, std::vector<Point2f> array1, float x_distance);
	Mat AdjustingHomography(Mat image, Mat H, vector<int> arrayHor, vector<int> arrayVer, float x_distance, int scale, int* gridPosArray);
	Mat FindingHomographyFunc(Mat image, std::vector<int> arrayHor, std::vector<int> arrayVer, int scale = 100, float tolerance = 0.05);

};