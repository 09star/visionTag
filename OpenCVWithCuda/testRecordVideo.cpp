/*

	录制视频，并记录录制视频的时间以及每一帧的时间点。

	摄像头淘宝链接 https://detail.tmall.com/item.htm?spm=a230r.1.14.94.8JJKHL&id=43558316376&ns=1&abbucket=11
*/
#include <iostream>    
#include <opencv2/opencv.hpp>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

#include <time.h>
#include <sys/timeb.h>
#include <sstream>

using namespace cv ; 
using namespace std ; 


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
   VideoWriter video("recordVideo.avi",CV_FOURCC('M','J','P','G'),24, Size(frame_width,frame_height),true);

   for(;;){
	   //Atime = getTickCount();TakeTime = (Btime - Atime) / getTickFrequency();
       Mat frame;
       vcap >> frame;
	   cout<< "CV_CAP_PROP_POS_MSEC "<<vcap.get(CV_CAP_PROP_POS_MSEC );
       video.write(frame);
       imshow( "Frame", frame );
       char c = (char)waitKey(40);// 1000/24 = 41.667
       if( c == 27 ) break;
    }

  return 0;

}