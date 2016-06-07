// trabajo1.cpp: archivo de proyecto principal.

#include "stdafx.h"

#include "cv.h" // incluye las definiciones de OpenCV
#include "cvaux.h" // Stuff.
#include "highgui.h" // incluye highGUI. Necesario para crear ventanas de visualizacion
#include <stdio.h>

using namespace System;

			

void main()
{
    // el fichero example.bmp debe estar en el directorio images
             char name0[] = "imagen\\auto1.jpg";
			 char name1[] = "imagen\\resized.jpg";
			 char name2[] = "imagen\\corte.jpg";
			 float dx=0;
			 float dy=0;
			 int deltax=0;
			 int deltay=0;
			 float zoom=2.01;
			CvRect faceCoords;
            IplImage* imagen=NULL;
			IplImage* faceImage=NULL; // inicializo imagen
			imagen=cvLoadImage(name0,1); // cargamos la imagen
            cvNamedWindow( "Fuente", 1); // representamos la imagen escalada
			cvShowImage( "Fuente", imagen); 			
            ////////////////////////////////////////////////
			int py = imagen->height*zoom;
			int px = imagen->width*zoom; 
			IplImage *resized = cvCreateImage( cvSize(px,py),IPL_DEPTH_8U, 3);
			cvResize(imagen,resized,CV_INTER_CUBIC); // escalado de la imagen
			cvNamedWindow( "Zoom", 1); // representamos la imagen escalada
			cvShowImage( "Zoom", resized);
			cvSaveImage(name1,resized);
			//////////////////////////////////
			dx=(px-320)/2;
			dy=(py-240)/2;
			faceCoords=cvRect(2*dx,2*dy,px-2*dx,py-2*dy);
			cvSetImageROI(resized,faceCoords);
			cvNamedWindow("Corte",1);
			cvShowImage("Corte",resized);
            IplImage* temp=cvCloneImage(resized);
			cvSaveImage(name2,temp);
			cvResetImageROI(resized);
			/////////////////////////////////
			cvWaitKey(0); // pulsamos cualquier tecla para terminar el programa
			cvDestroyAllWindows(); // destruimos todas las ventanas
			cvReleaseImage(&imagen);
			cvReleaseImage(&resized);
			         
}
 