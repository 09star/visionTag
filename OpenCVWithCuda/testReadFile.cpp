#include <iostream>
#include <fstream>
#include <vector>
 
using namespace std;
 
int main()
{
   /** vector<double> v;
     
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
    */
	   ifstream infile;
	
	infile.open("frameTime.txt");
		vector<double> videoFrameTime ; 
		double tempFrameTime ; 
		while(!infile.eof()){
			infile>>tempFrameTime;
			cout<<tempFrameTime<<endl;
			infile>>tempFrameTime;
			cout<<tempFrameTime<<endl;
			videoFrameTime.push_back(tempFrameTime);
		}
		for(int i=0;i<videoFrameTime.size();i++){
			cout<<videoFrameTime[i]<<endl;
		}
	return 0;
}