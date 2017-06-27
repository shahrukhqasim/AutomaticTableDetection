#include <iostream>
#include <string>
#include "opencv2/core.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char**argv) {
    if(argc!=2) {
        cout<<"Improper arguments \n";
        return -1;
    }

    string imageFileName = argv[1];
    Mat image;
    image = imread(imageFileName, 0);


    return 0;
}