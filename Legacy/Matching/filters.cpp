#include "imageProcessing.cpp"


vector <vector< vector <double> > > filter_fast(Mat image_mat, 
												Mat patch_mat,
												int jump_steps, 
												orineted_position &result_op,
												double & result_error)
{
	//op; op.x=0 ; op.y=0; op.r=0; 
	result_error = 100000000000000 ;	
	int W= image_mat.cols; int H = image_mat.rows;		
	
	// creation and initialization of patch rotations array
	Mat patch_arr[36];
	int patch_area[36];
	for(int r = 0; r<36 ; r++)
	{
		patch_arr[r] = Rotate(patch_mat, r*10);
		patch_area[r] =patchArea(patch_arr[r]);
	}	
	
	
	/// creation of rotates patches scores array //  
    vector <vector< vector <double> > > rot_patch_scores;	
    for(int r= 0; r<36; r++)   											/// number of rotations
    {
		vector< vector <double> >  patch_scores;
		for(int i = 0; i<patch_arr[r].cols; ++i )						/// number of cols
		{
			vector<double> p;       									     
			p.resize(patch_arr[r].rows);            					/// number of rows ... Length of Row-Vector is n
			patch_scores.push_back(p);         
		}
		rot_patch_scores.push_back(patch_scores);						
	}
	
	
    /// creation of final results scores array //  
    vector < vector< vector <double> > > rot_result_scores;
    for(int r= 0; r<36; r++)   											/// number of rotations
    {
		vector< vector <double> > result_scores;	  
		for(int i = 0; i<W-patch_arr[r].cols; ++i )						/// number of cols
		{
			
			vector<double> p; 
			p.resize(H-patch_arr[r].rows);            					/// number of rows ... Length of Row-Vector is n
			result_scores.push_back(p);         
		}
		 rot_result_scores.push_back(result_scores);					
	}
    
	/// Initialization of both score arrays //        
    for(int r = 0; r<36 ; r++)
    {
		for(int i = 0; i<patch_arr[r].cols; i++)
		{
			for(int j = 0; j<patch_arr[r].rows; j++){
				rot_patch_scores[r][i][j]=0;
			}
		}  
	}  
    for(int r = 0; r<36 ; r++)
    {
		for(int i = 0; i<W-patch_arr[r].cols; i++)
		{
			for(int j = 0; j<H-patch_arr[r].rows; j++){
				rot_result_scores[r][i][j]=10000;						/// the big vaule hepls in jumping. When 
			} 															/// looking for minima,this val will be discarded
		}
    }
	

	/// loop the patch on the image, in all rotated positions	
	
	for(int r = 0; r<36; r++)											/// patch rotation
	{				
		int w1= patch_arr[r].cols; int h1 = patch_arr[r].rows;
		//cout<< ((float)r / float(36))*100 << "%" << endl;
		
		for(int i=0 ; i < W-w1; i+=jump_steps)									/// patch position in image
		{			
			for(int j=0; j<H-h1; j+=jump_steps)
			{															///patch

				for(int x=0; x<w1 ; x++)								/// pixels in patch
				{				
					for(int y=0; y<h1; y++)
					{
						Vec4b img_point = image_mat.at<Vec4b>(Point(i+x,j+y));
						Vec4b pat_point = patch_arr[r].at<Vec4b>(Point(x, y));					
						
						if(pat_point[3]>0)
						{
							double err=	sqrtf  (((int)img_point[0]-(int)pat_point[0])*((int)img_point[0]-(int)pat_point[0])+
												((int)img_point[1]-(int)pat_point[1])*((int)img_point[1]-(int)pat_point[1])+
												((int)img_point[2]-(int)pat_point[2])*((int)img_point[2]-(int)pat_point[2]));
							rot_patch_scores[r][x][y]= err;
						}
						
					}
				}// end of patch loop
				double sum = 0;
				for(int px=0; px<w1 ; px++)								/// summation of patch score
				{
					for(int py=0; py<h1; py++)
					{
						sum += rot_patch_scores[r][px][py] ;
					}
				}
				rot_result_scores[r][i][j] = sum/patch_area[r];
				if(rot_result_scores[r][i][j] < result_error)	
				{
					result_error = rot_result_scores[r][i][j];
					result_op.x =i;
					result_op.y=j;
					result_op.r=r*10;
				}
			}
		}
	}
	
	/*cout<< "FILTER Found in x: " << result_op.x << "  y: " << 
			result_op.y << " at rotation: " << result_op.r*10 << endl;
	cout<< "FILTER Estimated Error: "<<result_error << endl;*/
	
	return rot_result_scores;
}

void filter_fine_tune(	Mat image_mat,
                      Mat patch_mat,
                      orineted_position seed_op,
                      int search_raduis,
                      orineted_position &result_op,
                      double & result_error)
{
	result_error = 100000000000000 ;	
	int W= image_mat.cols; int H = image_mat.rows;		
	
	
	

	
	// creation and initialization of patch rotations array
	Mat patch_arr[10];
	int patch_area[10];
	for(int r = 0; r<10 ; r++)
	{
		patch_arr[r] = Rotate(patch_mat, r+seed_op.r -5);
		patch_area[r] =patchArea(patch_arr[r]);
	}
	/// creation of rotates patches scores array //  
    vector <vector< vector <double> > > rot_patch_scores;	
    for(int r= 0; r<10; r++)   											/// number of rotations
    {
		vector< vector <double> >  patch_scores;
		for(int i = 0; i<patch_arr[r].cols; ++i )						/// number of cols
		{
			vector<double> p;       									     
			p.resize(patch_arr[r].rows);            					/// number of rows ... Length of Row-Vector is n
			patch_scores.push_back(p);         
		}
		rot_patch_scores.push_back(patch_scores);						
	}
	/// creation of final results scores array //  
    vector < vector< vector <double> > > rot_result_scores;
    for(int r= 0; r<10; r++)   											/// number of rotations
    {
		vector< vector <double> > result_scores;	  
		for(int i = 0; i<W-patch_arr[r].cols; ++i )						/// number of cols
		{
			vector<double> p;       									      
			p.resize(H-patch_arr[r].rows);            					/// number of rows ... Length of Row-Vector is n
			result_scores.push_back(p);         
		}
		 rot_result_scores.push_back(result_scores);					
	}
    
	/// Initialization of both score arrays //        
    for(int r = 0; r<10 ; r++)
    {
		for(int i = 0; i<patch_arr[r].cols; i++)
		{
			for(int j = 0; j<patch_arr[r].rows; j++){
				rot_patch_scores[r][i][j]=0;
			}
		}  
	}  
    for(int r = 0; r<10 ; r++)
    {
		for(int i = 0; i<W-patch_arr[r].cols; i++)
		{
			for(int j = 0; j<H-patch_arr[r].rows; j++){
				rot_result_scores[r][i][j]=10000;						/// the big vaule hepls in jumping. When 
			} 															/// looking for minima,this val will be discarded
		}
    }
    
    /// loop the patch on the image, in all rotated positions	
	for(int r = 0; r<10; r++)											/// patch rotation
	{		
		int w1= patch_arr[r].cols; int h1 = patch_arr[r].rows;
		seed_op.x =	(seed_op.x-search_raduis < 0) ? search_raduis 			: seed_op.x;
		seed_op.x =	(seed_op.x+search_raduis+w1 > W) ? W-search_raduis-w1 	: seed_op.x;
		seed_op.y =	(seed_op.y-search_raduis < 0) ? search_raduis 			: seed_op.y;
		seed_op.y =	(seed_op.y+search_raduis+h1 > H) ? H-search_raduis-h1 	: seed_op.y;
		
		//cout<< ((float)r / float(36))*100 << "%" << endl;
		
		for(int i=seed_op.x-search_raduis ; i < seed_op.x+search_raduis; i++)									/// patch position in image
		{			
			for(int j=seed_op.y-search_raduis; j< seed_op.y+search_raduis; j++)
			{															///patch
			 
				for(int x=0; x<w1 ; x++)								/// pixels in patch
				{				
					for(int y=0; y<h1; y++)
					{
						Vec4b img_point = image_mat.at<Vec4b>(Point(i+x,j+y));
						Vec4b pat_point = patch_arr[r].at<Vec4b>(Point(x, y));					
						
						if(pat_point[3]>0)
						{
							

							double err=	sqrtf  (((int)img_point[0]-(int)pat_point[0])*((int)img_point[0]-(int)pat_point[0])+
												((int)img_point[1]-(int)pat_point[1])*((int)img_point[1]-(int)pat_point[1])+
												((int)img_point[2]-(int)pat_point[2])*((int)img_point[2]-(int)pat_point[2]));
							rot_patch_scores[r][x][y]= err;
						}
						
					}
				}// end of patch loop
				double sum = 0;
				for(int px=0; px<w1 ; px++)								/// summation of patch score
				{
					for(int py=0; py<h1; py++)
					{
						sum += rot_patch_scores[r][px][py] ;
					}
				}
				rot_result_scores[r][i][j] = sum/patch_area[r];
				
				
				//cout<< "FINE TUNING x: "<<  i << " y: "<< j << " r: " <<  r+seed_op.r -5<< " err: "  << rot_result_scores[r][i][j] << endl;
				if(rot_result_scores[r][i][j] < result_error)	
				{
					result_error = rot_result_scores[r][i][j];
					result_op.x =i;
					result_op.y=j;
					result_op.r=  r+seed_op.r -5;					
				}
			}
		}
	}
	
		
	
}

vector< vector <double> > InitAvailabilityMatrix(	Mat src,
													Mat initial_BD)
{
	/// creation of Availability Matrix 2D vector //  
    vector< vector <double> >  avail_mat;	
	avail_mat.resize( src.cols ,
					vector<double>( src.rows , 450 ) );
	
	for(int x=0; x<src.cols; x++)
	{
		for(int y=0; y<src.rows; y++)
		{
			Vec4b p1 = src.at<Vec4b>(Point(x, y));	
			Vec4b p2 = initial_BD.at<Vec4b>(Point(x, y));	
			avail_mat[x][y] = PythagoreanDistance(p1, p2);
		}
	}	
	return avail_mat;
	
}

vector <vector< vector <double> > >  CalcImrpovementMatrix(	Mat image_mat, 
															Mat patch_mat,
															vector< vector <double> > avail_matrix,
															int jump_steps, 
															orineted_position &result_op,
															double & result_improvement)
{	
	result_improvement = -1 ;	
	int W= image_mat.cols; int H = image_mat.rows;		
	
	// creation and initialization of patch rotations array
	Mat patch_arr[36];
	int patch_area[36];
	for(int r = 0; r<36 ; r++)
	{
		patch_arr[r] = Rotate(patch_mat, r*10);
		patch_area[r] =patchArea(patch_arr[r]);
	}	
	
	
	/// creation of rotated patches scores array //  
    vector <vector< vector <double> > > rot_patch_scores;	
    for(int r= 0; r<36; r++)   											/// number of rotations
    {
		vector< vector <double> >  patch_scores;
		for(int i = 0; i<patch_arr[r].cols; ++i )						/// number of cols
		{
			vector<double> p;       									     
			p.resize(patch_arr[r].rows);            					/// number of rows ... Length of Row-Vector is n
			patch_scores.push_back(p);         
		}
		rot_patch_scores.push_back(patch_scores);						
	}
	
	/// creation of rotated original areas scores array //  
    vector <vector< vector <double> > > rot_origin_scores;	
    for(int r= 0; r<36; r++)   											/// number of rotations
    {
		vector< vector <double> >  origin_scores;
		for(int i = 0; i<patch_arr[r].cols; ++i )						/// number of cols
		{
			vector<double> p;       									     
			p.resize(patch_arr[r].rows);            					/// number of rows ... Length of Row-Vector is n
			origin_scores.push_back(p);         
		}
		rot_origin_scores.push_back(origin_scores);						
	}
	
	
    /// creation of final results scores array //  
    vector < vector< vector <double> > > rot_result_scores;
    for(int r= 0; r<36; r++)   											/// number of rotations
    {
		vector< vector <double> > result_scores;	  
		for(int i = 0; i<W-patch_arr[r].cols; ++i )						/// number of cols
		{
			
			vector<double> p; 
			p.resize(H-patch_arr[r].rows);            					/// number of rows ... Length of Row-Vector is n
			result_scores.push_back(p);         
		}
		 rot_result_scores.push_back(result_scores);					
	}
    
	/// Initialization of both score arrays //        
    for(int r = 0; r<36 ; r++)
    {
		for(int i = 0; i<patch_arr[r].cols; i++)
		{
			for(int j = 0; j<patch_arr[r].rows; j++){
				rot_patch_scores[r][i][j]=0;
			}
		}  
	}  
	for(int r = 0; r<36 ; r++)
    {
		for(int i = 0; i<patch_arr[r].cols; i++)
		{
			for(int j = 0; j<patch_arr[r].rows; j++){
				rot_origin_scores[r][i][j]=0;
			}
		}  
	}  
    for(int r = 0; r<36 ; r++)
    {
		for(int i = 0; i<W-patch_arr[r].cols; i++)
		{
			for(int j = 0; j<H-patch_arr[r].rows; j++){
				rot_result_scores[r][i][j]= -1;							/// the -ve vaule hepls in jumping. When 
			} 															/// looking for maxima ,this val will be discarded
		}
    }
	

	/// loop the patch on the image, in all rotated positions	
	
	for(int r = 0; r<36; r++)											/// patch rotation
	{				
		int w1= patch_arr[r].cols; int h1 = patch_arr[r].rows;
		//cout<< ((float)r / float(36))*100 << "%" << endl;
		
		for(int i=0 ; i < W-w1; i+=jump_steps)							/// patch position in image
		{			
			for(int j=0; j<H-h1; j+=jump_steps)
			{															///patch

				for(int x=0; x<w1 ; x++)								/// pixels in patch
				{				
					for(int y=0; y<h1; y++)
					{
						Vec4b img_point = image_mat.at<Vec4b>(Point(i+x,j+y));
						Vec4b pat_point = patch_arr[r].at<Vec4b>(Point(x, y));					
						
						if(pat_point[3]>0)
						{
							double err=	PythagoreanDistance(img_point,pat_point);
							rot_patch_scores[r][x][y]= err;
							rot_origin_scores[r][x][y] = avail_matrix[i+x][j+y];
						}
						
					}
				}// end of patch loop
				double patch_sum = 0;
				double origin_sum =0;
				for(int px=0; px<w1 ; px++)								/// summation of patch score
				{
					for(int py=0; py<h1; py++)
					{
						patch_sum += rot_patch_scores[r][px][py] ;
						origin_sum += rot_origin_scores[r][px][py] ;
					}
				}
				rot_result_scores[r][i][j] = 	(origin_sum/patch_area[r])-
												(patch_sum/patch_area[r]) ;
				if(rot_result_scores[r][i][j]  > result_improvement)	
				{
					result_improvement = rot_result_scores[r][i][j];
					result_op.x =i;
					result_op.y=j;
					result_op.r=r*10;
				}
			}
		}
	}
	
	/*cout<< "FILTER Found in x: " << result_op.x << "  y: " << 
			result_op.y << " at rotation: " << result_op.r*10 << endl;
	cout<< "FILTER Estimated Error: "<<result_error << endl;*/
	
	return rot_result_scores;
}

void CalcImrpovementMatrixTuned(	Mat image_mat,
									Mat patch_mat, 
									vector< vector <double> > avail_matrix,
									orineted_position seed_op, 
									int search_raduis,
									orineted_position &result_op,
									double & result_improvement)
{
	result_improvement = -1 ;	
	int W= image_mat.cols; int H = image_mat.rows;			

	
	// creation and initialization of patch rotations array
	Mat patch_arr[10];
	int patch_area[10];
	for(int r = 0; r<10 ; r++)
	{
		patch_arr[r] = Rotate(patch_mat, r+seed_op.r -5);
		patch_area[r] =patchArea(patch_arr[r]);
	}
	/// creation of rotates patches scores array //  
    vector <vector< vector <double> > > rot_patch_scores;	
    for(int r= 0; r<10; r++)   											/// number of rotations
    {
		vector< vector <double> >  patch_scores;
		for(int i = 0; i<patch_arr[r].cols; ++i )						/// number of cols
		{
			vector<double> p;       									     
			p.resize(patch_arr[r].rows);            					/// number of rows ... Length of Row-Vector is n
			patch_scores.push_back(p);         
		}
		rot_patch_scores.push_back(patch_scores);						
	}
	/// creation of rotated original areas scores array //  
    vector <vector< vector <double> > > rot_origin_scores;	
    for(int r= 0; r<10; r++)   											/// number of rotations
    {
		vector< vector <double> >  origin_scores;
		for(int i = 0; i<patch_arr[r].cols; ++i )						/// number of cols
		{
			vector<double> p;       									     
			p.resize(patch_arr[r].rows);            					/// number of rows ... Length of Row-Vector is n
			origin_scores.push_back(p);         
		}
		rot_origin_scores.push_back(origin_scores);						
	}
	/// creation of final results scores array //  
    vector < vector< vector <double> > > rot_result_scores;
    for(int r= 0; r<10; r++)   											/// number of rotations
    {
		vector< vector <double> > result_scores;	  
		for(int i = 0; i<W-patch_arr[r].cols; ++i )						/// number of cols
		{
			vector<double> p;       									      
			p.resize(H-patch_arr[r].rows);            					/// number of rows ... Length of Row-Vector is n
			result_scores.push_back(p);         
		}
		 rot_result_scores.push_back(result_scores);					
	}
    
	/// Initialization of both score arrays //        
    for(int r = 0; r<10 ; r++)
    {
		for(int i = 0; i<patch_arr[r].cols; i++)
		{
			for(int j = 0; j<patch_arr[r].rows; j++){
				rot_patch_scores[r][i][j]=0;
			}
		}  
	}  
	for(int r = 0; r<10 ; r++)
    {
		for(int i = 0; i<patch_arr[r].cols; i++)
		{
			for(int j = 0; j<patch_arr[r].rows; j++){
				rot_origin_scores[r][i][j]=0;
			}
		}  
	}  
    for(int r = 0; r<10 ; r++)
    {
		for(int i = 0; i<W-patch_arr[r].cols; i++)
		{
			for(int j = 0; j<H-patch_arr[r].rows; j++){
				rot_result_scores[r][i][j]=-1;							/// the -ve vaule hepls in jumping. When 
			} 															/// looking for maxima,this val will be discarded
		}
    }
    
    /// loop the patch on the image, in all rotated positions	
	for(int r = 0; r<10; r++)											/// patch rotation
	{		
		int w1= patch_arr[r].cols; int h1 = patch_arr[r].rows;
		seed_op.x =	(seed_op.x-search_raduis < 0) ? search_raduis 			: seed_op.x;
		seed_op.x =	(seed_op.x+search_raduis+w1 > W) ? W-search_raduis-w1 	: seed_op.x;
		seed_op.y =	(seed_op.y-search_raduis < 0) ? search_raduis 			: seed_op.y;
		seed_op.y =	(seed_op.y+search_raduis+h1 > H) ? H-search_raduis-h1 	: seed_op.y;
		
		//cout<< ((float)r / float(36))*100 << "%" << endl;
		
		for(int i=seed_op.x-search_raduis ; i < seed_op.x+search_raduis; i++)									/// patch position in image
		{			
			for(int j=seed_op.y-search_raduis; j< seed_op.y+search_raduis; j++)
			{															///patch
			 
				for(int x=0; x<w1 ; x++)								/// pixels in patch
				{				
					for(int y=0; y<h1; y++)
					{
						Vec4b img_point = image_mat.at<Vec4b>(Point(i+x,j+y));
						Vec4b pat_point = patch_arr[r].at<Vec4b>(Point(x, y));					
						
						if(pat_point[3]>0)
						{
							double err=	PythagoreanDistance(img_point,pat_point);
							rot_patch_scores[r][x][y]= err;
							rot_origin_scores[r][x][y] = avail_matrix[i+x][j+y];
						}
						
					}
				}// end of patch loop
				double patch_sum = 0;
				double origin_sum =0;
				for(int px=0; px<w1 ; px++)								/// summation of patch score
				{
					for(int py=0; py<h1; py++)
					{
						patch_sum += rot_patch_scores[r][px][py] ;
						origin_sum += rot_origin_scores[r][px][py] ;
					}
				}
				rot_result_scores[r][i][j] = 	(origin_sum/patch_area[r])-
												(patch_sum/patch_area[r]) ;
				if(rot_result_scores[r][i][j]  > result_improvement)
				{
					result_improvement = rot_result_scores[r][i][j];
					result_op.x =i;
					result_op.y=j;
					result_op.r=  r+seed_op.r -5;					
				}
			}
		}
	}
	
		
	
}




void UpdateAvailabiltyMatrix(	Mat image,
								Mat patch,
								orineted_position op, 
								vector< vector <double> > &avail_matrix)
{
	patch = Rotate(patch, op.r);
	for(int i=0; i<patch.cols; i++)
	{
		for(int j=0; j<patch.rows; j++)
		{
			Vec4b img_point = image.at<Vec4b>(Point(i+op.x,j+op.y));
			Vec4b pat_point = patch.at<Vec4b>(Point(i, j));						
			if(pat_point[3]>0)
			{
				avail_matrix[i+op.x][j+op.y] = PythagoreanDistance(img_point,pat_point);
			}
			
		}
	}
}


vector< vector <double> > InitDetailsMatrix(	Mat src,
													int rad)
{
	/// creation of Details Matrix 2D vector //  
    vector< vector <double> >  details_matrix;	
	details_matrix.resize(	src.cols ,
							vector<double>( src.rows , -1 ) );
	
	for(int i=rad; i<src.cols-rad; i++)
	{
		for(int j=rad; j<src.rows-rad; j++)
		{
			Vec4b p1 = src.at<Vec4b>(Point(i, j));	
			int sum_err = 0;
			int pts_count = 0;
			for(int x=-rad; x< rad+1; x++)
			{
				for(int y=-rad; y< rad+1; y++)
				{
					Vec4b p2 = src.at<Vec4b>(Point(i+x, j+y));
					sum_err += 	PythagoreanDistance(p1,p2);
					pts_count ++;
				}
			}
			sum_err = sum_err/pts_count;
			details_matrix[i][j] = sum_err;
		}
	}	
	return details_matrix;
	
}


