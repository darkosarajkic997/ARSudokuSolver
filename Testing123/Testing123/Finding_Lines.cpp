#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <vector>

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
	int drho = (int)((float)x * cos(dthetaInRadians) - (float)y * sin(dthetaInRadians));
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
		float dthetaInRadians = (float)(dtheta) * pi / 180;
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
std::vector<int> FindingLocalMaximums(int rho, int theta, int* houghSpaceMatrix,int matrixMaximum, float cuttingFactor = 0.5)
{
	std::vector<int> rhoThetaBeforeClipping;
	for (int height = 0; height < rho; height++)
		for (int width = 0; width < theta; width++)
		{
			if ((houghSpaceMatrix[theta * height + width] > (matrixMaximum * cuttingFactor)))
			{
				rhoThetaBeforeClipping.push_back(height);
				rhoThetaBeforeClipping.push_back(width);
			}
		}
	return rhoThetaBeforeClipping;
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
	if (s % 2 == 1 || (s==-1))
		s = s + 1;
	return s;
}
std::vector<int> Filtering(std::vector<int> rhoThetaBeforeClipping,int rhoDifference=15,int thetaDifference=5)
{
	int numberOfSimilarLines = 1;
	std::vector<int> outputArray;
	for (int i = 0; i < rhoThetaBeforeClipping.size()-3; i += 2)
	{
		if ((rhoThetaBeforeClipping[i + 2] - rhoThetaBeforeClipping[i] < rhoDifference ) && (abs(rhoThetaBeforeClipping[i+3]-rhoThetaBeforeClipping[i+1])<thetaDifference))
			numberOfSimilarLines++;
		else
		{
			int index = TakeTheMiddleLineIndex(i, numberOfSimilarLines);
			outputArray.push_back(rhoThetaBeforeClipping[index]);
			outputArray.push_back(rhoThetaBeforeClipping[index+1]);
			numberOfSimilarLines = 1;
		}
	}
	rhoThetaBeforeClipping.clear();
	return outputArray;
}
std::vector<int> doPicture(unsigned __int8* picture, int width, int height)
{

	int rho = sqrt(pow(width,2)+pow(height,2));
	int theta = 360;

	int* houghSpaceMatrix = new int[rho * theta]();

	CalculatingRhoThetaMatrix(picture, width, height, rho, theta, houghSpaceMatrix);

	int matrixMaximum=CalculatingMatrixMaximum(rho, theta, houghSpaceMatrix);

	//int SIZE = 1000;
	std::vector<int> rhoThetaBeforeClipping=FindingLocalMaximums(rho, theta, houghSpaceMatrix,matrixMaximum);

	delete[] houghSpaceMatrix;
	
	return rhoThetaBeforeClipping;
}

cv::Mat CalculateHomography(std::vector<Point2f> a,std::vector<Point2f> b)
{
	/*float** homography = new float* [3];
	for (int i = 0; i < 3; i++)
		homography[i] = new float[3]();*/


	Mat P(9, 9, CV_32F, Scalar(0));

	for (int i = 0; i < 4; i++)
	{
		float x1_a = a[i].x;
		float x1_b = b[i].x;
		float y1_a = a[i].y;
		float y1_b = b[i].y;
		P.at<float>(2 * i, 0) = -x1_a;
		P.at<float>(2 * i, 1) = -y1_a;
		P.at<float>(2 * i, 2) = -1;
		P.at<float>(2 * i, 6) = x1_a * x1_b;
		P.at<float>(2 * i, 7) = y1_a * x1_b;
		P.at<float>(2 * i, 8) = x1_b;
		P.at<float>(2 * i + 1, 3) = -x1_a;
		P.at<float>(2 * i + 1, 4) = -y1_a;
		P.at<float>(2 * i + 1, 5) = -1;
		P.at<float>(2 * i + 1, 6) = x1_a * y1_b;
		P.at<float>(2 * i + 1, 7) = y1_a * y1_b;
		P.at<float>(2 * i + 1, 8) = y1_b;
	}
	P.at<float>(8, 8) = 1;
	Mat P_inv(9, 9, CV_32F, Scalar(0));
	try {
		P_inv = P.inv();
	}
	catch (const cv::Exception& e) {
		cerr << e.msg << endl;
	}

	Mat row_mat(9, 1, CV_32F, Scalar(0));
	row_mat.at<float>(8, 0) = 1;
	Mat H = P_inv * row_mat;

	Mat H3X3(3, 3, CV_32F, Scalar(0));
	for (int row = 0; row < 3; row++)
		for (int col = 0; col < 3; col++)
		{
			H3X3.at<float>(row, col) = H.at<float>(3 * row + col, 0);
		}
	return H3X3;

}
Point2f CalculatePoint(int rho_1, int theta_1, int rho_2, int theta_2)
{
	float x = (sin((float)(theta_2)*pi/180) * (float)(rho_1) - sin((float)(theta_1)*pi/180) * (float)(rho_2)) / (cos((float)(theta_1)*pi/180) * sin((float)(theta_2)*pi/180) - cos((float)(theta_2)*pi/180) * sin((float)(theta_1)*pi/180));
	float y = (cos((float)(theta_1)*pi/180) * (float)(rho_2) - cos((float)(theta_2)*pi/180) * (float)(rho_1)) / (sin((float)(theta_1)*pi/180) * cos((float)(theta_2)*pi/180) - sin((float)(theta_2)*pi/180) * cos((float)(theta_1)*pi/180));
	Point2f p(x, y);
	return p;
}
void sort(std::vector<int> array)
{
	for (int i = 0; i < array.size()-2; i += 2)
		for (int j = i + 2; j < array.size(); j += 2)
			if (array[i] > array[j])
			{
				int t1 = array[i];
				int t2 = array[i + 1];
				array[i] = array[j];
				array[i + 1] = array[j + 1];
				array[j] = t1;
				array[j + 1] = t2;
			}
}
std::vector<Point2f> CalculateIntersections(std::vector<int> niz1,std::vector<int> niz2)
{
	sort(niz1);
	sort(niz2);
	std::vector<Point2f> vec;
	for (int i = 0; i < niz1.size(); i += 2)
		for (int j = 0; j < niz2.size(); j += 2)
		{
			Point2f p = CalculatePoint(niz1[i], niz1[i + 1], niz2[j], niz2[j + 1]);
			vec.push_back(p);
		}
	return vec;
}
float distance(Point2f p1, Point2f p2)
{
	return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
}
int CalculateNumOfInliers(std::vector<Point2f> vec,cv::Mat homography,float tolerance)
{
	int numOfInliers = 0;
	for (std::vector<Point2f>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		Mat X(3, 1, CV_32F);
		X.at<float>(0, 0) = (*it).x;
		X.at<float>(1, 0) = (*it).y;
		X.at<float>(2, 0) = 1;
		Mat X_warped = homography * X;
		float x_diff = X_warped.at<float>(0, 0) - round(X_warped.at<float>(0, 0));
		float y_diff = X_warped.at<float>(1, 0) - round(X_warped.at<float>(1, 0));
		if ((abs(x_diff) < tolerance) && (abs(y_diff) < tolerance))
			numOfInliers++;
	}
	return numOfInliers;
}
void swap(int* x, int* y)
{
	int t = *x;
	*x = *y;
	*y = t;
}
void DrawLines(Mat image, std::vector<int> array1)
{
	for (int i = 0; i < array1.size(); i += 2)
	{
		float a = 1 / tan((float)array1[i + 1] * pi / 180);
		float b = -(float)array1[i] / sin((float)array1[i + 1] * pi / 180);
		int x0 = -2000;
		int y0 = (int)(x0 * a + b);
		int x1 = 2000;
		int y1 = (int)(x1 * a + b);
		Point t1(x0, y0);
		Point t2(x1, y1);
		cv::line(image, t1, t2, Scalar(255), 3, 8, 0);
	}
}
void DrawLines2(Mat image, std::vector<int> array1)
{
	for (int i = 0; i < array1.size(); i += 2)
	{
		float rho = array1[i];
		float theta = array1[i+1];
		double a = cos(theta * pi / 180);
		double b = sin(theta * pi / 180);
		double x0 = a * rho;
		double y0 = b * rho;
		cv::Point p1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
		cv::Point p2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
		cv::line(image, p1, p2, Scalar(255), 3, 8, 0);
	}
}
Mat to_Mat(Point2f p)
{
	Mat P(3, 1, CV_32F);
	P.at<float>(0, 0) = p.x;
	P.at<float>(1, 0) = p.y;
	P.at<float>(2, 0) = 1;
	return P;
}
int BelongsToGrid(Mat X_warped,Mat P1_warped,float x_distance,float tolerance,float belongs_to_picture_tolerance=5)
{
	if ((X_warped.at<float>(0, 0) > (9 * x_distance + belongs_to_picture_tolerance)) && (X_warped.at<float>(1, 0) > (9 * x_distance + belongs_to_picture_tolerance)))
		return -1;
	if ((X_warped.at<float>(0, 0) > -belongs_to_picture_tolerance) && (X_warped.at<float>(1, 0) > -belongs_to_picture_tolerance))
	{
		float p1_to_p_x_distance = X_warped.at<float>(0, 0) - P1_warped.at<float>(0, 0);
		float p1_to_p_y_distance = X_warped.at<float>(1, 0) - P1_warped.at<float>(1, 0);
		float k1 = p1_to_p_x_distance / x_distance;//Q!!!!!
		float k1_round = round(k1);
		float k2 = p1_to_p_y_distance / x_distance;
		float k2_round = round(k2);
		if ((abs(k1 - k1_round) < tolerance) && (abs(k2 - k2_round) < tolerance))
			return 1;
		return 0;
	}
	return -1;
}
int CheckMissingEdge1(Mat imageCannied, float x_distance, float left_most_x, float right_most_x, int tolerance = 5)
{
	float leftEdge = left_most_x - x_distance;
	float rightEdge = right_most_x + x_distance;
	int leftVotes = 0;
	int rightVotes = 0;
	for (int i = leftEdge - tolerance; i < leftEdge + tolerance; i++)
		for (int j = 0; j < imageCannied.rows; j++)
			if (imageCannied.at<unsigned __int8>(j, i) == 255)
				leftVotes++;
	for (int i = rightEdge - tolerance; i < rightEdge + tolerance; i++)
		for (int j = 0; j < imageCannied.rows; j++)
			if (imageCannied.at<unsigned __int8>(j, i) == 255)
				rightVotes++;
	if (rightVotes > leftVotes)
		return 1;
	else return -1;
}
int CheckMissingEdge2(Mat imageCannied, float x_distance, float upper_most_y, float lower_most_y, int tolerance = 5)
{
	float upEdge = upper_most_y - x_distance;
	float downEdge = lower_most_y + x_distance;
	int upVotes = 0;
	int downVotes = 0;
	for (int i = upEdge - tolerance; i < upEdge + tolerance; i++)
		for (int j = 0; j < imageCannied.cols; j++)
			if (imageCannied.at<unsigned __int8>(i, j) == 255)
				upVotes++;
	for (int i = downEdge - tolerance; i < downEdge + tolerance; i++)
		for (int j = 0; j < imageCannied.cols; j++)
			if (imageCannied.at<unsigned __int8>(i, j) == 255)
				downVotes++;
	if (downVotes > upVotes)
		return 1;
	else return -1;
}
std::vector<Point2f> IntersectionPoints(vector<int> arrayHor, vector<int> arrayVer)
{
	int line1_index = 0;
	int line2_index = arrayHor.size() - 2;
	int line3_index = 0;
	int line4_index = arrayVer.size() - 2;

	int line1_rho = arrayHor[line1_index];
	int line1_theta = arrayHor[line1_index + 1];
	int line2_rho = arrayHor[line2_index];
	int line2_theta = arrayHor[line2_index + 1];
	int line3_rho = arrayVer[line3_index];
	int line3_theta = arrayVer[line3_index + 1];
	int line4_rho = arrayVer[line4_index];
	int line4_theta = arrayVer[line4_index + 1];

	Point2f p1 = CalculatePoint(line1_rho, line1_theta, line3_rho, line3_theta);
	Point2f p2 = CalculatePoint(line1_rho, line1_theta, line4_rho, line4_theta);
	Point2f p3 = CalculatePoint(line2_rho, line2_theta, line4_rho, line4_theta);
	Point2f p4 = CalculatePoint(line2_rho, line2_theta, line3_rho, line3_theta);

	std::vector<Point2f> pointVec;
	pointVec.push_back(p1);
	pointVec.push_back(p2);
	pointVec.push_back(p3);
	pointVec.push_back(p4);
	return pointVec;

}
Mat MoveBoard(std::vector<Point2f> array1, Mat H, float x_distance,int* gridPosArray)
{
	Mat P1 = to_Mat(array1[0]);
	Mat P2 = to_Mat(array1[1]);
	Mat P3 = to_Mat(array1[2]);
	Mat P1_warped = H * P1;
	Mat P2_warped = H * P2;
	Mat P3_warped = H * P3;

	float left_most_x = P1_warped.at<float>(0, 0);
	float right_most_x = P2_warped.at<float>(0, 0);
	float furthest_x_distance = right_most_x - left_most_x;

	float upper_most_y = P2_warped.at<float>(1, 0);
	float lower_most_y = P3_warped.at<float>(1, 0);
	float furthest_y_distance = lower_most_y - upper_most_y;

	float k = lower_most_y / x_distance;
	int lowest_y = round(k);
	int goingDown = 8 - lowest_y;

	k = right_most_x / x_distance;
	int rightest_x = round(k);
	int goingRight = 8 - rightest_x;

	Point2f p5(x_distance * gridPosArray[2] + goingRight * x_distance, x_distance * gridPosArray[3] + goingDown * x_distance);
	Point2f p6(x_distance * gridPosArray[2] + furthest_x_distance + goingRight * x_distance, x_distance * gridPosArray[3] + goingDown * x_distance);
	Point2f p7(x_distance * gridPosArray[2] + furthest_x_distance + goingRight * x_distance, x_distance * gridPosArray[3] + furthest_y_distance + goingDown * x_distance);
	Point2f p8(x_distance * gridPosArray[2] + goingRight * x_distance, x_distance * gridPosArray[3] + furthest_y_distance + goingDown * x_distance);

	std::vector<Point2f> array2;
	array2.push_back(p5);
	array2.push_back(p6);
	array2.push_back(p7);
	array2.push_back(p8);
	Mat homography = CalculateHomography(array1, array2);
	return homography;
}

Mat AdjustingHomography(Mat image, Mat H, vector<int> arrayHor, vector<int> arrayVer, int x_distance,int scale,int* gridPosArray)
{

	std::vector<Point2f> array1=IntersectionPoints(arrayHor, arrayVer);

	Mat movedHomography=MoveBoard(array1, H, x_distance, gridPosArray);

	Mat warpedImage;
	Size sz(1300, 900);
	cv::warpPerspective(image, warpedImage, movedHomography, sz);

	Mat P1 = to_Mat(array1[0]);
	Mat P2 = to_Mat(array1[1]);
	Mat P3 = to_Mat(array1[2]);
	Mat new_P1_warped = movedHomography * P1;
	Mat new_P2_warped = movedHomography * P2;
	Mat new_P3_warped = movedHomography * P3;

	float left_most_x = new_P1_warped.at<float>(0, 0);
	float right_most_x = new_P2_warped.at<float>(0, 0);
	float furthest_x_distance = right_most_x - left_most_x;
	int numOfVerLinesMinusOne = round(furthest_x_distance / x_distance);

	Mat imageSobeled;
	Mat imageCannied;
	cv::Sobel(warpedImage, imageSobeled, CV_8UC1, 1, 0);
	cv::Canny(imageSobeled, imageCannied, 175, 200);

	while (numOfVerLinesMinusOne < 9)
	{
		int direction = CheckMissingEdge1(imageCannied, x_distance, left_most_x, right_most_x);
		if (direction == -1)
			left_most_x = left_most_x - x_distance;
		else right_most_x = right_most_x + x_distance;
		numOfVerLinesMinusOne++;
	}

	float upper_most_y = new_P2_warped.at<float>(1, 0);
	float lower_most_y = new_P3_warped.at<float>(1, 0);
	float furthest_y_distance = lower_most_y - upper_most_y;
	int numOfHorLinesMinusOne = round(furthest_y_distance / x_distance);

	while (numOfHorLinesMinusOne < 9)
	{
		int direction = CheckMissingEdge2(imageCannied, x_distance, upper_most_y, lower_most_y);
		if (direction == -1)
			upper_most_y = upper_most_y - x_distance;
		else lower_most_y = lower_most_y + x_distance;
		numOfHorLinesMinusOne++;
	}
	return movedHomography;
}
Mat FindingHomography(Mat image,std::vector<int> arrayHor,std::vector<int> arrayVer,int scale=100, float tolerance = 0.1)
{
	std::vector<Point2f> vec = CalculateIntersections(arrayHor, arrayVer);
	int maxNumOfInliers = 0;
	Mat resultHomography(3, 3, CV_32F);
	int currHomography = 1;
	int bestHomography = 1;
	float x_distance = scale;
	bool p = true;
	int numOfTries = 0;
	int timesEntered = 0;
	int gridPosArray[4];
		while (p)
		{
			int line1_index = rand() % ((arrayHor.size()/2)-1);
			int line2_index = rand() % ((arrayHor.size()/2)-1);
			int line3_index = rand() % ((arrayVer.size()/2)-1);
			int line4_index = rand() % ((arrayVer.size()/2)-1);

				/*int line1_index = 1;
				int line2_index = 3;
				int line3_index = 0;
				int line4_index = 2;*/
			if (line1_index == line2_index)
				line2_index++;
			if (line3_index == line4_index)
				line4_index++;
			if (line1_index > line2_index)
				swap(line1_index, line2_index);
			if (line3_index > line4_index)
				swap(line3_index, line4_index);
			int line1_rho = arrayHor[2 * line1_index];
			int line1_theta = arrayHor[2 * line1_index + 1];
			int line2_rho = arrayHor[2 * line2_index];
			int line2_theta = arrayHor[2 * line2_index + 1];
			int line3_rho = arrayVer[2 * line3_index];
			int line3_theta = arrayVer[2 * line3_index + 1];
			int line4_rho = arrayVer[2 * line4_index];
			int line4_theta = arrayVer[2 * line4_index + 1];


			Point2f p1 = CalculatePoint(line1_rho, line1_theta, line3_rho, line3_theta);
			Point2f p2 = CalculatePoint(line1_rho, line1_theta, line4_rho, line4_theta);
			Point2f p3 = CalculatePoint(line2_rho, line2_theta, line4_rho, line4_theta);
			Point2f p4 = CalculatePoint(line2_rho, line2_theta, line3_rho, line3_theta);
			printf("%f %f \n", p1.x, p1.y);
			printf("%f %f \n", p2.x, p2.y);
			printf("%f %f \n", p3.x, p3.y);
			printf("%f %f \n", p4.x, p4.y);
			printf("\n");
			for (int sx = 1; sx < 10; sx++)
				for (int sy = 1; sy < 10; sy++)
					for (int swx=0;swx<10-sx;swx+=1)
 						for (int swy=0;swy<10-sy;swy+=1)
							if (p)
							{
								Point2f p5(scale * swx, scale * swy);
								Point2f p6(scale * swx + scale * sx, scale * swy);
								Point2f p7(scale * swx + scale * sx, scale * swy + scale * sy);
								Point2f p8(scale * swx, scale * swy + scale * sy);
								std::vector<Point2f> array1;
								std::vector<Point2f> array2;

								array1.push_back(p1);
								array1.push_back(p2);
								array1.push_back(p3);
								array1.push_back(p4);

								array2.push_back(p5);
								array2.push_back(p6);
								array2.push_back(p7);
								array2.push_back(p8);

								Mat homography = CalculateHomography(array1, array2);//ili tu...

								Mat P1 = to_Mat(p1);
								Mat P1_warped = homography * P1;

								Mat P2 = to_Mat(p2);
								Mat P2_warped = homography * P2;

								Mat P3 = to_Mat(p3);
								Mat P3_warped = homography * P3;


								x_distance = (P2_warped.at<float>(0, 0) - P1_warped.at<float>(0, 0)) / (float)sx;

								float p2_to_p3_distance = line2_rho - line1_rho;
								float p1_to_p2_distance = line4_rho - line3_rho;

								float ratio1 = p1_to_p2_distance / p2_to_p3_distance;
								float ratio2 = (float)sx / (float)sy;
	
								if (abs(ratio1 - ratio2) < 0.4)
								{
									timesEntered++;
									int numOfInliers = 0;
									int i = 0;
									for (std::vector<Point2f>::iterator it = vec.begin(); it != vec.end(); ++it)
									{
										Mat X(3, 1, CV_32F);
										X.at<float>(0, 0) = (*it).x;
										X.at<float>(1, 0) = (*it).y;
										X.at<float>(2, 0) = 1;
										Mat X_warped = homography * X;
										
										int a = BelongsToGrid(X_warped, P1_warped, x_distance, tolerance);
										if (a == 1)
											numOfInliers++;
										else if (a == -1)
										{
											numOfInliers = 0;
											it = vec.end() - 1;
										}
									}
									printf("\n");
									if (numOfInliers > maxNumOfInliers)
									{
										bool q = false;
										for (int row = 0; row < 3; row++)
										{
											for (int col = 0; col < 3; col++)
												if (homography.at<float>(row, col) != 0)
													q = true;
										}
										if (q)
										{
											gridPosArray[0] = sx;
											gridPosArray[1] = sy;
											gridPosArray[2] = swx;
											gridPosArray[3] = swy;
											resultHomography = homography;
											maxNumOfInliers = numOfInliers;
											bestHomography = currHomography;
											printf("%f %f ", p1.x, p1.y);
											printf("\n");
											printf("%f %f ", p2.x, p2.y);
											printf("\n");
											printf("%f %f ", p3.x, p3.y);
											printf("\n");
											printf("%f %f ", p4.x, p4.y);
											printf("\n");
											printf("%d %d\n", line1_index, line2_index);
											printf("%d %d \n", line3_index, line4_index);
											printf("swx=%d swy=%d sx=%d sy=%d \n", swx, swy, sx, sy);
											/*for (int row = 0; row < 3; row++)
											{
												for (int col = 0; col < 3; col++)
													printf("%f ", resultHomography.at<float>(row, col));
												printf("\n");
											}*/
											//if (maxNumOfInliers > (arrayHor.size()/2) * (arrayVer.size() / 2)/2 )
											  if (maxNumOfInliers > 11)
											{
												p = false;
												printf("\n");
											}
										}
									}
								}
								currHomography++;
								
							}
			numOfTries++;
		}
		 
		Mat newHomography = AdjustingHomography(image, resultHomography, arrayHor, arrayVer, x_distance,scale,gridPosArray);

	return newHomography;
}


int main(int argc, char** argv)
{

	int rows = 864;
	int cols = 1300;

	String s = "sudoku.jpg";
	Mat_<unsigned __int8> image = imread(s, IMREAD_GRAYSCALE);
	Mat_<unsigned __int8> imageCanny;
	Mat_<unsigned __int8> outputImage;
	Canny(image, imageCanny, 175, 200);
	waitKey(0);
	unsigned __int8* picture;
	picture = new unsigned __int8[rows * cols];

	for (int i=0;i<rows;i++)
		for (int j = 0; j < cols; j++)
		{
			picture[cols*i+j] = imageCanny[i][j];
		}

	std::vector<int> outputArrayWithoutFiltering=doPicture(picture, cols, rows);
	//DrawLines(imageCanny, outputArrayWithoutFiltering);
	cv::imshow("CannyBeforeFiltering", imageCanny);
	std::vector<int> outputArrayWithFiltering= Filtering(outputArrayWithoutFiltering,20,5);
	std::vector<int> arrayHor;
	std::vector<int> arrayVer;
	for (std::vector<int>::iterator it = outputArrayWithFiltering.begin(); it != outputArrayWithFiltering.end();)
	{
		if ((*(it+1) > 12) && (*(it + 1) < 25))
		{
			arrayVer.push_back(*it);
			arrayVer.push_back(*(it + 1));
		}
		if ((*(it + 1) > 280) && (*(it + 1) < 285))
		{
			arrayHor.push_back(*it);
			arrayHor.push_back(*(it + 1));
		}
		it += 2;
	}
	/*int theta =0;
	for (int rho = 130; rho < 130 * 11; rho += 130)
	{
		arrayVer.push_back(rho);
		arrayVer.push_back(theta);
	}
	theta = 270;
	for (int rho = 130; rho < 130 * 11; rho += 130)
	{
		arrayHor.push_back(rho);
		arrayHor.push_back(theta);
	}*/
	std::vector<int> arrayHorFiltered = Filtering(arrayHor, 20, 5);
	std::vector<int> arrayVerFiltered = Filtering(arrayVer, 20, 5);
	DrawLines(imageCanny, arrayHorFiltered);
	DrawLines(imageCanny, arrayVerFiltered);
	imshow("Output2", imageCanny);
	Mat H(3,3,CV_32F);
	H=FindingHomography(imageCanny, arrayHorFiltered, arrayVerFiltered,60);
	//AdjustingHomography(imageCanny,arrayHorFiltered,arrayVerFiltered)
	Mat_<unsigned __int8> imageSobeled;
	Mat_<unsigned __int8> imageCannied;

	//imshow("Sobel", imageSobeled);
	Size sz(1300, 900);
	cv::warpPerspective(image, outputImage, H,sz);
	//cv::Sobel(outputImage, imageSobeled, CV_8UC1, 1, 0);
	//cv::Canny(outputImage,imageCannied,200,200);
	imshow("Output1",outputImage );


	waitKey(0);
	return 0;
}