#include "imageProcessing.cpp"
 

/** @function main */
int main( int argc, char** argv )
{
	Mat src; Mat gen_poly;
	
	/// Load source image and convert it to 4 channels
	src = imread( argv[1], 1 );
	src = convert_BGRtoBGRA(src);
	imshow( "Source", src );


	gen_poly = generatePoly(100);
	imshow( "Generated", gen_poly );
	
	
	int i = 0;
	
	while(true)
	{
		imshow( "Rotated", Rotate(detectPoly(src),i) );
		imshow( "Transparent", transparency_check( Rotate(detectPoly(src),i)) );	
		
		imshow( "Rotated Poly", Rotate((gen_poly),i) );
		imshow( "Transparent Poly", transparency_check( Rotate((gen_poly),i)) );	
		
		i+=10;
		waitKey(500);
	}
	
	
	return(0);
}
