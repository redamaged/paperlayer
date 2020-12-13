#include <opencv2/opencv.hpp>
#include <iostream>

#include "helpers.hpp"
#include "algos.hpp"

using namespace std;
using namespace cv;

// Globals
bool finished=false;
Mat img,ROI, origin;
vector<Point> vertices;

void
CallBackFunc(int event,int x,int y,int flags,void* userdata)
{
    if(event==EVENT_RBUTTONDOWN){
        cout << "Right mouse button clicked at (" << x << ", " << y << ")" << endl;
        if(vertices.size()<3){
            cout << "You need a minimum of three points!" << endl;
            return;
        }
        // Close polygon
        line(img,vertices[vertices.size()-1],vertices[0],Scalar(0,0,0));
        imshow("ImageDisplay",img);

        // Mask is black with white where our ROI is
        Mat mask= Mat::zeros(img.rows,img.cols,CV_8UC1);
        
        vector<vector<Point>> pts{vertices};
        fillPoly(mask,pts,Scalar(255,255,255));
        
        origin.copyTo(ROI,mask);
        std::vector<cv::Mat> channels;
        cv::split(ROI, channels);
        channels.push_back(mask);
        cv::merge(channels, ROI);
        
        ROI = Helpers::cropToVisible(ROI);
        
        finished=true;
        
        Helpers::saveImageRandom(ROI, "out");
        
        return;
    }
    if(event==EVENT_LBUTTONDOWN){
        cout << "Left mouse button clicked at (" << x << ", " << y << ")" << endl;
        if(vertices.size()==0){
            // First click - just draw point
            img.at<Vec3b>(x,y)=Vec3b(255,255,255);
        } else {
            // Second, or later click, draw line to previous vertex
            line(img,Point(x,y),vertices[vertices.size()-1],Scalar(0,0,0));
        }
        vertices.push_back(Point(x,y));
        return;
    }
}

int main()
{
    img=imread("Lenna.png");
    origin =imread("Lenna.png");
    if(img.empty()){
        cout << "Error loading the image" << endl;
        exit(1);
    }
    
    //Create a window
    namedWindow("ImageDisplay",1);
    
    // Register a mouse callback
    setMouseCallback("ImageDisplay",CallBackFunc,nullptr);
    
    // Main loop
    while(!finished){
        imshow("ImageDisplay",img);
        waitKey(50);
    }
    
    // Show results
    namedWindow("Result",1);
    imshow("Result",ROI);
    waitKey();
}
