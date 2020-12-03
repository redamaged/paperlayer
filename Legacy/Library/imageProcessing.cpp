#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;


Mat generatePoly(int size)
{
	srand (time(NULL));	
	Mat tempMat = Mat::zeros( 480, 640, CV_8UC4 );
	tempMat = Scalar( 0, 0, 0, 0 );
	Point poly_points[1][4];
	
	int rad = rand() % size + 1;
	poly_points[0][0] = Point( 340-rad, 240-rad );
	rad = rand() % size + 1;
	poly_points[0][1] = Point( 340-rad, 240+rad );
	rad = rand() % size + 1;
	poly_points[0][2] = Point( 340+rad, 240+rad );	
	rad = rand() % size + 1;
	poly_points[0][3] = Point( 340+rad, 240-rad );
	
	int r, g, b; 
	r = rand()%255;
	g = rand()%255;
	b = rand()%255;
	
	/// Draw lines in mat
	for( int j = 0; j < 4; j++ )
	{
		//line( tempMat, poly_points[0][j],  poly_points[0][(j+1)%4], Scalar( r,g,b ), 1, CV_AA ); 
	}
	/// fill the polygon
	const Point* ppt[1] = { poly_points[0] };
	int npt[] = { 4 };
	fillPoly( tempMat, ppt, npt, 1, Scalar( r, g, b, 1 ), 8 );	
	
	int maxX = 0, minX = tempMat.cols, maxY=0, minY = tempMat.rows;				// bounding box
    for(int j=0; j<4; j++)
    {
        Point p = poly_points[0][j];
        maxX = max(maxX, p.x);
        minX = min(minX, p.x);
        maxY = max(maxY, p.y);
        minY = min(minY, p.y);
    }
	//rectangle( mt, Point(minX,minY), Point(maxX, maxY), Scalar(255) );
	Rect myROI(minX, minY, maxX-minX, maxY-minY);
	Mat croppedImage = tempMat(myROI);	
	return croppedImage;
}
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
Mat Rotate(Mat source, double angle)
{
	// get rotation matrix for rotating the image around its center
	Point2f center(source.cols/2.0, source.rows/2.0);
	Mat rot = getRotationMatrix2D(center, angle, 1.0);
	// determine bounding rectangle
	Rect bbox = RotatedRect(center,source.size(), angle).boundingRect();
	// adjust transformation matrix
	rot.at<double>(0,2) += bbox.width/2.0 - center.x;
	rot.at<double>(1,2) += bbox.height/2.0 - center.y;

	Mat dst;
	warpAffine(source, dst, rot, bbox.size());	
	
	
	
	
	/// checking for transparency
	/*
	for (int i=0; i<dst.rows; i++)
	{
		for (int j=0; j<dst.cols; j++)
		{
			Vec4b color = dst.at<Vec4b>(Point(j, i));
			if((int)color.val[3] == 1)			
			{
				dst.at<Vec4b>(Point(j, i))[0] = 0;
				dst.at<Vec4b>(Point(j, i))[1] = 0;
				dst.at<Vec4b>(Point(j, i))[2] = 255;
			}
			else if((int)color.val[3] == 0)		
			{
				dst.at<Vec4b>(Point(j, i))[0] = 0;
				dst.at<Vec4b>(Point(j, i))[1] = 255;
				dst.at<Vec4b>(Point(j, i))[2] = 0;
			}			
		}
	}	
	*/ 
	
	/// crop rotated image
	
	int maxX = 0, minX =dst.cols , maxY=0, minY = dst.rows;				// bounding box
    for (int r=0; r<dst.rows; r++)
	{
		for (int c=0; c<dst.cols; c++)
		{
			bool visible = (int)dst.at<Vec4b>(Point(c, r))[3];			
			if (visible)
			{
				minX = min(minX,c);				
				minY = min(minY,r);
				maxX = max(maxX, c);
				maxY = max(maxY,r);
			}				
		}
    }
    Rect myROI(minX, minY, maxX-minX, maxY-minY);	
	
	
	return dst(myROI);
}

Mat detectPoly(Mat picture)
{
	int thresh = 214;
	RNG rng(12345);

	Mat src_gray;
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat drawing;
	Mat croppedImage;
	
	/// Convert image to gray and blur it
	cvtColor( picture, src_gray, CV_BGR2GRAY );
	blur( src_gray, src_gray, Size(2,2) );	
	/// Detect edges using canny
	Canny( src_gray, canny_output, thresh, thresh*7, 5);
	
	/// Find contours
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );



	/// Draw contours
	drawing = Mat::zeros( canny_output.size(), CV_8UC4 );
	drawing = Scalar(0,0,0,0);
	for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255), 1 );       
       drawContours( drawing, contours, i, color, CV_FILLED, 8, hierarchy, 0, Point() );
     }
     
     /// Copy Original Contents
	for (int i=0; i<drawing.rows; i++)
	{
		for (int j=0; j<drawing.cols; j++)
		{
			Vec4b color = drawing.at<Vec4b>(Point(j, i));
			if((int)color.val[3] == 1)			
			{
				drawing.at<Vec4b>(Point(j, i))= picture.at<Vec4b>(Point(j, i));
			}
						
		}
	}
	
	/// Draw Rectangle around 
	for( int i = 0; i< contours.size(); i=hierarchy[i][0] ) // iterate through each contour.
    {
        Rect r= boundingRect(contours[i]);
        if(hierarchy[i][2]<0) //Check if there is a child contour
          rectangle(drawing,Point(r.x-10,r.y-10), Point(r.x+r.width+10,r.y+r.height+10), Scalar(0,0,255),2,8,0); //Opened contour
        else
        {
			croppedImage = drawing(r);
			rectangle(drawing,Point(r.x-10,r.y-10), Point(r.x+r.width+10,r.y+r.height+10), Scalar(0,255,0),2,8,0); //closed contour
			//std::string numStr = std::to_string(i);
			     
		}
    }

	imshow( "Detected Polygon", drawing)	;     
	return croppedImage;
}


Mat copyPolygon(Mat source)
{
	Mat temp = Mat::zeros( source.rows, source.cols, CV_8UC4);
	temp = Scalar(255,255,255,1);
	for (int i=0; i<source.rows; i++)
	{
		for (int j=0; j<source.cols; j++)
		{
			Vec4b color = source.at<Vec4b>(Point(j, i));
			if((int)color.val[3] != 0)
			{
				temp.at<Vec4b>(Point(j,i)) = color;	
				temp.at<Vec4b>(Point(j,i))[3] = 1;	
			}
			else
			{temp.at<Vec4b>(Point(j,i))[3] = 0;	}
		}
	}	
	return temp;
}

int patchArea(Mat src)
{
	int sum = 0;
	for (int i=0; i<src.cols; i++)
	{
		for (int j=0; j<src.rows; j++)
		{
			Vec4b color = src.at<Vec4b>(Point(i,j));
			if((int)color.val[3] == 1)			
			{
				sum++;
			}
		}
	}
	return sum;
} 
