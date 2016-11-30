#ifndef DETECTARVEHICULO2_H
#define DETECTARVEHICULO2_H

#include <QObject>
#include <opencv/cv.h>
#include <opencv/cv.hpp>

class DetectarVehiculo2 : public QObject
{
    Q_OBJECT
public:
    explicit DetectarVehiculo2(QObject *parent = 0);
    cv::Mat clasificar(std::vector<cv::Point> matriz);
    void comenzar();
    static void actualizarZoom(double zoomUI);

signals:

public slots:
};

#endif // DETECTARVEHICULO2_H
