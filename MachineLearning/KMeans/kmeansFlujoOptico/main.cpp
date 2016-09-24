#include <QCoreApplication>
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include "stdio.h"
#include "iostream"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

int main(){
    vector<Point> vec;
    for (int i =0; i < 10; i++) {
        Point punto;
        punto.x = i*i;
        punto.y = i*i;
        vec.push_back(punto);
    }
    Mat point = Mat(20, 2, CV_32F, Scalar(0));
    for (int i =0; i < vec.size(); i++) {
        float* PointX = point.ptr<float> (i, 0);
        *PointX = vec[i].x;
        float* PointY = point.ptr<float> (i, 1);
        *PointY = vec[i].y;
    }
    int clustersNumbers = 2;
    int flags = KMEANS_RANDOM_CENTERS;
    Mat centers, labels;
    kmeans(point, clustersNumbers, labels,
           TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 2, 1.0)
           , 3, flags, centers);
    for (int i=0; i < labels.rows; i++) {
        cout << "punto " << point.at<float>(i,0) << " " << point.at<float>(i,1) << " "<<labels.at<int>(i) << "\n";
    }
    for (int i=0; i < centers.rows; i++) {
        cout << "centro " << i << " " << centers.at<float>(i,0) << " " << centers.at<float>(i,1) << "\n";
    }
    namedWindow("centros", (CV_WINDOW_NORMAL));
    imshow("centros", point);
    waitKey(0);
}

