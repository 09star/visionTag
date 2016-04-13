#include <iostream>
#include <fstream>
#include <vector>
 
using namespace std;
 
int main()
{
    vector<double> v;
     
    ifstream infile;
     
    infile.open("recordVideo.txt");
    
	double firstFrameTime ; 

	double lastFrameTime;
	double totalFrame ;
  
    if(!infile.eof())
    {
        infile>>firstFrameTime;
      
    }
     
	if(!infile.eof())
    {
        infile>>lastFrameTime;
      
    }
	if(!infile.eof())
    {
        infile>>totalFrame;
      
    }
    infile.close();
     
	cout<< lastFrameTime-firstFrameTime<<" "<<lastFrameTime<<" "<<totalFrame<<" "<<(lastFrameTime-firstFrameTime)/totalFrame;
    return 0;
}