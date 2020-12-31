/**
 * @file algos.cpp
 * @brief Template matchers and location optimizers function
 * @author Reda Maged
 */

#include "algos.hpp"
#include "helpers.hpp"

using namespace std;
//using namespace cv;



class parallelRoughMatching : public cv::ParallelLoopBody
{
    int matchMethod;
	cv::Mat _img;
	cv::Mat _templ;
    vector<double> *m_rough_confidences;
    vector<cv::Point> *m_rough_locations;
public:
    parallelRoughMatching(cv::Mat img , cv::Mat templ, int method, vector<double>& _rough_confidences, vector<cv::Point>& _rough_locations
                          ):matchMethod(method), _img(img), _templ(templ), m_rough_confidences(&_rough_confidences), m_rough_locations(&_rough_locations)
    { }
    
    void operator()(const cv::Range& range) const override
    {
		cv::Mat _result;
        int result_cols = _img.cols - _templ.cols + 1;
        int result_rows = _img.rows - _templ.rows + 1;
        _result.create(result_rows, result_cols, CV_32FC1);
        double minVal; double maxVal; cv::Point minLoc, maxLoc;
        for (int i = range.start; i < range.end; ++i)
        {
			cv::Mat templ_temp = Helpers::removeAlphaChannel(Helpers::Rotate(_templ, i * 10));
            cv::matchTemplate(_img, templ_temp, _result, matchMethod, templ_temp);
            minMaxLoc(_result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
            if (matchMethod == cv::TM_SQDIFF || matchMethod == cv::TM_SQDIFF_NORMED) {
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

class parallelFineMatching : public cv::ParallelLoopBody
{
    int matchMethod;
	cv::Mat _img;
	cv::Mat _templ;
    vector<double> *m_fine_confidences;
    vector<cv::Point> *m_fine_locations;
    int _angle;
public:
    parallelFineMatching(cv::Mat img, cv::Mat templ, int method, vector<double>& _fine_confidences, vector<cv::Point>& _fine_locations, int& angle
                         ) :matchMethod(method), _img(img), _templ(templ), m_fine_confidences(&_fine_confidences), m_fine_locations(&_fine_locations), _angle(angle)
    { }
    
    void operator()(const cv::Range& range) const override
    {
		cv::Mat _result;
        int result_cols = _img.cols - _templ.cols + 1;
        int result_rows = _img.rows - _templ.rows + 1;
        _result.create(result_rows, result_cols, CV_32FC1);
        double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
        for (int i = range.start; i < range.end; ++i)
        {
			cv::Mat templ_temp = Helpers::removeAlphaChannel(Helpers::Rotate(_templ, _angle - 5 + i));
            cv::matchTemplate(_img, templ_temp, _result, matchMethod, templ_temp);
            minMaxLoc(_result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
            if (matchMethod == cv::TM_SQDIFF || matchMethod == cv::TM_SQDIFF_NORMED) {
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




void Algos::matchTemplate(cv::Mat img, cv::Mat templ, int matchMethod, cv::Point& matchLocation, int& rotationAngle, double& confidence)
{
    confidence = 0;
    vector<double> _rough_confidences(36), _fine_confidences(10);
    vector<cv::Point> _rough_locations(36), _fine_locations(10);
    int _angle= rotationAngle= 0;

	cv::Mat _result;
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    
    _result.create( result_rows, result_cols, CV_32FC1 );
    
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    double t1 = (double)cv::getTickCount();
	parallel_for_(cv::Range{ 0, 36 }, parallelRoughMatching{ img, templ, matchMethod, _rough_confidences, _rough_locations });
    t1 = ((double)cv::getTickCount() - t1) / cv::getTickFrequency();
    
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
    
    double t3 = (double)cv::getTickCount();
    parallel_for_(cv::Range{ 0, 10 }, parallelFineMatching{ img, templ, matchMethod, _fine_confidences, _fine_locations, _angle });
    t3 = ((double)cv::getTickCount() - t3) / cv::getTickFrequency();
    
    for (int i=0; i<10 ; i++)
    {
        if(_fine_confidences[i]> confidence)
        {
            confidence = _fine_confidences[i];
            matchLocation = _fine_locations[i];
            rotationAngle = _angle-5+i;
        }
    }
//    rotatedTempl = Helpers::Rotate(templ, rotationAngle);

}

float Algos::estimateImprovement(cv::Mat img,
                         cv::Mat templ,
                         cv::Mat collage,
                         cv::Point matchLoc,
                         int rotationAngle,
                         cv::Scalar& MSSIM)
{
    cv::Mat rotatedTempl = Helpers::Rotate(templ, rotationAngle);
    cv::Rect myROI(matchLoc.x, matchLoc.y, rotatedTempl.cols, rotatedTempl.rows);
    cv::Mat matched_ref = img(myROI);
    cv::Mat matched_col = collage(myROI);
    cv::Point p(0,0);
    cv::Mat expected =collage(myROI).clone();
    Helpers::collatePatch(expected, rotatedTempl, p);
    cv::Scalar MSSIM1, MSSIM2;
    MSSIM1= Similarity::getMAE(expected, matched_ref);
    MSSIM2= Similarity::getMAE(matched_col, matched_ref);
    MSSIM = MSSIM1;
    if((MSSIM1[0]+MSSIM1[1]+MSSIM1[2]) <= (MSSIM2[0]+MSSIM2[1]+MSSIM2[2]))
        return 0;
    else
        return (MSSIM1[0]+MSSIM1[1]+MSSIM1[2]);
}

// ![get-psnr]
double Similarity::getPSNR(const cv::Mat& I1, const cv::Mat& I2)
{
	cv::Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2
    
	cv::Scalar s = sum(s1);        // sum elements per channel
    
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

cv::Scalar Similarity::getMSSIM(const cv::Mat& i1, const cv::Mat& i2)
{
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d = CV_32F;
    
	cv::Mat I1, I2;
    i1.convertTo(I1, d);            // cannot calculate on one byte large values
    i2.convertTo(I2, d);
    
	cv::Mat I2_2 = I2.mul(I2);        // I2^2
	cv::Mat I1_2 = I1.mul(I1);        // I1^2
	cv::Mat I1_I2 = I1.mul(I2);        // I1 * I2
    
    /*************************** END INITS **********************************/
    
	cv::Mat mu1, mu2;                   // PRELIMINARY COMPUTING
    GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
    GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);
    
	cv::Mat mu1_2 = mu1.mul(mu1);
	cv::Mat mu2_2 = mu2.mul(mu2);
	cv::Mat mu1_mu2 = mu1.mul(mu2);
    
	cv::Mat sigma1_2, sigma2_2, sigma12;
    
    GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;
    
    GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;
    
    GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;
    
    ///////////////////////////////// FORMULA ////////////////////////////////
	cv::Mat t1, t2, t3;
    
    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);                 // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
    
    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);                 // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
    
	cv::Mat ssim_map;
    divide(t3, t1, ssim_map);        // ssim_map =  t3./t1;
    
	cv::Scalar mssim = mean(ssim_map);   // mssim = average of ssim map
    return mssim;
}
// ![get-mssim]

double Similarity::getMSE(const cv::Mat& I1, const cv::Mat& I2)
{
	cv::Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2
    
	cv::Scalar s = sum(s1);        // sum elements per channel
    
    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels
    double mse = sse / (double)(I1.channels() * I1.total());
    
    return  mse;
}

double Similarity::getMAE(const cv::Mat& I1, const cv::Mat& I2)
{
	cv::Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	cv::Scalar s = sum(s1);        // sum elements per channel
    
    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels
    double mae = 1- (sse / (double)(I1.channels() * I1.total()*255));
    return mae;
}
