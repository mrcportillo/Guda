#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "cv.h" // incluye las definiciones de OpenCV
#include <opencv/cv.h>
//#include "cvaux.h" // Stuff.
#include <opencv/cvaux.h>
//#include "highgui.h" // incluye highGUI. Necesario para crear ventanas de visualizacion
#include <opencv/highgui.h>
#include "iostream"
//#include <QtExtSerialPort/qextserialport.h>
#include <QtSerialPort/QtSerialPort>
#include "procimage.h"

using namespace std;

IplImage* frame;

string salText="";
string salTextTrasero="";


QSerialPort *port;

void MainWindow::putText(){
    salText="Cuidado!";
}

void MainWindow::putTextTrasero(){
    salTextTrasero="Mantenga distancia!";
}


void MainWindow::onDataAvailable(){
    cout << "Evento!!" << endl;
    QByteArray data = port->readAll();
    if(data.length() > 0){
        cout << "Recibido: " << endl;
        for(int i = 0; i < data.length(); i++){
            cout << data.at(i) << endl;
            if (data.at(i)=='C'){
                string result= "Cuidado!!!";
                putText();
                cout<<result<<endl;

            } else if (data.at(i)=='B'){
                string result2= "Mantenga distancia!!!";
                putTextTrasero();
                cout<<result2<<endl;
            }
        }
        cout << "Fin recibido" << endl;
    }
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //port = new QextSerialPort("/dev/ttyACM0");
    port = new QSerialPort("/dev/pts/1");        //puerto virtual. Ver comandos para configurar
    port->setBaudRate(115200);
//    port->setQueryMode(QSerialPort::EventDriven);
    port->open(QIODevice::ReadWrite);
    port->flush();
    connect(port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));

    /////////////////////////////////////////////////////////////////////////////////////////////
    //cvNamedWindow( "Parabrisa Frontal", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "Parabrisa Frontal", CV_WINDOW_NORMAL);
    cvResizeWindow("Parabrisa Frontal", 400, 300);
    cvMoveWindow("Parabrisa Frontal",0,0);
    int contador=1;

    std::string path;
    int contSinRecepcion=0;
    ////////////////////////////////////////////////////////////////////////////////////////////

    int contSinRecepcionTrasero=0;

    CvCapture *capture = NULL;
    capture = cvCreateCameraCapture(0);

    if(!capture){
        cout<<"error"<<endl;
    }

    IplImage* color_frame;

    color_frame = NULL;

    //cvNamedWindow( "Parabrisa Trasero", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "Parabrisa Trasero", CV_WINDOW_NORMAL );
    cvResizeWindow("Parabrisa Trasero", 400, 300);
    cvMoveWindow("Parabrisa Trasero",400,0);

    while(1) {


        //////////////////////////////// Secuencia de imagenes//////////////////////////////////////////////
        std::stringstream convert;
        convert<<contador;
        path="/home/juanmartin/Dropbox/GUDA 2016/Proyectos QT/T4/destino/"+convert.str()+".jpg";
        //std::cout<<path+"\n";
        const char * pathChar = path.c_str();
        frame = cvLoadImage(pathChar);

        CvFont font;
        double hScale=1.5;
        double vScale=2;
        int lineWidth=3;
        cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX, hScale,vScale,0,lineWidth);
        cvPutText (frame,salText.c_str(),cvPoint(50,50), &font, cvScalar(0,0,255));
        if (salText=="" & contSinRecepcion!=0){
            contSinRecepcion=0;
        }
        if (contSinRecepcion>20){
            salText="";
        }
        contSinRecepcion++;
        cvShowImage( "Parabrisa Frontal", frame );
        contador++;

        ////////////////////////////////////// Camara de video/////////////////////////////////////////////////

        color_frame = cvQueryFrame( capture );//Grabs the frame from a file
        if( !color_frame ) break;

        if( !color_frame ) break;// If the frame does not exist, quit the loop

        //***********************************************************************
        //
        // Aca va el algoritmo que queremos hacer para la captura que se obtiene
        //
        //**********************************************************************


        cvPutText (color_frame,salTextTrasero.c_str(),cvPoint(50,50), &font, cvScalar(0,0,255));
        if (salTextTrasero=="" & contSinRecepcionTrasero!=0){
            contSinRecepcionTrasero=0;
        }
        if (contSinRecepcionTrasero>20){
            salTextTrasero="";
        }
        contSinRecepcionTrasero++;

        cvShowImage("Parabrisa Trasero", color_frame);

        cvReleaseImage(&frame); //va liberando la memoria
        char c = cvWaitKey(10);
        if( c == 27 ) break;
    }

    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}
