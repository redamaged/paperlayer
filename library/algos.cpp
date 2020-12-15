/**
 * @file algos.cpp
 * @brief Template matchers and location optimizers function
 * @author Reda Maged
 */

#include "algos.hpp"
#include "helpers.hpp"

using namespace std;
using namespace cv;



class RandomFillBody : public ParallelLoopBody
{
    Mat3b _img;
    double m_rough_confidences[36];
    Point m_rough_locations[36];

public:
    RandomFillBody(Mat3b& img
                  , double (&_rough_confidences)[36], Point (&_rough_locations)[36]
    )
    {
        _img = img;
      //  m_rough_locations = _rough_locations;
    }

    void operator()(const Range& range) const override
    {
        theRNG().state = getTickCount();
        Vec3b color{ theRNG(), theRNG(), theRNG() };

        for (int i = range.start; i < range.end; ++i)
        {
            int x = i % 8, y = i / 8;
            _img(Range{ y * 100, (y + 1) * 100 }, Range{ x * 100, (x + 1) * 100 }) = color;
        }
    }
};




void Algos::matchTemplate(Mat img, Mat templ, int matchMethod, Point& matchLocation, Mat& rotatedTempl, int& rotationAngle, double& confidence)
{
    confidence = 0;
    
    double _rough_confidences[36];
//    Mat _rough_confidences(1,32,CV_64FC1);
    
    Point _rough_locations[36];
    double _fine_confidences[10];
    Point _fine_locations[10];
    int _angle= rotationAngle= 0;
    
    Mat _result;
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;
    
    _result.create( result_rows, result_cols, CV_32FC1 );
    
    
    
    
    
    double minVal; double maxVal; Point minLoc; Point maxLoc;
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
