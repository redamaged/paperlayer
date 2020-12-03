#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 214;
int max_thresh = 255;
RNG rng(12345);
int sub_thresh = 100;


Mat convert_BGRtoBGRA(Mat src)
{
	Mat dst = cv::Mat(src.rows,src.cols,CV_8UC4);
	for (int i=0; i<src.rows; i++)
	{
		for (int j=0; j<src.cols; j++)
		{
			dst.at<Vec4b>(Point(j,i))[0] = src.at<Vec3b>(Point(j,i))[0];
			dst.at<Vec4b>(Point(j,i))[1] = src.at<Vec3b>(Point(j,i))[1];
			dst.at<Vec4b>(Point(j,i))[2] = src.at<Vec3b>(Point(j,i))[2];
			dst.at<Vec4b>(Point(j,i))[3] = 1;	
		}
	}	
	return dst;
}
Mat transparency_check(Mat drawing)
{	
	for (int i=0; i<drawing.rows; i++)
	{
		for (int j=0; j<drawing.cols; j++)
		{
			Vec4b color = drawing.at<Vec4b>(Point(j, i));
			if((int)color.val[3] == 1)			
			{
				drawing.at<Vec4b>(Point(j, i))[0] = 0;
				drawing.at<Vec4b>(Point(j, i))[1] = 0;
				drawing.at<Vec4b>(Point(j, i))[2] = 255;
			}
			else if((int)color.val[3] == 0)		
			{
				drawing.at<Vec4b>(Point(j, i))[0] = 0;
				drawing.at<Vec4b>(Point(j, i))[1] = 255;
				drawing.at<Vec4b>(Point(j, i))[2] = 0;
			}			
		}
	}	
	return drawing;
}
/// Function header
Mat getCameraFrame();
/** @function main */
int main( int argc, char** argv )
{
	/// Load source image and convert it to gray
	src = imread( argv[1], 1 );


    //src =  getCameraFrame(); 
    //imwrite( "src.jpg", src );
	imshow( "src img", src );
	Mat image2;
	
	
	image2 = getCameraFrame();  
	//image2 = convert_BGRtoBGRA(src);

	
	
	cv::absdiff(src,image2,image2);// Absolute differences between the 2 images 
	cvtColor( image2, image2, CV_BGR2GRAY );
	blur( image2, image2, Size(2,2) );
	//cv::threshold(image2,image2,25,255,CV_THRESH_BINARY); // set threshold to ignore small differences you can also use inrange function
		
	imshow( "diff", image2 ); 

	
	waitKey(0);

	return(0);
}

Mat getCameraFrame()
{ 
	Mat frame;
	VideoCapture cap(1);
	//if(!cap.isOpened())
	//	return 0;
	waitKey(30);
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	waitKey(30);
	imshow( "frame", frame );
	waitKey(0);
	return frame;
}
