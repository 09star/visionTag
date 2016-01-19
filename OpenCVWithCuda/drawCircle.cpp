//
///****
// 在图片上通过鼠标画圆圈，鼠标单击，之后拖动鼠标，实时显示所画圆圈
// reference：
// 1. http://opencv-srf.blogspot.com/2011/11/mouse-events.html
//
//*/
//#include <iostream>    
//#include <opencv2/opencv.hpp>
//#include "opencv2\highgui\highgui.hpp"
//#include "opencv2\imgproc\imgproc.hpp"
//
//using namespace cv ; 
//using namespace std ; 
//
//// 鼠标坐标
//static int mouse_x = -1;
//static int mouse_y = -1; 
//
//static int draw_controll_flag = -1 ;
//static int draw_mouse_over_counter = 0 ; 
//
//static int start_x = -1 ; 
//static int start_y = - 1; 
//static int end_x = -1 ; 
//static int end_y = -1 ; 
//
//static Mat img  ;
//static Mat copyImg;
//
//
//
//void onMouse(int event,int x,int y, int flags, void* userdata);
//
//int main(){
//
//	
//	img = imread("C:\\Users\\tags\\Desktop\\img1.jpg");
//
//	if(img.empty()){
//		cout<<"Error loading the iamge"<<endl ;
//		return -1 ;
//	}
//
//	namedWindow("window1",1);
//	
//	setMouseCallback("window1",onMouse,&img);
//
//	imshow("window1",img);
//	int x = -1;
//	int y = -1 ; 
//	//while(waitKey(15) != 27){
//	//
//	//}
//	waitKey(0);
//	return 0 ; 
//}
//
//
//void onMouse(int event,int x,int y, int flags, void* userdata){
//	 // 左单击 EVENT_LBUTTONDOWN 右单击 EVENT_RBUTTONDOWN 中间 EVENT_MBUTTONDOWN 移动 EVENT_MOUSEMOVE 
//	//cout << " button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
//	
//	if  ( event == EVENT_LBUTTONDOWN )
//     {
//           cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
//     
//		  if(draw_controll_flag == -1 ){
//			draw_controll_flag =   0; 
//			cout<< "start point (" << x << ", " << y << ")" << endl;
//			start_x = x ; 
//			start_y = y ;
//
//		  }
//	 
//		 
//
//	
//	 }
//	else if(event == EVENT_LBUTTONUP){
//
//		 if(draw_controll_flag == 0 && (start_x!= x && start_y !=y)){
//			draw_controll_flag =   -1;
//			cout<< "end point (" << x << ", " << y << ")" << endl;
//			end_x = x; 
//			end_y = y; 
//
//			int center_x = (start_x+end_x)/2 ; 
//			int center_y = (start_y+end_y)/2 ; 
//
//			int length = abs((start_x-end_x)/2) ;
//			//int width  =  ;
//			Mat & img = (*(Mat*)userdata);
//			copyImg.release();
//			img.copyTo(copyImg);
//			circle(copyImg,cvPoint(center_x,center_y),length,CV_RGB(255,0,0),1);
//			imshow("window1",copyImg);
//		  }
//	}
//	 else if  ( event == EVENT_MOUSEMOVE )
//	 {
//		 
//		 if(draw_controll_flag == 0  && draw_mouse_over_counter%20 ==0){
//			cout<< "end point (" << x << ", " << y << ")" << endl;
//			end_x = x; 
//			end_y = y; 
//
//			int center_x = (start_x+end_x)/2 ; 
//			int center_y = (start_y+end_y)/2 ; 
//
//			int length = abs((start_x-end_x)/2) ;
//			//int width  =  ;
//			Mat & img = (*(Mat*)userdata);
//			copyImg.release();
//			img.copyTo(copyImg);
//			circle(copyImg,cvPoint(center_x,center_y),length,CV_RGB(255,0,0),1);
//			imshow("window1",copyImg);
//		 }
//		 
//		 if(draw_mouse_over_counter >10000){
//			 draw_mouse_over_counter= 1;
//		 }
//		 draw_mouse_over_counter++ ; 
//	 }
//}
