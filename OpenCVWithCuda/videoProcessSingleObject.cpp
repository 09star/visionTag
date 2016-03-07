
//  OpticalFlow_MutilpleObject.cpp
//  OpenCVPlay
//
// 
//  Copyright (c) 2015年 starsplendid. All rights reserved.
 //代码基础来源：http://blog.sina.com.cn/s/blog_662c78590100yyeg.html
//  加上 坐标转换环节
// 注意 视频 内容的 第一帧时间和最后一帧时间 都是手动设置，参考videoRecord程序输出 结果 ； 

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

using namespace cv;
using namespace std;

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


/*calibration */
// 鼠标坐标
static int mouse_x = -1;
static int mouse_y = -1; 

static int count_flag = 0; 

static vector<Point2f> contour;

static Mat img  ;
static Mat copyImg;
/*

<camera_matrix type_id="opencv-matrix">
  <rows>3</rows>
  <cols>3</cols>
  <dt>d</dt>
  <data>
    1.4572068353989741e+003 0. 3.1950000000000000e+002 0.
    1.4572068353989741e+003 2.3950000000000000e+002 0. 0. 1.</data></camera_matrix>
<distortion_coefficients type_id="opencv-matrix">
  <rows>5</rows>
  <cols>1</cols>
  <dt>d</dt>
  <data>
    5.6210676080216342e-002 2.0568555032574163e+000 0. 0.
    -4.4769469616519928e+001</data></distortion_coefficients>

*/
static Mat cameraMatrix = (Mat_<float>(3,3) << 1.4572068353989741e+003, 0, 3.1950000000000000e+002,0, 1.4572068353989741e+003, 2.3950000000000000e+002,0, 0, 1);
static Mat distCoeffs = (Mat_<float>(5,1) << 5.6210676080216342e-002, 2.0568555032574163e+000, 0, 0, -4.4769469616519928e+001);
void onMouse(int event,int x,int y, int flags, void* userdata);

static 	Mat  uvPoint = (Mat_<double>(3,1) << 0,0,1); ;
static Mat rvec =(Mat_<double>(1,3)); 
static Mat tvec =(Mat_<double>(1,3)) ;
static Mat rotationMatrix =(Mat_<double>(3,3)); 

static double s ; 
// calibration end 
Point3f doTranslate(Point2d imagePoint){
	uvPoint.at<double>(0,0) = imagePoint.x+0.0; //got this point using mouse callback
	uvPoint.at<double>(1,0) = imagePoint.y+0.0;
	cv::Mat wcPoint = rotationMatrix.inv() * (s * cameraMatrix.inv() * uvPoint - tvec);
	Point3f realPoint(wcPoint.at<double>(0, 0), wcPoint.at<double>(1, 0), wcPoint.at<double>(2, 0));
	return realPoint ; 
}
void doTranslatePrepation(){
	cv::Mat tempMat, tempMat2;
			double  zConst = 0;
			
			rotationMatrix.convertTo(rotationMatrix,CV_64FC1);
			cameraMatrix.convertTo(cameraMatrix,CV_64FC1);
				
			tempMat = rotationMatrix.inv() * cameraMatrix.inv() * uvPoint;
			

			tempMat2 = rotationMatrix.inv() * tvec;
		
			s = zConst + tempMat2.at<double>(2,0);
			s /= tempMat.at<double>(2,0);
}
int main()
{
    //    VideoCapture cap(0);
    //    ofstream myfile;
    //    myfile.open ("flowChange1.txt");
    Mat prevgray, gray, flow, cflow, frame,binaryPic,binaryPic2;
	Mat  dImg_rg, dimg; 
	cuda::GpuMat oldGpuImg,rGpuImg_Bgray,oldGpuImg_gray ,gpuFlow; 
	cuda::GpuMat nowGpuImg,nowGpuImg_gray ; 
    vector<vector<Point> > contours;
	Ptr <cuda::FarnebackOpticalFlow> farnebackOF = cuda::FarnebackOpticalFlow::create(5,0.5, false,15, 3, 5, 1.2, 0);
    
	 unsigned long Atime, Btime;
	 float TakeTime;
	
        VideoCapture cap("recordVideo.avi");
        
        if (!cap.isOpened())
        {
            cout<< "Error Acquireing video feed\n";
            return -1;
        }

		double totalFrames = cap.get(CV_CAP_PROP_FRAME_COUNT); 

		double firstFrameTime =  1457258395516; 

		double lastFrameTime = 1457258404560;

		double perFrame = (lastFrameTime-firstFrameTime)/(totalFrames-1);

		vector<Point2d> imagePoints  ; 
		vector<double> timeStampList  ; 
		vector<Point3f> realWorldPoints ; 
		int frameNum ; 
		cout<< "Total Frames :"<< cap.get(CV_CAP_PROP_FRAME_COUNT)<<endl ;
		cap>>frame;
		oldGpuImg.upload(frame);

		

        while (cap.get(CV_CAP_PROP_POS_FRAMES)< cap.get(CV_CAP_PROP_FRAME_COUNT)) {
           
			
            Atime = getTickCount();

			cuda::cvtColor(oldGpuImg, oldGpuImg_gray, CV_BGR2GRAY);
			oldGpuImg_gray.download(prevgray);
			frameNum = cap.get(CV_CAP_PROP_POS_FRAMES) ; 
			//cout<< "Frame "<<cap.get(CV_CAP_PROP_POS_FRAMES) <<" : "; 
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
                        sumX += x  ;
                        sumY += y  ;
                    }
                    // 画出选取的有明显光流的点的速度，改点为起始点，速度矢量为箭头
                    arrowedLine(cflow, Point(x, y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), CV_RGB(0,255,0));
                    // 标记处有明显光流的点  
                    circle(cflow, Point(x,y), 2, CV_RGB(255, 0, 0), -1);
                }
            }
          // cout<<"Position: ("<<sumX/numOfSelectedPoint <<","<<sumY/numOfSelectedPoint<<")"<<endl ;  
			//cout<<sumX/numOfSelectedPoint<<","<<sumY/numOfSelectedPoint <<","<<sumX<<","<<sumY<<","<<numOfSelectedPoint <<endl;
			cout<<sumX/numOfSelectedPoint<<","<<sumY/numOfSelectedPoint <<endl;

			if(numOfSelectedPoint>0){
				imagePoints.push_back(Point2d(sumX/numOfSelectedPoint,sumY/numOfSelectedPoint ));
				timeStampList.push_back(firstFrameTime+frameNum*perFrame);
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
           

            if (waitKey(40) >= 0)
            {
                break;
            }
            
            swap(prevgray, gray);
			//oldGpuImg = nowGpuImg ;
			oldGpuImg.upload(frame);

			Btime = getTickCount();
			TakeTime = (Btime - Atime) / getTickFrequency();  
			//printf("%lf sec / %lf fps \n", TakeTime, 1 / TakeTime);
        }
        cout<< "play done"<<endl ;
        cap.release();
        
   
		// begin convert image position to real world position 

		int x = -1;
	int y = -1 ; 

	vector<Point3f> objcetPoints ;
	objcetPoints.push_back(Point3f(0,150,0));
	objcetPoints.push_back(Point3f(150,0,0));
	objcetPoints.push_back(Point3f(0,-150,0));
	objcetPoints.push_back(Point3f(-150,0,0));

	

	//rvec.create(1,3,cv::DataType<double>::type);
	//tvec.create(1,3,cv::DataType<double>::type);
	//rotationMatrix.create(3,3,cv::DataType<double>::type);



	VideoCapture cap1(0);
        
    if (!cap1.isOpened())
    {
        cout<< "Error Acquireing video feed\n";
        return -1;
    }
	

	cap1>>img;

	if(img.empty()){
		cout<<"Error loading the iamge"<<endl ;
		return -1 ;
	}

	namedWindow("window1",1);
	
	setMouseCallback("window1",onMouse,&img);

	while(true){
		cap1>> img ; 
		for( int i = 0; i < contour.size(); i++ ){
			rectangle(img, contour[i],  contour[i], Scalar(0, 0, 255), 3, 8, 0); // RED point
		}
		imshow("window1",img);
		
		 
        if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
       {
                cout << "esc key is pressed by user" << endl; 
                break; 
       }
		// 计算
		if(count_flag == 4){

			cout<< "begin calculate "<<endl  ;

			// 输出mat 参考http://blog.sina.com.cn/s/blog_662c78590100yybq.html

			cout<< "objectPoints "<<endl<< objcetPoints <<endl<<endl ; 
			cout<<" imagePints" <<endl<< contour<<endl<<endl ; 
			cout<<" cameraMatrix" <<endl<< cameraMatrix<<endl<<endl ;
			cout<<" distCoeffs"<<endl << distCoeffs<<endl <<endl;

			solvePnP(objcetPoints,contour,cameraMatrix,distCoeffs,rvec,tvec);
			
	
			cout<<"rvec "<<endl<< rvec<<endl<<endl;
			cout<<"tvec "<<endl<< tvec<<endl<<endl;

			cv::Rodrigues(rvec,rotationMatrix);

			cout<<"rotationMatrix "<<endl<< rotationMatrix<<endl<<endl;


			
			break; 
		}

		
	}

	for( int i = 0; i < contour.size(); i++ ){
		rectangle(img, contour[i],  contour[i], Scalar(0, 0, 255), 3, 8, 0); // RED point
	}
	imshow("window1",img);

	doTranslatePrepation() ;

	for(Point2d imagePoint: imagePoints){
		realWorldPoints.push_back(doTranslate(imagePoint));
	}

	// save result 
	 ofstream file("singleVideoProcessResult.txt");
	 for( int i = 0; i < realWorldPoints.size(); i++ ){
		 file<< realWorldPoints[i].x<<","<<realWorldPoints[i].y<<","<<realWorldPoints[i].z<<","<< timeStampList[i]<<endl; 
	 }
	 file.close(); 
	 cout<<"end"<<endl;
	//waitKey(0);

    
    return 0;
}



void doTranslation(){
	cout<<"doTranslation begin "<<endl;
	//Mat m = (Mat_<float>(3,3) << 1.4572068353989741e+003, 0, 3.1950000000000000e+002,0, 1.4572068353989741e+003, 2.3950000000000000e+002,0, 0, 1);
	cv::Mat tempMat, tempMat2;
			double s, zConst = 0;
			
			rotationMatrix.convertTo(rotationMatrix,CV_64FC1);
			cameraMatrix.convertTo(cameraMatrix,CV_64FC1);
				
			tempMat = rotationMatrix.inv() * cameraMatrix.inv() * uvPoint;
			

			tempMat2 = rotationMatrix.inv() * tvec;
		
			s = zConst + tempMat2.at<double>(2,0);
			s /= tempMat.at<double>(2,0);

		/*	double m11,m12,m14,m21,m22,m24,m31,m32,m34 ; 
			double u,v;
			double ay,ax,by,bx,c ; 
			m11 = rotationMatrix.at<double>(0,0);
			m12 = rotationMatrix.at<double>(0,1);
			m21 = rotationMatrix.at<double>(1,0);
			m22 = rotationMatrix.at<double>(1,1);
			m31 = rotationMatrix.at<double>(2,0);
			m32 = rotationMatrix.at<double>(2,1);

			m14 = tvec.at<double>(0,0);
			m24 = tvec.at<double>(1,0);
			m34 = tvec.at<double>(2,0);

			u = uvPoint.at<double>(0,0);
			v = uvPoint.at<double>(1,0);

			c= m12*m21-m11*m22  ;
			ay = m21*u-m11*v ; 
			by = m14*m21 - m11*m24 ;

			ax = m22*u-m12*v ; 
			bx = m14*m22-m12*m24 ; 


			s = ((m31*bx-m32*by)/c+m34)/(1+(m31*ax-m32*ay)/c);*/

			cout<<"s"<<s<<endl ; 

			cv::Mat wcPoint = rotationMatrix.inv() * (s * cameraMatrix.inv() * uvPoint - tvec);
		

			Point3f realPoint(wcPoint.at<double>(0, 0), wcPoint.at<double>(1, 0), wcPoint.at<double>(2, 0));

			cout<<"image point "<<endl<<uvPoint<< endl<< "to real point "<<endl <<wcPoint<<endl<<endl ;
}
void onMouse(int event,int x,int y, int flags, void* userdata){
	 // 左单击 EVENT_LBUTTONDOWN 右单击 EVENT_RBUTTONDOWN 中间 EVENT_MBUTTONDOWN 移动 EVENT_MOUSEMOVE 
	//cout << " button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	
	if  ( event == EVENT_LBUTTONDOWN )
     {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		
		if(count_flag>=4){
			uvPoint.at<double>(0,0) = x+0.0; //got this point using mouse callback
			uvPoint.at<double>(1,0) = y+0.0;
			cout<<"uvPoint"<<uvPoint<<endl ; 
			doTranslation();
			count_flag= count_flag +1   ; 
		}else{
			contour.push_back(Point(x,y));
	 	
			count_flag= count_flag +1   ; 
		}
		
	
	 }
}
