#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
# define pi         3.14159265358979323846

using namespace cv;
using namespace std;

int* doPicture(unsigned __int8* picture, int width, int height)
{

	int rho = sqrt(pow(width,2)+pow(height,2));
	int theta = 360;
	Mat image(height,width,CV_8UC1);
	Mat imageCanny(height,width, CV_8UC1);
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			image.at<uchar>(j,i) = picture[j* width + i];
	Canny(image, imageCanny, 175, 200);
	int* ptr;
	ptr = (int*)malloc((rho * theta) * sizeof(int)); //Dynamically Allocating Memory
	for (int i = 0; i < rho * theta; i++)
	{
		ptr[i] = 0; //Giving value to the pointer and simultaneously printing it.
	}
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (imageCanny.at<uchar>(j,i) == 255)
			{
				for (int t = 0; t < 360; t++)
				{
					int d = int(i * cos(t * pi / 180) - j * sin(t * pi / 180));
					if ((d >= 0) && (d < rho))
						ptr[theta * d + t] = ptr[theta * d + t] + 1;
				}
			}
	int max = 0;
	int maxi = 0;
	int maxj = 0;
	for (int i = 0; i < rho; i++)
		for (int j = 0; j < theta; j++)
			if (ptr[theta * i + j] > max)
			{
				max = ptr[i * theta + j];
				maxi = i;
				maxj = j;
			}

	int* maxniz = (int*)malloc(1000 * sizeof(int));
	int t = 0;
	for (int i = 0; i < rho; i++)
		for (int j = 0; j < theta; j++)
		{
			if ((ptr[theta * i + j] > max * 0.5) && (t < 1000))
			{
				maxniz[t] = i;
				maxniz[t+1] = j;
				t+=2;
			}
		}
	int c = 0;
	for (int i = 0; i < 1000; i += 2)
	{
		if (maxniz[i + 2] - maxniz[i] < 10)
			c++;
		else
		{
			int s = i - c;
			if (s % 2 == 1)
				s = s + 1;
			float a = 1 / tan(maxniz[s + 1] * pi / 180);
			float b = -maxniz[s] / sin(maxniz[s + 1] * pi / 180);
			int x0 = -2000;
			int y0 = (int)(x0 * a + b);
			int x1 = 2000;
			int y1 = (int)(x1 * a + b);
			Point t1(x0, y0);
			Point t2(x1, y1);
			cv::line(imageCanny, t1, t2, Scalar(255), 3, 8, 0);
			c = 0;
		}
	}
	
	imshow("Output", imageCanny);
	waitKey(0);
	//int* maxniz = new int();
	return maxniz;
}

int main(int argc, char** argv)
{
	int rows = 864;
	int cols = 1300;
	unsigned __int8* picture;
	picture = (unsigned __int8*)malloc(rows * cols * sizeof(unsigned __int8));
    String s = "sudoku.jpg";
	Mat_<unsigned __int8> image = imread(s, IMREAD_GRAYSCALE);
	for (int i=0;i<rows;i++)
		for (int j = 0; j < cols; j++)
		{
			picture[cols*i+j] = image[i][j];
		}
	int* a=doPicture(picture, cols, rows);
	return 0;
}