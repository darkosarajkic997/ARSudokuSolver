//#include <iostream>
//#define _USE_MATH_DEFINES
//#include<cmath>
//#include <opencv2/opencv.hpp>
//#include <string>
//
//
//#define THETA_MAX 180
//#define MAX_LINES 500
//
//void getSinTable(float* sinTable, int maxAngle = THETA_MAX)
//{
//	for (int angle = 0; angle < maxAngle; angle++)
//		sinTable[angle] = sin(angle * M_PI / 180);
//}
//
//void getCosTable(float* cosTable, int maxAngle = THETA_MAX)
//{
//	for (int angle = 0; angle < maxAngle; angle++)
//		cosTable[angle] = cos(angle * M_PI / 180);
//}
//
//int CalculatingRhoThetaMatrix(unsigned __int8* picture, int width, int height, int rhoMax, int* houghSpaceMatrix)
//{
//	int  currentValue, drho;
//	float* cosTable, * sinTable;
//	int houghSpaceMax = 0;
//	cosTable = new float[THETA_MAX];
//	sinTable = new float[THETA_MAX];
//	getCosTable(cosTable);
//	getSinTable(sinTable);
//	for (int y = 0; y < height; y++)
//		for (int x = 0; x < width; x++)
//			if (picture[y * width + x] == 255)
//			{
//				for (int dtheta = 0; dtheta < THETA_MAX; dtheta++)
//				{
//					drho = int(float(x) * cosTable[dtheta] + float(y) * sinTable[dtheta])+rhoMax;
//					currentValue=++houghSpaceMatrix[THETA_MAX * drho + dtheta];
//					if (currentValue > houghSpaceMax)
//					{
//						houghSpaceMax = currentValue;
//					}
//				}
//			}
//	delete[] sinTable;
//	delete[] cosTable;
//	return houghSpaceMax;
//}
//
//int FindingLocalMaximums(int* houghSpaceMatrix, int rho, int theta, int* lines, int numberOfVoters, int maxNumberOfLines)
//{
//
//	int lineIndex = 0;
//	for (int drho = 0; drho < 2*rho; drho++)
//		for (int dtheta = 0; dtheta < THETA_MAX; dtheta++)
//		{
//			if ((houghSpaceMatrix[THETA_MAX * drho + dtheta] > numberOfVoters) && (lineIndex < maxNumberOfLines))
//			{
//				lines[lineIndex * 2 + 1] = dtheta;
//				lines[lineIndex * 2] = drho-rho;
//
//				lineIndex++;
//			}
//		}
//	return lineIndex;
//}
//
//int houghLineDetector(unsigned __int8* picture, int width, int height, int* lines, float votersThreshold = 0.6, int maxNumberOfLines = MAX_LINES)
//{
//	int maxHoughSpace, numberOfVoters, numberOfLines;
//	int rho = width+height;
//	int* houghSpaceMatrix = new int[2* rho * THETA_MAX]{ 0 };
//
//	maxHoughSpace = CalculatingRhoThetaMatrix(picture, width, height, rho, houghSpaceMatrix);
//	numberOfVoters = int(maxHoughSpace * votersThreshold);
//	numberOfLines = FindingLocalMaximums(houghSpaceMatrix, rho, THETA_MAX, lines, numberOfVoters, maxNumberOfLines);
//
//	return numberOfLines;
//
//}
//
//void drawLinesOnImageAndDisplay(int* lines, int numberOfLines, cv::Mat_<unsigned __int8> image, int colorRGB = 255)
//{
//	float rho, theta, a, b;
//	int x0, x1, x2, y0, y1, y2;
//
//	for (int index = 0; index < numberOfLines; index++)
//	{
//		rho = lines[2 * index];
//		theta = (lines[2 * index + 1] * CV_PI / 180);
//		a = cos(theta);
//		b = sin(theta);
//		x0 = a * rho;
//		y0 = b * rho;
//		x1 = int(x0 - 1500 * b);
//		y1 = int(y0 + 1500 * a);
//		x2 = int(x0 + 1500 * b);
//		y2 = int(y0 - 1500 * a);
//		cv::Point t1(x1, y1);
//		cv::Point t2(x2, y2);
//		cv::line(image, t1, t2, cv::Scalar(colorRGB), 2);
//		std::cout << rho << " " << lines[2 * index + 1] << "\n";
//	}
//
//	cv::namedWindow("Lines", cv::WINDOW_AUTOSIZE);
//	cv::imshow("Lines", image);
//	cv::waitKey(0);
//	cv::destroyAllWindows();
//}
//
//
//int main(int argc, char** argv)
//{
//	int rows, cols, * lines, numberOfLines;
//	cv::String s = "F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\Data\\Images\\sudoku_lines_3.jpg";
//	cv::Mat_<unsigned __int8> image = cv::imread(s, cv::IMREAD_GRAYSCALE);
//	rows = image.rows;
//	cols = image.cols;
//	cv::Mat_<unsigned __int8> imageCanny;
//	Canny(image, imageCanny, 140, 200);
//
//	unsigned __int8* picture;
//	picture = (unsigned __int8*)malloc(rows * cols * sizeof(unsigned __int8));
//
//	for (int i = 0; i < rows; i++)
//		for (int j = 0; j < cols; j++)
//		{
//			picture[cols * i + j] = imageCanny[i][j];
//		}
//	lines = new int[MAX_LINES * 2];
//	numberOfLines = houghLineDetector(picture, cols, rows, lines, 0.6);
//	drawLinesOnImageAndDisplay(lines, numberOfLines, image);
//
//	return 0;
//}
//
