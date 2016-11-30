#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

class proceso {
    IplImage *image = 0, *grey = 0, *prev_grey = 0, *pyramid = 0, *prev_pyramid = 0, *swap_temp;
    int win_size = 10;
    const int MAX_COUNT = 500;
    CvPoint2D32f* points[2] = {0,0}, *swap_points;
    CvPoint2D32f* points1[2] = {0,0};
    CvPoint2D32f* points2[2] = {0,0};
    char* status = 0;
    int count = 0;
    bool need_to_init = false;
    int flags = 0;
    int contador=0;
    int detecta1=0;
    int detecta2=0;
    int x=0;
    int y=0;
    int valor=0;
    double quality = 0.01;
    double min_distance = 10;
    double dx=0.0;
    double dy=0.0;
    double ds=0.0;
    double distancia=0.0;

    ///////////////////////////////////////////////////////////////////////////////////////
    CvMemStorage* storage = 0;
    CvHaarClassifierCascade* cascade = 0;
    CvPoint center;
    CvRect* r;
    CvSeq* faces;
    CvCapture* capture = 0;
    IplImage *frame, *frame_copy = 0;
    const char* cascade_name ="/home/mrc/ProyectosQt/Guda/utilFiles/haartTraining/carsPinchi.xml";
    const char* aviFile ="/home/mrc/ProyectosQt/Guda/utilFiles/video/ca3.avi";

    double scale = 1.3;
    int i,ii,k,c;
    int radius;

public:int principal() {
    CvScalar colores[] = {
                {{0,0,255}},
                {{0,128,255}},
                {{0,255,255}},
                {{0,255,0}},
                {{255,128,0}},
                {{255,255,0}},
                {{255,0,0}},
                {{255,0,255}}
            };
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    storage = cvCreateMemStorage(0);
    capture = cvCaptureFromAVI(aviFile);

    cvNamedWindow( "Movimiento", 1 );
    cvNamedWindow( "Resultado", 1 );
 ////////////////////////////////////////////////////////////////
    points[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
    points[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
    points1[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points1[0][0]));
    points1[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points1[0][0]));
    points2[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points2[0][0]));
    points2[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points2[0][0]));
    status = (char*)cvAlloc(MAX_COUNT);
//////////////////////////////////////////////////////////////////
//    while(1) {
        frame = cvQueryFrame( capture );
        if( !image ) {
            image = cvCreateImage( cvGetSize(frame), 8, 3 );
            image->origin = frame->origin;
            grey = cvCreateImage( cvGetSize(frame), 8, 1 );
            prev_grey = cvCreateImage( cvGetSize(frame), 8, 1 );
            pyramid = cvCreateImage( cvGetSize(frame), 8, 1 );
            prev_pyramid = cvCreateImage( cvGetSize(frame), 8, 1 );
            flags = 0;
        }

        cvCopy( frame, image, 0 );
        cvCvtColor( image, grey, CV_BGR2GRAY );

        if( need_to_init ) {
            IplImage* eig = cvCreateImage( cvGetSize(grey), 32, 1 );
            IplImage* temp = cvCreateImage( cvGetSize(grey), 32, 1 );
            count = MAX_COUNT;
            cvGoodFeaturesToTrack( grey, eig, temp, points[1], &count, quality,
                    min_distance, 0, 3, 0, 0.04 );
            cvFindCornerSubPix( grey, points[1], count,cvSize(win_size,win_size),
                    cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03));cvReleaseImage( &eig );
            cvReleaseImage( &temp );
            cvReleaseImage( &eig );
        } else if( count > 0 ) {
            cvCalcOpticalFlowPyrLK( prev_grey, grey, prev_pyramid, pyramid, points[0], points[1], count,
                    cvSize(win_size,win_size), 3, status, 0, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), flags );
            flags |= CV_LKFLOW_PYR_A_READY;
            for( i = k = 0; i < count; i++ ) {
                points[1][k++] = points[1][i];
                if(detecta1==1) {
                    x=points1[1][i].x=points[1][i].x;
                    y=points1[1][i].y=points[1][i].y;

                }
                if(detecta2==1) {
                    dx=(points1[1][i].x-points[1][i].x);
                    dy=(points1[1][i].y-points[1][i].y);
                    distancia=sqrt(pow(dx,2)+pow(dy,2));
                    if(distancia>5) {
                        points2[1][valor++]=points[1][i];cvCircle( image, cvPointFrom32f(points[1][i]), 3, CV_RGB(0,255,0), -1, 8,0);
                    }
                }
            }
            count = k;
        }
        CV_SWAP( prev_grey, grey, swap_temp );
        CV_SWAP( prev_pyramid, pyramid, swap_temp );
        CV_SWAP( points[0], points[1], swap_points );
        need_to_init = false;

        ////////////////////////////////////////////////////////
        IplImage* gray = cvCreateImage( cvSize(frame->width,frame->height), 8, 1 );
        IplImage* small_img = cvCreateImage( cvSize( cvRound (frame->width/scale),cvRound (frame->height/scale)),8, 1 );
        cvCvtColor( frame, gray, CV_BGR2GRAY );
        cvResize( gray, small_img, CV_INTER_LINEAR );
        cvEqualizeHist( small_img, small_img );
        cvClearMemStorage( storage );
        faces = cvHaarDetectObjects( small_img, cascade, storage,1.1, 2, 0,cvSize(30, 30) );
            for( ii = 0; ii < (faces ? faces->total : 0); ii++ ) {
                r = (CvRect*)cvGetSeqElem( faces, ii );
                center.x = cvRound((r->x + r->width*0.5)*scale);
                center.y = cvRound((r->y + r->height*0.5)*scale);
                radius = cvRound((r->width + r->height)*0.25*scale);
                cvCircle( image, center, radius, colores[i%8], 3, 8, 0 );
                for(i=0;i<valor;i++) {
                    x=points2[1][i].x-center.x;
                    y=points2[1][i].y-center.y;
                    ds=pow((x*x+y*y),0.5);
                    if(ds<10.0) {
                        cvCircle( frame, center, radius, colores[i%8], 3, 10, 0 );
                        //cvRectangle(frame,center.x,center.y,cvRound((r->width*0.5)*scale),
                        //cvRound((r->height*0.5)*scale),cvScalar(255,0,0));
                    }
                    printf("%0.2f",(float)ds);
                    printf("\n");
                }
            }
            valor=0;
            cvReleaseImage( &gray );
            cvReleaseImage( &small_img );
            ///////////////////////////////////////////////////////
            cvShowImage( "Resultado", frame);
            cvShowImage( "Movimiento", image);
            c = cvWaitKey(1);
            contador++;

//            if( (char)c == 27 )break;

            if(contador==5) {
                need_to_init = true;
                detecta1=1;
                detecta2=0;
            }
            if(contador==7) {
                detecta1=0;
                detecta2=1;
            }
            if(contador==8) {
                contador=0;
            }
//    }

    cvReleaseImage( &frame_copy );
    cvReleaseCapture( &capture );
    cvDestroyWindow("Resultado");
    cvDestroyWindow("Movimiento");
}
};
