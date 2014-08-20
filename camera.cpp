#include "camera.h"
#include <QCameraViewfinder>
#include <QMediaRecorder>
#include <QCamera>
#include <QCameraImageCapture>
#include <MapGraphicsView.h>


Camera::Camera( MapGraphicsView *inputGraphicsView, QObject *parent) :
    mapGraphicsView(inputGraphicsView),
     QObject(parent),
    cameraL(0),
    imageCaptureLeft(0),
    mediaRecorderLeft(0),
    cameraR(0),
    imageCaptureRight(0),
    mediaRecorderRight(0),
    viewfinderL(0),
    viewfinderR(0),
    widget(0)
{
    QByteArray cameraDeviceLeft;
    QByteArray cameraDeviceRight;
    QStringList m_descriptionList;
    QList<QByteArray> devices = QCamera::availableDevices();
//    foreach(const QByteArray deviceName, devices) {
//        QString description = camera->deviceDescription(deviceName);
//        m_descriptionList.append(description);
//    }
    if (devices.size() > 0)
    {
        cameraDeviceLeft = devices.at(0);
        m_cameraLeft = QCamera::deviceDescription(cameraDeviceLeft);
        setCameraLeft(cameraDeviceLeft);
    }
    if (devices.size() > 1)
    {
        cameraDeviceRight = devices.at(1);
        m_cameraRight = QCamera::deviceDescription(cameraDeviceRight);
        setCameraRight(cameraDeviceRight);
    }

    widget = new QWidget();
    widget->setFixedHeight(mapGraphicsView->height() * 3 / 4 ) ;
    widget->setFixedWidth(mapGraphicsView->width());
    widget->setLayoutDirection(Qt::LeftToRight);
    widget->setVisible(false);
    widget->setObjectName("cameraWidget");
   viewfinderL = new QCameraViewfinder(widget);

//   viewfinderL->width()=  mapGraphicsView->width() / 2;
//   viewfinderL->height() = mapGraphicsView->height() * 3 / 4;
//   viewfinderL->setTopMargin(mapGraphicsView->height() / 4 + 30);
   viewfinderL->setFixedHeight(widget->height()) ;
   viewfinderL->setFixedWidth(widget->width() / 2);
//   viewfinderL->setVisible(false);
   viewfinderR = new QCameraViewfinder(widget);
   viewfinderR->setFixedHeight(widget->height()) ;
   viewfinderR->setFixedWidth(widget->width() / 2);
    mapGraphicsView->scene()->addWidget(widget);
}


QString Camera::getCameraRightD()
{
    return m_cameraRight;
}

QString Camera::getCameraLeftD()
{
    return m_cameraLeft;
}

void Camera::setCameraLeft(QByteArray cameraDeviceLeft)
{
        if (cameraDeviceLeft.isEmpty())
            return;
        else
            cameraL = new QCamera(cameraDeviceLeft);
//        connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)));
//        connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));
        cameraL->setViewfinder(viewfinderL);
        cameraL->start();

}



void Camera::setCameraRight(QByteArray cameraDeviceRight)
{

}

QWidget *Camera::getWidget()
{
    return this->widget;
}
