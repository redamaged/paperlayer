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
using namespace cv;

namespace Algos
{
void matchTemplate(Mat img, Mat templ, int matchMethod, Point& matchLoc, Mat& rotatedTempl, int& rotationAngle, double& confidence);
};
 
#endif


