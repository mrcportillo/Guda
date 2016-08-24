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

void DetectarVehiculo2::comenzar() {
    string path;

    IplImage* imgAnterior=NULL;

    CvHistogram* hist_anterior = NULL;
    int contador = 1, hist_size = 256;
    float range[]= {0,256};
    float* ranges[] = {range};
    float max_value, max = 0.0, w_scale = 0.0;

    float val_corr_Histogramas;


    CvCapture *capture = cvCreateCameraCapture(0);

    //    controla inicializacion de camara
    if(!capture) {
        cout << "error al cargar camara" << endl;
    }

    while (true) {
        contador++;

        std::stringstream convert;
        convert << contador;
        //        path="/home/juanmartin/Dropbox/GUDA 2016/Proyectos QT/T4/destino/"+convert.str()+".jpg";
//        path="/home/juanmartin/Dropbox/GUDA 2016/Proyectos QT/Temas basicos/Filtro de particulas con correlacion de histograma/Imagenes/"+convert.str()+".jpg";
        path="/home/mrc/ProyectosQt/Guda/filtroParticulas/Imagenes"+convert.str()+".jpg";
        const char * pathChar = path.c_str();

        //      Las 3 lineas son para usar la camara en gris.  Hay que eliminar el cvReleaseImage(&imgConFiltro)
        IplImage* imgCamara = cvQueryFrame( capture );
        IplImage* imgConFiltro = cvCreateImage(cvGetSize(imgCamara), imgCamara->depth,1);
        cvCvtColor(imgCamara, imgConFiltro, CV_BGR2GRAY);

        //        Para usar la secuencia de imagenes. Hay que hacer el cvReleaseImage(&imgConFiltro) para no aumentar la memoria RAM
        //IplImage* imgConFiltro  = cvLoadImage(pathChar, CV_LOAD_IMAGE_GRAYSCALE);

        IplImage* imgOriginal = cvCreateImage(cvGetSize(imgConFiltro), imgConFiltro->depth, imgConFiltro->nChannels);
        cvCopy(imgConFiltro, imgOriginal, NULL);

        IplImage* channel = cvCreateImage(cvGetSize(imgConFiltro), 8, 1 );
        IplImage* imgHistograma = cvCreateImage(cvSize(300,240), 8, 3);

        cvSet(imgHistograma, cvScalarAll(100), 0);

        CvHistogram *histograma = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

        cvSetImageCOI(imgConFiltro,1);
        cvCopy(imgConFiltro,channel);
        cvResetImageROI(imgConFiltro);

        cvCalcHist(&channel, histograma, 0, NULL);

        /* Find the minimum and maximum values of the histograms */
        cvGetMinMaxHistValue(histograma, 0, &max, 0, 0 );
        max_value = 0.0;
        // The variable max_value has the maximum of the three histograms
        max_value = (max > max_value) ? max : max_value;
        /* Using the maximum value, Scale/Squeeze the histogram (to fit the image) */
        cvScale( histograma->bins, histograma->bins, ((float)imgHistograma->height)/max_value, 0 );

        if (hist_anterior!=NULL){
            val_corr_Histogramas = (float)cvCompareHist(histograma, hist_anterior, CV_COMP_CORREL);
            if (val_corr_Histogramas > 0.95){
                //                printf("###########################################\n");
                // Ejecutar el filtro de particulas
                filtroDeParticulas(imgAnterior, imgConFiltro, hist_anterior, histograma);
            } else{
                //printf("Hist  son distintos. Cambio la iluminacion\n");
                cout << "Hist  son distintos. Cambio la iluminacion\n\n\n\n\n\n\n\n\n\n"<<endl;
                //esperar a que se estabilice la imagen con la nueva luminosidad
            }
        }

        /* Scale/Squeeze the histogram range to image width */
        w_scale = ((float)imgHistograma->width)/hist_size;

        /* Plot the Histograms */
        for( int i = 0; i < hist_size; i++ ) {
            cvRectangle(imgHistograma, cvPoint((int)i*w_scale , imgHistograma->height),
                        cvPoint((int)(i+1)*w_scale, imgHistograma->height - cvRound(cvGetReal1D(histograma->bins,i))),
                        CV_RGB(0,0,255), -1, 8, 0);
        }

        if (imgAnterior==NULL){ //Para evitar que una vez se cree la image. O sino me mata la memoria
            imgAnterior = cvCreateImage(cvGetSize(imgOriginal), imgOriginal->depth, imgOriginal->nChannels);
        }

        //guardo el frame actual sin los puntos marcados, para luego compararlo en el futuro son su pasado.
        cvCopy(imgOriginal, imgAnterior, NULL);
        cvNamedWindow( "Image original", 1 );
        cvShowImage( "Image original", imgAnterior);

        //guardar histograma actual para proxima comparacion
        cvCopyHist(histograma, &hist_anterior);
        cvNamedWindow( "Image con filtro", 1 );
        cvShowImage( "Image con filtro", imgConFiltro);

        /* create a window to show the histogram of the image */
        cvNamedWindow("Histogram", 1);
        cvShowImage( "Histogram", imgHistograma);

        cvWaitKey(5);

        //cvDestroyWindow( "Image" );
        //cvDestroyWindow( "Histogram" );
        cvReleaseImage( &imgConFiltro );
        cvReleaseImage(&imgOriginal);
        cvReleaseImage(&channel);
        cvReleaseImage(&imgHistograma);
        cvReleaseHist(&histograma);
    }
}

DetectarVehiculo2::DetectarVehiculo2(QObject *parent) : QObject(parent)
{

}
