//#include "imageProcessing.cpp"
#include "filters.cpp"
      
#include <dirent.h>
 
  
/// Global variables
Mat image;
Mat patch;
Mat art;
Mat art_ref; 
Mat art_ref_tuned;
Mat avialabilty; 
Mat details; 

vector <vector < vector <double> > > result_scores;						/// container for filter output	
orineted_position oriented_pos;
double filter_patch_err=0;
   
orineted_position new_oriented_pos;
double tuned_filter_patch_err=0;        

/** @function main */
                
        
 
int main( int argc, char** argv )
 {	 
	/// Load the images
	image = imread( argv[1], 1 );
	image = convert_BGRtoBGRA(image);
	patch = imread( argv[2], 1 );	
	patch = convert_BGRtoBGRA(patch);
	 
	/// init the ART mats
	art_ref = copyPolygon(image);
	art_ref_tuned = copyPolygon(image);
	art = Mat::zeros( image.rows, image.cols, CV_8UC4 );
	art = Scalar( 255, 255, 255, 255 );	
    avialabilty = 	Mat::zeros( image.rows, image.cols, CV_8UC4 );
    details = 		Mat::zeros( image.rows, image.cols, CV_8UC4 );
    
	int count = 0;	
	int addmited = 0;
	
	double total_filter_time = 0; 
	double total_tuned_filter_time = 0; 
	double total_session_time = 0; 
	clock_t session_begin = clock();   


	vector< vector <double> >  a_mat;	
	a_mat = InitAvailabilityMatrix(image, art);
	
	for(int x=0; x<art_ref_tuned.cols; x++)
	{
		for(int y=0; y<art_ref_tuned.rows; y++)
		{
			Vec4b p1 ;	
			p1[0]= p1[1]= p1[2]= (a_mat[x][y]/441.67)*255;
			 avialabilty.at<Vec4b>(Point(x, y)) = p1;
		}
	}	
	
	vector< vector <double> >  details_matrix;	
	details_matrix = InitDetailsMatrix(image, 10);
	for(int x=0; x<image.cols; x++)
	{
		for(int y=0; y<image.rows; y++)
		{
			Vec4b p1 ;	
			p1[0]= p1[1]= p1[2]= (details_matrix[x][y]/441.67)*255;
			details.at<Vec4b>(Point(x, y)) = p1;
		}
	}
	
	//imshow("image tr", transparency_check(image)); 
	//imshow("image", (image)); 
	//imshow("art", (art)); 
	//imshow("avialabilty", (avialabilty)); 
	//imshow("details", (details)); 
	 
	//puzzleImage(image, 2500, image.cols, image.rows);
	 
	
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("./puzzle")) != NULL) 
	{
	 
	  while ((ent = readdir (dir)) != NULL) 
	  //while (0) 
	  {		
		
		if(((string)ent->d_name != "..") && ((string)ent->d_name != "."))
		{
			std::ostringstream ss;
			ss << "./puzzle/"<<  ent->d_name  ; 
			cout << ss.str()<< endl;
			patch = imread(ss.str(), -1);
			//imshow("temp tran", transparency_check(temp)); 
			
		
		if(patch.rows < image.rows &&
			patch.cols < image.rows &&
			patch.rows < image.cols &&
			patch.rows < image.cols)	
		{
		count++;
		
				
		/// Run the template matching filter
		clock_t begin_filter = clock(); 				
		CalcImrpovementMatrix(	image,
								patch,
								a_mat,
								8,
								oriented_pos,
								filter_patch_err);		
		clock_t end_filter = clock();		

		/// Run the Fine Tune template matching filter
		clock_t begin_tunefilter = clock(); 
		CalcImrpovementMatrixTuned(	image,										/// source img
									patch, 										/// patch
									a_mat,
									oriented_pos, 								/// seed orinted position(out from filter_fast)
									4,											/// search radius in x,y (usually half x,y jumps)
									new_oriented_pos ,							/// OUTPUT found best orinted position 
									tuned_filter_patch_err);					/// OUTPUT error of found best oriented positon
		clock_t end_tunefilter = clock();
		
				
		 
		/// Drawing in ART mats	
				
		if(tuned_filter_patch_err>0)
		{  
			addmited++;	
			UpdateAvailabiltyMatrix(image, patch, new_oriented_pos, a_mat);
			collatePatch(art, patch, new_oriented_pos);	
			collatePatch(art_ref, patch, new_oriented_pos);
			
			for(int x=0; x<art_ref_tuned.cols; x++)
			{
				for(int y=0; y<art_ref_tuned.rows; y++)
				{
					Vec4b p1 ;	
					p1[0]= p1[1]= p1[2]= (a_mat[x][y]/441.67)*255;
					 avialabilty.at<Vec4b>(Point(x, y)) = p1;
				}
			}
				
		}
		
		
		
		/// Display all Mats
		imshow( "Source", image );
		imshow( "Patch", transparency_check( patch) );
		imshow( "ART", art );
		imshow( "new_rotated_patch", Rotate(patch,new_oriented_pos.r) );
		imshow("avialabilty", (avialabilty));
		imshow("art_ref", (art_ref));
		
		
		
		/// Compute times
		double filter_elapsed_secs = double(end_filter - begin_filter) / CLOCKS_PER_SEC;
		double tune_filter_elapsed_secs = double(end_tunefilter - begin_tunefilter) / CLOCKS_PER_SEC;
		total_filter_time += filter_elapsed_secs;
		total_tuned_filter_time += tune_filter_elapsed_secs;
		total_session_time = double(clock()- session_begin) / CLOCKS_PER_SEC;
		 
		/// Printing
		cout<< "----------------------------------"<< endl;
		cout<< "DONE"<< endl;
		cout<< "Image Area: "<<patchArea(image) << endl;
		cout<< "Patch Area: "<<patchArea(patch)<< endl;
		cout<< "----------------------------------"<< endl;
		cout<< "Found in x: " << oriented_pos.x << "  y: " << oriented_pos.y << " at rotation: " << oriented_pos.r << endl; 		
		cout<< "Estimated Improvement: "<<filter_patch_err << endl;
		cout<< "Filter Processing Time: "<<filter_elapsed_secs<< " sec"<<  endl;
		cout<< "----------------------------------"<< endl;
		cout<< "Tuned Found in x: " << new_oriented_pos.x << "  y: " << new_oriented_pos.y << " at rotation: " << new_oriented_pos.r << endl; 		
		cout<< "Tuned Estimated Improvement: "<<tuned_filter_patch_err << endl;
		cout<< "Tuned Filter Processing Time: "<<tune_filter_elapsed_secs<< " sec"<<  endl;
		cout<< "----------------------------------"<< endl; 
		cout<< "Total Generated Count: "<< count<< " Total Admitted: " << addmited << endl;
		cout<< "Total Filter Processing Time: " <<total_filter_time << endl;
		cout<< "Total Tuned Filter Processing Time: "<< total_tuned_filter_time << endl;
		cout<< "Total Session Time: "<< total_session_time << endl;

		 
		}
		waitKey(30);
			
		}
	  }
	  closedir (dir);
	} 
	
	saveImage(art, "art");
	saveImage(art_ref_tuned, "art_ref_tuned");
	cout<< "--------------------------------------" << endl;
	cout<< "END OF SESSION" << endl;
	
	waitKey();
		
	
	
	while(0)
	{	
		count++;
		patch = generateGrayscalePoly(17);			
				
		/// Run the template matching filter
		clock_t begin_filter = clock(); 		
		result_scores = filter_fast(image,								/// source img
									patch, 								/// patch
									10, 									/// x, y jumps (larger = faster)
									oriented_pos ,						/// OUTPUT found best orinted position 
									filter_patch_err);					/// OUTPUT error of found best oriented positon
		clock_t end_filter = clock();
					

		/// Run the Fine Tune template matching filter
		clock_t begin_tunefilter = clock(); 
		filter_fine_tune(	image,										/// source img
							patch, 										/// patch
							oriented_pos, 								/// seed orinted position(out from filter_fast)
							5,											/// search radius in x,y (usually half x,y jumps)
							new_oriented_pos ,							/// OUTPUT found best orinted position 
							tuned_filter_patch_err);					/// OUTPUT error of found best oriented positon
		clock_t end_tunefilter = clock();
		
				
		
		/// Drawing in ART mats	
				
		if(tuned_filter_patch_err<20)
		{  
			addmited++;	
			collatePatch(art, patch, new_oriented_pos);	
			collatePatch(art_ref_tuned, patch, new_oriented_pos);
			
		}			
			
		
		/// Display all Mats
		imshow( "Source", image );
		imshow( "Patch", transparency_check( patch) );
		imshow( "ART", art );
		imshow( "new_rotated_patch", Rotate(patch,new_oriented_pos.r) );
		imshow("art_ref_tuned", (art_ref_tuned));
		
		
		/// Compute times
		double filter_elapsed_secs = double(end_filter - begin_filter) / CLOCKS_PER_SEC;
		double tune_filter_elapsed_secs = double(end_tunefilter - begin_tunefilter) / CLOCKS_PER_SEC;
		total_filter_time += filter_elapsed_secs;
		total_tuned_filter_time += tune_filter_elapsed_secs;
		total_session_time = double(clock()- session_begin) / CLOCKS_PER_SEC;
		 
		/// Printing
		cout<< "----------------------------------"<< endl;
		cout<< "DONE"<< endl;
		cout<< "Image Area: "<<patchArea(image) << endl;
		cout<< "Patch Area: "<<patchArea(patch)<< endl;
		cout<< "----------------------------------"<< endl;
		cout<< "Found in x: " << oriented_pos.x << "  y: " << oriented_pos.y << " at rotation: " << oriented_pos.r << endl; 		
		cout<< "Estimated Error: "<<filter_patch_err << endl;
		cout<< "Filter Processing Time: "<<filter_elapsed_secs<< " sec"<<  endl;
		cout<< "----------------------------------"<< endl;
		cout<< "Tuned Found in x: " << new_oriented_pos.x << "  y: " << new_oriented_pos.y << " at rotation: " << new_oriented_pos.r << endl; 		
		cout<< "Tuned Estimated Error: "<<tuned_filter_patch_err << endl;
		cout<< "Tuned Filter Processing Time: "<<tune_filter_elapsed_secs<< " sec"<<  endl;
		cout<< "----------------------------------"<< endl; 
		cout<< "Total Generated Count: "<< count<< " Total Admitted: " << addmited << endl;
		cout<< "Total Filter Processing Time: " <<total_filter_time << endl;
		cout<< "Total Tuned Filter Processing Time: "<< total_tuned_filter_time << endl;
		cout<< "Total Session Time: "<< total_session_time << endl;

		
		
		waitKey(30);
	
	} /// end of while(1) loop
	
	
	return 0; 
  }   
