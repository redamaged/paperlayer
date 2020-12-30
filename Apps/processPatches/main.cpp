#include <stdio.h>
#include "helpers.hpp"
#include "algos.hpp"

#ifdef _WIN32
#include "win_dirent.h"  // Windows
#else
#include <dirent.h>     // MacOS or Ubuntu
#endif


using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    string dir_address ="../../../../Data15/";
    string img_address = "../../../../Data/Lenna.png";
    Mat original= imread( img_address, IMREAD_UNCHANGED );
//   Mat collage = cv::Mat::zeros(original.size(), CV_8UC4);
    Mat collage(original.size(), CV_8UC3, Scalar::all(0));
//    Mat collage= imread("collage_conf90_2.png", IMREAD_UNCHANGED);
    Mat patch;
    Mat rotated_patch;
    
    imshow("original", original);
    
    DIR *dir;
    struct dirent *ent;
    int total_count = 0 ;
    int admitted_count = 0 ;
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
                    cout<< total_count<< ": ";
                    Algos::matchTemplate(original, patch, collage, 3, matchLoc, rotated_patch, rot_angle, conf);
                    
                    if(conf > 0)
                    {
                        Helpers::collatePatch(collage, rotated_patch, matchLoc);
                        admitted_count++;
                    }
                    imshow("Coolage", collage);
                    imshow("original", original);
                    waitKey(1);
                    total_count++;
                }
            }
        }
    }
    
    std::cout<< "-----------------"<< std::endl;
    std::cout<< "Finished"<< std::endl;
    std::cout<< "Total Patches:"<< total_count<< std::endl;
    std::cout<< "Collated Patches: "<< admitted_count << std::endl;
    waitKey(0);
    Helpers::saveImageRandom(collage, "collage");
    
    return 0;
}

