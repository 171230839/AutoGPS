#include "camera.h"
#include <QCameraViewfinder>
#include <QMediaRecorder>
#include <QCamera>
#include <QCameraImageCapture>
#include <MapGraphicsView.h>
#include <QStackedWidget>
#include <QStackedLayout>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QLabel>

Camera::Camera(MapGraphicsView*view,  QObject *parent) :
    mapGraphicsView(view),
    QObject(parent),
      proxyWidget(NULL),
    captureLabel(NULL),
    stackedWidget(NULL),
    currentIndex(-1)
//    widget(NULL)
{
    proxyWidget = new QGraphicsProxyWidget();

    QWidget *widget = new QWidget();
    widget->setVisible(false);
    QList<QByteArray> devices = QCamera::availableDevices();
    QList<QCamera*> cameraList;
    widget->setContentsMargins(0,0,0,0);
    stackedWidget = new QStackedWidget(widget);
    stackedWidget->setVisible(false);
    stackedWidget->setContentsMargins(0,0,0,0);

  captureLabel = new QLabel(widget);
  captureLabel->setContentsMargins(0,0,0,0);
//  QPixmap pix;
//  pix.load("camera-0-00-18-55");
//  this->captureLabel->setPixmap(pix);
    foreach(QByteArray byteArray, devices)
    {
        QCameraViewfinder *viewfinder = new QCameraViewfinder(stackedWidget);
        stackedWidget->layout()->addWidget(viewfinder);
        QString description = QCamera::deviceDescription(byteArray);
        this->cameraList.append(description);
        QCamera * camera = new QCamera(byteArray, this);
        camera->setViewfinder(viewfinder);
//        camera->start();
        cameraList.append(camera);
        camera->setCaptureMode(QCamera::CaptureStillImage);
        QCameraImageCapture* imageCapture = new QCameraImageCapture(camera);
        cameraImageCaptureList.append(imageCapture);

//        connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));

    }
    proxyWidget->setWidget(widget);

    proxyWidget->setContentsMargins(0,0,0,0);
    mapGraphicsView->scene()->addItem(proxyWidget);
    foreach(QCamera* camera, cameraList)
    {
        camera->start();
    }
//    QTimer *timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
//    timer->start(1000);
    QTimer::singleShot(1000, this, SLOT(onTimeout()));
}

void Camera::onTimeout()
{
    qDebug()<<"onTimeout"<<cameraImageCaptureList.size();
    QTime time = QTime::currentTime();
    QString timeString = time.toString("hh-mm-ss");

//    foreach( QCameraImageCapture* capture, cameraImageCaptureList)
//    {
//        capture->capture(file);
//    }
    for (int i = 0; i < cameraImageCaptureList.size(); ++i)
    {
        QCameraImageCapture* capture = cameraImageCaptureList.at(i);
         QString file = "camera-" + QString::number(i) + "-" + timeString;
//         qDebug()<<"file"<<file;
//         qDebug()<<"captur4e"<<capture;
        int requestId = capture->capture(file);
//         qDebug()<<"requestId"<<requestId;
//        map[i] = requestId;
    }
}

QStringList Camera::getCameraList()
{
    return this->cameraList;
}

void Camera::setGeometry(const QRectF& rectf )
{

        qDebug()<<"rectf x"<<rectf.x()<<rectf.y();
    this->proxyWidget->setGeometry(rectf);
        QRect rect = rectf.toRect();
//  this->captureLabel->setGeometry(rectf.x()/2, rectf.y()/2, rectf.x(), rectf.y());
    this->stackedWidget->setGeometry(rectf.toRect());
    this->captureLabel->setGeometry(rect.width() / 2, rect.height()/2, rect.width(), rect.height());
}

void Camera::setVisible(bool state)
{
    this->proxyWidget->setVisible(state);
}

void Camera::handleCameraIndexChanged(int  index)
{
    currentIndex = index;
       this->stackedWidget->	setCurrentIndex(index);
}

//void Camera::processCapturedImage(int requestId, QImage img)
//{
//    qDebug()<<"processCapturedImage"<<requestId;
//    int id = map.key(requestId, -1);
//    if (id == -1)
//        return;

//    QImage scaledImage = img.scaled(stackedWidget->size(),
//                                    Qt::KeepAspectRatio,
//                                    Qt::SmoothTransformation);
//    this->captureLabel->setPixmap(QPixmap::fromImage(scaledImage));
//    QPixmap pix;
//    pix.load("camera-0-00-18-55");
//    this->captureLabel->setPixmap(pix);
//}
