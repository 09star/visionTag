#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafilters.hpp>
 
int maintest (int argc, char* argv[])
{
	try
	{
		//cv::Mat src_host = cv::imread("../img105.jpg", CV_LOAD_IMAGE_GRAYSCALE);
		//C:\Users\tags\Desktop\image.png
		cv::Mat src_host = cv::imread("C:\\Users\\tags\\Desktop\\image.png", CV_LOAD_IMAGE_GRAYSCALE);
		cv::cuda::GpuMat dst, src;
		src.upload(src_host);
 
		cv::cuda::threshold(src, dst, 128.0, 255.0, CV_THRESH_BINARY);
 
//		cv::Mat result_host(dst);
		cv::Mat result_host;
		dst.download(result_host);
		cv::imshow("Result", result_host);
		cv::waitKey();
	}
	catch(const cv::Exception& ex)
	{
		std::cout << "Error: " << ex.what() << std::endl;
	}
 
	std::cin.get();
 
	return 0;
}