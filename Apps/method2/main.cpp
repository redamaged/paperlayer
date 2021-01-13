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
    string dir_address ="../../../../Covers/";
    string img_address = "../../../../Data/puppy512.png";
    Mat original= imread( img_address, IMREAD_UNCHANGED );
//   Mat collage = cv::Mat::zeros(original.size(), CV_8UC4);
//    Mat collage(original.size(), CV_8UC3, Scalar::all(0));
//    Mat collage= imread("collage_conf90_2.png", IMREAD_UNCHANGED);
   Mat collage= imread("../../../../Data/pup.png", IMREAD_UNCHANGED);

   // Mat patch;
    Mat rotated_patch;

	Mat cover;

	cover = imread("../../../../Covers/13.jpg", IMREAD_COLOR);
  


    
	DIR *dir;
	struct dirent *ent;
	int total_count = 0 ;
	int admitted_count = 0 ;
    
    Point matchLoc;
    int rot_angle;
    double conf;
    
    cv::Mat rotated_templ;
    //Algos::matchTemplate(cover, shard, display, 3, matchLoc, rotated_templ, rot_angle, conf);
    //Algos::matchTemplate(cover, shard, collage, 3, matchLoc, rotated_patch, rot_angle, conf);
    //cv::Rect patchROI(matchLoc.x, matchLoc.y, rotated_patch.rows, rotated_patch.cols);
    //Mat coloredpatch = cover(patchROI);
    //imshow("coloredPatch", coloredpatch);
    cout << "Loaded..........." << endl;
    
    
    
    imshow("Origin", original);
    imshow("Collage", collage);
    imshow("Cover", cover);
    cv::waitKey(1);
    
    
    
    if ((dir = opendir (dir_address.c_str())) != NULL)
    {
        
        for (int img_row = 0; img_row<512; img_row += ((original.rows / 16)))
        for (int img_cols = 0; img_cols < 512; img_cols += ((original.cols / 16)))
        {
            cv::Rect myROI(img_row, img_cols, original.rows / 16, original.cols / 16);
            Mat patch = original(myROI);
            Mat mask(cv::Size(patch.rows,patch.cols), CV_8UC1, Scalar::all(255));
            
            std::vector<cv::Mat> patch_channels;
            cv::split(patch, patch_channels);
            patch_channels.push_back(mask);
            cv::merge(patch_channels, patch);
            
            //                patch = Helpers::Rotate(patch, 45);
            //Helpers::saveImageRandom(patch, "patch");
            
            Point p(img_row,img_cols);
            
            cv::Rect colROI(img_row, img_cols, original.rows / 16, original.cols / 16);
            Mat col_patch = collage(colROI);
            col_patch = Helpers::addAplhaChannel(col_patch);
            Scalar MSSIM = Similarity::getMSSIM(patch, col_patch);
            cout << " MSSIM:" << MSSIM[0]<< endl;
            if(MSSIM[0]<0.9)
            {
                Algos::matchTemplate(cover, patch, 3, matchLoc, rot_angle, conf);
                rotated_patch = Helpers::Rotate(patch, rot_angle);
                std::vector<cv::Mat> rot_patch_channels;
                cv::split(rotated_patch, rot_patch_channels);
                
                cv::Rect new_ROI (matchLoc.x, matchLoc.y, rotated_patch.rows, rotated_patch.cols);
                
                Mat new_patch = cover(new_ROI);
                cout<< new_patch.channels() <<  "..." ;
                new_patch= Helpers::addAplhaChannel(new_patch);
                cout<< new_patch.channels() <<  endl ;
                std::vector<cv::Mat> patch_channels2;
                cv::split(new_patch, patch_channels2);
                patch_channels2.pop_back();
                
                Mat dst;
                resize(rot_patch_channels.at(3), dst, Size(patch_channels2.at(0).cols, patch_channels2.at(0).rows), 0, 0, INTER_CUBIC);
                
                
                patch_channels2.push_back(dst);
                cout << "1:" << dst.cols << "  2:" <<patch_channels2.at(0).cols<< endl;
                
                
                cv::merge(patch_channels2, new_patch);
                cout<< new_patch.channels() <<  endl ;
                
                Helpers::collatePatch(collage, Helpers::Rotate(new_patch, rot_angle*-1),p);
                Helpers::collateNoise(cover, Helpers::Rotate(patch, rot_angle), matchLoc);
            
                imshow("new patch", new_patch);
                imshow("rotated patch", rotated_patch);
                imshow("Origin", original);
                imshow("temp_patch", patch);
                imshow("Collage", collage);
                imshow("Cover", cover);
                cv::waitKey(1);
            }
                
                
                
                cout << "Position" << ": " << matchLoc<< "           ANGLE: " << rot_angle<< "       Accuracy: "<< conf<< endl;


				/*while ((ent = readdir(dir)) != NULL)
				{
					std::ostringstream ss;
					ss << dir_address << ent->d_name;
					if (((string)ent->d_name != "..") && ((string)ent->d_name != ".")
						&& ss.str().substr(ss.str().length() - 4, 4) == ".png")
					{
						patch = imread(ss.str(), IMREAD_UNCHANGED);
						imshow("Cover", cover);
						cv::waitKey(0);

						if (patch.rows < cover.rows &&
							patch.cols < cover.rows)
						{
						

							

							if (conf > 0)
							{
								//Helpers::collatePatch(collage, coloredpatch, matchLoc);
								admitted_count++;
							}
							imshow("Collage", collage);
							imshow("Origin", original);
							waitKey(1);
                            cout << total_count << ": " << endl;

							total_count++;
						}
					}
				}*/

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

