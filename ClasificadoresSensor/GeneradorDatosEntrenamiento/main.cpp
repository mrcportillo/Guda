// prueba6.cpp : Defines the entry point for the console application.
//
// practica6.c - creacion y visualizacion de una imagen usando OpenCV
//
// Robotica y Vision por Computador
#include "cv.h" // incluye las definiciones de OpenCV
#include "cvaux.h" // Stuff.
#include "highgui.h" // incluye highGUI. Necesario para crear ventanas de visualizacion
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
using namespace std;
int dato1 = 5;


#define PI 3.14159265

char c ;
int dato;
int x,y;
float xx1,yy1;
float dx=(230-161);
float dy=(302-161);
float ro=0.0;
float alfa=atan2(dx,dy)*180/PI;
float delta=0;
bool detecta1=false;
bool detecta2=false;

CvCapture* capture = NULL;
IplImage *cuadro;
IplImage *imagen;

int main(void) {

    CvScalar s;
    CvScalar color = CV_RGB(0,120,0);

    char texto[100];
    int vector[180];
    float temp=0.0;
    int conta=0;
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);

    cvNamedWindow( "Transito", CV_WINDOW_AUTOSIZE );
    capture = cvCaptureFromAVI( "/home/mrc/ProyectosQt/Guda/ClasificadoresSensor/GeneradorDatosEntrenamiento/ca1.avi" );
    dato= (int) cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);
    ofstream fs("/home/mrc/ProyectosQt/Guda/ClasificadoresSensor/GeneradorDatosEntrenamiento/samples.txt");
    while(1) {
        imagen =  cvQueryFrame(capture);
        cuadro =  cvCreateImage( cvGetSize(imagen), IPL_DEPTH_8U, 3 );
        cvSmooth( imagen, cuadro, CV_GAUSSIAN, 5, 5 );

        sprintf( texto, "%d %d", x,y);
        cvPutText( cuadro, texto, cvPoint(x,y),&font,cvScalar(0,255,0));
        cvCircle(cuadro,cvPoint(x,y),20,cvScalar(255,255,255),1,8);
        for(delta=0.0;delta<180;delta++) {
            vector[(int)delta]=0.0;
        }
        temp=100;
        for(delta=-100;delta<80;delta=delta+15) {
            for(ro=40;ro<120;ro=ro+7) {
                xx1=230+cos(delta*PI/180)*ro/sin(alfa*PI/180);
                yy1=302+sin(delta*PI/180)*ro/cos(alfa*PI/180);
                cvCircle(cuadro,cvPoint((int)xx1,(int)yy1),4,cvScalar(255,255,255),1,8);
                cvFloodFill( cuadro, cvPoint((int)xx1,(int)yy1), color, cvScalarAll(5.0), cvScalarAll(5.0), NULL, 4, NULL );
                s=cvGet2D(cuadro,(int)yy1-1,(int)xx1-1);
                if((int)s.val[1]!=120) break;
            }
            vector[(int)(temp+delta)]=(int)ro;
        }
        cvShowImage( "Transito", cuadro);

        if( !cuadro ) break;

        c= cvWaitKey(0);

        if(c == 27) {
            fs.close();
            break;
        } else if(c != ' '){
            for(conta=0;conta<180;conta=conta+15) {
                fs <<  vector[conta] <<" ";
            }
            fs << c <<endl;
        }
    }
    cvReleaseCapture( &capture );
    cvDestroyWindow( "Ejemplo0" );
    return 0;
}
