#include "procimage.h"


//#include "stdafx.h"
#include "cv.h" // incluye las definiciones de OpenCV
#include "cvaux.h" // Stuff.
#include "highgui.h" // incluye highGUI. Necesario para crear ventanas de visualizacion
#include <stdio.h>
#include <string>

using namespace std;

IplImage* frame;

string salText="";

ProcImage::ProcImage(){

}

void ProcImage::putText(){
    salText="Mantener Distancia!";
}

void ProcImage::deleteText(){
    salText="";
}

void ProcImage::procesar(){


    cvNamedWindow( "Recorrido", CV_WINDOW_AUTOSIZE );
    int contador=1;

    std::string path;
    int contSinRecepcion=0;
    while (contador<=864){

        std::stringstream convert;
        convert<<contador;
        path="/home/juanmartin/Dropbox/GUDA 2016/Proyectos QT/T4/destino/"+convert.str()+".jpg";
        //std::cout<<path+"\n";
        const char * pathChar = path.c_str();
        frame = cvLoadImage(pathChar);

        CvFont font;
        double hScale=1.0;
        double vScale=0.5;
        int lineWidth=2;
        cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX, hScale,vScale,0,lineWidth);
        cvPutText (frame,salText.c_str(),cvPoint(50,50), &font, cvScalar(0,0,255));
        if (salText=="" & contSinRecepcion!=0){
            contSinRecepcion=0;
        }
        if (contSinRecepcion>20){
            salText="";
        }
        contSinRecepcion++;
        cvShowImage( "Recorrido", frame );
        contador++;
        cvWaitKey(100);
    }
    cvDestroyWindow( "Recorrido" );
}

