//#pragma once
//#include <iostream>
//#include <fstream>
//
//#include "opencv2/core.hpp"
//#include <opencv2/core/utility.hpp>
//#include "opencv2/highgui.hpp"
//#include "opencv2/cudaoptflow.hpp"
//#include <opencv2/videoio/videoio.hpp>
//using namespace std;
//using namespace cv;
//using namespace cv::cuda;
//
//class CUDA_OpticalFlow
//{
//public:
//	CUDA_OpticalFlow(void);
//	cv::Size img_size;
//	GpuMat d_flowx;
//    GpuMat d_flowy;
//
//	BroxOpticalFlow brox;
//	PyrLKOpticalFlow lk;
//	FarnebackOpticalFlow farn;
//    OpticalFlowDual_TVL1_CUDA tvl1;
//    FastOpticalFlowBM fastBM;
//
//	void drawFlow(const Mat_<float>& flowx, const Mat_<float>& flowy,Mat& pic, float maxmotion = -1);
//	void compute_draw(Mat& pre, Mat& current, Mat& dst);
//
//	void calcflow_Brox(GpuMat& pre, GpuMat& current);
//	void calcflow_LK(GpuMat& pre, GpuMat& current);
//	void calcflow_Farn(GpuMat& pre, GpuMat& current);
//	void calcflow_TVL1(GpuMat& pre, GpuMat& current);
//	void calcflow_fastBM(GpuMat& pre, GpuMat& current);
//
//	CUDA_OpticalFlow(cv::Size size);
//	~CUDA_OpticalFlow(void);
//private:
//	static Vec3b computeColor(float fx, float fy);
//	inline bool isFlowCorrect(Point2f u)
//	{
//		return !cvIsNaN(u.x) && !cvIsNaN(u.y) && fabs(u.x) < 1e9 && fabs(u.y) < 1e9;
//	}
//};
