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


const double MHI_DURATION = 1;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;
// number of cyclic frame buffer used for motion detection
// (should, probably, depend on FPS)
const int N = 4;

// ring image buffer
IplImage **buf = 0;
int last = 0;

// temporary images
IplImage *mhi = 0; // MHI
IplImage *orient = 0; // orientation
IplImage *mask = 0; // valid orientation mask
IplImage *segmask = 0; // motion segmentation map
CvMemStorage* storage = 0; // temporary storage
CvMemStorage* storageHaart = 0; // temporary storage



static CvHaarClassifierCascade* cascade = 0;

void detect_and_draw( IplImage* image, IplImage* imgAnterior );
bool analizarFlujo( IplImage* image, IplImage* imgAnterior, CvRect rect);

const char* cascadeFile ="/home/mrc/ProyectosQt/Guda/utilFiles/haartTraining/profe.xml";
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
    IplImage *frameAnterior;
    cascade = (CvHaarClassifierCascade*)cvLoad(cascadeFile, 0, 0, 0 );

    if( !cascade )
    {
        fprintf( stderr,"ERROR: Could not load classifier cascade\n" );
    }
    storageHaart = cvCreateMemStorage(0);

    /*
    if( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0') )
        capture = cvCaptureFromCAM( !input_name ? 0 : input_name[0] - '0' );
    else
        capture = cvCaptureFromAVI( input_name );

    //capture = cvCaptureFromCAM(0);

    */
    capture = cvCaptureFromAVI(aviFile);


    cvNamedWindow( "Detecta", 1 );
    cvNamedWindow( "bluedetect", 1 );
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
                frameAnterior = cvCreateImage(size,
                                        IPL_DEPTH_8U, frame->nChannels );
            }
            if( frame->origin == IPL_ORIGIN_TL ) {
                cvCopy( frame, frame_copy, 0 );
            } else {
                cvFlip( frame, frame_copy, 0 );
            }


            if( cvWaitKey( 10 ) >= 0 ){
                break;
            }
            //copia para trabajar con un solo frame
//            cvWaitKey(100);
//            cvCopy(frame, frameAnterior, 0);

            // allocate images at the beginning or
            //mhi == frame_copy

            if( !mhi) {
                if( buf == 0 ) {
                    buf = (IplImage**)malloc(N*sizeof(buf[0]));
                    memset( buf, 0, N*sizeof(buf[0]));
                }

                for(int i = 0; i < N; i++ ) {
                    cvReleaseImage( &buf[i] );
                    buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
                    cvZero( buf[i] );
                }
                cvReleaseImage( &mhi );
                cvReleaseImage( &orient );
                cvReleaseImage( &segmask );
                cvReleaseImage( &mask );

                mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
                cvZero( mhi ); // clear MHI at the beginning
                orient = cvCreateImage( size, IPL_DEPTH_32F, 1 );
                segmask = cvCreateImage( size, IPL_DEPTH_32F, 1 );
                mask = cvCreateImage( size, IPL_DEPTH_8U, 1 );
            }
            detect_and_draw(frame_copy, frameAnterior);
        }

        cvReleaseImage( &frame_copy );
        cvReleaseImage( &frameAnterior );
        cvReleaseCapture( &capture );
    }
    cvDestroyWindow("Detecta");
    cvDestroyWindow("bluedetect");
    cvDestroyWindow("onlyhaart");
}


bool analizarMhi( IplImage* img, IplImage* dst, int diff_threshold, CvRect rect ) {
    double timestamp = (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
    int i, idx1 = last, idx2;
    IplImage* silh;
    CvSeq* seq;
    CvRect comp_rect;
    cv::Rect result;
    double count;
    double angle;
    CvPoint center;
    double magnitude;
    CvScalar color;

    cvCvtColor( img, buf[last], CV_BGR2GRAY ); // convert frame to grayscale

    idx2 = (last + 1) % N; // index of (last - (N-1))th frame
    last = idx2;

    silh = buf[idx2];
    cvAbsDiff( buf[idx1], buf[idx2], silh ); // get difference between frames

    cvThreshold( silh, silh, diff_threshold, 1, CV_THRESH_BINARY ); // and threshold it
    cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION ); // update MHI

    // convert MHI to blue 8u image
    cvCvtScale( mhi, mask, 255./MHI_DURATION,
                (MHI_DURATION - timestamp)*255./MHI_DURATION );
    cvZero( dst );
    cvCvtPlaneToPix( mask, 0, 0, 0, dst );

    // calculate motion gradient orientation and valid orientation mask
    cvCalcMotionGradient( mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );

    if( !storage )
        storage = cvCreateMemStorage(0);
    else
        cvClearMemStorage(storage);

    // segment motion: get sequence of motion components
    // segmask is marked motion components map. It is not used further
    seq = cvSegmentMotion( mhi, segmask, storage, timestamp, MAX_TIME_DELTA );

    // iterate through the motion components,
    // One more iteration (i == -1) corresponds to the whole image (global motion)
//    for( i = -1; i < seq->total; i++ ) {
    i = 1;
    comp_rect = cvRect( 0, 0, img->width, img->height );
    color = CV_RGB(255,255,255);
    magnitude = 100;
    while (result.area() < 10 & i < seq->total) {

            comp_rect = ((CvConnectedComp*)cvGetSeqElem( seq, i ))->rect;
            if( comp_rect.width + comp_rect.height < 100 ) {// reject very small components
                i++;
                continue;
            }
            color = CV_RGB(255,0,0);
            magnitude = 30;

        // select component ROI
        cvSetImageROI( silh, comp_rect );
        cvSetImageROI( mhi, comp_rect );
        cvSetImageROI( orient, comp_rect );
        cvSetImageROI( mask, comp_rect );

        // calculate orientation
        angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, MHI_DURATION);
        angle = 360.0 - angle;  // adjust for images with top-left origin

        count = cvNorm( silh, 0, CV_L1, 0 ); // calculate number of points within silhouette ROI

        cvResetImageROI( mhi );
        cvResetImageROI( orient );
        cvResetImageROI( mask );
        cvResetImageROI( silh );

        center = cvPoint( (comp_rect.x + comp_rect.width/2),
                          (comp_rect.y + comp_rect.height/2) );

        cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
        cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
                cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );

        result = (cv::Rect)comp_rect & (cv::Rect)rect;
        i++;
    }

    if (result.area() > 10) {
        return true;
    } else {
        return false;
    }
}

bool analizarFlujo(IplImage *img, IplImage *imgAnterior, CvRect *rect) {

        cvSetImageROI(img, *rect);
        IplImage *imgA1 = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
        cvCopy(img, imgA1);

        cvSetImageROI(imgAnterior, *rect);
        IplImage *imgB1 = cvCreateImage(cvGetSize(imgAnterior), imgAnterior->depth, imgAnterior->nChannels);
        cvCopy(imgAnterior, imgB1);

        cvResetImageROI(img);
        cvResetImageROI(imgAnterior);

        cvNamedWindow( "img", 1);
        cvNamedWindow( "imgA", 1);
        cvShowImage( "img", imgA1);
        cvShowImage( "imgA", imgB1);


        int py = imgA1->height;
        int px = imgA1->width;
        IplImage *imgA=cvCreateImage( cvSize(px,py),IPL_DEPTH_8U, 1);
        IplImage *imgB=cvCreateImage( cvSize(px,py),IPL_DEPTH_8U, 1);
        cvCvtColor( imgA1, imgA, CV_BGR2GRAY ); //
        cvCvtColor( imgB1, imgB, CV_BGR2GRAY ); //

        CvSize img_sz = cvGetSize( imgA );

        /////////////////////////////
        IplImage *eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
        IplImage *tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
        int corner_count = MAX_CORNERS;
        CvSize pyr_sz;
        int win_size = 5;


        cvGoodFeaturesToTrack(imgA,eig_image,tmp_image,cornersA,&corner_count,0.01,5.0,0,3,0,0.04);
        cvFindCornerSubPix(imgA,cornersA,corner_count,cvSize(win_size,win_size),cvSize(-1,-1),cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,1));
        // Call the Lucas Kanade algorithm

        pyr_sz = cvSize( imgA->width+8, imgB->height/3 );
        pyrA = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
        pyrB = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );

        cvCalcOpticalFlowPyrLK(imgA,imgB,pyrA,pyrB,cornersA,cornersB,corner_count,cvSize( win_size,win_size ),5,features_found,feature_errors,cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .03 ),0);
        // Now make some image of what we are looking at:
        float dx=0.0;

        for (int i=0; i<corner_count; i++) {
            if( features_found[i]==0|| feature_errors[i]>100 ) {continue;}

            dx=sqrt((cornersA[i].x-cornersB[i].x)*(cornersA[i].x-cornersB[i].x)+(cornersA[i].y-cornersB[i].y)*(cornersA[i].y-cornersB[i].y));
            if(dx>1 && dx<50){
                return true;
            } else {
                return false;
            }
        }
        return false;
}


void detect_and_draw( IplImage* img, IplImage* imgAnterior ) {
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

            if ((rect.height < (img->height + 1)) & (rect.width < (img->width + 1))
                    & analizarMhi(img, imgAnterior, 30, rect)) {
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

//            if (analizarFlujo(img, imgAnterior, r)) {
//                CvPoint center;
//                int radius;
//                center.x = cvRound((r->x + r->width*0.5)*scale);
//                center.y = cvRound((r->y + r->height*0.5)*scale);
//                radius = cvRound((r->width + r->height)*0.25*scale);
//                cvCircle( img, center, radius, colors[i%8], 3, 8, 0 );
//            }
        }
    }
    cvShowImage( "Detecta", img );
    cvShowImage( "onlyhaart", onlyhaart);
    cvShowImage("bluedetect", imgAnterior);
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );
}
