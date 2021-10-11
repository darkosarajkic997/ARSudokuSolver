#include <iostream>
#include <opencv2/opencv.hpp>
#include"BoardFinderLibrary.h"
#include"RANSACHomography.h"


void drawLinesOnImageAndDisplay(int* lines, int numberOfLines, cv::Mat_<unsigned __int8> image, int colorRGB = 255, bool display = true)
{
	float rho, theta, a, b;
	int x0, x1, x2, y0, y1, y2;

	for (int index = 0; index < numberOfLines; index++)
	{
		rho = lines[2 * index];
		theta = (lines[2 * index + 1] * CV_PI / 180);
		a = cos(theta);
		b = sin(theta);
		x0 = a * rho;
		y0 = b * rho;
		x1 = int(x0 - 1500 * b);
		y1 = int(y0 + 1500 * a);
		x2 = int(x0 + 1500 * b);
		y2 = int(y0 - 1500 * a);
		cv::Point t1(x1, y1);
		cv::Point t2(x2, y2);
		cv::line(image, t1, t2, cv::Scalar(colorRGB), 2);
	}

	if (display)
	{
		cv::namedWindow("Lines", cv::WINDOW_AUTOSIZE);
		cv::imshow("Lines", image);
		cv::waitKey(0);
		cv::destroyAllWindows();
	}
}

void drawLinesOnImageAndDisplayF(float* lines, int numberOfLines, cv::Mat_<unsigned __int8> image, int colorRGB = 255, bool display = true)
{
	float rho, theta, a, b;
	int x0, x1, x2, y0, y1, y2;

	for (int index = 0; index < numberOfLines; index++)
	{
		rho = lines[2 * index];
		theta = (lines[2 * index + 1] * CV_PI / 180);
		a = cos(theta);
		b = sin(theta);
		x0 = a * rho;
		y0 = b * rho;
		x1 = int(x0 - 1500 * b);
		y1 = int(y0 + 1500 * a);
		x2 = int(x0 + 1500 * b);
		y2 = int(y0 - 1500 * a);
		cv::Point t1(x1, y1);
		cv::Point t2(x2, y2);
		cv::line(image, t1, t2, cv::Scalar(colorRGB), 2);
	}

	if (display)
	{
		cv::namedWindow("Lines", cv::WINDOW_AUTOSIZE);
		cv::imshow("Lines", image);
		cv::waitKey(0);
		cv::destroyAllWindows();
		
	}
}

void drawPointsAndDispaly(Point* points, int numberOfPoints, cv::Mat_<unsigned __int8>  image, int colorRGB = 255, bool display = true)
{
	for (int index = 0; index < numberOfPoints; index++)
	{
		cv::circle(image, cv::Point(points[index].x, points[index].y), 3, cv::Scalar(colorRGB), -1);
	}
	if (display)
	{
		cv::imwrite("F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\Data\\Images\\Lines.jpg", image);
		cv::namedWindow("Points", cv::WINDOW_AUTOSIZE);
		cv::imshow("Points", image);
		cv::waitKey(0);
		cv::destroyAllWindows();

	}
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		int imgRows, imgCols, numberOfHorisontalLines, numberOfVerticalLines;
		int horisontalClusters, verticalClusters, rollingRange, peakWidth;
		int numberOfLines, minNumberOfLines, votersStep;
		int theta;
		int* horisontalLines = new int[int(MAX_LINES / 2)];
		int* verticalLines = new int[int(MAX_LINES / 2)];
		int* clusters, * lines;
		float* verticalClusteredLines, *horizontalClusteredLines;
		float votersThreshold;
		float homographyMatrix[9];

		std::string imagePath = argv[1];
		cv::Mat_<unsigned __int8> image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);


		imgRows = image.rows;
		imgCols = image.cols;


		cv::Mat_<unsigned __int8> imageCanny;
		cv::Canny(image, imageCanny, 120, 160);



		unsigned __int8* picture = new unsigned __int8[imgRows * imgCols];

		for (int rowIndex = 0; rowIndex < imgRows; rowIndex++)
			for (int colIndex = 0; colIndex < imgCols; colIndex++)
			{
				picture[imgCols * rowIndex + colIndex] = imageCanny[rowIndex][colIndex];
			}

		lines = new int[MAX_LINES * 2];
		votersThreshold = 0.55;
		numberOfLines = houghLineDetector(picture, imgCols, imgRows, lines, votersThreshold);

		drawLinesOnImageAndDisplay(lines, numberOfLines, imageCanny, 100);

		rollingRange = 10;
		peakWidth = 30;
		findTwoBiggestClustersOfLines(lines, numberOfLines, rollingRange, peakWidth, MAX_LINES, horisontalLines, verticalLines, &numberOfHorisontalLines, &numberOfVerticalLines);

		clusters = new int[int(MAX_LINES / 2)]{ 0 };
		horisontalClusters = linesDBSCAN(horisontalLines, numberOfHorisontalLines, clusters,1);
		horizontalClusteredLines = new float[2 * horisontalClusters];
		findAverageForClusteredLines(horisontalLines, numberOfHorisontalLines, clusters, horisontalClusters, horizontalClusteredLines);
		drawLinesOnImageAndDisplayF(horizontalClusteredLines, horisontalClusters, image, 100, false);
		delete[] clusters;

		clusters = new int[int(MAX_LINES / 2)]{ 0 };
		verticalClusters = linesDBSCAN(verticalLines, numberOfVerticalLines, clusters);
		verticalClusteredLines = new float[2 * verticalClusters];
		findAverageForClusteredLines(verticalLines, numberOfVerticalLines, clusters, verticalClusters, verticalClusteredLines);
		drawLinesOnImageAndDisplayF(verticalClusteredLines, verticalClusters, image, 100);

		delete[] lines;
		delete[] horisontalLines;
		delete[] verticalLines;
		delete[] clusters;
		delete[] picture;

		Point *points=new Point[horisontalClusters*verticalClusters];
		bool *valid=new bool[horisontalClusters * verticalClusters];

		findAllIntersectionPoints(horizontalClusteredLines, horisontalClusters, verticalClusteredLines, verticalClusters, points, valid);
		drawPointsAndDispaly(points, horisontalClusters * verticalClusters,image);
		
		findHomograpyFromTwoClustersOfLines(horizontalClusteredLines, horisontalClusters, verticalClusteredLines, verticalClusters, homographyMatrix);


		delete[] points;
		delete[] valid;
		delete[] horizontalClusteredLines;
		delete[] verticalClusteredLines;
	}

}
