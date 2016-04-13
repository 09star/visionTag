#include <iostream>    
#include <opencv2/opencv.hpp>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include <ctime>

#include "atltime.h"


using namespace cv ; 
using namespace std ; 


int main(){

	Mat frame ; 
	namedWindow("controller",1);
	VideoCapture vcap("recordVideo.avi"); 
   if(!vcap.isOpened()){
             cout << "Error opening video stream or file" << endl;
             return -1;
      }
   cout<< "CV_CAP_PROP_POS_MSEC "<<vcap.get(CV_CAP_PROP_POS_MSEC );
   cout<< "CV_CAP_PROP_FPS"<<vcap.get(CV_CAP_PROP_FPS) ; 
   while (vcap.get(CV_CAP_PROP_POS_FRAMES)< vcap.get(CV_CAP_PROP_FRAME_COUNT)-1) {
		vcap>> frame ; 
		cout<< "CV_CAP_PROP_POS_MSEC "<<vcap.get(CV_CAP_PROP_POS_MSEC )<<endl ;
		 //time_t t = time(0);   // get time now
		 CTime t = GetCurrentTime(); 
	SYSTEMTIME t1;
GetSystemTime(&t1) ;
CTime curTime(t1); 
		 cout<< "Time "<<vcap.get(CV_CAP_PROP_POS_FRAMES)<<" :"<< t1.wMilliseconds ;
		imshow( "controller", frame );
		char c = (char)waitKey(20);// 1000/24 = 41.667
        if( c == 27 ) break;
   }

}