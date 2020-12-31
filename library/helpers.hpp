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
#include<string.h>

using namespace std;
//using namespace cv;

namespace Helpers
{
cv::Mat cropToVisible(cv::Mat source);
cv::Mat addAplhaChannel(cv::Mat source);
void saveImageRandom(cv::Mat image, cv::String filename);
void saveImageLabeled(cv::Mat image, cv::String dirName , cv::String filename);
cv::Mat Rotate(cv::Mat source, double angle);
cv::Mat removeAlphaChannel(cv::Mat source);
void collatePatch(cv::Mat src, cv::Mat overlay, cv::Point& location);
void collateNoise(cv::Mat src, cv::Mat overlay, cv::Point& location);
cv::Mat multiplyAlphaChannel(cv::Mat src, float value);
};

namespace Setup
{
cv::Mat subdivideImage(cv::Mat img, int patchCount, cv::String dir_name, bool save);
}





#endif


