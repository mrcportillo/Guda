//#include "stdafx.h"
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>

int main()
{
    char name0[] = "/home/mrc/ProyectosQt/Guda/Ejercicio1/Ejercicio1/img/1.jpg";
    char name1[] = "/home/mrc/ProyectosQt/Guda/Ejercicio1/Ejercicio1/img/resizedB.jpg";
    char name2[] = "/home/mrc/ProyectosQt/Guda/Ejercicio1/Ejercicio1/img/corteB.jpg";
    float dx = 2;
    float dy = 2;
    int deltax = 50;
    int deltay = 50;
    int i = 0, j = 0, R = 0, G = 0, B = 0;

    CvScalar s;
    CvRect faceCoords;
    IplImage* imagen = NULL;
    IplImage* faceImagen = NULL;
    imagen = cvLoadImage(name0, 1);
    cvNamedWindow("Fuente", 1);
    cvShowImage("Fuente", imagen);

    int py = imagen->height*dy;
    int px = imagen->width*dx;
    IplImage *resized = cvCreateImage(cvSize(px, py), IPL_DEPTH_8U, 3);
    IplImage *imag = cvCreateImage(cvSize(imagen->height, imagen->width), IPL_DEPTH_8U, 3);

    cvResize(imagen, resized, CV_INTER_CUBIC);
    cvSaveImage(name1, resized);
    cvNamedWindow("Resized", 1);
    cvShowImage("Resized", resized);

    faceCoords = cvRect(10, 10, 250, 300);
    cvSetImageROI(resized, faceCoords);

    for (i = 10; i <= 100; i++) {
      for (j=10; j <= 150; j++) {
        s = cvGet2D(resized, i, j);
        R=s.val[0];
        G=s.val[1];
        B=s.val[2];
        s.val[0]=100;
        s.val[1]=120;
        s.val[2]=200;
      }
    }
    return 0;
}
