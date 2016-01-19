//#include "CUDA_OpticalFlow.h"
//
//CUDA_OpticalFlow::CUDA_OpticalFlow(void):brox(0.197f, 50.0f, 0.8f, 10, 77, 10)
//{
//	 lk.winSize = Size(7, 7);
//}
//CUDA_OpticalFlow::CUDA_OpticalFlow(cv::Size size):brox(0.197f, 50.0f, 0.8f, 10, 77, 10)
//{
//	img_size = size;
//	d_flowx.create(size, CV_32FC1);
//    d_flowy.create(size, CV_32FC1);
//}
//Vec3b CUDA_OpticalFlow::computeColor(float fx, float fy)
//{
//    static bool first = true;
//
//    // relative lengths of color transitions:
//    // these are chosen based on perceptual similarity
//    // (e.g. one can distinguish more shades between red and yellow
//    //  than between yellow and green)
//    const int RY = 15;
//    const int YG = 6;
//    const int GC = 4;
//    const int CB = 11;
//    const int BM = 13;
//    const int MR = 6;
//    const int NCOLS = RY + YG + GC + CB + BM + MR;
//    static Vec3i colorWheel[NCOLS];
//
//    if (first)
//    {
//        int k = 0;
//
//        for (int i = 0; i < RY; ++i, ++k)
//            colorWheel[k] = Vec3i(255, 255 * i / RY, 0);
//
//        for (int i = 0; i < YG; ++i, ++k)
//            colorWheel[k] = Vec3i(255 - 255 * i / YG, 255, 0);
//
//        for (int i = 0; i < GC; ++i, ++k)
//            colorWheel[k] = Vec3i(0, 255, 255 * i / GC);
//
//        for (int i = 0; i < CB; ++i, ++k)
//            colorWheel[k] = Vec3i(0, 255 - 255 * i / CB, 255);
//
//        for (int i = 0; i < BM; ++i, ++k)
//            colorWheel[k] = Vec3i(255 * i / BM, 0, 255);
//
//        for (int i = 0; i < MR; ++i, ++k)
//            colorWheel[k] = Vec3i(255, 0, 255 - 255 * i / MR);
//
//        first = false;
//    }
//
//    const float rad = sqrt(fx * fx + fy * fy);
//    const float a = atan2(-fy, -fx) / (float) CV_PI;
//
//    const float fk = (a + 1.0f) / 2.0f * (NCOLS - 1);
//    const int k0 = static_cast<int>(fk);
//    const int k1 = (k0 + 1) % NCOLS;
//    const float f = fk - k0;
//
//    Vec3b pix;
//
//    for (int b = 0; b < 3; b++)
//    {
//        const float col0 = colorWheel[k0][b] / 255.0f;
//        const float col1 = colorWheel[k1][b] / 255.0f;
//
//        float col = (1 - f) * col0 + f * col1;
//
//        if (rad <= 1)
//            col = 1 - rad * (1 - col); // increase saturation with radius
//        else
//            col *= .75; // out of range
//
//        pix[2 - b] = static_cast<uchar>(255.0 * col);
//    }
//
//    return pix;
//}
//CUDA_OpticalFlow::~CUDA_OpticalFlow(void)
//{
//}
//void CUDA_OpticalFlow::drawFlow(const Mat_<float>& flowx, const Mat_<float>& flowy,Mat& dst, float maxmotion)
//{
//	dst.create(d_flowx.size(), CV_8UC3);
//    dst.setTo(Scalar::all(0));
//    // determine motion range:
//    float maxrad = maxmotion;
//
//    if (maxmotion <= 0)
//    {
//        maxrad = 1;
//        for (int y = 0; y < flowx.rows; ++y)
//        {
//            for (int x = 0; x < flowx.cols; ++x)
//            {
//                Point2f u(((Mat_<float>)flowx)(y, x), flowy(y, x));
//
//                if (!isFlowCorrect(u))
//                    continue;
//                maxrad = max(maxrad, sqrt(u.x * u.x + u.y * u.y));
//            }
//        }
//    }
//
//    for (int y = 0; y < flowx.rows; ++y)
//    {
//        for (int x = 0; x < flowx.cols; ++x)
//        {
//            Point2f u(flowx(y, x), flowy(y, x));
//
//            if (isFlowCorrect(u))
//                dst.at<Vec3b>(y, x) = computeColor(u.x / maxrad, u.y / maxrad);
//        }
//    }
//
//}
//void CUDA_OpticalFlow::compute_draw(Mat& pre, Mat& current, Mat& dst)
//{
//	GpuMat d_frame0(pre);
//	GpuMat d_frame1(current);
//	int64 start;double timeSec;
///*
//	start = getTickCount();
//	calcflow_Brox(d_frame0,d_frame1);
//		timeSec = (getTickCount() - start) / getTickFrequency();
//        cout << "Brox : " << timeSec << " sec" << endl;
//	start = getTickCount();
//	calcflow_fastBM(d_frame0,d_frame1);
//		timeSec = (getTickCount() - start) / getTickFrequency();
//        cout << "fastBM : " << timeSec << " sec" << endl;
//	start = getTickCount();
//	calcflow_LK(d_frame0,d_frame1);
//		timeSec = (getTickCount() - start) / getTickFrequency();
//        cout << "LK : " << timeSec << " sec" << endl;
//	start = getTickCount();
//	calcflow_Farn(d_frame0,d_frame1);
//		timeSec = (getTickCount() - start) / getTickFrequency();
//        cout << "Farn : " << timeSec << " sec" << endl;
//	start = getTickCount();
//	calcflow_TVL1(d_frame0,d_frame1);
//		timeSec = (getTickCount() - start) / getTickFrequency();
//        cout << "TVL1 : " << timeSec << " sec" << endl;
//		*/
//	calcflow_Farn(d_frame0,d_frame1);
//	Mat flowx(d_flowx);
//    Mat flowy(d_flowy);
//    drawFlow(flowx, flowy,dst, 10);
//}
//
//void CUDA_OpticalFlow::calcflow_Brox(GpuMat& pre, GpuMat& current)
//{
//	    GpuMat d_frame0f;
//        GpuMat d_frame1f;
//        pre.convertTo(d_frame0f, CV_32F, 1.0 / 255.0);
//        current.convertTo(d_frame1f, CV_32F, 1.0 / 255.0);
//        brox(d_frame0f, d_frame1f, d_flowx, d_flowy);
//}
//void CUDA_OpticalFlow::calcflow_LK(GpuMat& pre, GpuMat& current)
//{
//	    lk.dense(pre, current, d_flowx, d_flowy);
//}
//void CUDA_OpticalFlow::calcflow_Farn(GpuMat& pre, GpuMat& current)
//{
//	farn(pre, current, d_flowx, d_flowy);
//}
//void CUDA_OpticalFlow::calcflow_TVL1(GpuMat& pre, GpuMat& current)
//{
//	tvl1(pre, current, d_flowx, d_flowy);
//}
//void CUDA_OpticalFlow::calcflow_fastBM(GpuMat& pre, GpuMat& current)
//{
//	GpuMat buf;
//    calcOpticalFlowBM(pre, current, Size(7, 7), Size(1, 1), Size(21, 21), false, d_flowx, d_flowy, buf);
//}