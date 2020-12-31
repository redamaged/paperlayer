/**
 * @file helpers.cpp
 * @brief Misc low level helper functions
 * @author Reda Maged
 */

#include "helpers.hpp"
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

cv::Mat Helpers::cropToVisible(cv::Mat source)
{
    int maxX = 0, minX =source.cols , maxY=0, minY = source.rows;                // bounding box
    for (int r=0; r<source.rows; r++)
    {
        for (int c=0; c<source.cols; c++)
        {
            bool visible = (int)source.at<cv::Vec4b>(cv::Point(c, r))[3];
            if (visible)
            {
                minX = min(minX,c);
                minY = min(minY,r);
                maxX = max(maxX, c);
                maxY = max(maxY,r);
            }
        }
    }
	cv::Rect myROI(minX, minY, maxX-minX, maxY-minY);
    
    
    return source(myROI);
}

cv::Mat Helpers::addAplhaChannel(cv::Mat source)
{
    cv::Mat newSrc(source.size(), CV_MAKE_TYPE(source.depth(), 4));
    cvtColor(source,newSrc, cv::COLOR_BGR2BGRA);
    return newSrc;
}


void Helpers::saveImageRandom(cv::Mat image, cv::String filename)
{
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
    srand (time(NULL));
    std::ostringstream ss;
    long num = rand();
    ss << filename << "_"<< num << ".png";
    
    imwrite(ss.str(), image, compression_params);
}

void Helpers::saveImageLabeled(cv::Mat image, cv::String dirName, cv::String filename)
{
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
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


cv::Mat Helpers::Rotate(cv::Mat source, double angle)
{
    // get rotation matrix for rotating the image around its center
	cv::Point2f center(source.cols/2.0, source.rows/2.0);
	cv::Mat rot = getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle
	cv::Rect bbox = cv::RotatedRect(center,source.size(), angle).boundingRect();
    // adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - center.x;
    rot.at<double>(1,2) += bbox.height/2.0 - center.y;
    
	cv::Mat dst;
    warpAffine(source, dst, rot, bbox.size());
    
    return cropToVisible(dst);
}

cv::Mat Helpers::removeAlphaChannel(cv::Mat source)
{
    cv::Mat newSrc(source.size(), CV_MAKE_TYPE(source.depth(), 3));
    cvtColor(source,newSrc, cv::COLOR_BGRA2BGR);
    return newSrc;
}

void Helpers::collatePatch(cv::Mat src, cv::Mat overlay, cv::Point& location)
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
}


void Helpers::collateNoise(cv::Mat src, cv::Mat overlay, cv::Point& location)
{
    cv::Mat noise = overlay.clone();
    cv::randn(noise, (255,255,255), (255,255,255));
    std::vector<cv::Mat> tmpl_channels;
    cv::split(overlay, tmpl_channels);
    std::vector<cv::Mat> noise_channels;
    cv::split(noise, noise_channels);
    noise_channels.pop_back();
    noise_channels.push_back(tmpl_channels.at(3));
    cv::merge(noise_channels, noise);
    Helpers::collatePatch(src, noise, location);
}


cv::Mat Helpers::multiplyAlphaChannel(cv::Mat src, float value)
{
    return src.mul(cv::Scalar(1,1,1,value));
}



static void paint_voronoi(const cv::Mat img, cv::Mat& display, cv::Subdiv2D& subdiv, cv::String dirName, bool save)
{
    vector<vector<cv::Point2f> > facets;
    vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(vector<int>(), facets, centers);
    vector<cv::Point> ifacet;
    vector<vector<cv::Point> > ifacets(1);
    for (size_t i = 0; i < facets.size(); i++)
    {
        ifacet.resize(facets[i].size());
        for (size_t j = 0; j < facets[i].size(); j++)
        ifacet[j] = facets[i][j];
		cv::Mat mask= cv::Mat::zeros(img.rows,img.cols,CV_8UC1);
        fillPoly(mask,ifacet, cv::Scalar(255,255,255));
		cv::Mat ROI;
        img.copyTo(ROI,mask);
        std::vector<cv::Mat> channels;
        cv::split(ROI, channels);
        channels.push_back(mask);
        cv::merge(channels, ROI);
        ROI = Helpers::cropToVisible(ROI);
        if(save)
            Helpers::saveImageLabeled(ROI, dirName,to_string(i));
        ifacets[0] = ifacet;
        polylines(display, ifacets, true, cv::Scalar(0,255,0), 1, cv::LINE_AA, 0);
    }
}

cv::Mat Setup::subdivideImage(cv::Mat img, int patchCount, cv::String dir_name, bool save)
{
	cv::Mat display = img.clone();
	cv::Scalar active_facet_color(0, 0, 255), delaunay_color(255, 255, 255);
	cv::Rect rect(0, 0, img.cols, img.rows);
	cv::Subdiv2D subdiv(rect);
    for (int i = 0; i < patchCount; i++)
    {
		cv::Point2f fp((float)(rand() % (rect.width - 10) + 5),
                   (float)(rand() % (rect.height - 10) + 5));
		subdiv.insert(fp);
    }
    paint_voronoi(img, display,subdiv, dir_name, save);
    return display;
}
