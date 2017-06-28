#include <iostream>
#include <string>
#include "opencv2/core.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <docproc/binarize/binarize.h>
#include <docproc/segment/segment.h>

using namespace std;
using namespace cv;

Point2i midPoint(Rect r) {
    return Point2i(r.x+r.width/2,r.y+r.height/2);
}

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

    vector<Point2i> intersections;
    vector<Rect>allComponents(verticalComponents);
    allComponents.insert(allComponents.end(), horizontalComponents.begin(), horizontalComponents.end());

    for(auto i : allComponents) {
        for(auto j : allComponents) {
            if(i==j)
                continue;
            Rect intersection = (i&j);
            if(intersection.area()>0) {
                intersections.push_back(midPoint(intersection));
            }

        }
    }

    for(auto i:intersections) {
        circle(imageRgb, i,10, Scalar(0,0,255),CV_FILLED, 8,0);
    }

    imwrite("bin.png", binarizedImage);
    imwrite("horizontal.png", horizontalLines);
    imwrite("vertical.png", verticalLines);
    imwrite("rgb.png", imageRgb);


    return 0;
}