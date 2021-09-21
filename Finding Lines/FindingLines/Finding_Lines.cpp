#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
# define pi         3.14159265358979323846

using namespace cv;
using namespace std;

int* rhoThetaMatrixInitilization(int rho, int theta)
{
	int* rhoThetaMatrix = new int[rho * theta]();
	return rhoThetaMatrix;
}
int CalculatingDRho(int x, int y, float dthetaInRadians)
{
	int drho = int(x * cos(dthetaInRadians) - y * sin(dthetaInRadians));
	return drho;
}
void IncrementingHoughSpaceMatrixField(int drho, int rho, int* houghSpaceMatrix, int theta, int dtheta)
{
	if ((drho >= 0) && (drho < rho))
		houghSpaceMatrix[theta * drho + dtheta] = houghSpaceMatrix[theta * drho + dtheta] + 1;
}
void GoingThrewAllPossibleValuesOfDTheta(int x, int y, int rho, int theta, int* houghSpaceMatrix)
{
	for (int dtheta = 0; dtheta < 360; dtheta++)
	{
		float dthetaInRadians = dtheta * pi / 180;
		int  drho = CalculatingDRho(x, y, dthetaInRadians);
		IncrementingHoughSpaceMatrixField(drho, rho, houghSpaceMatrix, theta, dtheta);
	}
}
void CalculatingRhoThetaMatrix(unsigned __int8* picture, int width, int height, int rho, int theta, int* houghSpaceMatrix)
{
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			if (picture[y * width + x] == 255)
			{
				GoingThrewAllPossibleValuesOfDTheta(x, y, rho, theta, houghSpaceMatrix);
			}
}

int CalculatingMatrixMaximum(int rho, int theta, int* houghSpaceMatrix)
{
	int matrixMaximum = 0;
	for (int height = 0; height < rho; height++)
		for (int width = 0; width < theta; width++)
			if (houghSpaceMatrix[theta * height + width] > matrixMaximum)
			{
				matrixMaximum = houghSpaceMatrix[height * theta + width];
			}
	return matrixMaximum;
}
void FindingLocalMaximums(int SIZE,int rho, int theta, int* houghSpaceMatrix,int* rhoThetaBeforeClipping,int matrixMaximum, float cuttingFactor = 0.5)
{

	int arrayIncrement = 0;
	for (int height = 0; height < rho; height++)
		for (int width = 0; width < theta; width++)
		{
			if ((houghSpaceMatrix[theta * height + width] > (matrixMaximum * cuttingFactor)) && (arrayIncrement < SIZE))
			{
				rhoThetaBeforeClipping[arrayIncrement] = height;
				rhoThetaBeforeClipping[arrayIncrement + 1] = width;
				arrayIncrement += 2;
			}
		}
}
void AddALineToOutputArray(int* outputArray, int* arrayIncrement, int* rhoThetaBeforeClipping,int index)
{

	outputArray[*arrayIncrement] = rhoThetaBeforeClipping[index];
	outputArray[*(arrayIncrement) + 1] = rhoThetaBeforeClipping[index + 1];
	*arrayIncrement += 2;
}
int TakeTheMiddleLineIndex(int i, int numberOfSimilarLines)
{
	int s = i - numberOfSimilarLines;
	if (s % 2 == 1)
		s = s + 1;
	return s;
}
int* Filtering(int* rhoThetaBeforeClipping,int SIZE=1000,int rhoDifference=10)
{
	int* outputArray = new int[SIZE]();
	int numberOfSimilarLines = 0;
	int arrayIncrement = 0;

	for (int i = 0; i < SIZE; i += 2)
	{
		if (rhoThetaBeforeClipping[i + 2] - rhoThetaBeforeClipping[i] < rhoDifference)
			numberOfSimilarLines++;
		else
		{
			int index = TakeTheMiddleLineIndex(i, numberOfSimilarLines);
			AddALineToOutputArray(outputArray, &arrayIncrement, rhoThetaBeforeClipping, index);
			numberOfSimilarLines = 0;
		}
	}
	delete[] rhoThetaBeforeClipping;
	return outputArray;
}
int* doPicture(unsigned __int8* picture, int width, int height)
{

	int rho = sqrt(pow(width,2)+pow(height,2));
	int theta = 360;

	int* houghSpaceMatrix = new int[rho * theta]();

	CalculatingRhoThetaMatrix(picture, width, height, rho, theta, houghSpaceMatrix);

	int matrixMaximum=CalculatingMatrixMaximum(rho, theta, houghSpaceMatrix);

	int SIZE = 1000;
	int* rhoThetaBeforeClipping = new int[SIZE]();

	FindingLocalMaximums(SIZE,rho, theta, houghSpaceMatrix,rhoThetaBeforeClipping,matrixMaximum);

	delete[] houghSpaceMatrix;
	
	return rhoThetaBeforeClipping;
}

int main(int argc, char** argv)
{
	int rows = 864;
	int cols = 1300;

	String s = "sudoku.jpg";
	Mat_<unsigned __int8> image = imread(s, IMREAD_GRAYSCALE);
	Mat_<unsigned __int8> imageCanny;
	Canny(image, imageCanny, 175, 200);

	unsigned __int8* picture;
	picture = (unsigned __int8*)malloc(rows * cols * sizeof(unsigned __int8));

	for (int i=0;i<rows;i++)
		for (int j = 0; j < cols; j++)
		{
			picture[cols*i+j] = imageCanny[i][j];
		}

	int* outputArrayWithoutFiltering=doPicture(picture, cols, rows);
	int* outputArrayWithFiltering= Filtering(outputArrayWithoutFiltering);

	for (int i = 0; i < 1000; i += 2)
	{
		float a = 1 / tan(outputArrayWithFiltering[i + 1] * pi / 180);
		float b = -outputArrayWithFiltering[i] / sin(outputArrayWithFiltering[i + 1] * pi / 180);
		int x0 = -2000;
		int y0 = (int)(x0 * a + b);
		int x1 = 2000;
		int y1 = (int)(x1 * a + b);
		Point t1(x0, y0);
		Point t2(x1, y1);
		cv::line(imageCanny, t1, t2, Scalar(255), 3, 8, 0);
	}

	imshow("Output", imageCanny);
	waitKey(0);
	return 0;
}