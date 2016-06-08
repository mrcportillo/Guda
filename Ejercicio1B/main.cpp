#include "stdafx.h"
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>

int main()
{
    char name0[] = "/home/mrc/ProyectosQt/Guda/Ejercicio1/Ejercicio1/img/1.jpg";
    char name1[] = "/home/mrc/ProyectosQt/Guda/Ejercicio1/Ejercicio1/img/resized.jpg";
    char name2[] = "/home/mrc/ProyectosQt/Guda/Ejercicio1/Ejercicio1/img/corte.jpg";";
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
}
