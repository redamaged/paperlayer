#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include<string>
#include "helpers.hpp"


using namespace cv;
using namespace std;





int main(int argc, char** argv)
{
    Mat img=imread("../../../../Covers/3.jpg",-1);
    Mat display= Setup::subdivideImage(img, 1000, "1", 0);
	imshow("result", display);

	waitKey(0);

	return 0;
}
