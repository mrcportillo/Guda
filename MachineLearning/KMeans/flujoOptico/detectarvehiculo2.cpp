
//#include "cv.h" // incluye las definiciones de OpenCV
#include <opencv/cv.h>
//#include "cvaux.h" // Stuff.
#include <opencv/cvaux.h>
//#include "highgui.h" // incluye highGUI. Necesario para crear ventanas de visualizacion
#include <opencv/highgui.h>
#include "iostream"
#include "detectarvehiculo2.h"
#include "mainwindow.h"

using namespace cv;
using namespace std;

const int MAX_CORNERS = 100;
const double MHI_DURATION = 2;
const int PUNTOS_CARACTERISTICOS = 50, CLUSTERS_NUMBERS = 2;

Mat DetectarVehiculo2::clasificar(vector<Point> puntosCaracteristicos){
    int flags = KMEANS_RANDOM_CENTERS;
    Mat centers, labels;

    Mat point = Mat(100, 2, CV_32F, Scalar(0));
    for (int i =0; i < puntosCaracteristicos.size(); i++) {
        float* PointX = point.ptr<float> (i,0);
        *PointX = puntosCaracteristicos[i].x;
        float* PointY = point.ptr<float> (i,1);
        *PointY = puntosCaracteristicos[i].y;
    }
    kmeans(point, CLUSTERS_NUMBERS, labels,
        TermCriteria(TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0)
           , 3, flags, centers);
    for (int i=0; i <= 19; i++) {
        cout << "punto " << point.at<float>(i,0) << " "
             << point.at<float>(i,1) << " "<<labels.at<int>(i) << "\n";
    }
    return labels;

}


void DetectarVehiculo2::comenzar() {
    //////////////////////////////////////////////////////////

    // Initialize, load two images from the file system, and
    // allocate the images and other structures we will need for
    // results.
    int corner_count = MAX_CORNERS;
    CvCapture* capture = 0;
    CvCapture* capture1 = 0;
    IplImage* imgA;
    IplImage* imgB;
    IplImage* imgA1=NULL;
    IplImage* imgB1;
    IplImage* imgC1=NULL;
    IplImage* eig_image;
    IplImage* tmp_image;
    CvSize img_sz;
    int win_size = 5;
    IplImage* imgC;
    CvPoint2D32f* cornersA = new CvPoint2D32f[ MAX_CORNERS ];
    CvPoint2D32f* cornersB = new CvPoint2D32f[ MAX_CORNERS ];
    CvSize pyr_sz;
    char features_found[ MAX_CORNERS ];
    float feature_errors[ MAX_CORNERS ];
    float dx=0.0;
    int contador=0;
    int a=1;
    int px=0;
    int py=0;
    CvPoint p0, p1;
    cvNamedWindow("Image A",1);
    cvNamedWindow("Image B",1);
    IplImage* pyrA ;
    IplImage* pyrB ;
    // The first thing we need to do is get the features
    // we want to track.
    double timestamp = (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
    capture = cvCaptureFromCAM(0);
    //capture1 = cvCaptureFromCAM(1);

    Mat matrizDeSalida;
    std::vector<Point> vec;
    std::vector<CvPoint> vectorDePuntos;
    float anguloPuntos;

    while(a)
    {

        ////////////////////////////////////////////////////
        imgC1 = cvQueryFrame( capture );

        //////////////////////////////////////////////////

        imgA1= cvCreateImage(cvGetSize(imgC1), imgC1->depth, imgC1->nChannels);
        cvCopy(imgC1, imgA1, NULL);

        cvWaitKey(500);

        imgB1 = cvQueryFrame( capture );

        ////////////////////////////////////////////////////
        py = imgA1->height;
        px = imgA1->width;
//        imgA=cvCreateImage( cvSize(px,py),IPL_DEPTH_32F, 1);
//        imgB=cvCreateImage( cvSize(px,py),IPL_DEPTH_32F, 1);
//        imgC=cvCreateImage( cvSize(px,py),IPL_DEPTH_32F, 3);
        imgA=cvCreateImage( cvSize(px,py),IPL_DEPTH_8U, 1);
        imgB=cvCreateImage( cvSize(px,py),IPL_DEPTH_8U, 1);
        imgC=cvCreateImage( cvSize(px,py),IPL_DEPTH_8U, 3);
        cvCvtColor( imgA1, imgA, CV_BGR2GRAY ); //
        cvCvtColor( imgB1, imgB, CV_BGR2GRAY ); //



        img_sz = cvGetSize( imgA );



        /////////////////////////////
        eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
        tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );

        cvGoodFeaturesToTrack(imgA,eig_image,tmp_image,cornersA,&corner_count,0.01,5.0,0,3,0,0.04);
        cvFindCornerSubPix(imgA,cornersA,corner_count,cvSize(win_size,win_size),cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,1));
        // Call the Lucas Kanade algorithm

        pyr_sz = cvSize( imgA->width+8, imgB->height/3 );
        pyrA = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
        pyrB = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );

        cvCalcOpticalFlowPyrLK(imgA1,imgB1,pyrA,pyrB,cornersA,cornersB,corner_count,cvSize( win_size,win_size ),5,features_found,feature_errors,cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .03 ),0);
        // Now make some image of what we are looking at:


        vec.clear();
        vectorDePuntos.clear();
        for( int i=0; i<corner_count; i++ )
        {
            if( features_found[i]==0|| feature_errors[i]>100 ) {continue;}

            dx=sqrt((cornersA[i].x-cornersB[i].x)*(cornersA[i].x-cornersB[i].x)+(cornersA[i].y-cornersB[i].y)*(cornersA[i].y-cornersB[i].y));
            //if((dx>5)&&(dx<15)){
            if(dx>5 && dx<50){
                p0 = cvPoint(cvRound( cornersA[i].x ),cvRound( cornersA[i].y));
                p1 = cvPoint(cvRound( cornersB[i].x ),cvRound( cornersB[i].y));
//                cvLine( imgB1, p0, p1, CV_RGB(255,0,0),5 );
                vectorDePuntos.push_back(p0);
                vectorDePuntos.push_back(p1);

                anguloPuntos = atan2(p0.y - p1.y, p0.x - p1.x) * 180 / 3,14;
                if (anguloPuntos<0) {
                    anguloPuntos = 360 + anguloPuntos;
                }
                vec.push_back(Point(dx,anguloPuntos));
                if (vec.size() > PUNTOS_CARACTERISTICOS) {
                    matrizDeSalida = clasificar(vec);
                    int i = 0, j = 0;

                    while (i < matrizDeSalida.rows) {
                        int clusterId = matrizDeSalida.at<int>(i);;
                        while (clusterId = matrizDeSalida.at<int>(i)) {
                            cvLine( imgB1, vectorDePuntos[j], vectorDePuntos[j+1]
                                    , CV_RGB(10*i, 10*i, 10*j),5 );
                            i = i + 1;
                            j = j + 2;
                        }
                    }
                }


            }
            /*if(dx>20){
                p0 = cvPoint(cvRound( cornersA[i].x ),cvRound( cornersA[i].y));
                p1 = cvPoint(cvRound( cornersB[i].x ),cvRound( cornersB[i].y));
                if(p0.x)
                    cvLine( imgB1, p0, p1, CV_RGB(0,0,int(dx+80)),10 );
            }*/

        }
        /////////////////////////////////////////////////////////////////
        cvShowImage("Image A",imgA1);
        cvShowImage("Image B",imgB1);
        /////////////////////////////////////////////////////////////////
        //cvAbsDiff( imgA1, imgB1, imgC ); // get difference between frames
        //cvShowImage("Diferencia",imgC);
        /////////////////////////////////////////
        ///
        ///
        ///

        cvReleaseImage(&imgA1);
        //cvReleaseImage(&imgB1);
        //cvReleaseImage(&imgC1);
        cvReleaseImage(&eig_image);
        cvReleaseImage(&tmp_image);
        cvReleaseImage(&imgA);
        cvReleaseImage(&imgB);
    }

}

DetectarVehiculo2::DetectarVehiculo2(QObject *parent) : QObject(parent)
{

}

