#include <QCoreApplication>
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include "stdio.h"
#include "iostream"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

/*
La clase CutAndResize mediante el metodo getZoom devuelve un corte en profundidad de
la imagen recibida, en funcion a filas, columnas y numero de pieza que son sus parametros.
Hace uso de dos metodos para obtener las coordenandas a partir de los cuales hacer los cortes(getXCoordForCut y
getYCoordForCut).
El metodo getResizedImage adapta la imagen recibida como parametro y los valores de ancho y alto parametrizados.
*/

class CutAndResize {

    public: float getXCoordForCut(int piece, float dx, int col) {
        int i;
        for ( i = piece; i > col; i=i-col) {

        }
        return dx*(i-1);
    }

    public: float getYCoordForCut(int piece, float dy, int fil) {
        for ( int i = 1; i <= fil; i++) {
            if (piece <= i*fil) {
                return dy*(i-1);
            }
        }
        return 0;
    }

    public: IplImage* getZoom(IplImage* imagen, int zoomCol, int zoomFil, int piece) {
        if (piece > (zoomCol*zoomFil)) {
            return 0;
        }

        CvRect faceCoords;

        float dx = imagen->width / zoomCol;
        float dy = imagen->height / zoomFil;

        faceCoords = cvRect(getXCoordForCut(piece, dx, zoomCol), getYCoordForCut(piece, dy, zoomFil), dx , dy);
        cvSetImageROI(imagen, faceCoords);
        IplImage* corte = cvCloneImage(imagen);
        cvResetImageROI(imagen);

        return corte;
    }

    public: IplImage* getResizedImage(IplImage* imagen, int width, int height) {
        IplImage* zoom = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
        cvResize(imagen, zoom, CV_INTER_CUBIC);

        return zoom;
    }

};

/*
La clase imageComparation mediante el metodo cutPieceImage compara las partes
analogas de ambas imagenes, haciendo uso del metodo getComparateImage, finalmente y
dependiendo del resultado de la comparacion usa el metodo pasteOver para generar
una nueva imagen con parches en las zonas consideradas distintas.
*/

class ImageComparation {
    public: double getComparateImage(IplImage* imagen1, IplImage* imagen2) {
        Mat hsvImagen1, hsvImagen2;
        cvtColor(cv::cvarrToMat(imagen1), hsvImagen1, COLOR_BGR2HSV );
        cvtColor(cv::cvarrToMat(imagen2), hsvImagen2, COLOR_BGR2HSV );

        int h_bins = 50; int s_bins = 60;
        int histSize[] = { h_bins, s_bins };
        float h_ranges[] = { 0, 180 };
        float s_ranges[] = { 0, 256 };
        const float* ranges[] = { h_ranges, s_ranges };
        int channels[] = { 0, 1 };
        MatND histImagen1, histImagen2;

        calcHist( &hsvImagen1, 1, channels, Mat(), histImagen1, 2, histSize, ranges, true, false );
        normalize( histImagen1, histImagen1, 0, 1, NORM_MINMAX, -1, Mat() );
        calcHist( &hsvImagen1, 1, channels, Mat(), histImagen2, 2, histSize, ranges, true, false );
        normalize( histImagen2, histImagen2, 0, 1, NORM_MINMAX, -1, Mat() );


        return compareHist( histImagen1, histImagen2, 2);
    }

    public: Mat cutPieceImage(IplImage* imagen1, IplImage* imagen2) {
        vector<int> difPieces(9);
        int index = 0;
        CutAndResize cutAndResize;
        IplImage* imagen1Piece;
        IplImage* imagen2Piece;
        double histResult = 30;

        for (int i=1; i <10; i++) {
            imagen1Piece = cutAndResize.getZoom(imagen1, 3, 3, i);
            imagen2Piece = cutAndResize.getZoom(imagen2, 3, 3, i);
            if (getComparateImage(imagen1Piece, imagen2Piece) > histResult) {
                difPieces[index] = i;
                index = index + 1;
            }
        }
        return pasteOver(imagen2, difPieces, 3, 3);
    }

    public: Mat pasteOver(IplImage* fuente, vector<int> difPieces, int col, int fil) {
        char parcheC[] = "/home/mrc/ProyectosQt/imagenes/1.jpg";
        CutAndResize cutAndResize;
        Mat parche = cvLoadImage(parcheC, 1);
        Mat fuenteMat = cv::cvarrToMat(fuente);
        int tamano = difPieces.size();
        float dx, dy;
        for (int i=0; i < tamano; i++) {
            if (difPieces[i]>0) {
                dx = cutAndResize.getXCoordForCut(difPieces[i], parche.cols, col);
                dy = cutAndResize.getYCoordForCut(difPieces[i], parche.rows, fil);
                parche.copyTo(fuenteMat(cv::Rect(dx, dy, parche.cols , parche.rows)));
            }
        }
        return fuenteMat;
    }

};

int main() {

    char frame1[] = "/home/mrc/ProyectosQt/imagenes/2.jpg";
    char frame2[] = "/home/mrc/ProyectosQt/imagenes/208.jpg";

    ImageComparation i;
    IplImage* fuente1 = cvLoadImage(frame1, 1);
    IplImage* fuente2 = cvLoadImage(frame2, 1);

    cvNamedWindow("Fuente1", CV_WINDOW_AUTOSIZE);
    cvShowImage("Fuente1", fuente1);

    cvNamedWindow("Fuente2", CV_WINDOW_AUTOSIZE);
    cvShowImage("Fuente2", fuente2);

    Mat resultado = i.cutPieceImage(fuente1, fuente2);

    cvNamedWindow("Resultado", CV_WINDOW_AUTOSIZE);
    cv::imshow("Resultado", resultado);


    cvWaitKey(0);
    cvDestroyAllWindows();
    return 0;
}
