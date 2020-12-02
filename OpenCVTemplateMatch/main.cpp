#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/// Global Variables
Mat img; Mat templ; Mat result; Mat art;
char* image_window = "Source Image";
char* result_window = "Result window";

int match_method;
int max_Trackbar = 5;

Mat mt;
Mat croppedPoly;
Point poly_points[1][4];
int h = 337; int w = 450;
vector<Point2f> vert(4);											/// Create a sequence of points to make a polygon
int r, g, b; 			

Point matchLoc;
/// 


/// Function Headers
void MatchingMethod( int, void* );
Mat drawPoly();

/** @function main */
int main( int argc, char** argv )
{
  /// Load image and template
  img = imread( argv[1], 1 );
  art =  Mat::zeros( img.rows, img.cols, CV_8UC3 );
  art = Scalar(255,255,255);
  //templ = imread( argv[2], 1 );
  

  /// Create windows
  namedWindow( image_window, CV_WINDOW_AUTOSIZE );
  namedWindow( result_window, CV_WINDOW_AUTOSIZE );

  /// Create Trackbar
  char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
  createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );

  while(1)
	{
	templ = drawPoly();
	MatchingMethod( 0, 0 );
	
	
	
	}
  return 0;
}


Mat drawPoly( )
{
	
	mt = Mat::zeros( h, w, CV_8UC3 );
	srand (time(NULL));		
	
	int size =25;
	
	
	int rad = rand() % size + 1;
	vert[0] = 			Point( w/2-rad, h/2-rad );
	poly_points[0][0] = Point( w/2-rad, h/2-rad );
	
	rad = rand() % size + 1;
	vert[1] = 			Point( w/2-rad, h/2+rad );
	poly_points[0][1] = Point( w/2-rad, h/2+rad );
	
	rad = rand() % size + 1;
	vert[2] = 			Point( w/2+rad, h/2+rad );
	poly_points[0][2] = Point( w/2+rad, h/2+rad );
	
	rad = rand() % size + 1;
	vert[3] = 			Point( w/2+rad, h/2-rad );
	poly_points[0][3] = Point( w/2+rad, h/2-rad );
	
	
	r = rand()%255;
	g = rand()%255;
	b = rand()%255;
	//cout<<"r:"<<r << " g:"<<g<< " b:"<< b << endl;
	
																		/// Draw lines in mat
	for( int j = 0; j < 4; j++ )
	{
		line( mt, vert[j],  vert[(j+1)%4], Scalar( r,g,b ), 1, CV_AA ); 
	}
	
	const Point* ppt[1] = { poly_points[0] };
	int npt[] = { 4 };
	fillPoly( mt,
            ppt,
            npt,
            1,
            Scalar( r, g, b ),
            8 );
	
	/// bounding box
	int maxX = 0, minX = mt.cols, maxY=0, minY = mt.rows;

    for(int j=0; j<4; j++)
    {
        Point p = vert[j];

        maxX = max(maxX, p.x);
        minX = min(minX, p.x);

        maxY = max(maxY, p.y);
        minY = min(minY, p.y);
    }
	//rectangle( mt, Point(minX,minY), Point(maxX, maxY), Scalar(255) );
	Rect myROI(minX, minY, maxX-minX, maxY-minY);
	croppedPoly = mt(myROI);
	//imshow( "cropped", croppedPoly );    
	
	return croppedPoly;
}




/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
void MatchingMethod( int, void* )
{
  /// Source image to display
  Mat img_display;
  img.copyTo( img_display );

  /// Create the result matrix
  int result_cols =  img.cols - templ.cols + 1;
  int result_rows = img.rows - templ.rows + 1;

  result.create( result_rows, result_cols, CV_32FC1 );

  /// Do the Matching and Normalize
  matchTemplate( img, templ, result, match_method );
  //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

  /// Localizing the best match with minMaxLoc
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  

  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
  
  
  
  /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
  if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    { matchLoc = minLoc; }
  else
    { matchLoc = maxLoc; }

  /// Show me what you got
  rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar(0,255,0), 2, 8, 0 );
  rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar(0,255,0), 2, 8, 0 );

  cout<< "min:" << minVal/(croppedPoly.cols*croppedPoly.rows) << " max:" << maxVal << endl;
  imshow( image_window, img_display );
  
  for( int j = 0; j < 4; j++ )
	{
		Point temp ; temp.x = vert[j].x -w/2 + matchLoc.x ; temp.y = vert[j].y -h/2 + matchLoc.y;
		Point temp1 ; temp1.x = vert[(j+1)%4].x-w/2 + matchLoc.x; temp1.y = vert[(j+1)%4].y -h/2 + matchLoc.y;
		line( art, temp,  temp1, Scalar( r,g,b ), 1, CV_AA ); 
	}
	  for( int j = 0; j < 4; j++ )
	{
		poly_points[0][j].x = poly_points[0][j].x - w/2 + matchLoc.x;
		poly_points[0][j].y = poly_points[0][j].y - h/2 + matchLoc.y;
	}
	
	const Point* ppt[1] = { poly_points[0] };
	int npt[] = { 4 };
	fillPoly( art,
            ppt,
            npt,
            1,
            Scalar( r, g, b ),
            8 );
	
  imshow( result_window, art );
	waitKey(1000);
  return;
}
