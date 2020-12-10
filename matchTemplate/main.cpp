#include <stdio.h>
#include <unistd.h>

//#include <opencv2/opencv.hpp>
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"

#include "helpers.hpp"

using namespace cv;
using namespace std;

using namespace Helpers;



#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

//! [declare]
/// Global Variables
bool use_mask;
Mat img; Mat templ_a; Mat templ; Mat mask; Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";
const char* templ_window = "Template window";
const char* mask_window = "Mask window";



int match_method;
int max_Trackbar = 5;
//! [declare]

/// Function Headers
void MatchingMethod( int, void* );

/**
 * @function main
 */
int main( int argc, char** argv )
{

    //cout << "Not enough parameters" << endl;
    img = imread( "Lenna.png" , IMREAD_COLOR);
    templ_a = imread( "shard36.png", -1 );
    templ= Rotate(templ_a, -46 );
    templ= removeAlphaChannel(templ);
    
  //! [load_image]
  /// Load image and template
 // img = imread( argv[1], IMREAD_COLOR );
 // templ = imread( argv[2], IMREAD_COLOR );

    use_mask = 1;
    mask = imread( "shard36.png", -1);
    mask = Rotate(mask, -46);
    mask= removeAlphaChannel(mask);


  if(img.empty() || templ.empty() || (use_mask && mask.empty()))
  {
    cout << "Can't read one of the images" << endl;
    return EXIT_FAILURE;
  }
  //! [load_image]

  //! [create_windows]
  /// Create windows
    
  namedWindow( image_window, WINDOW_AUTOSIZE );
  namedWindow( result_window, WINDOW_AUTOSIZE );
  namedWindow( templ_window, WINDOW_AUTOSIZE );
  namedWindow( mask_window, WINDOW_AUTOSIZE );
  //! [create_windows]

  //! [create_trackbar]
  /// Create Trackbar
  const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
  createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );
  //! [create_trackbar]

  MatchingMethod( 0, 0 );

  //! [wait_key]
  waitKey(0);
  return EXIT_SUCCESS;
  //! [wait_key]
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
void MatchingMethod( int, void* )
{
  //! [copy_source]
  /// Source image to display
  Mat img_display;
  img.copyTo( img_display );
  //! [copy_source]

  //! [create_result_matrix]
  /// Create the result matrix
  int result_cols =  img.cols - templ.cols + 1;
  int result_rows = img.rows - templ.rows + 1;

  result.create( result_rows, result_cols, CV_32FC1 );
  //! [create_result_matrix]

  //! [match_template]
  /// Do the Matching and Normalize
//  bool method_accepts_mask = (TM_SQDIFF == match_method || match_method == TM_CCORR_NORMED);
  bool method_accepts_mask = 1;
  if (use_mask && method_accepts_mask)
    { matchTemplate( img, templ, result, match_method, mask); }
  else
    { matchTemplate( img, templ, result, match_method); }
  //! [match_template]

  //! [normalize]
 // normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
  //! [normalize]

  //! [best_match]
  /// Localizing the best match with minMaxLoc
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  Point matchLoc;

  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
  //! [best_match]

    cout<< "MIN VALUE:" << minVal << endl;
  //! [match_loc]
  /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
  if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED )
    { matchLoc = minLoc; }
  else
    { matchLoc = maxLoc; }
  //! [match_loc]

  //! [imshow]
  /// Show me what you got
  rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

  imshow( image_window, img_display );
  imshow( result_window, result );
  imshow( templ_window, templ_a );
  imshow( mask_window, mask );
  //! [imshow]

  return;
}

