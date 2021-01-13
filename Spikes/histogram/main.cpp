#include <stdio.h>

#include "helpers.hpp"

using namespace cv;
using namespace std;

using namespace Helpers;



int main(int argc, char** argv )
{
    Mat image;
    image = imread( "../../../../Covers/3.jpg", IMREAD_UNCHANGED );
    image = addAplhaChannel(image);
    cout<< "channels:"<<  image.channels()<<endl;
    
    imshow("Display Image", image);

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
   
    
    Mat image2= Rotate(image,45);
    saveImageRandom(image2, "output");
    imshow("Output", image2);
    cout<< "channels:"<<  image2.channels()<<endl;

    
    waitKey(0);

    std::cout<< "Kaaaaaak"<< std::endl;
    
    
    return 0;
}

