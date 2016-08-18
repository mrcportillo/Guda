#include <QCoreApplication>
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include "stdio.h"
#include "iostream"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

IplImage* fuente1;
int select_object = 0;

int colorChalR=0;
int colorChalG=0;
int colorChalB=0;
CvScalar ss;

void calcularHist() {
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    int channels[] = {0};
    Mat hist;
    cvtColor(cv::cvarrToMat(fuente1), hist, COLOR_RGB2GRAY);
    MatND histR;
    calcHist(&hist, 1, channels, Mat(), histR, 1, &histSize, &histRange, true, false);
    cout << "valor en histograma" << histR.at<float>((int)ss.val[2] << endl);
}

void on_mouse( int event, int x, int y, int flags, void* param )
{
    if( !fuente1 )
        return;

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:{
        ss= cvGet2D(fuente1, y, x);
        colorChalB=(int)ss.val[0];
        colorChalG=(int)ss.val[1];
        colorChalR=(int)ss.val[2];
        cout<<"Se selecciono el color del chaleco \n"<<endl;
        cout<< colorChalR<<" "<<colorChalG<<" "<<colorChalB<<endl;
        calcularHist();
        break;
    }
    case CV_EVENT_LBUTTONUP:
        break;
    }
}

void imagen() {
    char frame1[] = "/home/mrc/ProyectosQt/imagenes/2.jpg";
//    fuente1 = cvLoadImage(frame1, CV_LOAD_IMAGE_GRAYSCALE);
    fuente1 = cvLoadImage(frame1, 1);
    cvNamedWindow("Fuente1", CV_WINDOW_AUTOSIZE);
    cvSetMouseCallback( "Fuente1", on_mouse, 0 );
    cvShowImage("Fuente1", fuente1);
}

int main()
{
    imagen();
    cvWaitKey(0);
    cvDestroyAllWindows();
    return 0;
}
