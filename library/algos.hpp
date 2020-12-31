 /**
 * @file algos.hpp
 * @brief Template matchers, similarity estimators, noise generator functions
 * @author Reda Maged
 */
#ifndef ALGOS_H
#define ALGOS_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;

namespace Algos
{	
void matchTemplate(cv::Mat img,
                   cv::Mat templ,
                   int matchMethod,
                   cv::Point& matchLoc,
                   int& rotationAngle,
                   double& confidence);


/// Description
/// @param img Reference Canvas
/// @param templ Template Patch
/// @param collage Collage Canvas
/// @param matchLoc Matched Location to be put in Collage canvas
/// @param rotationAngle Rotation angle of the templ patch in the Collage canvas
/// @param MSSIM OUTPUT
float estimateImprovement(cv::Mat img,
                         cv::Mat templ,
                         cv::Mat collage,
                         cv::Point matchLoc,
                         int rotationAngle,
                         cv::Scalar& MSSIM);
};



namespace Similarity
{
double getPSNR(const cv::Mat& I1, const cv::Mat& I2);
cv::Scalar getMSSIM(const cv::Mat& i1, const cv::Mat& i2);
double getMSE(const cv::Mat& I1, const cv::Mat& I2);
double getMAE(const cv::Mat& I1, const cv::Mat& I2);
}

#endif


