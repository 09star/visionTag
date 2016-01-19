/*
 *Created by starsplendid on 15/6/18.
 Copyright (c) 2015年 starsplendid. All rights reserved.
 代码基础来源：http://blog.sina.com.cn/s/blog_662c78590100yyeg.html
 *
 */
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
using namespace cv;
using namespace std;

string intToString(int number){
    
    
    std::stringstream ss;
    ss << number;
    return ss.str();
}
string floatToString(float number){
    
    
    std::stringstream ss;
    ss << number;
    return ss.str();
}
/*
 
 找最大的contours
 http://harismoonamkunnu.blogspot.com/2013/06/opencv-find-biggest-contour-using-c.html
 
 */
int  drawBiggestContours(Mat& inputMat,Mat& orignialMat,float vx , float vy){
    int largest_area=0;
    int largest_contour_index=0;
    Rect bounding_rect;
    vector<vector<Point>> contours; // Vector for storing contour
    vector<Vec4i> hierarchy;

    findContours( inputMat, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
    
    for( int i = 0; i< contours.size(); i++ ) // iterate through each contour.
    {
       double a=contourArea( contours[i],false);//  Find the area of contour
       if(a>largest_area){
             largest_area=a;
             largest_contour_index=i;//Store the index of largest contour
             bounding_rect=boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
           }
    }
    Scalar color( 255,255,255);
    drawContours( orignialMat, contours,largest_contour_index, color, 1, 8, hierarchy ); // Draw the largest contour using previously stored index.
    rectangle(orignialMat, bounding_rect,Scalar(0,255,0),2, 8,0);
    
    // 画中心点，并且标注速度 参考:http://docs.opencv.org/doc/tutorials/imgproc/shapedescriptors/moments/moments.html
    Scalar color1(0,0,255);
    Moments mu = moments(contours[largest_contour_index],false);
    float x  = mu.m10/mu.m00;
    float y =mu.m01/mu.m00 ;
    circle( orignialMat, Point2f(x , y ), 4,  CV_RGB(0,255,255), -1, 8, 0 );
    arrowedLine(orignialMat, Point2f(x, y), Point2f(x+vx, y+vy), CV_RGB(0,255,255),2);
    
    putText(orignialMat,"at ("+intToString(x)+","+intToString(y)+") with vector ("+floatToString(vx)+","+floatToString(vy)+")",bounding_rect.br(),1,1,Scalar(0,255,0),2);
    return 0 ;
}

/*
 找到所有contours 之后每个都画矩形和圆形
 
 http://docs.opencv.org/doc/tutorials/imgproc/shapedescriptors/bounding_rects_circles/bounding_rects_circles.html
 
 */
int drawAllContours(Mat& inputMat,Mat& orignialMat){
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);

    findContours( inputMat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    vector<Point2f>center( contours.size() );
    vector<float>radius( contours.size() );
    
	for(unsigned int i = 0; i < contours.size(); i++ )
    { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
    }
    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );

    
    for(unsigned int i = 0; i < contours.size(); i++ )
    {
        drawContours( orignialMat, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        rectangle( orignialMat, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        circle( orignialMat, center[i], (int)radius[i], color, 2, 8, 0 );
    }

    return 0 ;
}


int maindd()
{
//    VideoCapture cap(0);
    //    ofstream myfile;
//    myfile.open ("flowChange1.txt");
     Mat prevgray, gray, flow, cflow, frame,binaryPic,binaryPic2;
    vector<vector<Point> > contours;

    do
    {
        VideoCapture cap("../out.avi");
        
        if (!cap.isOpened())
        {
            cout<< "Error Acquireing video feed\n";
            return -1;
        }
        while (cap.get(CV_CAP_PROP_POS_FRAMES)< cap.get(CV_CAP_PROP_FRAME_COUNT)-1) {
            cap>>frame;
            cvtColor(frame, gray, CV_BGR2GRAY);
            
            if (prevgray.data)
            {
                calcOpticalFlowFarneback(prevgray, gray, flow, 0.5, 3,15, 3, 5, 1.2, 0);
                cvtColor(prevgray, cflow, CV_GRAY2RGB);
                
                float diff_XY= 0.0 ;
                // 用来计算平均 flow 速度
                int numOfSelectedPoint = 0 ;
                float sumX = 0.0 ;
                float sumY = 0.0 ;
                
                binaryPic = Mat::zeros(prevgray.size(), prevgray.type());
                for (int y = 0; y < cflow.rows; y += 10 )
                {
                    for (int x = 0; x < cflow.cols; x += 10)
                    {
                        Point2f fxy = flow.at<Point2f>(y, x);
                        /*
                         选取光流明显的点
                         */
                        diff_XY =abs(fxy.x)+abs(fxy.y);
                        if (diff_XY > 6) {
                            binaryPic.at<uchar>(y,x) = 128 ;
                            
                            numOfSelectedPoint++ ;
                            sumX += fxy.x  ;
                            sumY += fxy.y  ;
                        }
                        
                        arrowedLine(cflow, Point(x, y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), CV_RGB(0,255,0));
                        
                        circle(cflow, Point(x,y), 2, CV_RGB(255, 0, 0), -1);
                    }
                }
                
               // Mat dilateElement = getStructuringElement( MORPH_RECT,Size(10,10));
               // 
               // dilate(binaryPic,binaryPic2,dilateElement);
               // // binaryPic2 to binary
               // threshold(binaryPic2, binaryPic2, 127.0, 255.0, THRESH_BINARY);
               // // find contours
               // 
               // 
               // drawBiggestContours(binaryPic2,cflow,sumX/numOfSelectedPoint,sumY/numOfSelectedPoint);
               //// drawAllContours(binaryPic2,cflow);
               // 
               // 
                imshow("FLOW", cflow);
//                imshow("Binary", binaryPic);
//                imshow("Binary dilate", binaryPic2);
            }
            if (waitKey(100) >= 0)
            {
                break;
            }
            
            swap(prevgray, gray);
        }
        cout<< "play done"<<endl ;
        cap.release();
     
    } while (1);
    return 0;
}