/**
 * @file algos.cpp
 * @brief Template matchers and location optimizers function
 * @author Reda Maged
 */

#include "algos.hpp"
#include "helpers.hpp"

using namespace std;
using namespace cv;



class parallelRoughMatching : public ParallelLoopBody
{
	int matchMethod;
    Mat _img;
	Mat _templ;
	vector<double> *m_rough_confidences;
	vector<Point> *m_rough_locations;
public:
    parallelRoughMatching(Mat img , Mat templ, int method, vector<double>& _rough_confidences, vector<Point>& _rough_locations
    ):matchMethod(method), _img(img), _templ(templ), m_rough_confidences(&_rough_confidences), m_rough_locations(&_rough_locations)
	{ }

    void operator()(const Range& range) const override
    {
		Mat _result;
		int result_cols = _img.cols - _templ.cols + 1;
		int result_rows = _img.rows - _templ.rows + 1;
		_result.create(result_rows, result_cols, CV_32FC1);
		double minVal; double maxVal; Point minLoc; Point maxLoc;
		for (int i = range.start; i < range.end; ++i)
		{
			Mat templ_temp = Helpers::removeAlphaChannel(Helpers::Rotate(_templ, i * 10));
			cv::matchTemplate(_img, templ_temp, _result, matchMethod, templ_temp);
			minMaxLoc(_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
			if (matchMethod == TM_SQDIFF || matchMethod == TM_SQDIFF_NORMED) {
				m_rough_locations->at(i) = minLoc;
				m_rough_confidences->at(i) = 1 - minVal;
			}
			else {
				m_rough_locations->at(i) = maxLoc;
				m_rough_confidences->at(i) = maxVal;
			}
		}
    }
};

class parallelFineMatching : public ParallelLoopBody
{
	int matchMethod;
	Mat _img;
	Mat _templ;
	vector<double> *m_fine_confidences;
	vector<Point> *m_fine_locations;
	int _angle;
public:
	parallelFineMatching(Mat img, Mat templ, int method, vector<double>& _fine_confidences, vector<Point>& _fine_locations, int& angle
	) :matchMethod(method), _img(img), _templ(templ), m_fine_confidences(&_fine_confidences), m_fine_locations(&_fine_locations), _angle(angle)
	{ }

	void operator()(const Range& range) const override
	{
		Mat _result;
		int result_cols = _img.cols - _templ.cols + 1;
		int result_rows = _img.rows - _templ.rows + 1;
		_result.create(result_rows, result_cols, CV_32FC1);
		double minVal; double maxVal; Point minLoc; Point maxLoc;
		for (int i = range.start; i < range.end; ++i)
		{
			Mat templ_temp = Helpers::removeAlphaChannel(Helpers::Rotate(_templ, _angle - 5 + i));
			cv::matchTemplate(_img, templ_temp, _result, matchMethod, templ_temp);
			minMaxLoc(_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
			if (matchMethod == TM_SQDIFF || matchMethod == TM_SQDIFF_NORMED) {
				m_fine_locations->at(i) = minLoc;
				m_fine_confidences->at(i) = 1 - minVal;
			}
			else {
				m_fine_locations->at(i) = maxLoc;
				m_fine_confidences->at(i) = maxVal;
			}
		}
	}
};




void Algos::matchTemplate(Mat img, Mat templ, int matchMethod, Point& matchLocation, Mat& rotatedTempl, int& rotationAngle, double& confidence)
{
    confidence = 0;
    
    //double _rough_confidences[36];
	vector<double> _rough_confidences(36);    
    //Point _rough_locations[36];
	vector<Point> _rough_locations(36);

    double _fine_confidences[10];
    Point _fine_locations[10];
    int _angle= rotationAngle= 0;
    
    Mat _result;
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    
    _result.create( result_rows, result_cols, CV_32FC1 );
    
    
    
     double minVal; double maxVal; Point minLoc; Point maxLoc;
    

	 /*
	 double t0 = (double)getTickCount();
   
    for (int i=0; i<36 ; i++)
    {
        Mat templ_temp = Helpers::removeAlphaChannel(Helpers::Rotate(templ, i*10)  );
        cv::matchTemplate(img, templ_temp, _result, matchMethod,templ_temp);
        minMaxLoc( _result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
        if( matchMethod  == TM_SQDIFF || matchMethod == TM_SQDIFF_NORMED ){
            _rough_locations[i] = minLoc;
            _rough_confidences[i] = 1- minVal;
        }
        else{
            _rough_locations[i] = maxLoc;
            _rough_confidences[i] = maxVal;
        }
    }
	
	t0 = ((double)getTickCount() - t0) / getTickFrequency();
	cout << "Sequential Rough: " << t0 << " s" << endl;
	*/
	double t1 = (double)getTickCount();
	parallel_for_(Range{ 0, 36 }, parallelRoughMatching{ img, templ, matchMethod, _rough_confidences, _rough_locations });
	t1 = ((double)getTickCount() - t1) / getTickFrequency();
	cout << "Parallel Rough: " << t1 << " s" << endl;

    for (int i=0; i<36 ; i++)
    {
        if(_rough_confidences[i]> confidence)
        {
            confidence = _rough_confidences[i];
            matchLocation = _rough_locations[i];
            _angle = i*10;
            rotationAngle = _angle;
        }
    }
    
    /*
	double t2 = (double)getTickCount();
    for(int i=0; i<10 ; i++)
    {
        Mat templ_temp = Helpers::removeAlphaChannel(Helpers::Rotate(templ, _angle-5+i)  );
        cv::matchTemplate(img, templ_temp, _result, matchMethod,templ_temp);
        minMaxLoc( _result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
        if( matchMethod  == TM_SQDIFF || matchMethod == TM_SQDIFF_NORMED ){
            _fine_locations[i] = minLoc;
            _fine_confidences[i] = 1- minVal;
        }
        else{
            _fine_locations[i] = maxLoc;
            _fine_confidences[i] = maxVal;
        }
    }
	t2 = ((double)getTickCount() - t2) / getTickFrequency();
	cout << "Sequential Fine: " << t2 << " s" << endl;
	*/

	double t3 = (double)getTickCount();
	parallel_for_(Range{ 0, 10 }, parallelFineMatching{ img, templ, matchMethod, _rough_confidences, _rough_locations, _angle });
	t3 = ((double)getTickCount() - t3) / getTickFrequency();
	cout << "Parallel Fine: " << t3 << " s" << endl;


    for (int i=0; i<10 ; i++)
    {
        if(_fine_confidences[i]> confidence)
        {
            confidence = _fine_confidences[i];
            matchLocation = _fine_locations[i];
            rotationAngle = _angle-5+i;
        }
    }
    rotatedTempl = Helpers::Rotate(templ, rotationAngle);
    
}

// ![get-psnr]
double Algos::getPSNR(const Mat& I1, const Mat& I2)
{
	Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2

	Scalar s = sum(s1);        // sum elements per channel

	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if (sse <= 1e-10) // for small values return zero
		return 0;
	else
	{
		double mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0 * log10((255 * 255) / mse);
		return psnr;
	}
}
// ![get-psnr]

// ![get-mssim]

Scalar Algos::getMSSIM(const Mat& i1, const Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	int d = CV_32F;

	Mat I1, I2;
	i1.convertTo(I1, d);            // cannot calculate on one byte large values
	i2.convertTo(I2, d);

	Mat I2_2 = I2.mul(I2);        // I2^2
	Mat I1_2 = I1.mul(I1);        // I1^2
	Mat I1_I2 = I1.mul(I2);        // I1 * I2

								   /*************************** END INITS **********************************/

	Mat mu1, mu2;                   // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);

	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);

	Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	///////////////////////////////// FORMULA ////////////////////////////////
	Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);                 // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);                 // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	Mat ssim_map;
	divide(t3, t1, ssim_map);        // ssim_map =  t3./t1;

	Scalar mssim = mean(ssim_map);   // mssim = average of ssim map
	return mssim;
}
// ![get-mssim]
