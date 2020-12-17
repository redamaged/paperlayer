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


void recursive_tri(Mat img, vector<Point> contour,  Mat &display, String dir)
{
	// Draw The current Triangle
	line(display, contour[0], contour[1], Scalar(0, 0, 0));
	line(display, contour[1], contour[2], Scalar(0, 0, 0));
	line(display, contour[2], contour[0], Scalar(0, 0, 0));

	cout << "Contour Area: " << contourArea(contour) << endl;
	if(contourArea(contour)< 20000)
	{ }
	else
	{


		float m1; 
		float b1;

		if ((contour[0].x - contour[1].x) == 0) m1 = 0;
		else m1= (contour[0].y - contour[1].y) / (contour[0].x - contour[1].x);
		if ((m1 * contour[0].x) == 0) b1 = 0;
		else  b1 = contour[0].y / (m1 * contour[0].x);

		int min = (contour[0].x < contour[1].x) ? contour[0].x : contour[1].x;
		float x1 = min + (std::abs(contour[0].x - contour[1].x)) / 2;
		float y1 = m1 * x1 + b1;

		cout << "x1:" << x1 << endl;
		cout << "y1:" << y1 << endl;
		//////////////////
		if ((contour[1].x - contour[2].x) == 0) m1 = 0;
		else m1 = (contour[1].y - contour[2].y) / (contour[1].x - contour[2].x);
		if ((m1 * contour[1].x) == 0) b1 = 0;
		else b1 = contour[1].y / (m1 * contour[1].x);

		min = (contour[1].x < contour[2].x) ? contour[1].x : contour[2].x;
		float x2 = min + (std::abs(contour[1].x - contour[2].x)) / 2;
		float y2 = m1 * x1 + b1;	
		
		cout << "m2:" << m1 << endl;
		cout << "b1:" << b1 << endl;
		cout << "x2:" << x2 << endl;
		cout << "y2:" << y2 << endl;
		/////////////////
		if ((contour[2].x - contour[0].x) == 0)
		{
			m1 = 0; b1 = contour[2].y;
		}
		else
		{
			m1 = (contour[2].y - contour[0].y) / (contour[2].x - contour[0].x);
			b1 = contour[2].y / (m1 * contour[2].x);
		}

		min = (contour[2].x < contour[0].x) ? contour[2].x : contour[0].x;
		float x3 = min + (std::abs(contour[2].x - contour[0].x)) / 2;
		float y3 = m1 * x1 + b1;
		
		Point a(x1,x2);
		Point b(x2, y2);
		Point c((contour[2].x + contour[0].x) / 2, (contour[2].y + contour[0].y) / 2);

		cv::circle(display, a, 10, Scalar(255, 0, 0), 2);
		cv::circle(display, b, 10, Scalar(0, 255, 0), 2);
	//	cv::circle(display, c, 10, Scalar(128, 128, 128), 2);

		cv::circle(display, contour[0], 20, Scalar(0, 0, 0), 2);
		cv::circle(display, contour[1], 20, Scalar(255, 255, 255), 2);
		cv::circle(display, contour[2], 20, Scalar(127, 127, 127), 2);
		//recursive_tri(img, { contour[0], a, c }, display, dir);
		//recursive_tri(img, { contour[1], a, b }, display, dir);
		//recursive_tri(img, { contour[2], b, c }, display, dir);
		//recursive_tri(img, { a, b, c }, display, dir);

	}

}

Mat Helpers::puzzle(Mat img, String dir_name)
{
	Mat display = img.clone();

	vector<Point> contour1 = { Point(0,0), Point(img.cols,img.rows), Point(img.cols,0) };
	vector<Point> contour2 = { Point(0,0), Point(img.cols,img.rows), Point(0,img.rows) };
	//recursive_tri(img, contour1, display, dir_name);
	recursive_tri(img, contour2, display, dir_name);

	return display;
}