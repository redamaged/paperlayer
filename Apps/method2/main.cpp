#include <stdio.h>
#include "helpers.hpp"
#include "algos.hpp"

#include <Windows.h>
#include "dirent.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    string dir_address ="../../../../Data2/";
    string img_address = "../../../../Data/Lenna.png";
    Mat original= imread( img_address, IMREAD_UNCHANGED );
//   Mat collage = cv::Mat::zeros(original.size(), CV_8UC4);
    Mat collage(original.size(), CV_8UC3, Scalar::all(0));
//    Mat collage= imread("collage_conf90_2.png", IMREAD_UNCHANGED);
    Mat patch;
    Mat rotated_patch;

	Mat cover;

	Mat shard = imread("../../../../Data/shard.png", IMREAD_UNCHANGED);
	cover = imread("../../../../Covers/3.jpg", cv::IMREAD_UNCHANGED);
  
	Mat templ = imread("../../../../Data/shard.png", IMREAD_UNCHANGED);
	Mat display = cv::Mat::zeros(cover.size(), CV_8UC3);

	

    
	DIR *dir;
	struct dirent *ent;
	int total_count = 0 ;
	int admitted_count = 0 ;

	Point matchLoc;
	int rot_angle;
	double conf;

	cv::Mat rotated_templ;
	Algos::matchTemplate(cover, shard, display, 3, matchLoc, rotated_templ, rot_angle, conf);
	cout << total_count << ": ";
	//Algos::matchTemplate(cover, shard, collage, 3, matchLoc, rotated_patch, rot_angle, conf);
	//cv::Rect patchROI(matchLoc.x, matchLoc.y, rotated_patch.rows, rotated_patch.cols);
	//Mat coloredpatch = cover(patchROI);
	//imshow("coloredPatch", coloredpatch);
	cout << "done..........." << endl;





    if ((dir = opendir (dir_address.c_str())) != NULL)
    {

		for (int img_row = 0; img_row<512; img_row += (original.rows / 16))
			for (int img_cols = 0; img_cols < 512; img_cols += (original.cols / 16))
			{
				rectangle(original, Point(img_row, img_cols), Point(img_row+ (original.rows / 16), img_cols+ (original.cols / 16)), cv::Scalar(0, 255, 0));
				//circle(original, Point(img_row, img_cols), 1, Scalar(0, 255, 0), 5, 8, 0);
				//original.
				cv::Rect myROI(img_row, img_cols, original.rows / 16, original.cols / 16);
				patch = original(myROI);
				Helpers::addAplhaChannel(patch);
				imshow("Origin", original);
				imshow("temp_patch", patch);
				cv::waitKey(0);


				while ((ent = readdir(dir)) != NULL)
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
							//imshow("Collage", collage);
							//imshow("Origin", original);
							//waitKey(1);
							total_count++;
						}
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

