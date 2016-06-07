// prueba6.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

// practica6.c - creacion y visualizacion de una imagen usando OpenCV
//
// Robotica y Vision por Computador
#include "cv.h" // incluye las definiciones de OpenCV
#include "cvaux.h" // Stuff.
#include "highgui.h" // incluye highGUI. Necesario para crear ventanas de visualizacion
#include <stdio.h>



/////////////////////////////////////////////////////////////
int main()
 {
	                
               
	               
	            
	            char c; 
				char camino0[]="              ";
				char camino1[]="              ";
				char camino2[]="fuente\\diplo00000";
				char camino3[]="  ";
				char camino4[]="-L";
                char camino5[]=".png";
				char camino6[]="fuente\\diplo0000";
                char camino7[]="fuente\\diplo000";
                ///////////////////////////////////
				char camino8[]="              ";
				char camino9[]="destino\\";
				char camino10[]=".jpg";
				///////////////////////////////////
			    cvNamedWindow( "Example2", CV_WINDOW_AUTOSIZE );
				int contador=0; 
				IplImage* frame;
                int a=1; 

				while(a) {
                contador++;
				if ((contador>=0)&&(contador<=9))
				{   
                    sprintf(camino3,"%d",contador);
                    strcpy(camino1,camino0);
                    strcpy(camino1,camino2);
					strcat(camino1,camino3);
                    strcat(camino1,camino4);
                    strcat(camino1,camino5);
					/////////////////////////////
                    strcpy(camino8,camino0);
                    strcpy(camino8,camino9);
					strcat(camino8,camino3);
                    strcat(camino8,camino10);
					/////////////////////////////
				}
				else if(((contador>=10)&&(contador<=99)))
				{
				    sprintf(camino3,"%d",contador);
                    strcpy(camino1,camino0);
                    strcpy(camino1,camino6);
					strcat(camino1,camino3);
                    strcat(camino1,camino4);
                    strcat(camino1,camino5);
					/////////////////////////////
                    strcpy(camino8,camino0);
                    strcpy(camino8,camino9);
					strcat(camino8,camino3);
                    strcat(camino8,camino10);
					/////////////////////////////
				}
				else if(((contador>=100)&&(contador<=864)))
				{
				    sprintf(camino3,"%d",contador);
                    strcpy(camino1,camino0);
                    strcpy(camino1,camino7);
					strcat(camino1,camino3);
                    strcat(camino1,camino4);
                    strcat(camino1,camino5);
					/////////////////////////////
                    strcpy(camino8,camino0);
                    strcpy(camino8,camino9);
					strcat(camino8,camino3);
                    strcat(camino8,camino10);
					/////////////////////////////
				}
                

				if(contador==864){a=-1;}
				
				frame = cvLoadImage(camino1);

				cvShowImage( "Example2", frame );
                cvSaveImage(camino8,frame);
				c= cvWaitKey(100);
				
				}
			
				cvDestroyWindow( "Example2" );
                

            
 }


