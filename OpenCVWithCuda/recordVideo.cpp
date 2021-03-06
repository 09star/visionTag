/*

	录制视频，制定录制时间， 并记录 第一帧 时间戳以及最后一帧时间戳 以及总帧数+1。

	摄像头淘宝链接 https://detail.tmall.com/item.htm?spm=a230r.1.14.94.8JJKHL&id=43558316376&ns=1&abbucket=11
*/
#include <iostream>    
#include <opencv2/opencv.hpp>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

#include <time.h>
#include <sys/timeb.h>
#include <sstream>
#include <fstream>

using namespace cv ; 
using namespace std ; 

/*
	读取系统毫秒时间： http://www.cnblogs.com/justlikethat/p/3623504.html

**/
string getCurrentTime(){
  //  long long time_last;
    //time_last = time(NULL);  
 
    struct timeb t1;
    ftime(&t1);
    time_t ttt= t1.millitm+t1.time*1000;
    std::stringstream ss;
    ss << ttt;
    std::string ts = ss.str();
    return ts;
}

int main(){
	
	 VideoCapture vcap(0); 
      if(!vcap.isOpened()){
             cout << "Error opening video stream or file" << endl;
             return -1;
      }

   int frame_width=   vcap.get(CV_CAP_PROP_FRAME_WIDTH);
   int frame_height=   vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
   cout<< "CV_CAP_PROP_FPS"<<vcap.get(CV_CAP_PROP_FPS) ;
   String currentTimeStr = getCurrentTime(); 
   String videoName = "recordVideo"+currentTimeStr+".avi";
   String videoFileTxt = "recordVideo"+currentTimeStr+".txt";
   VideoWriter video(videoName,CV_FOURCC('M','J','P','G'),24, Size(frame_width,frame_height),true);
   
   unsigned long Atime =0, Btime =0;
   double TakeTime;
   double totalTime = 10 ; // 视频时间 单位：秒
    Atime = getTickCount();
	int frameCount = 0 ;

	String firstFrameTime , lastFrameTime ; 
	/**
	
	 ofstream file_hand("singleResultByHand.txt");
	 file_hand<< fixed ;
	 for( int i = 0; i < imagePoints_hand.size(); i++ ){
		 cout<< imagePoints_hand[i].x<<","<<imagePoints_hand[i].y<<","<< realWorldPoints_hand[i].x<<","<<realWorldPoints_hand[i].y<<","<<realWorldPoints_hand[i].z<<","<<frameNumList_hand[i]<<","<< firstFrameTime+frameNumList_hand[i]*perFrame <<endl ; 
		 file_hand<<imagePoints_hand[i].x<<","<<imagePoints_hand[i].y<<","<< realWorldPoints_hand[i].x<<","<<realWorldPoints_hand[i].y<<","<<realWorldPoints_hand[i].z<<","<<frameNumList_hand[i]<<","<<  firstFrameTime+frameNumList_hand[i]*perFrame<<endl; 
	 }
	 file_hand.close();

	*/
	 ofstream file("frameTime.txt");
	  file<< fixed ;
   for(;;){
	  
       Mat frame;
       vcap >> frame;
	   
	   frameCount++  ; 
	  // file<<frameCount<<","<<getCurrentTime()<<endl ; 
	    file<<getCurrentTime()<<endl ;  
       video.write(frame);
	   Btime = getTickCount();

	   TakeTime = (Btime - Atime) / getTickFrequency();
	   if(TakeTime > totalTime){
		  
		   break ; 
	   }
	   imshow( "Frame", frame );
       char c = (char)waitKey(40);// 1000/24 = 41.667
       if( c == 27 ) break;
    }

   file.close() ; 

  
  return 0;

}