//#include <QCoreApplication>
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include "stdio.h"

int main()
{
    char name0[] = "/home/mrc/ProyectosQt/Guda/Ejercicio1/Ejercicio1/img/1.jpg";
    char name1[] = "/home/mrc/ProyectosQt/Guda/Ejercicio1/Ejercicio1/img/resized.jpg";
    char name2[] = "/home/mrc/ProyectosQt/Guda/Ejercicio1/Ejercicio1/img/corte.jpg";
    float dx = 0;
    float dy = 0;
    int detax = 0;
    int deltay = 0;
    float zoom = 2.01;
    CvRect faceCoords;
    IplImage* imagen = NULL;
    IplImage* faceImage = NULL;
    imagen = cvLoadImage(name0, 1);
    cvNamedWindow("Fuente", CV_WINDOW_AUTOSIZE);
    cvShowImage("Fuente", imagen);

    int py = imagen->height*zoom;
    int px = imagen->width*zoom;
    IplImage *resized = cvCreateImage(cvSize(px, py), IPL_DEPTH_8U, 3);
    cvResize(imagen, resized, CV_INTER_CUBIC);
    cvNamedWindow("zoom", CV_WINDOW_AUTOSIZE);
    cvShowImage("Zoom", resized);
    cvSaveImage(name1, resized);

    dx = (px - 320) / 2;
    dy = (py - 240) / 2;
    faceCoords = cvRect(2*dx, 2*dy, px-2*dx, py-2*dy);
    cvSetImageROI(resized, faceCoords);
    cvNamedWindow("Corte", CV_WINDOW_AUTOSIZE);
    cvShowImage("Corte", resized);
    IplImage* temp = cvCloneImage(resized);
    cvSaveImage(name2, temp);
    cvResetImageROI(resized);

    cvWaitKey(0);
    cvDestroyAllWindows();
    cvReleaseImage(&imagen);
    cvReleaseImage(&resized);
    return 0;
}

