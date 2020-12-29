 /**
 * @file algos.hpp
 * @brief Template matchers and location optimizers function
 * @author Reda Maged
 */
#ifndef ALGOS_H
#define ALGOS_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
//using namespace cv;

namespace Algos
{	
	void matchTemplate(cv::Mat img, cv::Mat templ, cv::Mat collage, int matchMethod, cv::Point& matchLoc, cv::Mat& rotatedTempl, int& rotationAngle, double& confidence);
};

namespace Similarity
{
	double getPSNR(const cv::Mat& I1, const cv::Mat& I2);
	cv::Scalar getMSSIM(const cv::Mat& i1, const cv::Mat& i2);
	double getMSE(const cv::Mat& I1, const cv::Mat& I2);
	double getMAE(const cv::Mat& I1, const cv::Mat& I2);
}
 
#endif


