/**
*	最基本的画出一个多边形， 判断两个点是否在多边形里面
*/
#include <iostream>    
#include <opencv2/opencv.hpp>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
using namespace std;
using namespace cv;
 
int mainasdfxxccd( )
{    
  //// Create black empty images
  //Mat image = Mat::zeros( 400, 400, CV_8UC3 );
  // 
  //int w=400;
  //// Draw a circle 
  ///** Create some points */
  //Point rook_points[1][20];
  //rook_points[0][0] = Point( w/4.0, 7*w/8.0 );
  //rook_points[0][1] = Point( 3*w/4.0, 7*w/8.0 );
  //rook_points[0][2] = Point( 3*w/4.0, 13*w/16.0 );
  //rook_points[0][3] = Point( 11*w/16.0, 13*w/16.0 );
  //rook_points[0][4] = Point( 19*w/32.0, 3*w/8.0 );
  //rook_points[0][5] = Point( 3*w/4.0, 3*w/8.0 );
  //rook_points[0][6] = Point( 3*w/4.0, w/8.0 );
  //rook_points[0][7] = Point( 26*w/40.0, w/8.0 );
  //rook_points[0][8] = Point( 26*w/40.0, w/4.0 );
  //rook_points[0][9] = Point( 22*w/40.0, w/4.0 );
  //rook_points[0][10] = Point( 22*w/40.0, w/8.0 );
  //rook_points[0][11] = Point( 18*w/40.0, w/8.0 );
  //rook_points[0][12] = Point( 18*w/40.0, w/4.0 );
  //rook_points[0][13] = Point( 14*w/40.0, w/4.0 );
  //rook_points[0][14] = Point( 14*w/40.0, w/8.0 );
  //rook_points[0][15] = Point( w/4.0, w/8.0 );
  //rook_points[0][16] = Point( w/4.0, 3*w/8.0 );
  //rook_points[0][17] = Point( 13*w/32.0, 3*w/8.0 );
  //rook_points[0][18] = Point( 5*w/16.0, 13*w/16.0 );
  //rook_points[0][19] = Point( w/4.0, 13*w/16.0) ;
 
  //const Point* ppt[1] = { rook_points[0] };
  //int npt[] = { 20 };
 
  //fillPoly( image, ppt, npt, 1, Scalar( 255, 255, 255 ), 8 );
  //imshow("Image",image);
 
  //waitKey( 0 );
  //return(0);
	// create a RGB colour image (set it to a black background)
	
  	Mat img = Mat::zeros(400, 400, CV_8UC3);
		
	// define a polygon (as a vector of points)

	vector<Point> contour;
	contour.push_back(Point(50,50));
	contour.push_back(Point(300,50));
	contour.push_back(Point(350,200));
	contour.push_back(Point(300,150));
	contour.push_back(Point(150,350));
	contour.push_back(Point(100,100));

	// create a pointer to the data as an array of points (via a conversion to 
	// a Mat() object)

	const cv::Point *pts = (const cv::Point*) Mat(contour).data;
	int npts = Mat(contour).rows;

	std::cout << "Number of polygon vertices: " << npts << std::endl;
	
	// draw the polygon 

	polylines(img, &pts,&npts, 1,
	    		true, 			// draw closed contour (i.e. joint end to start) 
	            Scalar(0,255,0),// colour RGB ordering (here = green) 
	    		3, 		        // line thickness
			    CV_AA, 0);


	// do point in polygon test (by conversion/cast to a Mat() object)
	// define and test point one (draw it in red)

	Point2f test_pt;
	test_pt.x = 150;
	test_pt.y = 75;

	rectangle(img, test_pt, test_pt, Scalar(0, 0, 255), 3, 8, 0); // RED point
	
	if (pointPolygonTest(Mat(contour), test_pt, true) > 0){
		std::cout << "RED {" << test_pt.x << "," << test_pt.y
				  << "} is in the polygon (dist. " 
				  << pointPolygonTest(Mat(contour), test_pt, 1) << ")" 
			      << std::endl;
	}

	// define and test point two (draw it in blue)

	test_pt.x = 50;
	test_pt.y = 350;
	 
	rectangle(img, test_pt, test_pt, Scalar(255, 0, 0), 3, 8, 0); // BLUE point
	
	if (pointPolygonTest(Mat(contour), test_pt, true) < 0){
		std::cout << "BLUE {" << test_pt.x << "," << test_pt.y
				  << "} is NOT in the polygon (dist. " 
				  << pointPolygonTest(Mat(contour), test_pt, 1) << ")" 
			      << std::endl;
	}

	// pointPolygonTest :- 
	// "The function determines whether the point is inside a contour, outside, 
	// or lies on an edge (or coincides with a vertex). It returns positive 
	// (inside), negative (outside) or zero (on an edge) value, correspondingly. 
	// When measureDist=false , the return value is +1, -1 and 0, respectively. 
	// Otherwise, the return value it is a signed distance between the point 
	// and the nearest contour edge." - OpenCV Manual version 2.1
	 
	// create an image and display the image
	
  	namedWindow("Polygon Test", 0);		
	imshow( "Polygon Test", img );
	waitKey(0);

    return 0;
}