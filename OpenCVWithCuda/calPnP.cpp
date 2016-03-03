
/*
* 已知内参矩阵，通过solvePnp 方法 传入 实际世界 参考点，与像素坐标参考点。 来计算 外参矩阵 RT 即摄像头pose 
*/
#include <iostream>    
#include <opencv2/opencv.hpp>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

using namespace cv ; 
using namespace std ; 

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
int main(){

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



	VideoCapture cap(0);
        
    if (!cap.isOpened())
    {
        cout<< "Error Acquireing video feed\n";
        return -1;
    }
	

	cap>>img;

	if(img.empty()){
		cout<<"Error loading the iamge"<<endl ;
		return -1 ;
	}

	namedWindow("window1",1);
	
	setMouseCallback("window1",onMouse,&img);

	while(true){
		cap>> img ; 
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


	
	
	//while(waitKey(15) != 27){
	//
	//}
	waitKey(0);
	return 0 ; 
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
