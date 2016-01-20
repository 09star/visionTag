
/**

reference:
1. drawPolygonTest2.cpp  :　draw polygon method　＋ mouse　controller
2. opticalFlow_MultipleObject.cpp : optical flow function to track
2.
http://breckon.eu/toby/teaching/dip/practicals/ia/polygons.cpp

**/
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

#include "opencv2\cudaobjdetect.hpp"
#include "opencv2\cudaimgproc.hpp"
#include "opencv2\cudawarping.hpp"
#include <opencv2\cudabgsegm.hpp>  
#include <opencv2\cudaoptflow.hpp>  
using namespace std;
using namespace cv;
 
// draw and controller 
void onMouse(int event,int x,int y, int flags, void* userdata);

static vector<Point> contour;

static Mat controllerImg  ;
static Mat controllerCopyImg;

static int controllerStatus ; // 0 need set  ; 1 already set

// track 
string intToString(int number){
    
    std::stringstream ss;
    ss << number;
    return ss.str();
}
string floatToString(float number){
    
    std::stringstream ss;
    ss << number;
    return ss.str();
}
/*
 
 找最大的contours
 http://harismoonamkunnu.blogspot.com/2013/06/opencv-find-biggest-contour-using-c.html
 
 */
int  drawBiggestContours(Mat& inputMat,Mat& orignialMat,float vx , float vy){
    int largest_area=0;
    int largest_contour_index=0;
    Rect bounding_rect;
    vector<vector<Point>> contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    
    findContours( inputMat, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
    
    for( int i = 0; i< contours.size(); i++ ) // iterate through each contour.
    {
        double a=contourArea( contours[i],false);//  Find the area of contour
        if(a>largest_area){
            largest_area=a;
            largest_contour_index=i;//Store the index of largest contour
            bounding_rect=boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
        }
    }
    Scalar color( 255,255,255);
    drawContours( orignialMat, contours,largest_contour_index, color, 1, 8, hierarchy ); // Draw the largest contour using previously stored index.
    rectangle(orignialMat, bounding_rect,Scalar(0,255,0),2, 8,0);
    
    // 画中心点，并且标注速度 参考:http://docs.opencv.org/doc/tutorials/imgproc/shapedescriptors/moments/moments.html
    Scalar color1(0,0,255);
    Moments mu = moments(contours[largest_contour_index],false);
    float x  = mu.m10/mu.m00;
    float y =mu.m01/mu.m00 ;
    circle( orignialMat, Point2f(x , y ), 4,  CV_RGB(0,255,255), -1, 8, 0 );
    arrowedLine(orignialMat, Point2f(x, y), Point2f(x+vx, y+vy), CV_RGB(0,255,255),2);
    
    putText(orignialMat,"at ("+intToString(x)+","+intToString(y)+") with vector ("+floatToString(vx)+","+floatToString(vy)+")",bounding_rect.br(),1,1,Scalar(0,255,0),2);
    return 0 ;
}

/*
 找到所有contours 之后每个都画矩形和圆形
 
 http://docs.opencv.org/doc/tutorials/imgproc/shapedescriptors/bounding_rects_circles/bounding_rects_circles.html
 
 */
int drawAllContours(Mat& inputMat,Mat& orignialMat){
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);
    
    findContours( inputMat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    vector<Point2f>center( contours.size() );
    vector<float>radius( contours.size() );
    
    for( int i = 0; i < contours.size(); i++ )
    { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
    }
    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    
    
    for( int i = 0; i< contours.size(); i++ )
    {
        drawContours( orignialMat, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        rectangle( orignialMat, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        circle( orignialMat, center[i], (int)radius[i], color, 2, 8, 0 );
    }
    
    return 0 ;
}
int main()
{    
	controllerStatus = 0 ; 
	cout<< "getCudaEnabledDeviceCount : " <<cuda::getCudaEnabledDeviceCount()<<endl ;
	 //variable
	Mat prevgray, gray, flow, cflow, frame,binaryPic,binaryPic2;
	Mat  dImg_rg, dimg; 
	cuda::GpuMat oldGpuImg,rGpuImg_Bgray,oldGpuImg_gray ,gpuFlow; 
	cuda::GpuMat nowGpuImg,nowGpuImg_gray ; 
    vector<vector<Point> > contours;
	Ptr <cuda::FarnebackOpticalFlow> farnebackOF = cuda::FarnebackOpticalFlow::create(5,0.5, false,15, 3, 5, 1.2, 0);
    
	unsigned long Atime, Btime;
	float TakeTime;
	VideoCapture cap("../twoTrain.avi");
        
    if (!cap.isOpened())
    {
        cout<< "Error Acquireing video feed\n";
        return -1;
    }
	

	cap>>controllerImg;

	namedWindow("controller",1);
	
	setMouseCallback("controller",onMouse,&controllerImg);
	imshow( "controller", controllerImg );
	if (waitKey(100) >= 0)
	{
		return 0;
	}

	do
    {
        VideoCapture cap("../twoTrain.avi");
        
        if (!cap.isOpened())
        {
            cout<< "Error Acquireing video feed\n";
            return -1;
        }

		cap>>frame;
		oldGpuImg.upload(frame);
		

        while (cap.get(CV_CAP_PROP_POS_FRAMES)< cap.get(CV_CAP_PROP_FRAME_COUNT)-1) {
           
            Atime = getTickCount();

			cuda::cvtColor(oldGpuImg, oldGpuImg_gray, CV_BGR2GRAY);
			oldGpuImg_gray.download(prevgray);

            cap >> frame;
           
			nowGpuImg.upload(frame);
			cuda::cvtColor(nowGpuImg, nowGpuImg_gray, CV_BGR2GRAY);
			nowGpuImg_gray.download(gray);

			farnebackOF->calc(oldGpuImg_gray,nowGpuImg_gray,flow);
            // calcOpticalFlowFarneback(prevgray, gray, flow, 0.5, 3,15, 3, 5, 1.2, 0);
			
            cvtColor(prevgray, cflow, CV_GRAY2RGB);
                
            float diff_XY= 0.0 ;
            // 用来计算平均 flow 速度
            int numOfSelectedPoint = 0 ;
            float sumX = 0.0 ;
            float sumY = 0.0 ;
                
            binaryPic = Mat::zeros(prevgray.size(), prevgray.type());
            for (int y = 0; y < cflow.rows; y += 10 )
            {
                for (int x = 0; x < cflow.cols; x += 10)
                {
                    Point2f fxy = flow.at<Point2f>(y, x);
                    /*
                        选取光流明显的点
                        */
                    diff_XY =abs(fxy.x)+abs(fxy.y);
                    if (diff_XY > 6) {
                        binaryPic.at<uchar>(y,x) = 128 ;
                            
                        numOfSelectedPoint++ ;
                        sumX += fxy.x  ;
                        sumY += fxy.y  ;
                    }
                    // 画出选取的有明显光流的点的速度，改点为起始点，速度矢量为箭头
                    arrowedLine(cflow, Point(x, y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), CV_RGB(0,255,0));
                    // 标记处有明显光流的点  
                    circle(cflow, Point(x,y), 2, CV_RGB(255, 0, 0), -1);
                }
            }
                
            Mat dilateElement = getStructuringElement( MORPH_RECT,Size(10,10));
                
            dilate(binaryPic,binaryPic2,dilateElement);
            // binaryPic2 to binary
            threshold(binaryPic2, binaryPic2, 127.0, 255.0, THRESH_BINARY);
            // find contours
                
                
           drawAllContours(binaryPic2,cflow);
			//drawBiggestContours(binaryPic2,cflow,sumX/numOfSelectedPoint,sumY/numOfSelectedPoint);  
                
            imshow("FLOW", cflow);
            imshow("Binary", binaryPic);
            imshow("Binary dilate", binaryPic2);
           

            if (waitKey(100) >= 0)
            {
                break;
            }
            
            swap(prevgray, gray);
			//oldGpuImg = nowGpuImg ;
			oldGpuImg.upload(frame);

			Btime = getTickCount();
			TakeTime = (Btime - Atime) / getTickFrequency();  
			printf("%lf sec / %lf fps \n", TakeTime, 1 / TakeTime);
        }
        cout<< "play done"<<endl ;
        cap.release();
        
    } while (1);

    return 0;
}

void onMouse(int event,int x,int y, int flags, void* userdata){
	 // 左单击 EVENT_LBUTTONDOWN 右单击 EVENT_RBUTTONDOWN 中间 EVENT_MBUTTONDOWN 移动 EVENT_MOUSEMOVE 
	//cout << " button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	
	if  ( event == EVENT_LBUTTONDOWN )
     {
         cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     
		 contour.push_back(Point(x,y));
		 
		Mat & Img = (*(Mat*)userdata);
		controllerCopyImg.release();
		Img.copyTo(controllerCopyImg);
		for( int i = 0; i < contour.size(); i++ ){
			rectangle(controllerCopyImg, contour[i],  contour[i], Scalar(0, 0, 255), 3, 8, 0); // RED point
		}
		

		imshow("controller",controllerCopyImg);
	
	 }
	 else if  ( event == EVENT_RBUTTONDOWN  )
	 {
		  cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		    Mat & Img = (*(Mat*)userdata);
			controllerCopyImg.release();
			Img.copyTo(controllerCopyImg);

			// create a pointer to the data as an array of points (via a conversion to 
			// a Mat() object)

			const cv::Point *pts = (const cv::Point*) Mat(contour).data;
			int npts = Mat(contour).rows;

			std::cout << "Number of polygon vertices: " << npts << std::endl;
	
			// draw the polygon 

			polylines(controllerCopyImg, &pts,&npts, 1,
	    				true, 			// draw closed contour (i.e. joint end to start) 
						Scalar(0,255,0),// colour RGB ordering (here = green) 
	    				3, 		        // line thickness
						CV_AA, 0);


			// do point in polygon test (by conversion/cast to a Mat() object)
			// define and test point one (draw it in red)

			Point2f test_pt;
			test_pt.x = 150;
			test_pt.y = 75;

			rectangle(controllerCopyImg, test_pt, test_pt, Scalar(0, 0, 255), 3, 8, 0); // RED point
	
			if (pointPolygonTest(Mat(contour), test_pt, true) > 0){
				std::cout << "RED {" << test_pt.x << "," << test_pt.y
						  << "} is in the polygon (dist. " 
						  << pointPolygonTest(Mat(contour), test_pt, 1) << ")" 
						  << std::endl;
			}

			// define and test point two (draw it in blue)

			test_pt.x = 50;
			test_pt.y = 350;
	 
			rectangle(controllerCopyImg, test_pt, test_pt, Scalar(255, 0, 0), 3, 8, 0); // BLUE point
	
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
	 

			imshow("controller",controllerCopyImg);

			// refresh the  vector of points 
			contour.clear();
	 }
}
