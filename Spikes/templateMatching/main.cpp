/**
 * @file MatchTemplate_Demo.cpp
 * @brief Sample code to use the function MatchTemplate
 * @author OpenCV team
 */

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>


using namespace std;
using namespace cv;

//! [declare]
/// Global Variables
bool use_mask;
Mat img; Mat templ; Mat mask;
Mat result[36];
const char* image_window = "Source Image";
const char* result_window = "Result window";

int match_method;
int max_Trackbar = 5;
//! [declare]

/// Function Headers
void MatchingMethod( int, void* );


int largest(double arr[], int n)
{
    int i;
     
    // Initialize maximum element
    double max = arr[0];
  
    // Traverse array elements from second and
    // compare every element with current max
    for (i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];
  
    return max;
}

Mat copyPolygon(Mat source)
{
    cv::Mat newSrc(source.size(), CV_MAKE_TYPE(source.depth(), 4));
    cvtColor(source,newSrc,COLOR_BGR2BGRA);
    return newSrc;
}

void saveImage(Mat image, String filename)
{
    std::vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
    srand (time(NULL));
    std::ostringstream ss;
    long num = rand();
    ss << filename << "_"<< num << ".png";
    
    imwrite(ss.str(), image, compression_params);
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
    

    int maxX = 0, minX =dst.cols , maxY=0, minY = dst.rows;                // bounding box
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





/**
 * @function main
 */
int main( int argc, char** argv )
{
  if (argc < 3)
  {
    cout << "Not enough parameters" << endl;
    cout << "Usage:\n" << argv[0] << " <image_name> <template_name> [<mask_name>]" << endl;
      img = imread( "Lenna.png", IMREAD_COLOR );
      templ = imread( "temp.png", IMREAD_COLOR );
   // return -1;
  }
    else
    {
      //! [load_image]
      /// Load image and template
      img = imread( argv[1], IMREAD_COLOR );
      templ = imread( argv[2], IMREAD_COLOR );
    }

  if(argc > 3) {
    use_mask = true;
    mask = imread( argv[3], IMREAD_COLOR );
  }

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

  double minVal[36]; double maxVal[36]; Point minLoc[36]; Point maxLoc[36];
  Point matchLoc;
    
    for (int i=0; i<36; i++)
    {
      result[i].create( result_rows, result_cols, CV_32FC1 );
      //! [create_result_matrix]

      //! [match_template]
      /// Do the Matching and Normalize
      bool method_accepts_mask = (TM_SQDIFF == match_method || match_method == TM_CCORR_NORMED);
      if (use_mask && method_accepts_mask)
        { matchTemplate( img, Rotate(templ, i), result[i], match_method, mask); }
      else
        { matchTemplate( img, Rotate(templ, i), result[i], match_method); }
      //! [match_template]

      //! [normalize]
      //normalize( result[i], result[i], 0, 1, NORM_MINMAX, -1, Mat() );
      //! [normalize]

      //! [best_match]
      /// Localizing the best match with minMaxLoc
      

      minMaxLoc( result[i], &minVal[i], &maxVal[i], &minLoc[i], &maxLoc[i], Mat() );
      //! [best_match]
        

      //! [match_loc]
      /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    
    }
    cout << "minVal:" ;
    for(int j=0; j<36; j++)
    {
     cout  << minVal[j]<< "," ;
    }cout <<endl;
    cout << "maxVal:";
    for(int j=0; j<36; j++)
    {
      cout<< maxVal[j]<< "," ;
    }cout <<endl;
    
    int abs_max = maxVal[0];
    int max_loc_idx= 0;
    int abs_min = minVal[0];
    int min_loc_idx= 0;
    
    for (int i = 1; i < 36; i++)
    {
        if (maxVal[i] > abs_max)
        {
            abs_max = maxVal[i];
            max_loc_idx = i;
        }
        if (minVal[i] < abs_min)
        {
            abs_min = minVal[i];
            min_loc_idx = i;
        }
    }
    
    
    cout<< endl << "-- MinLoc:" << min_loc_idx;
    cout<< endl << "-- MaxLoc:" << max_loc_idx;
    
    
    
    
  if( match_method  == TM_SQDIFF || match_method == TM_SQDIFF_NORMED )
    { matchLoc= minLoc[min_loc_idx]; }
  else
    { matchLoc =  maxLoc[max_loc_idx] ; }
  //! [match_loc]

  //! [imshow]
  /// Show me what you got
  rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( result[1], matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

  imshow( image_window, img_display );
  imshow( result_window, result[1] );
  //! [imshow]

  return;
}
