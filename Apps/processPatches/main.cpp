#include <stdio.h>
#include "helpers.hpp"
#include "algos.hpp"
#include <dirent.h>


using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    string dir_address ="../../../../Data2/";
    string img_address = "../../../../Data/Lenna.png";
    Mat original= imread( img_address, IMREAD_UNCHANGED );
    Mat collage = cv::Mat::zeros(original.size(), CV_8UC4);
    Mat patch;
    Mat rotated_patch;
    
    imshow("original", original);
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (dir_address.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            std::ostringstream ss;
            ss << dir_address<<  ent->d_name  ;
            if(((string)ent->d_name != "..") && ((string)ent->d_name != ".")
               && ss.str().substr(ss.str().length()-4,4) == ".png")
            {
                patch = imread(ss.str(), -1);
                if(patch.rows < original.rows &&
                   patch.cols < original.rows &&
                   patch.rows < original.cols &&
                   patch.rows < original.cols)
                {
                    Point matchLoc;
                    int rot_angle;
                    double conf;
                    
                    imshow("Patch", patch);
                    Algos::matchTemplate(original, patch, 3, matchLoc, rotated_patch, rot_angle, conf);
                    cout<< conf<< endl;
                    if(conf > 0.9)
                        Helpers::collatePatch(collage, rotated_patch, matchLoc);
                    
                    imshow("Coolage", collage);
                    waitKey(1);
                }
            }
        }
    }
    
    std::cout<< "Finished"<< std::endl;
    waitKey(0);
    Helpers::saveImageRandom(collage, "collage");
    
    return 0;
}

