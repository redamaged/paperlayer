#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{


    Mat image;
    image = imread( "lena.jpg" );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Imagesss", image);

    waitKey(0);

    system("pause");
    std::cout<< "Kaaaaaak"<< std::endl;
    return 0;
}
