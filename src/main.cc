#include <iostream>
#include <string>
#include "opencv2/core.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <docproc/binarize/binarize.h>
#include <docproc/segment/segment.h>
#include <docproc/clean/clean.h>

using namespace std;
using namespace cv;

Point2i midPoint(Rect r) {
    return Point2i(r.x+r.width/2,r.y+r.height/2);
}

void linearRegression(vector<Point2i>points) {
    //    Regression Equation(y) = a + bx
    //    Slope(b) = (NΣXY - (ΣX)(ΣY)) / (NΣX2 - (ΣX)2)
    //    Intercept(a) = (ΣY - b(ΣX)) / N
    long sum_XY = 0;
    long sum_X = 0;
    long sum_Y = 0;
    long sum_X_2 = 0;
    for(auto i:points) {
        sum_XY += i.x*i.y;
        sum_X += i.x;
        sum_Y += i.y;
        sum_X_2 += i.x*i.x;
    }

}

struct HorizontalLine {
    int x_min;
    int x_max;
    int y;
};

struct VerticalLine {
    int x;
    int y_min;
    int y_max;
};

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

    Mat cleanedImage = docproc::clean::removeLines(binarizedImage);
    cvtColor(cleanedImage,cleanedImage,CV_GRAY2BGR);

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

    const int THRESHOLD = 10;

    Mat onlyIntersectionsImage (cleanedImage.rows,cleanedImage.cols, CV_8UC1, 255);
    for(auto i : intersections) {
        circle(onlyIntersectionsImage, i,5, 0,CV_FILLED, 8,0);
    }
    imwrite("only_intersections.png", onlyIntersectionsImage);
    vector<Rect> intersectionComponents = docproc::segment::findConnectedComponents(onlyIntersectionsImage);

    vector<vector<Point2i>>horizontalGroups;
    vector<vector<Point2i>>verticalGroups;

    {
        Mat verticalGroupImage = onlyIntersectionsImage.clone();
        for (auto i : verticalComponents) {
            rectangle(verticalGroupImage, i, 0, CV_FILLED, 8, 0);
        }
        vector<Rect> verticalGroupComponents = docproc::segment::findConnectedComponents(verticalGroupImage);


        for (auto i : verticalGroupComponents) {
            vector<Point2i> verticalGroup;
            for (auto j : intersectionComponents) {
                if ((i & j).area() > 0) {
                    verticalGroup.push_back(Point2i(j.x + j.width / 2, j.y + j.height / 2));
                }
            }
            if (verticalGroup.size() > 0) {
                verticalGroups.push_back(verticalGroup);
            }
        }
        imwrite("intersections_vertical.png", verticalGroupImage);
    }
    {
        Mat horizontalGroupImage = onlyIntersectionsImage.clone();
        for (auto i : horizontalComponents) {
            rectangle(horizontalGroupImage, i, 0, CV_FILLED, 8, 0);
        }
        vector<Rect> horizontalGroupComponents = docproc::segment::findConnectedComponents(horizontalGroupImage );


        for (auto i : horizontalGroupComponents) {
            vector<Point2i> horizontalGroup;
            for (auto j : intersectionComponents) {
                if ((i & j).area() > 0) {
                    horizontalGroup.push_back(Point2i(j.x + j.width / 2, j.y + j.height / 2));
                }
            }
            if (horizontalGroup.size() > 0) {
                horizontalGroups.push_back(horizontalGroup);
            }
        }
        imwrite("intersections_horizontal.png", horizontalGroupImage);
    }


//    return 0;

//    vector<vector<Point2i>>horizontalGroups;
//    vector<vector<Point2i>>verticalGroups;
//
//    sort(intersections.begin(),intersections.end(),[] (const auto& lhs, const auto& rhs) {
//        return lhs.y<rhs.y;
//    });
//
//    if(intersections.size()>0)
//    {
//        Point2i temporaryLast;
//        vector<Point2i> temporary;
//        for(auto i : intersections) {
//            if(temporary.size()==0) {
//                temporary.push_back(i);
//            }
//            else if(abs(temporaryLast.y - i.y) <= THRESHOLD){
//                temporary.push_back(i);
//            }
//            else {
//                horizontalGroups.push_back(temporary);
//                temporary.clear();
//            }
//            temporaryLast = i;
//        }
//        horizontalGroups.push_back(temporary);
//    }
//
//    sort(intersections.begin(),intersections.end(),[] (const auto& lhs, const auto& rhs) {
//        return lhs.x<rhs.x;
//    });
//
//    if(intersections.size()>0)
//    {
//        Point2i temporaryLast;
//        vector<Point2i> temporary;
//        for(auto i : intersections) {
//            if(temporary.size()==0) {
//                temporary.push_back(i);
//            }
//            else if(abs(temporaryLast.x - i.x) <= THRESHOLD){
//                temporary.push_back(i);
//            }
//            else {
//                verticalGroups.push_back(temporary);
//                temporary.clear();
//            }
//            temporaryLast = i;
//        }
//        verticalGroups.push_back(temporary);
//    }


    cout<<"All:\n";
    for(auto j:intersections) {
        cout<<"("<<j.x<<","<<j.y<<") ";
    }
    cout<<endl;

    vector<HorizontalLine>horizontalLinesX;
    vector<VerticalLine>verticalLinesX;

    cout<<"Horizontal:\n";
    for(auto i:horizontalGroups) {
        if(i.size()<2)
            continue;
        HorizontalLine horizontalLine;
        horizontalLine.x_min = i[0].x;
        horizontalLine.x_max = i[0].x;

        long sum=0;
        int count=0;
        Point2i last(-1,-1);
        for(auto j:i) {
            horizontalLine.x_min = min(horizontalLine.x_min,j.x);
            horizontalLine.x_max = max(horizontalLine.x_max,j.x);
            cout<<"("<<j.x<<","<<j.y<<") ";
            if(j!=last) {
                sum+=j.y;
                count++;
            }
            last=j;
        }
        horizontalLine.y=sum/count;
        cout<<"Y: "<<horizontalLine.y<<endl;
        cout<<"Min: "<<horizontalLine.x_min<<endl;
        cout<<"Max: "<<horizontalLine.x_max<<endl;
        horizontalLinesX.push_back(horizontalLine);
        cout<<endl;
    }
    cout<<"Vertical:\n";
    for(auto i:verticalGroups) {
        if(i.size()<2)
            continue;
        VerticalLine verticalLine;
        verticalLine.y_min = i[0].y;
        verticalLine.y_max = i[0].y;
        long sum=0;
        int count=0;
        Point2i last(-1,-1);
        for(auto j:i) {
            verticalLine.y_min = min(verticalLine.y_min,j.y);
            verticalLine.y_max = max(verticalLine.y_max,j.y);
            cout<<"("<<j.x<<","<<j.y<<") ";
            if(j!=last) {
                sum+=j.x;
                count++;
            }
            last=j;
        }
        verticalLine.x=sum/count;
        verticalLinesX.push_back(verticalLine);
        cout<<endl;
    }

    for(auto i:horizontalLinesX) {
        line(cleanedImage,Point2i(i.x_min,i.y),Point2i(i.x_max,i.y),Scalar(0,255,0),2);
    }
    for(auto i:verticalLinesX) {
        line(cleanedImage,Point2i(i.x,i.y_min),Point2i(i.x,i.y_max),Scalar(0,255,0),1);
    }


    for(auto i:intersections) {
        circle(cleanedImage, i,10, Scalar(0,0,255),CV_FILLED, 8,0);
    }

    imwrite("bin.png", binarizedImage);
    imwrite("horizontal.png", horizontalLines);
    imwrite("vertical.png", verticalLines);
    imwrite("rgb.png", cleanedImage);


    return 0;
}