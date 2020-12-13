/**
 * @file helpers.cpp
 * @brief Misc low level helper functions
 * @author Reda Maged
 */

#include "helpers.hpp"

using namespace std;
using namespace cv;

Mat Helpers::cropToVisible(Mat source)
{
    int maxX = 0, minX =source.cols , maxY=0, minY = source.rows;                // bounding box
    for (int r=0; r<source.rows; r++)
    {
        for (int c=0; c<source.cols; c++)
        {
            bool visible = (int)source.at<Vec4b>(Point(c, r))[3];
            if (visible)
            {
                minX = min(minX,c);
                minY = min(minY,r);
                maxX = max(maxX, c);
                maxY = max(maxY,r);
            }
        }
    }
    Rect myROI(minX, minY, maxX-minX, maxY-minY);
    
    
    return source(myROI);
}

Mat Helpers::addAplhaChannel(Mat source)
{
    cv::Mat newSrc(source.size(), CV_MAKE_TYPE(source.depth(), 4));
    cvtColor(source,newSrc,COLOR_BGR2BGRA);
    return newSrc;
}


void Helpers::saveImageRandom(Mat image, String filename)
{
    std::vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
    srand (time(NULL));
    std::ostringstream ss;
    long num = rand();
    ss << filename << "_"<< num << ".png";
    
    imwrite(ss.str(), image, compression_params);
}

Mat Helpers::Rotate(Mat source, double angle)
{
    // get rotation matrix for rotating the image around its center
    Point2f center(source.cols/2.0, source.rows/2.0);
    Mat rot = getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle
    Rect bbox = RotatedRect(center,source.size(), angle).boundingRect();
    // adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - center.x;
    rot.at<double>(1,2) += bbox.height/2.0 - center.y;

    Mat dst;
    warpAffine(source, dst, rot, bbox.size());
    
    return cropToVisible(dst);
}

Mat Helpers::removeAlphaChannel(Mat source)
{
    cv::Mat newSrc(source.size(), CV_MAKE_TYPE(source.depth(), 3));
    cvtColor(source,newSrc,COLOR_BGRA2BGR);
    return newSrc;
}

void Helpers::collatePatch(Mat src, Mat overlay, Point& location)
{
    for (int y = max(location.y, 0); y < src.rows; ++y)
    {
        int fY = y - location.y;

        if (fY >= overlay.rows)
            break;

        for (int x = max(location.x, 0); x < src.cols; ++x)
        {
            int fX = x - location.x;

            if (fX >= overlay.cols)
                break;

            double opacity = ((double)overlay.data[fY * overlay.step + fX * overlay.channels() + 3]) / 255;

            for (int c = 0; opacity > 0 && c < src.channels(); ++c)
            {
                unsigned char overlayPx = overlay.data[fY * overlay.step + fX * overlay.channels() + c];
                unsigned char srcPx = src.data[y * src.step + x * src.channels() + c];
                src.data[y * src.step + src.channels() * x + c] = srcPx * (1. - opacity) + overlayPx * opacity;
            }
        }
    }
}

Mat Helpers::multiplyAlphaChannel(Mat src, float value)
{
    return src.mul(Scalar(1,1,1,value));
}

