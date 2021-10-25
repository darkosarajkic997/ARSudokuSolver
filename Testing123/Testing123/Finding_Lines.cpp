#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <vector>
#include "FindingHomography.h"
#include "Source1.cpp"
#include "Source2.cpp"



# define pi         3.14159265358979323846

using namespace cv;
using namespace std;




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
int CheckMissingVerticalEdges(Mat imageCannied, float x_distance, float left_most_x, float right_most_x, int tolerance = 5)
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
int CheckMissingHorizontalEdges(Mat imageCannied, float x_distance, float upper_most_y, float lower_most_y, int tolerance = 5)
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
Mat MoveBoard(std::vector<Point2f> src_array, Mat H, float x_distance,int* gridPosArray,int margin=6)
{
	Mat P1 = to_Mat(src_array[0]);
	Mat P2 = to_Mat(src_array[1]);
	Mat P3 = to_Mat(src_array[2]);
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
	int goingDown = 9 - lowest_y;

	k = right_most_x / x_distance;
	int rightest_x = round(k);
	int goingRight = 9 - rightest_x;

	Point2f p5(x_distance * gridPosArray[2] + goingRight * x_distance+margin, x_distance * gridPosArray[3] + goingDown * x_distance + margin);
	Point2f p6(x_distance * gridPosArray[2] + furthest_x_distance + goingRight * x_distance + margin, x_distance * gridPosArray[3] + goingDown * x_distance + margin);
	Point2f p7(x_distance * gridPosArray[2] + furthest_x_distance + goingRight * x_distance + margin, x_distance * gridPosArray[3] + furthest_y_distance + goingDown * x_distance + margin);
	Point2f p8(x_distance * gridPosArray[2] + goingRight * x_distance + margin, x_distance * gridPosArray[3] + furthest_y_distance + goingDown * x_distance + margin);

	std::vector<Point2f> dst_array;
	dst_array.push_back(p5);
	dst_array.push_back(p6);
	dst_array.push_back(p7);
	dst_array.push_back(p8);
	Mat homography = CalculateHomography(src_array, dst_array);
	return homography;
}
std::vector<float> Lowest_X_Biggest_X (Mat P1, Mat P2, Mat imageCannied, float x_distance, std::vector<float> edgeArray)
{
	float left_most_x = P1.at<float>(0, 0);
	float right_most_x = P2.at<float>(0, 0);
	float furthest_x_distance = right_most_x - left_most_x;
	int numOfVerLinesMinusOne = round(furthest_x_distance / x_distance);

	while (numOfVerLinesMinusOne < 9)
	{
		int direction = CheckMissingVerticalEdges(imageCannied, x_distance, left_most_x, right_most_x);
		if (direction == -1)
			left_most_x = left_most_x - x_distance;
		else right_most_x = right_most_x + x_distance;
		numOfVerLinesMinusOne++;
	}
	edgeArray.push_back(left_most_x);
	edgeArray.push_back(right_most_x);

	return edgeArray;
}
std::vector<float> Lowest_Y_Biggest_Y(Mat P1, Mat P2, Mat imageCannied, float x_distance, std::vector<float> edgeArray)
{
	float upper_most_y = P1.at<float>(1, 0);
	float lower_most_y = P2.at<float>(1, 0);
	float furthest_y_distance = lower_most_y - upper_most_y;
	int numOfHorLinesMinusOne = round(furthest_y_distance / x_distance);

	while (numOfHorLinesMinusOne < 9)
	{
		int direction = CheckMissingHorizontalEdges(imageCannied, x_distance, upper_most_y, lower_most_y);
		if (direction == -1)
			upper_most_y = upper_most_y - x_distance;
		else lower_most_y = lower_most_y + x_distance;
		numOfHorLinesMinusOne++;
	}
	edgeArray.push_back(upper_most_y);
	edgeArray.push_back(lower_most_y);
	return edgeArray;
}
std::vector<Point2f> FindSudokuBoardCorners(Mat warpedImage, Mat movedHomography, std::vector<Point2f> array1, float x_distance)
{
	Mat P1 = to_Mat(array1[0]);
	Mat P2 = to_Mat(array1[1]);
	Mat P3 = to_Mat(array1[2]);
	Mat new_P1_warped = movedHomography * P1;
	Mat new_P2_warped = movedHomography * P2;
	Mat new_P3_warped = movedHomography * P3;

	Mat imageSobeled;
	Mat imageCannied;
	cv::Sobel(warpedImage, imageSobeled, CV_8UC1, 1, 0);
	cv::Canny(imageSobeled, imageCannied, 175, 200);
	
	std::vector<float> edge_array;
	edge_array=Lowest_X_Biggest_X(new_P1_warped, new_P2_warped, imageCannied, x_distance, edge_array);
	edge_array=Lowest_Y_Biggest_Y(new_P2_warped, new_P3_warped, imageCannied, x_distance, edge_array);
	
	std::vector<Point2f> corners;
	corners.push_back(Point2f(edge_array[0], edge_array[2]));
	corners.push_back(Point2f(edge_array[1], edge_array[2]));
	corners.push_back(Point2f(edge_array[1], edge_array[3]));
	corners.push_back(Point2f(edge_array[0], edge_array[3]));


	return corners;
}
Mat AdjustingHomography(Mat image, Mat H, vector<int> arrayHor, vector<int> arrayVer, float x_distance,int scale,int* gridPosArray)
{

	std::vector<Point2f> array1=IntersectionPoints(arrayHor, arrayVer);

	Mat movedHomography=MoveBoard(array1, H, x_distance, gridPosArray);

	Mat warpedImage;

	Size sz(18*x_distance,18*x_distance);
	cv::warpPerspective(image, warpedImage, movedHomography, sz);

	std::vector<Point2f> sudokuCorners=FindSudokuBoardCorners(warpedImage, movedHomography, array1, x_distance);
	//MoveBackBoard
	return movedHomography;

}
Mat FindingHomographyFunc(Mat image,std::vector<int> arrayHor,std::vector<int> arrayVer,int scale=100, float tolerance = 0.05)
{
	std::vector<Point2f> intersection_points_vector = CalculateIntersections(arrayHor, arrayVer);
	int max_num_of_inliers = 0;
	Mat result_homography(3, 3, CV_32F);
	float x_distance = scale;
	bool NOT_FOUND = true;
	int grid_pos_array[4];

	//int currHomography = 1;
	//int bestHomography = 1;
	//int numOfTries = 0;
	//int timesEntered = 0;

		while (NOT_FOUND)
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
			/*printf("%f %f \n", p1.x, p1.y);
			printf("%f %f \n", p2.x, p2.y);
			printf("%f %f \n", p3.x, p3.y);
			printf("%f %f \n", p4.x, p4.y);
			printf("\n");*/
			for (int sx = 1; sx < 10; sx++)
				for (int sy = 1; sy < 10; sy++)
					for (int swx=0;swx<10-sx;swx+=1)
 						for (int swy=0;swy<10-sy;swy+=1)
							if (NOT_FOUND)
							{
								Point2f p5(scale * swx, scale * swy);
								Point2f p6(scale * swx + scale * sx, scale * swy);
								Point2f p7(scale * swx + scale * sx, scale * swy + scale * sy);
								Point2f p8(scale * swx, scale * swy + scale * sy);
								std::vector<Point2f> src_pic_array;
								std::vector<Point2f> dst_pic_array;

								src_pic_array.push_back(p1);
								src_pic_array.push_back(p2);
								src_pic_array.push_back(p3);
								src_pic_array.push_back(p4);

								dst_pic_array.push_back(p5);
								dst_pic_array.push_back(p6);
								dst_pic_array.push_back(p7);
								dst_pic_array.push_back(p8);

								Mat homography = CalculateHomography(src_pic_array, dst_pic_array);//ili tu...

								Mat P1 = to_Mat(p1);
								Mat P1_warped = homography * P1;

								Mat P2 = to_Mat(p2);
								Mat P2_warped = homography * P2;

								Mat P3 = to_Mat(p3);
								Mat P3_warped = homography * P3;


								x_distance = (P2_warped.at<float>(0, 0) - P1_warped.at<float>(0, 0)) / (float)sx;
								//mozda bi trebalo da se racuna i y_distance
								float p2_to_p3_distance = line2_rho - line1_rho;
								float p1_to_p2_distance = line4_rho - line3_rho;

								float ratio1 = p1_to_p2_distance / p2_to_p3_distance;
								float ratio2 = (float)sx / (float)sy;
	
								if (abs(ratio1 - ratio2) < 0.4)
								{
									int num_of_inliers = 0;

									for (std::vector<Point2f>::iterator it = intersection_points_vector.begin(); it != intersection_points_vector.end(); ++it)
									{
										Mat X(3, 1, CV_32F);
										X.at<float>(0, 0) = (*it).x;
										X.at<float>(1, 0) = (*it).y;
										X.at<float>(2, 0) = 1;
										Mat X_warped = homography * X;

										int a = BelongsToGrid(X_warped, P1_warped, x_distance, tolerance);
										if (a == 1)
											num_of_inliers++;
										else if (a == -1)
										{
											num_of_inliers = 0;
											it = intersection_points_vector.end() - 1;
										}
									}
									//printf("\n");
									if (numOfInliers > maxNumOfInliers)
									{
										bool IS_HOMOGRAPHY_VALID = false;
										for (int row = 0; row < 3; row++)
										{
											for (int col = 0; col < 3; col++)
												if (homography.at<float>(row, col) != 0)
													IS_HOMOGRAPHY_VALID = true;
										}
										if (IS_HOMOGRAPHY_VALID)
										{
											gridPosArray[0] = sx;
											gridPosArray[1] = sy;
											gridPosArray[2] = swx;
											gridPosArray[3] = swy;
											resultHomography = homography;
											maxNumOfInliers = numOfInliers;
											if (maxNumOfInliers > 18)//it has to be greater than hor*ver/4 because of 2x1 and 2x2
											{
												NOT_FOUND = false;
												for (std::vector<Point2f>::iterator it = intersection_points_vector.begin(); it != intersection_points_vector.end(); ++it)
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
														it = intersection_points_vector.end() - 1;
													}
												}
												//printf("\n");
											}
										}
									}
								}
								//}
								//currHomography++;
								
							}
			//numOfTries++;
		}
		 
		Mat newHomography = AdjustingHomography(image, resultHomography, arrayHor, arrayVer, x_distance,scale,gridPosArray);

	return newHomography;
}
