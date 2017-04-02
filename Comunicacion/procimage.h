#ifndef PROCIMAGE_H
#define PROCIMAGE_H
#include <QObject>
#include <string>


class ProcImage
{

public:
    static void putText();
    static void putTextTrasero();
    ProcImage();
    void procesar();
    void deleteText();

private slots:
   void onDataAvailable();
};  



#endif // PROCIMAGE_H
