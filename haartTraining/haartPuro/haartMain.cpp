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

CvMemStorage* storageHaart = 0; // temporary storage

static CvHaarClassifierCascade* cascade = 0;

void detect_and_draw( IplImage* image);

const char* cascadeFile ="/home/mrc/ProyectosQt/Guda/utilFiles/haartTraining/cars.xml";
const char* aviFile ="/home/mrc/ProyectosQt/Guda/utilFiles/video/ca3.avi";

const int MAX_CORNERS = 100;

CvPoint2D32f* cornersA = new CvPoint2D32f[ MAX_CORNERS ];
CvPoint2D32f* cornersB = new CvPoint2D32f[ MAX_CORNERS ];
char features_found[ MAX_CORNERS ];
float feature_errors[ MAX_CORNERS ];
IplImage* pyrA;
IplImage* pyrB;

int main(void) {
    CvCapture* capture = 0;
    IplImage *frame, *frame_copy = 0;
    cascade = (CvHaarClassifierCascade*)cvLoad(cascadeFile, 0, 0, 0 );

    if( !cascade ) {
        fprintf( stderr,"ERROR: Could not load classifier cascade\n" );
    }
    storageHaart = cvCreateMemStorage(0);
    capture = cvCaptureFromAVI(aviFile);


    cvNamedWindow( "Detecta", 1 );
    cvNamedWindow( "onlyhaart", 1 );

    if (capture) {
        for(;;) {
            if( !cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture );
            CvSize size = cvSize(frame->width,frame->height); // get current frame size
            if( !frame )
                break;
            if( !frame_copy ) {
                frame_copy = cvCreateImage(size,
                                           IPL_DEPTH_8U, frame->nChannels );
            }
            if( frame->origin == IPL_ORIGIN_TL ) {
                cvCopy( frame, frame_copy, 0 );
            } else {
                cvFlip( frame, frame_copy, 0 );
            }
            if( cvWaitKey( 1 ) >= 0 ){
                break;
            }
            detect_and_draw(frame_copy);
        }

        cvReleaseImage( &frame_copy );
        cvReleaseCapture( &capture );
    }
    cvDestroyWindow("Detecta");
    cvDestroyWindow("onlyhaart");
}

void detect_and_draw( IplImage* img) {
    static CvScalar colors[] = {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}}
    };

    double scale = 1.3;
    IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
                                                 cvRound (img->height/scale)),
                                         8, 1 );
    int i;

    IplImage* onlyhaart = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
    cvCopy(img, onlyhaart);

    cvCvtColor( img, gray, CV_BGR2GRAY );
    cvResize( gray, small_img, CV_INTER_LINEAR );
    cvEqualizeHist( small_img, small_img );
    cvClearMemStorage( storageHaart );

    if(cascade) {
        double t = (double)cvGetTickCount();
        CvSeq* faces = cvHaarDetectObjects(small_img, cascade, storageHaart,
                                           1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/,
                                           cvSize(30, 30) );
        t = (double)cvGetTickCount() - t;
        for(i = 0; i < (faces ? faces->total : 0); i++ ) {
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

            CvRect rect = cvRect(r->x, r->y, r->width, r->height);

            if ((rect.height < (img->height + 1)) & (rect.width < (img->width + 1))) {
                printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*100.) );
                CvPoint center;
                int radius;
                center.x = cvRound((rect.x + rect.width*0.5)*scale);
                center.y = cvRound((rect.y + rect.height*0.5)*scale);
                radius = cvRound((rect.width + rect.height)*0.25*scale);
                cvCircle( img, center, radius, colors[i%8], 3, 8, 0 );
            }
            CvPoint center;
            int radius;
            center.x = cvRound((rect.x + rect.width*0.5)*scale);
            center.y = cvRound((rect.y + rect.height*0.5)*scale);
            radius = cvRound((rect.width + rect.height)*0.25*scale);
            cvCircle( onlyhaart, center, radius, colors[i%8], 3, 8, 0 );
        }
    }
    cvShowImage( "Detecta", img );
    cvShowImage( "onlyhaart", onlyhaart);
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );
}
