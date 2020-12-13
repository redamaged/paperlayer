#include <stdio.h>
#include <unistd.h>

#include "helpers.hpp"
#include "algos.hpp"

using namespace Helpers;

bool use_mask;
Mat img; Mat templ;
const char* image_window = "Source Image";
const char* templ_window = "Template window";
const char* mask_window = "Mask window";
int match_method;
int max_Trackbar = 5;
void MatchingMethod( int, void* );

/**
 * @function main
 */
int main( int argc, char** argv )
{
    img = imread( "Lenna.png" ,IMREAD_UNCHANGED);
    templ = imread( "shard45.png", IMREAD_UNCHANGED );
    
  if(img.empty() || templ.empty() )
  {
    cout << "Can't read one of the images" << endl;
    return EXIT_FAILURE;
  }
    
  namedWindow( image_window, WINDOW_AUTOSIZE );

  const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
  createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );
  MatchingMethod( 0, 0 );

  waitKey(0);
  return EXIT_SUCCESS;
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
void MatchingMethod( int, void* )
{
    Point matchLoc;
    Mat img_display;
    img.copyTo( img_display );
    
    Mat rotated_templ;
    int rotation_angle;
    double acc;
    Algos::matchTemplate(img, templ, match_method, matchLoc, rotated_templ, rotation_angle, acc);
    cout<< "Confidence:" << acc << endl;
    cout<< "Rotation Angle:" << rotation_angle << endl;
    rectangle( img_display, matchLoc, Point( matchLoc.x + rotated_templ.cols , matchLoc.y + rotated_templ.rows ), Scalar::all(0), 2, 8, 0 );
  
    rotated_templ= Helpers::multiplyAlphaChannel(rotated_templ, 0.5);
   
    Helpers::collatePatch(img_display, rotated_templ, matchLoc);
    
    imshow( image_window, img_display );
    imshow( templ_window, rotated_templ);
}

