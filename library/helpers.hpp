/**
 * @file helpers.hpp
 * @brief Misc low level helper functions
 * @author Reda Maged
 */

#ifndef HELPERS_H
#define HELPERS_H


#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

namespace Helpers
{
	Mat cropToVisible(Mat source);
	Mat addAplhaChannel(Mat source);
	void saveImageRandom(Mat image, String filename);
	Mat Rotate(Mat source, double angle);
	Mat removeAlphaChannel(Mat source);
	void collatePatch(Mat src, Mat overlay, Point& location);
	Mat multiplyAlphaChannel(Mat src, float value);

	Mat puzzle(Mat img, String dir_name);

};




 
#endif


