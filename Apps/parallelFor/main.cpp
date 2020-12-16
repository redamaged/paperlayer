#include <opencv2/opencv.hpp>
#include "algos.hpp"

using namespace cv;

int main()
{

	const Mat img1 = imread("../../../../Data/Lenna.png", IMREAD_GRAYSCALE);
	const Mat img2 = imread("../../../../Data/Lenna2.png", IMREAD_GRAYSCALE);

	double PSNR;
	double t1 = (double)getTickCount();
	PSNR = Algos::getPSNR(img1, img2);
	t1 = ((double)getTickCount() - t1) / getTickFrequency();
	cout << "PSNR: " << PSNR<< "     Time: "<< t1 << " s" << endl;

	Scalar MSSIM;
	t1 = (double)getTickCount();
	Algos::getMSSIM(img1, img2);
	t1 = ((double)getTickCount() - t1) / getTickFrequency();
	cout << "MSSIM: " << MSSIM << "      Time: "<< t1 << " s" << endl;

    waitKey();
}

