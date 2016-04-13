/*
    play optical flow with cuda 
	http://study.marearts.com/2015/11/motion-flow-and-direction-gpu-cuda.html

*/
#include <iostream>    
#include "opencv2\objdetect\objdetect.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\cudaobjdetect.hpp"
#include "opencv2\cudaimgproc.hpp"
#include "opencv2\cudawarping.hpp"
//#include <opencv2\bgsegm.hpp>  
#include <opencv2\cudabgsegm.hpp>  
#include <opencv2\cudaoptflow.hpp>  



/*#ifdef _DEBUG               
#pragma comment(lib, "opencv_core300d.lib")       
#pragma comment(lib, "opencv_highgui300d.lib")    
#pragma comment(lib, "opencv_imgcodecs300d.lib")  
#pragma comment(lib, "opencv_objdetect300d.lib")  
#pragma comment(lib, "opencv_imgproc300d.lib")  
#pragma comment(lib, "opencv_videoio300d.lib")    
#pragma comment(lib, "opencv_cudaobjdetect300d.lib")  
#pragma comment(lib, "opencv_cudawarping300d.lib")
#pragma comment(lib, "opencv_cudaimgproc300d.lib")
#pragma comment(lib, "opencv_cudabgsegm300d.lib")  
#pragma comment(lib, "opencv_cudaoptflow300d.lib") 
#else       
#pragma comment(lib, "opencv_core300.lib")       
#pragma comment(lib, "opencv_highgui300.lib")    
#pragma comment(lib, "opencv_imgcodecs300.lib")    
#pragma comment(lib, "opencv_objdetect300.lib")  
#pragma comment(lib, "opencv_imgproc300.lib")  
#pragma comment(lib, "opencv_videoio300.lib")    
#pragma comment(lib, "opencv_cudaobjdetect300.lib")
#pragma comment(lib, "opencv_cudawarping300.lib")
#pragma comment(lib, "opencv_cudaimgproc300.lib")
#pragma comment(lib, "opencv_cudabgsegm300.lib")  
#pragma comment(lib, "opencv_cudaoptflow300.lib") 
#endif  */      

using namespace cv;
using namespace std;

static void download(const cuda::GpuMat& d_mat, vector< Point2f>& vec);
static void download(const cuda::GpuMat& d_mat, vector< uchar>& vec);
static void drawArrows(Mat& frame, const vector< Point2f>& prevPts, const vector< Point2f>& nextPts, const vector< uchar>& status, Scalar line_color = Scalar(0, 0, 255));


void main()
{
 cout<< "getCudaEnabledDeviceCount : " <<cuda::getCudaEnabledDeviceCount()<<endl ;
 //variable
 cuda::GpuMat GpuImg, rGpuImg_Bgray;
 cuda::GpuMat oldGpuImg_Agray;

 //video
 Mat img, dImg_rg, dimg;
 do{
	 // VideoCapture cap("../out.avi");
	VideoCapture cap("../twoTrain.avi");

	 cap >> img;
	 if (img.empty())
	  return;

	 //scale
	 double scale = 800. / img.cols;

	 //first gpumat
	 GpuImg.upload(img);// 将video 获取的img 从 Mat 格式 转化成 GpuMat 
	 cuda::resize(GpuImg, oldGpuImg_Agray, Size(GpuImg.cols * scale, GpuImg.rows * scale)); // resize from GpuImg --> oldGpuImg_Agray
	 cuda::cvtColor(oldGpuImg_Agray, oldGpuImg_Agray, CV_BGR2GRAY);// oldGpuImg_Agray 转化颜色空间 从 BGR 转化成 GRAY
 
	 cuda::GpuMat d_prevPts;
	 cuda::GpuMat d_nextPts;
	 cuda::GpuMat d_status;
	 Ptr< cuda::CornersDetector> detector = cuda::createGoodFeaturesToTrackDetector(oldGpuImg_Agray.type(), 4000, 0.01, 0);
	 //opticla flow
	 Ptr< cuda::SparsePyrLKOpticalFlow> d_pyrLK = cuda::SparsePyrLKOpticalFlow::create(Size(21, 21), 3, 30);

	 unsigned long Atime, Btime;
	 float TakeTime;

	 while (1)
	 {
	  Atime = getTickCount();

	  cap >> img;
	  if (img.empty())
	   break;

	  //get image
	  GpuImg.upload(img);
	  cuda::resize(GpuImg, rGpuImg_Bgray, Size(GpuImg.cols * scale, GpuImg.rows * scale));
	  rGpuImg_Bgray.download(dimg);
	  cuda::cvtColor(rGpuImg_Bgray, rGpuImg_Bgray, CV_BGR2GRAY);
	  rGpuImg_Bgray.download(dImg_rg);


	  //A,B image  
	  //oldGpuImg_Agray;
	  //rGpuImg_Bgray;

	  //feature
	  detector->detect(oldGpuImg_Agray, d_prevPts);
	  d_pyrLK->calc(oldGpuImg_Agray, rGpuImg_Bgray, d_prevPts, d_nextPts, d_status);


	  //old  用当前img 更新光流法 old img
	  oldGpuImg_Agray = rGpuImg_Bgray;
    

	  // Draw arrows
	  vector< Point2f> prevPts(d_prevPts.cols);
	  download(d_prevPts, prevPts);

	  vector< Point2f> nextPts(d_nextPts.cols);
	  download(d_nextPts, nextPts);

	  vector< uchar> status(d_status.cols);
	  download(d_status, status);

	 /* Mat prevPts ;
	  d_prevPts.download(prevPts);
	  Mat prevPts ;
	  d_nextPts.download(nextPts);
	  Mat prevPts ;
	  d_prevPts.download(prevPts);*/

	  drawArrows(dimg, prevPts, nextPts, status, Scalar(255, 0, 0));


	  //show
	  imshow("PyrLK [Sparse]", dimg);  
	  imshow("origin", dImg_rg);
	  if (waitKey(10)>0)
	   break;


	  Btime = getTickCount();
	  TakeTime = (Btime - Atime) / getTickFrequency();  
	  printf("%lf sec / %lf fps \n", TakeTime, 1 / TakeTime);
	  
	
	}
	   cap.release();
 }while(1);
 

}



static void download(const cuda::GpuMat& d_mat, vector< uchar>& vec)
{
 vec.resize(d_mat.cols);
 Mat mat(1, d_mat.cols, CV_8UC1, (void*)&vec[0]);
 d_mat.download(mat);
}

static void download(const cuda::GpuMat& d_mat, vector< Point2f>& vec)
{
 vec.resize(d_mat.cols);
 Mat mat(1, d_mat.cols, CV_32FC2, (void*)&vec[0]);
 d_mat.download(mat);
}

static void drawArrows(Mat& frame, const vector< Point2f>& prevPts, const vector< Point2f>& nextPts, const vector< uchar>& status, Scalar line_color)
{
 for (size_t i = 0; i <  prevPts.size(); ++i)
 {
  if (status[i])
  {
   int line_thickness = 1;

   Point p = prevPts[i];
   Point q = nextPts[i];

   double angle = atan2((double)p.y - q.y, (double)p.x - q.x);

   double hypotenuse = sqrt((double)(p.y - q.y)*(p.y - q.y) + (double)(p.x - q.x)*(p.x - q.x));

   if (hypotenuse <  1.0)
    continue;

   // Here we lengthen the arrow by a factor of three.
   q.x = (int)(p.x - 3 * hypotenuse * cos(angle));
   q.y = (int)(p.y - 3 * hypotenuse * sin(angle));

   // Now we draw the main line of the arrow.
   line(frame, p, q, line_color, line_thickness);

   // Now draw the tips of the arrow. I do some scaling so that the
   // tips look proportional to the main line of the arrow.

   p.x = (int)(q.x + 9 * cos(angle + CV_PI / 4));
   p.y = (int)(q.y + 9 * sin(angle + CV_PI / 4));
   line(frame, p, q, line_color, line_thickness);

   p.x = (int)(q.x + 9 * cos(angle - CV_PI / 4));
   p.y = (int)(q.y + 9 * sin(angle - CV_PI / 4));
   line(frame, p, q, line_color, line_thickness);
  }
 }
}



