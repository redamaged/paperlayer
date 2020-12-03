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
void thresh_callback(int, void* );
Mat getCameraFrame();
/** @function main */
int main( int argc, char** argv )
{
  /// Load source image and convert it to gray
  src = imread( argv[1], 1 );
  src = convert_BGRtoBGRA(src);
  
   src = getCameraFrame();  
  src = convert_BGRtoBGRA(src);

  /// Convert image to gray and blur it
  cvtColor( src, src_gray, CV_BGR2GRAY );
  blur( src_gray, src_gray, Size(2,2) );
  //threshold(src_gray,src_gray,127,255,CV_THRESH_BINARY);
	
 
 

  /// Create Window
  namedWindow( "Source", CV_WINDOW_AUTOSIZE );
  imshow( "Source", src );

  createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
  thresh_callback( 0, 0 );

	  waitKey(0);

  return(0);
}

Mat getCameraFrame()
{
	Mat frame;
	VideoCapture cap(2);
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

/** @function thresh_callback */
void thresh_callback(int, void* )
{
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// Detect edges using canny
  Canny( src_gray, canny_output, thresh, thresh*7, 5);
  /// Find contours
  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Draw contours
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC4 );
  drawing = Scalar(0,0,0,0);
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255), 1 );       
       drawContours( drawing, contours, i, color, CV_FILLED, 8, hierarchy, 0, Point() );
     }
    cout << contours.size() << endl;
     
   //////////////////////////////////////////

	for (int i=0; i<drawing.rows; i++)
		{
			for (int j=0; j<drawing.cols; j++)
			{
				Vec4b color = drawing.at<Vec4b>(Point(j, i));
				if((int)color.val[3] == 1)			
				{
					drawing.at<Vec4b>(Point(j, i))= src.at<Vec4b>(Point(j, i));
				}
							
			}
		}	

    for( int i = 0; i< contours.size(); i=hierarchy[i][0] ) // iterate through each contour.
    {
        Rect r= boundingRect(contours[i]);
        if(hierarchy[i][2]<0) //Check if there is a child contour
          rectangle(drawing,Point(r.x-10,r.y-10), Point(r.x+r.width+10,r.y+r.height+10), Scalar(0,0,255),2,8,0); //Opened contour
        else
        {
          rectangle(drawing,Point(r.x-10,r.y-10), Point(r.x+r.width+10,r.y+r.height+10), Scalar(0,255,0),2,8,0); //closed contour
          Mat croppedImage = drawing(r);
          
          std::string numStr = std::to_string(i);
          imshow( numStr, croppedImage)	;
          
		}
    }   


	
	//Scalar color = src.at<Vec4b>(Point(j, i));
	 //transparency_check(drawing);
 
  
   
  ///
   
   imshow( "Contours", drawing );
   
   

  /// Show in a window
  namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
}
