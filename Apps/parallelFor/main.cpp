#include <opencv2/opencv.hpp>
#include "algos.hpp"



using namespace cv;

int main()
{

	const Mat img1 = imread("../../../../Data/Lenna.png", IMREAD_GRAYSCALE);
	const Mat img2 = imread("../../../../Data/Lenna2.png", IMREAD_GRAYSCALE);

	double PSNR;
	double t1 = (double)getTickCount();
	PSNR = Similarity::getPSNR(img1, img2);
	t1 = ((double)getTickCount() - t1) / getTickFrequency();
	cout << "PSNR: " << PSNR<< "     Time: "<< t1 << " s" << endl;

	Scalar MSSIM;
	t1 = (double)getTickCount();
    MSSIM= Similarity::getMSSIM(img1, img2);
	t1 = ((double)getTickCount() - t1) / getTickFrequency();
	cout << "MSSIM: " << MSSIM << "      Time: "<< t1 << " s" << endl;

	
	t1 = (double)getTickCount();
	double mse = Similarity::getMSE(img1, img2);
	t1 = ((double)getTickCount() - t1) / getTickFrequency();
	cout << "MSE: " << mse << "      Time: " << t1 << " s" << endl;
	


	t1 = (double)getTickCount();
	double mae = Similarity::getMAE(img1, img2);
	t1 = ((double)getTickCount() - t1) / getTickFrequency();
	cout << "MAE: " << mae << "      Time: " << t1 << " s" << endl;

    waitKey();
}

