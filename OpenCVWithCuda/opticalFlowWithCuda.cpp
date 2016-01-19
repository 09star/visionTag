//#include <iostream>
//#include <fstream>
//
//#include "opencv2/core.hpp"
//#include <opencv2/core/utility.hpp>
//#include "opencv2/highgui.hpp"
//#include "opencv2/cudaoptflow.hpp"
//#include <opencv2/videoio/videoio.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/imgproc/types_c.h>
////#include "CUDA_OpticalFlow.h"
//using namespace std;
//using namespace cv;
//using namespace cv::cuda;
//#define I_WIDTH 320
//#define I_HEIGHT 240
//int mainsasdf()
//{
//	VideoCapture capture("opencv.avi");
//	Mat current;
//	Mat pre;
//	Mat dst;
//	Mat read;
//	capture.read(read);
//	resize(read,read,Size(I_WIDTH,I_HEIGHT));
//	CUDA_OpticalFlow m_flow(read.size());
//	if (!capture.isOpened())
//	{
//		return 0;
//	}
//	int cnt = 0;
//	while(1)
//	{
//		current.copyTo(pre);
//		if (!capture.read(read))
//		{
//			break;
//		}
//		resize(read,read,Size(I_WIDTH,I_HEIGHT));
//		cvtColor(read,current,CV_BGR2GRAY);
//		cnt++;
//		if(cnt == 1)continue;
//		
//		m_flow.compute_draw(pre,current,dst);
//		imshow("optical flow",dst);
//		//imshow("pic",current);
//		waitKey(0);
//	}
//}