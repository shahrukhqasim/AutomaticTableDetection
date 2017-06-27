#include <iostream>
#include <string>
#include "opencv2/core.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <docproc/binarize/binarize.h>
#include <docproc/segment/segment.h>

using namespace std;
using namespace cv;

int main(int argc, char**argv) {
    // Read the image
    if(argc!=2) {
        cout<<"Improper arguments \n";
        return -1;
    }

    string imageFileName = argv[1];
    Mat image;
    image = imread(imageFileName, 0);
    Mat imageRgb = imread(imageFileName, 1);

    // Binarize the image
    Mat binarizedImage;
    Mat temp;
    docproc::binarize::binarizeBG(image, temp, binarizedImage);

    // Find lines
    Mat horizontalLines;
    Mat verticalLines;
    docproc::segment::findLineMasks(binarizedImage, horizontalLines, verticalLines);

    vector<Rect>horizontalComponents;
    vector<Rect>verticalComponents;

    horizontalComponents = docproc::segment::findConnectedComponents(horizontalLines);
    verticalComponents = docproc::segment::findConnectedComponents(verticalLines);

    Scalar colors[] = {Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(0, 0, 255), Scalar(0, 255, 255),
                       Scalar(255, 255, 0), Scalar(), Scalar(255, 0, 255)};

    int x=0;
    for(auto i:horizontalComponents) {
        x++;
        rectangle(imageRgb, i, colors[x % 6], 2);
    }
    for(auto i:verticalComponents) {
        x++;
        rectangle(imageRgb, i, colors[x % 6], 2);
    }

    vector<Point2i> intersections;

    for(auto i:horizontalComponents) {
    }

    imwrite("bin.png", binarizedImage);
    imwrite("horizontal.png", horizontalLines);
    imwrite("vertical.png", verticalLines);
    imwrite("rgb.png", imageRgb);


    return 0;
}