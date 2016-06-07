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
			 float dx=2;
			 float dy=2;
			 int deltax=50;
			 int deltay=20;
			 int i=0,j=0;
			 int R=0,G=0,B=0;
			 /////////////////////////////////////////////// 
			CvScalar s;
			CvRect faceCoords;
            IplImage* imagen=NULL;
			IplImage* faceImage=NULL; // inicializo imagen
			imagen=cvLoadImage(name0,1); // cargamos la imagen
			cvNamedWindow( "Fuente", 1); // representamos la imagen escalada
			cvShowImage( "Fuente", imagen);
            ////////////////////////////////////////////////

			int py = imagen->height*dy; // píxeles en el eje x de la imagen escalada,
			// es decir, estamos definiendo la escala X
			int px = imagen->width*dx; // píxeles en el eje x de la imagen escalada,
			// es decir, estamos definiendo la escala Y
			IplImage *resized = cvCreateImage( cvSize(px,py),IPL_DEPTH_8U, 3);
			IplImage *imag = cvCreateImage( cvSize(imagen->height,imagen->width),IPL_DEPTH_8U, 3);
			// creamos la estructura
			// donde ubicaremos la imagen escalada,
			// siendo px y py los píxeles de la imagen
			// destino, es decir, el propio factor de escala
			cvResize(imagen,resized,CV_INTER_CUBIC); // escalado de la imagen
			cvSaveImage(name1,resized); 
			cvNamedWindow( "Resized", 1); // representamos la imagen escalada
			cvShowImage( "Resized", resized);
			//////////////////////////////////
			faceCoords=cvRect(10,10,250,300);
			cvSetImageROI(resized,faceCoords);
            /////////////////////////////////
			for(i=10;i<=100;i++){
				for(j=10;j<=150;j++)
				{
			s=cvGet2D(resized,i,j); // obtenemos el valor en el píxel (i,j)
            //Para hacer operaciones en la imagen
			R=s.val[0];
			G=s.val[1];
			B=s.val[2];
            s.val[0]=100;
			s.val[1]=120;
			s.val[2]=200;

			///estaria la funcion()
            cvSet2D(imag,i,j,s); //Funcion destino
            cvSet2D(resized,i,j,s); //Funcion destino
            ////////////////////////////////
				}}
			cvNamedWindow("Destino",1);
			cvShowImage("Destino",imag);
			////////////////////////////////
            cvNamedWindow("Corte",1);
			cvShowImage("Corte",resized);
			cvSaveImage(name2,resized); 
			cvResetImageROI(resized);
			//////////////////////////////////
			
            
			cvWaitKey(0); // pulsamos cualquier tecla para terminar el programa
			cvDestroyAllWindows(); // destruimos todas las ventanas
			cvReleaseImage(&imagen);
			cvReleaseImage(&resized);
			}
