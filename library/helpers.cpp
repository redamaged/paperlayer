/**
 * @file helpers.cpp
 * @brief Misc low level helper functions
 * @author Reda Maged
 */

#include "helpers.hpp"


#include <sys/types.h>
#include <sys/stat.h>


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

void Helpers::saveImageLabeled(Mat image, String dirName, String filename)
{
    std::vector<int> compression_params;
    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
    std::ostringstream ss;
    
    
    struct stat s;
    int err = stat(dirName.c_str(), &s);
    if(-1 == err) {
        if(ENOENT == errno) {
            cout << "Creating " << dirName << " directory ..."<< endl;
            string folderCreateCommand = "mkdir " + dirName;
            system(folderCreateCommand.c_str());
        }
    }
    ss << dirName<< "/"<<  filename << ".png";
    
    cout << "Saving patch to:" << ss.str()<<endl;
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
            if (opacity>0) opacity = 1;
            
            for (int c = 0; opacity > 0 && c < src.channels(); ++c)
            {
                unsigned char overlayPx = overlay.data[fY * overlay.step + fX * overlay.channels() + c];
                unsigned char srcPx = src.data[y * src.step + x * src.channels() + c];
                src.data[y * src.step + src.channels() * x + c] = srcPx * (1. - opacity) + overlayPx * opacity;
            }
        }
    }
    
    
    /*
    for(int i = location.x; i<(location.x+overlay.cols) ; i++ )
    {
        for(int j= location.y ; j <(location.y+overlay.rows); j ++)
        {
            if(overlay.at<Vec4b>(Point(i-location.x,j-location.y))[3] > 0)
            {
                src.at<Vec4b>(Point(i,j))[0] =
                overlay.at<Vec4b>(Point(i-location.x,j-location.y))[0];
                src.at<Vec4b>(Point(i,j))[1] =
                overlay.at<Vec4b>(Point(i-location.x,j-location.y))[1];
                src.at<Vec4b>(Point(i,j))[2] =
                overlay.at<Vec4b>(Point(i-location.x,j-location.y))[2];
                src.at<Vec4b>(Point(i,j))[3] = 255;
            }
        }
    }*/
}

Mat Helpers::multiplyAlphaChannel(Mat src, float value)
{
    return src.mul(Scalar(1,1,1,value));
}



static void paint_voronoi(const Mat img, Mat& display, Subdiv2D& subdiv, String dirName, bool save)
{
    vector<vector<Point2f> > facets;
    vector<Point2f> centers;
    subdiv.getVoronoiFacetList(vector<int>(), facets, centers);
    vector<Point> ifacet;
    vector<vector<Point> > ifacets(1);
    for (size_t i = 0; i < facets.size(); i++)
    {
        ifacet.resize(facets[i].size());
        for (size_t j = 0; j < facets[i].size(); j++)
        ifacet[j] = facets[i][j];
        Mat mask= Mat::zeros(img.rows,img.cols,CV_8UC1);
        fillPoly(mask,ifacet,Scalar(255,255,255));
        Mat ROI;
        img.copyTo(ROI,mask);
        std::vector<cv::Mat> channels;
        cv::split(ROI, channels);
        channels.push_back(mask);
        cv::merge(channels, ROI);
        ROI = Helpers::cropToVisible(ROI);
        if(save)
            Helpers::saveImageLabeled(ROI, dirName,to_string(i));
        ifacets[0] = ifacet;
        polylines(display, ifacets, true, Scalar(0,255,0), 1, LINE_AA, 0);
    }
}

Mat Setup::subdivideImage(Mat img, int patchCount, String dir_name, bool save)
{
    Mat display = img.clone();
    Scalar active_facet_color(0, 0, 255), delaunay_color(255, 255, 255);
    Rect rect(0, 0, img.cols, img.rows);
    Subdiv2D subdiv(rect);
    for (int i = 0; i < patchCount; i++)
    {
        Point2f fp((float)(rand() % (rect.width - 10) + 5),
                   (float)(rand() % (rect.height - 10) + 5));
        subdiv.insert(fp);
    }
    paint_voronoi(img, display,subdiv, dir_name, save);
    return display;
}
