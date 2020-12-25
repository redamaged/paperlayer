#include <opencv2/opencv.hpp>
#include <iostream>

#include "helpers.hpp"
#include "algos.hpp"

using namespace std;
using namespace cv;

// Globals
bool finished=false;
Mat img,ROI, origin, img_display;
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
//        /Users/reda/Projects/paperlayer/Apps/annotation/main.cpp
        vector<vector<Point>> pts{vertices};
        fillPoly(mask,pts,Scalar(255,255,255));
        
        origin.copyTo(ROI,mask);
        std::vector<cv::Mat> channels;
        cv::split(ROI, channels);
        channels.push_back(mask);
        cv::merge(channels, ROI);
        
        ROI = Helpers::cropToVisible(ROI);
        
        finished=true;

		ROI = Helpers::Rotate(ROI, 273);
        
        Helpers::saveImageRandom(ROI, "out");

		Point matchLoc(9,99);
        
        
		origin.copyTo(img_display);

		Mat rotated_templ;
		int rotation_angle;
		double acc;
		Algos::matchTemplate(origin, ROI, img_display, 1, matchLoc, rotated_templ, rotation_angle, acc);
		cout << "Confidence:" << acc << endl;
		cout << "Rotation Angle:" << rotation_angle << endl;
        cout <<"matchLoc: " << matchLoc << endl;
		rectangle(img_display, matchLoc, Point(matchLoc.x + rotated_templ.cols, matchLoc.y + rotated_templ.rows), Scalar::all(0), 2, 8, 0);

		rotated_templ = Helpers::multiplyAlphaChannel(rotated_templ,1);

		Helpers::collatePatch(img_display, rotated_templ, matchLoc);

        
		imshow("result", img_display);
        imshow("ImageDisplay",origin);
  
        
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
    img     =imread("../../../../Data/Lenna.png");
    origin  =imread("../../../../Data/Lenna.png");
    img_display = cv::Mat::zeros(origin.size(), CV_8UC4);
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
