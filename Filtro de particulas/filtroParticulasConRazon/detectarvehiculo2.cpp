#include "detectarvehiculo2.h"
#include "mainwindow.h"

//#include "cv.h" // incluye las definiciones de OpenCV
#include <opencv/cv.h>
//#include "cvaux.h" // Stuff.
#include <opencv/cvaux.h>
//#include "highgui.h" // incluye highGUI. Necesario para crear ventanas de visualizacion
#include <opencv/highgui.h>
#include "iostream"

using namespace std;

float zoom=1.00;
float umbralVariacionEntorno = 5;
float umbralVariacion = 15;
float umbralVariacionHistograma = 0.1;

void DetectarVehiculo2::actualizarZoom(double zoomUI){
    cout<<zoomUI<<endl;
    zoom=zoomUI;
    cout<<zoom <<endl;
}

/*A partir de la comparacion del valor de gris en todo el entorno de pixeles seleccionados al
azar se determina el movimiento y se marca el pixel central*/
void analisisDeParticulasEntorno (IplImage* imgAnt, IplImage* imgActual){

    int x,y,i,j;
    int detecta=0;
    int puntoMarcado=0;
    CvScalar sActual;
    CvScalar sAnt;

    int anchoImg= imgActual->width;
    int altoImg= imgActual->height;
    int delta = 1;
    while (true){
        x= (rand() % (anchoImg- (delta*2)) )+delta;
        y= (rand() % (altoImg- (delta*2))) +delta;

        detecta++;
        if (x>=0 && y>=0){
            int entorno=0;
            for (i=-delta; i<=delta; i=i+ delta){
                for (j=-delta; j<=delta; j=j+delta){
                    sActual= cvGet2D(imgActual, y+j, x+i);
                    sAnt= cvGet2D(imgAnt, y+j, x+i);

                    int escalaGrisPixelActual= (int)sActual.val[0];
                    int escalaGrisPixelAnt= (int)sAnt.val[0];

                    if (abs(escalaGrisPixelActual - escalaGrisPixelAnt) > umbralVariacionEntorno ){
                        entorno++;
                    }
                }
            }
            if (entorno>8){
                cvCircle(imgActual, cvPoint(x,y),3, CV_RGB(0,0,0), CV_FILLED, CV_AA,0);
                puntoMarcado++;
            }
        }
        if (puntoMarcado==200) {break;}
        if (detecta==10000) {break;}
    }
}

/*Se compara el valor de probabilidad del pixel y su entorno en el histograma actual y el de la imagen anterior*/
void analisiParticulasEntornoConHistograma (IplImage* imgAnt, IplImage* imgActual, CvHistogram* hist_ant,  CvHistogram* hist_blue){

    int x,y,i,j;
    int detecta=0;
    int puntoMarcado=0;
    CvScalar sActual;
    CvScalar sAnt;

    int anchoImg= imgActual->width;
    int altoImg= imgActual->height;
    int delta = 1;
    while (true){
        x= (rand() % (anchoImg- (delta*2)) )+delta;
        y= (rand() % (altoImg- (delta*2))) +delta;

        detecta++;
        if (x>=0 && y>=0){
            int entorno=0;
            for (i=-delta; i<=delta; i=i+ delta){
                for (j=-delta; j<=delta; j=j+delta){
                    sActual= cvGet2D(imgActual, y+j, x+i);
                    sAnt= cvGet2D(imgAnt, y+j, x+i);

                    int escalaGrisPixelActual= (int)sActual.val[0];
                    int escalaGrisPixelAnt= (int)sAnt.val[0];

                    float valorHistogramaActual= (float)cvQueryHistValue_1D(hist_blue, escalaGrisPixelActual);
                    float valorHistogramaAnt= (float)cvQueryHistValue_1D(hist_ant, escalaGrisPixelAnt);

                    float k=200;
                    float probPixelActual= valorHistogramaActual/(altoImg*anchoImg)*k;
                    float probPixelAnt= valorHistogramaAnt/(altoImg*anchoImg)*k;

                    if (abs(probPixelActual / probPixelAnt) < umbralVariacionHistograma ){
                        entorno++;
                    }
                }
            }
            if (entorno>8){
                cvCircle(imgActual, cvPoint(x,y),3, CV_RGB(0,0,0), CV_FILLED, CV_AA,0);
                puntoMarcado++;
            }
        }
        if (puntoMarcado==200) {break;}
        if (detecta==10000) {break;}
    }
}

/*A partir de la comparacion del valor de gris en pixeles seleccionados al
azar se determina el movimiento y se marca el pixel central*/
void analisisDeParticulas (IplImage* imgAnt, IplImage* imgActual){

    int x,y;
    int detecta=0;
    int puntoMarcado=0;
    CvScalar sActual;
    CvScalar sAnt;

    int anchoImg= imgActual->width;
    int altoImg= imgActual->height;
    while (true){
        x= rand() % (anchoImg-0);
        y= rand() % (altoImg-0);
        detecta++;
        if (x>=0 && y>=0){
            sActual= cvGet2D(imgActual, y, x);
            sAnt= cvGet2D(imgAnt, y, x);
            int escalaGrisPixelActual= (int)sActual.val[0];
            int escalaGrisPixelAnt= (int)sAnt.val[0];

            if (abs(escalaGrisPixelActual - escalaGrisPixelAnt) > umbralVariacion){
                cvCircle(imgActual, cvPoint(x,y),3, CV_RGB(0,0,0), CV_FILLED, CV_AA,0);
                puntoMarcado++;
            }
        }
        if (puntoMarcado==200) {break;}
        if (detecta==10000) {break;}
    }
}

/*Se compara el valor de probabilidad del pixel en el histograma actual y el de la imagen anterior*/
void analisisDeParticulasConHistograma (IplImage* imgAnt, IplImage* imgActual, CvHistogram* hist_ant,  CvHistogram* hist_blue){

    int x,y;
    int detecta=0;
    int puntoMarcado=0;
    CvScalar sActual;
    CvScalar sAnt;

    int anchoImg= imgActual->width;
    int altoImg= imgActual->height;
    while (true){
        x= rand() % (anchoImg-0);
        y= rand() % (altoImg-0);
        detecta++;
        if (x>=0 && y>=0){
            sActual= cvGet2D(imgActual, y, x);
            sAnt= cvGet2D(imgAnt, y, x);

            int escalaGrisPixelActual= (int)sActual.val[0];
            int escalaGrisPixelAnt= (int)sAnt.val[0];

            float valorHistogramaActual= (float)cvQueryHistValue_1D(hist_blue, escalaGrisPixelActual);
            float valorHistogramaAnt= (float)cvQueryHistValue_1D(hist_ant, escalaGrisPixelAnt);

            float k=200;
            float probPixelActual= valorHistogramaActual/(altoImg*anchoImg)*k;
            float probPixelAnt= valorHistogramaAnt/(altoImg*anchoImg)*k;

            if (abs(probPixelActual / probPixelAnt) < umbralVariacionHistograma ){
                cvCircle(imgActual, cvPoint(x,y),3, CV_RGB(0,0,0), CV_FILLED, CV_AA,0);
                puntoMarcado++;
            }
        }
        if (puntoMarcado==200) {break;}
        if (detecta==10000) {break;}
    }
}

/*Se inicializa el flujo de video, las imagenes a trabajar, se generan los histogramas y en funcion de la correlacion
 * entre estos se compara*/
void DetectarVehiculo2::comenzar(){
    int contador=1;

    CvHistogram* hist_anterior;
    hist_anterior=NULL;

    CvCapture *capture = NULL;
    capture = cvCreateCameraCapture(0);
    if(!capture){
        cout<<"error de inicializacion"<<endl;
    }

    IplImage* imgAnt=NULL;
    while (true){
        contador++;
        std::stringstream convert;
        convert<<contador;

        IplImage* img2 = cvQueryFrame( capture );
        IplImage* img= cvCreateImage(cvGetSize(img2), img2->depth,1);
        cvCvtColor(img2, img, CV_BGR2GRAY);


        IplImage* imgOriginal;
        imgOriginal= cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
        cvCopy(img, imgOriginal, NULL);

        IplImage* channel = cvCreateImage( cvGetSize(img), 8, 1 );
        IplImage *hist_img = cvCreateImage(cvSize(300,240), 8, 3);

        cvSet( hist_img, cvScalarAll(100), 0 );

        CvHistogram *hist_blue;

        int hist_size = 256;
        float range[]={0,256};
        float* ranges[] = { range };

        float max_value = 0.0;
        float max = 0.0;
        float w_scale = 0.0;

        hist_blue = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

        cvSetImageCOI(img,1);
        cvCopy(img,channel);
        cvResetImageROI(img);

        cvCalcHist( &channel, hist_blue, 0, NULL );

        max_value = (max > max_value) ? max : max_value;

        cvGetMinMaxHistValue( hist_blue, 0, &max, 0, 0 );

        max_value = (max > max_value) ? max : max_value;

        cvScale( hist_blue->bins, hist_blue->bins, ((float)hist_img->height)/max_value, 0 );
        if (hist_anterior!=NULL){
            float val_corr_Histogramas= (float)cvCompareHist(hist_blue,hist_anterior, CV_COMP_CORREL);
            if (val_corr_Histogramas>0.95){
                printf("###########################################\n");
                // Ejecutar el filtro
                analisisDeParticulasEntorno(imgAnt, img);
            } else{
                printf("\n\n\n\n\n\nHist  son distintos. Cambio la iluminacion");
                //esperar a que se estabilice la imagen con la nueva luminosidad
            }
        }

        w_scale = ((float)hist_img->width)/hist_size;

        /* Plot the Histograms */
        for( int i = 0; i < hist_size; i++ )
        {
            cvRectangle( hist_img, cvPoint((int)i*w_scale , hist_img->height),
                         cvPoint((int)(i+1)*w_scale, hist_img->height - cvRound(cvGetReal1D(hist_blue->bins,i))),
                         CV_RGB(0,0,255), -1, 8, 0 );
        }

        if (imgAnt==NULL){
            imgAnt= cvCreateImage(cvGetSize(imgOriginal), imgOriginal->depth, imgOriginal->nChannels);
        }

        cvCopy(imgOriginal, imgAnt, NULL);
        cvNamedWindow( "Image original", 1 );
        cvShowImage( "Image original",imgAnt);

        cvCopyHist(hist_blue, &hist_anterior);
        cvNamedWindow( "Image", 1 );
        cvShowImage( "Image",img);

        cvNamedWindow("Histogram", 1);
        cvShowImage( "Histogram", hist_img);

        cvWaitKey(1);

        cvReleaseImage(&imgOriginal);
        cvReleaseImage(&img);
        cvReleaseImage(&channel);
        cvReleaseImage(&hist_img);

        cvReleaseHist(&hist_blue);

    }
}

DetectarVehiculo2::DetectarVehiculo2(QObject *parent) : QObject(parent)
{

}

