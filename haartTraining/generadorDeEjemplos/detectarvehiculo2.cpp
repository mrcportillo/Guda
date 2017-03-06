
//#include "cv.h" // incluye las definiciones de OpenCV
#include <opencv/cv.h>
//#include "cvaux.h" // Stuff.
#include <opencv/cvaux.h>
//#include "highgui.h" // incluye highGUI. Necesario para crear ventanas de visualizacion
#include <opencv/highgui.h>
#include "iostream"
#include "detectarvehiculo2.h"
#include "mainwindow.h"

IplImage *image = 0;
IplImage *grey= 0;
IplImage *small_image= 0;
IplImage *frameOriginalGray=0;
IplImage *frameOriginal=0;
int select_object = 0;
int contador=0;
int xx,yy,ww,hh;

CvPoint origin;
CvRect selection;
IplImage* frame;
std::vector<CvRect> listSelection;


void on_mouse( int event, int x, int y, int flags, void* param )
{

    if( !image )
        return;

    if( image->origin )
        y = image->height - y;

    if( select_object )
    {
        selection.x = MIN(x,origin.x);
        selection.y = MIN(y,origin.y);
        selection.width = selection.x + CV_IABS(x - origin.x);
        selection.height = selection.y + CV_IABS(y - origin.y);

        selection.x = MAX( selection.x, 0 );
        selection.y = MAX( selection.y, 0 );
        selection.width = MIN( selection.width, image->width );
        selection.height = MIN( selection.height, image->height );
        selection.width -= selection.x;
        selection.height -= selection.y;
    }

    switch( event )
    {
    case CV_EVENT_LBUTTONDOWN:
        origin = cvPoint(x,y);
        selection = cvRect(x,y,0,0);
        select_object = 1;
        break;
    case CV_EVENT_LBUTTONUP:
        CvRect rectanguloSelec= cvRect(selection.x, selection.y, selection.width, selection.height);
        cvRectangleR(frame, rectanguloSelec, cvScalar(0, 255, 0, 0),1,8,0);
        listSelection.push_back(rectanguloSelec);

        select_object = 0;
        if( selection.width > 0 && selection.height > 0 )

            break;
    }
}


void DetectarVehiculo2::comenzar() {
    CvCapture* capture = 0;
    capture = cvCaptureFromCAM(1);
    ////////////////////////////////////////////////////
    char camino0[]="                                                                                                                             ";
    char camino1[]="              ";
    char camino2[]="/home/mrc/ProyectosQt/Guda/entrena cascade clasification 3/positivos/";
    char camino3[]=".jpg";
    ////////////////////////////////////////////////////

    CvCapture *fileVideo=cvCaptureFromFile("/home/mrc/ProyectosQt/Guda/entrena cascade clasification 3/ca3.avi");
    if (fileVideo==NULL)
        printf("fileVideo is null\n");
    else
        printf("fileVideo is not null\n");

    cvNamedWindow( "Imagen", CV_WINDOW_AUTOSIZE );
    cvSetMouseCallback( "Imagen", on_mouse, 0 );
    FILE *fptrb = fopen("/home/mrc/ProyectosQt/Guda/entrena cascade clasification 3/entrena.txt","w");
    char c;
    frame = 0;
    frame = cvQueryFrame( fileVideo );
    while(1)
    {

        if( !image )
        {
            /* allocate all the buffers */
            image = cvCreateImage( cvGetSize(frame), 8, 3 );

            frameOriginalGray = cvCreateImage( cvGetSize(frame), 8, 1 );
            frameOriginalGray->origin = frame->origin;
            //cvCopy(frame, frameOriginal,0);
            cvCvtColor(frame, frameOriginalGray, CV_BGR2GRAY );

            frameOriginal = cvCreateImage( cvGetSize(frame), 8, 3 );
            frameOriginal->origin = frame->origin;
            cvCopy(frame, frameOriginal,0);

            image->origin = frame->origin;
            grey = cvCreateImage( cvGetSize(image), 8, 1 );
            small_image= cvCreateImage( cvGetSize(image), 8, 1 );

        }

        cvCopy( frame, image, 0 );

        if( select_object && selection.width > 0 && selection.height > 0 )
        {
            cvRectangle(image,cvPoint(selection.x,selection.y),cvPoint(selection.x+selection.width,selection.y+selection.height),cvScalar(255,0,0));
            xx=(int)selection.x;
            yy=(int)selection.y;
            ww=(int)selection.width;
            hh=(int)selection.height;
            cvCvtColor( image, grey, CV_BGR2GRAY );
            sprintf(camino1,"%d",contador);
            strcpy(camino0,camino2);
            strcat(camino0,camino1);
            strcat(camino0,camino3);


        }
        cvShowImage( "Imagen", image );
        c = cvWaitKey(10);
        if( c == 27 ) break; //tecla escape para terminar
        if( c == 8 ) { //tecla borrar para limpiar el frame
            cvCopy(frameOriginal, frame,0);
        }
        if( c == 32 ){ //tecla barra espaciadora para cambiar al siguiente frame
            if (listSelection.empty()==false){
                contador++;
                fprintf(fptrb,"%s %d %d %d %d %d\n",camino0, 1,xx,yy,ww,hh);

                //cvSaveImage(camino0,grey);
                cvSaveImage(camino0,frameOriginalGray);
            }
            listSelection.clear();
            frame = cvQueryFrame( fileVideo );
            cvCopy(frame, frameOriginal);
            cvCvtColor(frame, frameOriginalGray, CV_BGR2GRAY );
        }

    }
    fclose(fptrb);
    cvReleaseCapture( &capture );
    cvDestroyWindow("Imagen");
    cvDestroyWindow("Imagen Gray");

}

DetectarVehiculo2::DetectarVehiculo2(QObject *parent) : QObject(parent)
{

}

