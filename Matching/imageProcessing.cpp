#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


#include <string>
#include <iostream>

using namespace cv;
using namespace std;

struct orineted_position {
  int x;
  int y;
  int r;
} ;


Mat copyPolygon(Mat source)
{
	Mat temp = Mat::zeros( source.rows, source.cols, CV_8UC4);
	temp = Scalar(255,255,255,255);
	for (int i=0; i<source.rows; i++)
	{
		for (int j=0; j<source.cols; j++)
		{
			Vec4b color = source.at<Vec4b>(Point(j, i));
			if((int)color.val[3] >0)
			{
				temp.at<Vec4b>(Point(j,i)) = color;	
				temp.at<Vec4b>(Point(j,i))[3] = 255;	
			}
			else
			{temp.at<Vec4b>(Point(j,i))[3] = 0;	}
		}
	}	
	return temp;
}


Mat generateColoredPoly(int size)
{
	srand (time(NULL));	
	Mat tempMat = Mat::zeros( 480, 640, CV_8UC4 );
	tempMat = Scalar( 0, 0, 0, 0 );
	Point poly_points[1][4];
	
	int rad = rand() % size + 1;
	poly_points[0][0] = Point( 340-rad, 240-rad );
	rad = rand() % size + 1;
	poly_points[0][1] = Point( 340-rad, 240+rad );
	rad = rand() % size + 1;
	poly_points[0][2] = Point( 340+rad, 240+rad );	
	rad = rand() % size + 1;
	poly_points[0][3] = Point( 340+rad, 240-rad );
	
	int r, g, b; 
	r = rand()%255;
	g = rand()%255;
	b = rand()%255;
	
	/// Draw lines in mat
	for( int j = 0; j < 4; j++ )
	{
		line( tempMat, poly_points[0][j],  poly_points[0][(j+1)%4], Scalar( r,g,b, 255 ), 1, CV_AA ); 
	}
	/// fill the polygon
	const Point* ppt[1] = { poly_points[0] };
	int npt[] = { 4 };
	fillPoly( tempMat, ppt, npt, 1, Scalar( r, g, b, 255 ), 8 );	
	
	int maxX = 0, minX = tempMat.cols, maxY=0, minY = tempMat.rows;				// bounding box
    for(int j=0; j<4; j++)
    {
        Point p = poly_points[0][j];
        maxX = max(maxX, p.x);
        minX = min(minX, p.x);
        maxY = max(maxY, p.y);
        minY = min(minY, p.y);
    }
	//rectangle( mt, Point(minX,minY), Point(maxX, maxY), Scalar(255) );
	Rect myROI(minX, minY, maxX-minX, maxY-minY);
	Mat croppedImage = tempMat(myROI);	
	return croppedImage;
}
Mat generateGrayscalePoly(int size)
{
	srand (time(NULL));	
	Mat tempMat = Mat::zeros( 480, 640, CV_8UC4 );
	tempMat = Scalar( 0, 0, 0, 0 );
	Point poly_points[1][4];
	
	int rad = rand() % size + 1;
	poly_points[0][0] = Point( 340-rad, 240-rad );
	rad = rand() % size + 1;
	poly_points[0][1] = Point( 340-rad, 240+rad );
	rad = rand() % size + 1;
	poly_points[0][2] = Point( 340+rad, 240+rad );	
	rad = rand() % size + 1;
	poly_points[0][3] = Point( 340+rad, 240-rad );
	
	int r= rand()%255;
	int g = r;
	int b =r;
	
	/// Draw lines in mat
	for( int j = 0; j < 4; j++ )
	{
		line( tempMat, poly_points[0][j],  poly_points[0][(j+1)%4], Scalar( r,g,b,255 ), 1, CV_AA ); 
	}
	/// fill the polygon
	const Point* ppt[1] = { poly_points[0] };
	int npt[] = { 4 };
	fillPoly( tempMat, ppt, npt, 1, Scalar( r, g, b, 255 ), 8 );	
	
	int maxX = 0, minX = tempMat.cols, maxY=0, minY = tempMat.rows;				// bounding box
    for(int j=0; j<4; j++)
    {
        Point p = poly_points[0][j];
        maxX = max(maxX, p.x);
        minX = min(minX, p.x);
        maxY = max(maxY, p.y);
        minY = min(minY, p.y);
    }
	//rectangle( mt, Point(minX,minY), Point(maxX, maxY), Scalar(255) );
	Rect myROI(minX, minY, maxX-minX, maxY-minY);
	Mat croppedImage = tempMat(myROI);	
	return croppedImage;
}
Mat convert_BGRtoBGRA(Mat src)
{
	Mat dst = cv::Mat(src.rows,src.cols,CV_8UC4);
	CV_Assert(dst.channels() == 4);
	for (int i=0; i<src.rows; i++)
	{
		for (int j=0; j<src.cols; j++)
		{	Vec4b& bgra =dst.at<Vec4b>(Point(j,i));
			bgra[0] = saturate_cast<uchar>(src.at<Vec3b>(Point(j,i))[0]);
			bgra[1] = saturate_cast<uchar>(src.at<Vec3b>(Point(j,i))[1]);
			bgra[2] = saturate_cast<uchar>(src.at<Vec3b>(Point(j,i))[2]);
			bgra[3] = saturate_cast<uchar>(255);	
		}
	}	
	return dst;
}
Mat transparency_check(Mat drawing)
{	
	Mat dst = copyPolygon(drawing);
	for (int i=0; i<dst.rows; i++)
	{
		for (int j=0; j<dst.cols; j++)
		{
			Vec4b color = dst.at<Vec4b>(Point(j, i));
			if((int)color.val[3] > 0)			
			{
				dst.at<Vec4b>(Point(j, i))[0] = 0;
				dst.at<Vec4b>(Point(j, i))[1] = 0;
				dst.at<Vec4b>(Point(j, i))[2] = 255;
			}
			else if((int)color.val[3] == 0)		
			{
				dst.at<Vec4b>(Point(j, i))[0] = 0;
				dst.at<Vec4b>(Point(j, i))[1] = 255;
				dst.at<Vec4b>(Point(j, i))[2] = 0;
			}			
		}
	}	
	return dst;
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
	
	
	
	
	/// checking for transparency
	/*
	for (int i=0; i<dst.rows; i++)
	{
		for (int j=0; j<dst.cols; j++)
		{
			Vec4b color = dst.at<Vec4b>(Point(j, i));
			if((int)color.val[3] >0)			
			{
				dst.at<Vec4b>(Point(j, i))[0] = 0;
				dst.at<Vec4b>(Point(j, i))[1] = 0;
				dst.at<Vec4b>(Point(j, i))[2] = 255;
			}
			else if((int)color.val[3] == 0)		
			{
				dst.at<Vec4b>(Point(j, i))[0] = 0;
				dst.at<Vec4b>(Point(j, i))[1] = 255;
				dst.at<Vec4b>(Point(j, i))[2] = 0;
			}			
		}
	}	
	*/ 
	
	/// crop rotated image
	
	int maxX = 0, minX =dst.cols , maxY=0, minY = dst.rows;				// bounding box
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
Mat detectPoly(Mat picture)
{
	int thresh = 214;
	RNG rng(12345);

	Mat src_gray;
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat drawing;
	Mat croppedImage;
	
	/// Convert image to gray and blur it
	cvtColor( picture, src_gray, CV_BGR2GRAY );
	blur( src_gray, src_gray, Size(2,2) );	
	/// Detect edges using canny
	Canny( src_gray, canny_output, thresh, thresh*7, 5);
	
	/// Find contours
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );



	/// Draw contours
	drawing = Mat::zeros( canny_output.size(), CV_8UC4 );
	drawing = Scalar(0,0,0,0);
	for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255), 255 );       
       drawContours( drawing, contours, i, color, CV_FILLED, 8, hierarchy, 0, Point() );
     }
     
     /// Copy Original Contents
	for (int i=0; i<drawing.rows; i++)
	{
		for (int j=0; j<drawing.cols; j++)
		{
			Vec4b color = drawing.at<Vec4b>(Point(j, i));
			if((int)color.val[3] > 0)			
			{
				drawing.at<Vec4b>(Point(j, i))= picture.at<Vec4b>(Point(j, i));
			}
						
		}
	}
	
	/// Draw Rectangle around 
	for( int i = 0; i< contours.size(); i=hierarchy[i][0] ) // iterate through each contour.
    {
        Rect r= boundingRect(contours[i]);
        if(hierarchy[i][2]<0) //Check if there is a child contour
          rectangle(drawing,Point(r.x-10,r.y-10), Point(r.x+r.width+10,r.y+r.height+10), Scalar(0,0,255),2,8,0); //Opened contour
        else
        {
			croppedImage = drawing(r);
			rectangle(drawing,Point(r.x-10,r.y-10), Point(r.x+r.width+10,r.y+r.height+10), Scalar(0,255,0),2,8,0); //closed contour
			//std::string numStr = std::to_string(i);
			     
		}
    }

	imshow( "Detected Polygon", drawing)	;     
	return croppedImage;
}
int patchArea(Mat src)
{
	int sum = 0;
	for (int i=0; i<src.cols; i++)
	{
		for (int j=0; j<src.rows; j++)
		{
			Vec4b color = src.at<Vec4b>(Point(i,j));
			if((int)color.val[3] > 0)			
			{
				sum++;
			}
		}
	}
	return sum;
} 

void collatePatch(Mat &image, Mat patch, orineted_position or_pos)
{
	patch =	Rotate(patch, or_pos.r);		
	for(int i = or_pos.x; i<(or_pos.x+patch.cols) ; i++ )
	{
		for(int j= or_pos.y ; j <(or_pos.y+patch.rows); j ++)
		{
			if(patch.at<Vec4b>(Point(i-or_pos.x,j-or_pos.y))[3] > 0)
			{
				image.at<Vec4b>(Point(i,j))[0] = 
					patch.at<Vec4b>(Point(i-or_pos.x,j-or_pos.y))[0];
				image.at<Vec4b>(Point(i,j))[1] = 
					patch.at<Vec4b>(Point(i-or_pos.x,j-or_pos.y))[1];
				image.at<Vec4b>(Point(i,j))[2] = 
					patch.at<Vec4b>(Point(i-or_pos.x,j-or_pos.y))[2];
			} 
		}
	} 
}

vector< vector <double> > filter(Mat image_mat, Mat patch_mat)
{
	int W= image_mat.cols; int H = image_mat.rows;
	int w1= patch_mat.cols; int h1 = patch_mat.rows;
	int patch_area = patchArea(patch_mat);	
	
	// creation of patch_scores array //  
    vector< vector <double> > patch_scores;	  
    for(int i = 0; i<w1; ++i )											/// number of cols
    {
        vector<double> p;       										/// Row-Vector       
        p.resize(h1);            										/// number of rows ... Length of Row-Vector is n
        patch_scores.push_back(p);         
    }    
    // creation of result_scores array //  
    vector< vector <double> > result_scores;	  
    for(int i = 0; i<W-w1; ++i )										/// number of cols
    {
        vector<double> p;       										/// Row-Vector       
        p.resize(H-h1);            										/// number of rows ... Length of Row-Vector is n
        result_scores.push_back(p);         
    }
	// Initialization of both score arrays //        
    for(int i = 0; i<w1; i++)
    {
        for(int j = 0; j<h1; j++){
            patch_scores[i][j]=0;
        }
    }    
    for(int i = 0; i<W-w1; i++)
    {
        for(int j = 0; j<H-h1; j++){
            result_scores[i][j]=0;
        } 
    }


	/// loop the patch on the image
	
	for(int i=0 ; i < W-w1; i++)
	{
		cout<< ((float)i / float(W-w1))*100 << "%" << endl;
		
		for(int j=0; j<H-h1; j++)
		{																//patch
		 
			for(int x=0; x<w1 ; x++)
			{	
				//#pragma parallel
			
				for(int y=0; y<h1; y++)
				{
					Vec4b img_point = image_mat.at<Vec4b>(Point(i+x,j+y));
					Vec4b pat_point = patch_mat.at<Vec4b>(Point(x, y));					
					
					if(pat_point[3] >0)
					{
						double err=	sqrtf  (((int)img_point[0]-(int)pat_point[0])*((int)img_point[0]-(int)pat_point[0])+
											((int)img_point[1]-(int)pat_point[1])*((int)img_point[1]-(int)pat_point[1])+
											((int)img_point[2]-(int)pat_point[2])*((int)img_point[2]-(int)pat_point[2]));
						patch_scores[x][y]= err;
					}
					
				}
			}// end of patch loop
			double sum = 0;
			for(int px=0; px<w1 ; px++)									// summation of patch score
			{
				for(int py=0; py<h1; py++)
				{
					sum += patch_scores[px][py] ;
				}
			}
			result_scores[i][j] = sum/patch_area;		
		
		}
	}
	
	return result_scores;
}


void savePatch(Mat patch)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
	
	srand (time(NULL));	
	std::ostringstream ss;
	long num = rand();
	ss << "puzzle/"<< num<< ".png"; 
	
    imwrite(ss.str(), patch, compression_params);
}

void saveImage(Mat image, string name)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
	
	srand (time(NULL));	
	std::ostringstream ss;
	long num = rand();
	ss << name << "_"<< num << ".png"; 
	
    imwrite(ss.str(), image, compression_params);
}


vector<Mat> splitImage(Mat image)
{
	vector<Mat> vec ;
	vec.resize(2); 

	
	srand (time(NULL));	
	Point p1 = Point(0,0);
	Point p2 = Point(0,0);
	
	
	
	image = Rotate(image, rand()%360 );
	p1.x =0 ;
	//p1.y = rand()%image.rows ;
	p1.y = image.rows/2 ;
	p2.x = image.cols ;
	p2.y = p1.y ;
		
	Mat patch1 = copyPolygon(image);
	Mat patch2 = copyPolygon(image);
	
	// split
	Rect myROI1(p2, Point(0,0));
	patch1 = patch1(myROI1);	
	Rect myROI2(Point(0,patch2.rows), p2);
	patch2 = patch2(myROI2);	


	// crop patch1	
	int maxX = 0, minX =patch1.cols , maxY=0, minY = patch1.rows;				// bounding box
    for (int r=0; r<patch1.rows; r++)
	{
		for (int c=0; c<patch1.cols; c++)
		{
			bool visible = (int)patch1.at<Vec4b>(Point(c, r))[3];			
			if (visible)
			{
				minX = min(minX,c);				
				minY = min(minY,r);
				maxX = max(maxX, c);
				maxY = max(maxY,r);
			}				
		}
    }
    myROI1 = Rect(minX, minY, maxX-minX, maxY-minY);
	patch1 = patch1(myROI1);
	
	// crop patch2	
	maxX = 0; minX =patch2.cols ; maxY=0; minY = patch2.rows;				// bounding box
    for (int r=0; r<patch2.rows; r++)
	{
		for (int c=0; c<patch2.cols; c++)
		{
			bool visible = (int)patch2.at<Vec4b>(Point(c, r))[3];			
			if (visible)
			{
				minX = min(minX,c);				
				minY = min(minY,r);
				maxX = max(maxX, c);
				maxY = max(maxY,r);
			}				
		}
    }
    myROI2 = Rect(minX, minY, maxX-minX, maxY-minY);
	patch2 = patch2(myROI2);
	
	
	
	// put patches in vector
	vec[0] = patch1;
	vec[1] = patch2;
	
	return vec;
}

void puzzleImage(Mat image, int src_cols, int src_rows, int minArea)
{   
	if((patchArea(image)< minArea)
		//(image.rows< src_cols) &&
		//(image.rows< src_rows) &&
		//(image.cols< src_cols) &&
		//(image.cols< src_rows)
		)
	{
		
		
		if(patchArea(image)>0)
		{
			cout<< "area:" << patchArea(image) << endl;
			srand (time(NULL));	
			std::ostringstream ss;
			long num = rand();
			ss << "puzzle/"<< num<< ".png"; 
			savePatch(image);
			imshow(ss.str(), image);
			waitKey(1000);	
		}
	}
	else
	{
		vector<Mat> tempVec = splitImage(image);
		cout<< "area:" << patchArea(tempVec[0]) << endl;
		cout<< "area:" << patchArea(tempVec[1]) << endl;
		puzzleImage(tempVec[0], minArea, src_cols, src_rows);
		puzzleImage(tempVec[1], minArea, src_cols, src_rows);
	}	
	
	//cout<< "area:" << patchArea(image) << endl;
	

	
}



vector<Mat> splitImage2(Mat image)
{
	vector<Mat> vec ;
	vec.resize(2); 
	Mat patch1 = copyPolygon(image);
	Mat patch2 = copyPolygon(image);
	
	
		
	srand (time(NULL));	
	Point corners[4] = {Point(0,0), 					Point(image.cols,0),
						Point(image.cols,image.rows),	Point(0,image.rows)};
	Point p1 = Point(0,0);
	Point p2 = Point(0,0);
	 
	while((p1.x == p2.x)||(p1.y == p2.y))
	{	
		int axis_choice = rand()%2;			
		int y_choice = rand()%2;	
		int x_choice = rand()%2;
		int y_options[2] = {0, image.rows };
		int x_options[2] = {0, image.cols };
		
		if(axis_choice){
			p1.x 		 = rand()%image.cols;
			p1.y 		 = y_options[y_choice];			
		}
		else{
			p1.y 		 = rand()%image.rows;
			p1.x	 	 = x_options[x_choice];			
		}
		
		axis_choice = rand()%2;			
		y_choice = rand()%2;	
		x_choice = rand()%2;
		
		if(axis_choice){
			p2.x 		 = rand()%image.cols;
			p2.y 		 = y_options[y_choice];			
		}
		else{
			p2.y 		 = rand()%image.rows;
			p2.x	 	 = x_options[x_choice];			
		}
		
	}
	
	cout << "P1:"<<  p1.x<< "," << p1.y << endl;
	cout << "P2:"<<  p2.x<< "," << p2.y << endl;
	
	if( p1 == corners[0] || p1 == corners[1] || p1 == corners[2] || p1 == corners[3] ||
		p2 == corners[0] || p2 == corners[1] || p2 == corners[2] || p2 == corners[3])
	{
			cout<< "type3" << endl;
	}
	
	else if ((((p1.x - p2.x)*(p1.x - p2.x)) == image.cols*image.cols) ||
		(((p1.y - p2.y)*(p1.y - p2.y)) == image.rows*image.rows) )
	{
		int start=0;
		Point poly1[1][4];
		Point poly2[1][4];
		
		if(p1.y == 0) {start= 1;}
		else 
		{	
			if(p1.x == image.cols)	start= 2;
			else 
			{	if(p1.y == image.rows)  start= 3;
				else 
				{if(p1.x == 0) 			start= 0;}
			}
		}		
		poly1[0][0] = p1;
		poly1[0][1] = corners[start];
		poly1[0][2] = corners[(start+1)%4];
		poly1[0][3] = p2;
		
		poly2[0][0] = p2;
		poly2[0][1] = corners[(start+2)%4];
		poly2[0][2] = corners[(start+3)%4];
		poly2[0][3] = p1;
		
				
		const Point* ppt[1] = { poly1[0] };
		int npt[] = { 4 };
		fillPoly( patch2, ppt, npt, 1, Scalar( 255, 0, 0, 255 ), 8 );
		
		const Point* ppt2[1] = { poly2[0] };
		int npt2[] = { 4 };
		fillPoly( patch2, ppt2, npt2, 1, Scalar( 0, 255, 0, 255 ), 8 );
		
		
		cout << "start:" <<start << endl;
		
		 
	}
	else
	{
		//if(p1.y)
		cout<< "case2" << endl;		
	}

	
	
			
	
	line( patch2, p1,  p2, Scalar( 0,0,255, 255 ), 2, CV_AA ); 
	

	vec[0] = patch1;
	vec[1] = patch2;
	
	return vec;
}


double PythagoreanDistance(Vec4b p1, Vec4b p2)
{
	double dist =0;
	dist		=	sqrtf  (((int)p1[0]-(int)p2[0])*((int)p1[0]-(int)p2[0])+
							((int)p1[1]-(int)p2[1])*((int)p1[1]-(int)p2[1])+
							((int)p1[2]-(int)p2[2])*((int)p1[2]-(int)p2[2]));
							
	return dist;
}

