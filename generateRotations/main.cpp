#include <stdio.h>
#include <unistd.h>

#include <opencv2/opencv.hpp>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "helpers.hpp"

using namespace cv;
using namespace std;

using namespace Helpers;



int main(int argc, char** argv )
{


    Mat image_a;
    image_a = imread( "Lenna.png" , -1);
    Mat image = copyPolygon(image_a);
    cout<< "channels:"<<  image.channels()<<endl;
    
    imshow("Display Imagesss", image);

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
   
    
   
    for(int i=0; i<360; i+=10)
    {
        Mat image2= Rotate(image,i);
        saveImageRandom(image2, "output");
        usleep(1000000);
    }
    waitKey(0);

    std::cout<< "Kaaaaaak"<< std::endl;
    
    
    return 0;
}

