//#include "cv.h" // incluye las definiciones de OpenCV
#include <opencv/cv.h>
//#include "cvaux.h" // Stuff.
#include <opencv/cvaux.h>
//#include "highgui.h" // incluye highGUI. Necesario para crear ventanas de visualizacion
#include <opencv/highgui.h>
#include "iostream"
#include "detectarvehiculo2.h"
#include "mainwindow.h"

using namespace std;

float zoom=1.00;

void DetectarVehiculo2::actualizarZoom(double zoomUI){
    zoom=zoomUI;
    cout<<"zoom: "<<zoom <<endl;
}

int diferenciaPixelPorColor (CvScalar scalActual, CvScalar scalAnterior) {
    return abs((int)scalActual.val[0] - (int)scalAnterior.val[0]);
}

float diferenciaPixelPorProbabilidad (CvScalar scalActual, CvScalar scalAnterior, CvHistogram* histActual, CvHistogram* histAnterior,
                                      int areaImagen) {
    float valorHistogramaActual= (float)cvQueryHistValue_1D(histActual, (int)scalActual.val[0]);
    float valorHistogramaAnterior= (float)cvQueryHistValue_1D(histAnterior, (int)scalAnterior.val[0]);
    return ((valorHistogramaActual/areaImagen)/(valorHistogramaAnterior/areaImagen));
}

float diferenciaPixelPorProbabilidadDeEntorno (IplImage* imgActual, IplImage* imgAnterior, CvHistogram* histActual, CvHistogram* histAnterior,
                                               int areaImagen, int x, int y) {
    float probImgActualAcumulada=0, probImgAnteriorAcumulada=0;
    int ptoX, ptoY, delta=10;
    for (int dx = -delta; dx <= delta; dx = dx+delta) {
        ptoX = x + dx;
        if (ptoX >= 0) {
            for (int dy = -delta; dy <= delta; dy = dy+delta) {
                ptoY = y + dy;
                if (ptoY >= 0) {
                    probImgActualAcumulada = probImgActualAcumulada +
                            (float)cvQueryHistValue_1D(histActual, (int)(cvGet2D(imgActual, ptoY, ptoX)).val[0]) / areaImagen;
                    probImgAnteriorAcumulada = probImgAnteriorAcumulada +
                            (float)cvQueryHistValue_1D(histAnterior, (int)(cvGet2D(imgAnterior, ptoY, ptoX)).val[0]) / areaImagen;
                }
            }
        }
    }
    return probImgActualAcumulada/probImgAnteriorAcumulada;
}

void filtroDeParticulas (IplImage* imgAnterior, IplImage* imgActual, CvHistogram* histAnterior,  CvHistogram* histActual){
    int x,y, detecta=0, puntoMarcado=0, anchoimg= imgActual->width, altoimg= imgActual->height;
    while (true){
        x= rand() % (anchoimg-10);
        y= rand() % (altoimg-10);
        detecta++;
        if (x>=0 && y>=0){


            //          Marca punto segun diferencia de color de pixel
            /*if (diferenciaPixelPorColor(cvGet2D(imgActual, y, x), cvGet2D(imgAnterior, y, x)) > 50) {
                cvCircle(imgActual, cvPoint(x,y),3, CV_RGB(0,0,0), CV_FILLED, CV_AA,0);
                puntoMarcado++;
             }*/

            //Marca punto segun diferencia de color y la probablidades
            if (diferenciaPixelPorColor(cvGet2D(imgActual, y, x), cvGet2D(imgAnterior, y, x)) > 10) {
                float razonProbabilidades= diferenciaPixelPorProbabilidadDeEntorno(imgActual, imgAnterior, histActual, histAnterior, anchoimg*altoimg, x, y );
                if (razonProbabilidades<0.1 || razonProbabilidades >1.9) {
                    cvCircle(imgActual, cvPoint(x,y),3, CV_RGB(0,0,0), CV_FILLED, CV_AA,0);
                    puntoMarcado++;
                }
            }


            //          Marca punto segun razon de probabilidades en histograma
            //            if (diferenciaPixelPorProbabilidad(cvGet2D(imgActual, y, x), cvGet2D(imgAnterior, y, x),
            //                                               histActual, histAnterior, anchoimg*altoimg) < 0.3 ) {
            //                cvCircle(imgActual, cvPoint(x,y),3, CV_RGB(0,0,0), CV_FILLED, CV_AA,0);
            //                puntoMarcado++;
            //            }

            //          Marca punto segun razon de probabilidades en el entorno del punto
            /*float razonProbabilidades= diferenciaPixelPorProbabilidadDeEntorno(imgActual, imgAnterior, histActual, histAnterior, anchoimg*altoimg, x, y );
            if (razonProbabilidades<0.1 || razonProbabilidades >1.9) {
                cvCircle(imgActual, cvPoint(x,y),3, CV_RGB(0,0,0), CV_FILLED, CV_AA,0);
                puntoMarcado++;
            }*/

        }
        if (puntoMarcado==300) {break;}
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
                filtroDeParticulas(imgAnt, img, hist_anterior, hist_blue);
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
