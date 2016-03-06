#include <iostream>  
#include <time.h>
#include <sys/timeb.h>
#include <sstream>
 
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
//
//int main(){
//
//	cout <<"ms :"<< getCurrentTime(); 
//}