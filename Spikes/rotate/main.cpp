#include <stdio.h>
#include <unistd.h>

#include <opencv2/opencv.hpp>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

Mat copyPolygon(Mat source)
{
    cv::Mat newSrc(source.size(), CV_MAKE_TYPE(source.depth(), 4));
    cvtColor(source,newSrc,COLOR_BGR2BGRA);
    return newSrc;
}

void saveImage(Mat image, String filename)
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

Mat Rotate(Mat source, double angle)
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
    

    int maxX = 0, minX =dst.cols , maxY=0, minY = dst.rows;                // bounding box
    for (int r=0; r<dst.rows; r++)
    {
        for (int c=0; c<dst.cols; c++)
        {
            bool visible = (int)dst.at<Vec4b>(Point(c, r))[3];
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
    
    
    return dst(myROI);
}

int main(int argc, char** argv )
{


    Mat image_a;
    image_a = imread( "nose.png" , -1);
    Mat image = copyPolygon(image_a);
    cout<< "channels:"<<  image.channels()<<endl;
    
    imshow("Display Imagesss", image);

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
   
    
    namedWindow("Display Image", WINDOW_AUTOSIZE );
   
    for(int i=0; i<360; i+=10)
    {
        Mat image2= Rotate(image,i);
        saveImage(image2, "output");
        usleep(1000000);
    }
    waitKey(0);

    std::cout<< "Kaaaaaak"<< std::endl;
    
    
    
    
    
    
    
    
    
    
    return 0;
}

