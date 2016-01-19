
/**
1 图片上点很多点，之后连成多边形。
  左单击画点，右单击确定最后形成的多边形
2. 含有调用的 判断一个点是否在一个多边形（区域）里面的函数使用
3. 含有自动画出一张全黑图的方法 直接调用zeros方法
reference:
http://breckon.eu/toby/teaching/dip/practicals/ia/polygons.cpp

**/
#include <iostream>    
#include <opencv2/opencv.hpp>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
using namespace std;
using namespace cv;
 
void onMouse(int event,int x,int y, int flags, void* userdata);

static vector<Point> contour;

static Mat img  ;
static Mat copyImg;
int main( )
{    
	// create a RGB colour image (set it to a black background)
	
  	img = Mat::zeros(1200, 900, CV_8UC3);

	namedWindow("window1",1);
	setMouseCallback("window1",onMouse,&img);
	

	

	
	// create an image and display the image
	

	imshow( "window1", img );
	waitKey(0);

    return 0;
}

void onMouse(int event,int x,int y, int flags, void* userdata){
	 // 左单击 EVENT_LBUTTONDOWN 右单击 EVENT_RBUTTONDOWN 中间 EVENT_MBUTTONDOWN 移动 EVENT_MOUSEMOVE 
	//cout << " button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	
	if  ( event == EVENT_LBUTTONDOWN )
     {
         cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     
		 contour.push_back(Point(x,y));
		 
		Mat & img = (*(Mat*)userdata);
		copyImg.release();
		img.copyTo(copyImg);
		for( int i = 0; i < contour.size(); i++ ){
			rectangle(copyImg, contour[i],  contour[i], Scalar(0, 0, 255), 3, 8, 0); // RED point
		}
		

		imshow("window1",copyImg);
	
	 }
	 else if  ( event == EVENT_RBUTTONDOWN  )
	 {
		  cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		    Mat & img = (*(Mat*)userdata);
			copyImg.release();
			img.copyTo(copyImg);

			// create a pointer to the data as an array of points (via a conversion to 
			// a Mat() object)

			const cv::Point *pts = (const cv::Point*) Mat(contour).data;
			int npts = Mat(contour).rows;

			std::cout << "Number of polygon vertices: " << npts << std::endl;
	
			// draw the polygon 

			polylines(copyImg, &pts,&npts, 1,
	    				true, 			// draw closed contour (i.e. joint end to start) 
						Scalar(0,255,0),// colour RGB ordering (here = green) 
	    				3, 		        // line thickness
						CV_AA, 0);


			// do point in polygon test (by conversion/cast to a Mat() object)
			// define and test point one (draw it in red)

			Point2f test_pt;
			test_pt.x = 150;
			test_pt.y = 75;

			rectangle(copyImg, test_pt, test_pt, Scalar(0, 0, 255), 3, 8, 0); // RED point
	
			if (pointPolygonTest(Mat(contour), test_pt, true) > 0){
				std::cout << "RED {" << test_pt.x << "," << test_pt.y
						  << "} is in the polygon (dist. " 
						  << pointPolygonTest(Mat(contour), test_pt, 1) << ")" 
						  << std::endl;
			}

			// define and test point two (draw it in blue)

			test_pt.x = 50;
			test_pt.y = 350;
	 
			rectangle(copyImg, test_pt, test_pt, Scalar(255, 0, 0), 3, 8, 0); // BLUE point
	
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
	 

			imshow("window1",copyImg);

			// refresh the  vector of points 
			contour.clear();
	 }
}
