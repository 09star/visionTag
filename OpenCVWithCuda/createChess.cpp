//  createChessPicture.cpp
//  OpenCVPlay
//
//  Created by starsplendid on 15/12/24.
//  Copyright (c) 2015Äê starsplendid. All rights reserved.
//

#include <opencv2/opencv.hpp>

#include "opencv2/highgui/highgui.hpp"

#include <string>



using namespace cv;

using namespace std;



void setBlock(IplImage*img, int x, int y,int step)

{
    
    for (int i = x; i < x + step; i++)
        
    {
        
        for (int j = y; j < y + step; j++)
            
        {
            
            cvSet2D(img, i, j, Scalar(0, 0, 0));
            
        }
        
    }
    
}



int main()

{
    
    IplImage*f = cvCreateImage(cvSize(700, 700), IPL_DEPTH_8U, 3);
    
    cvSet(f, Scalar(255, 255, 255));
    
    bool black = true;
    
    
    
    for (int i = 0; i < 700; i+=100)
        
    {
        
        for (int j = 0; j < 700; j+=100)
            
        {
            
            if (black)
                
            {
                
                setBlock(f, i, j, 100);
                
                black = !black;
                
            }
            
            else
                
            {
                
                black = !black;
                
                continue;
                
            }
            
        }
        
    }
    
    cvShowImage("img", f);
	
	Mat matImage = cvarrToMat(f);
	imwrite("../chess.jpg",matImage);
    
    
    waitKey(0);
    
    
    
    return 0;
    
}

