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
    currentIndex(0)
//    widget(NULL)
{
    proxyWidget = new QGraphicsProxyWidget();

    QWidget *widget = new QWidget();
    widget->setVisible(false);
    QList<QByteArray> devices = QCamera::availableDevices();
    QList<QCamera*> cameraList;
    widget->setContentsMargins(0,0,0,0);
    stackedWidget = new QStackedWidget(widget);
//    stackedWidget->setVisible(false);
    stackedWidget->setContentsMargins(0,0,0,0);

  captureLabel = new QLabel(widget);
  captureLabel->setContentsMargins(0,0,0,0);
  captureLabel->setVisible(false);
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

        connect(imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));

    }
    proxyWidget->setWidget(widget);

    proxyWidget->setContentsMargins(0,0,0,0);
    mapGraphicsView->scene()->addItem(proxyWidget);
    foreach(QCamera* camera, cameraList)
    {
        camera->start();
    }
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start(2000);
//    QTimer::singleShot(1000, this, SLOT(onTimeout()));
}

void Camera::onTimeout()
{

    qDebug()<<"onTimeout"<<cameraImageCaptureList.size();
    QTime time = QTime::currentTime();
    QString timeString = time.toString("hh-mm-ss");

    //requestId 每次timeout总是相同，导致无法区分，暂时只能只捕捉当前页面的。
    QCameraImageCapture* capture = cameraImageCaptureList.at(currentIndex);
    QString file = "camera-" + QString::number(currentIndex) + "-" + timeString;
    capture->capture(file);
//    for (int i = 0; i < cameraImageCaptureList.size(); ++i)
//    {
//        QCameraImageCapture* capture = cameraImageCaptureList.at(i);
//         QString file = "camera-" + QString::number(i) + "-" + timeString;
//         qDebug()<<"file"<<file;
//         qDebug()<<"captur4e"<<capture;
//        int requestId = capture->capture(file);
//         qDebug()<<"requestId"<<requestId << "currentIdex"<<currentIndex;
//        map[i] = requestId;
//    }
    //--------------------------------------------
}

QStringList Camera::getCameraList()
{
    return this->cameraList;
}

void Camera::setGeometry(const QRectF& rectf )
{

        qDebug()<<"rectf x"<<rectf.height()<<rectf.width();
    this->proxyWidget->setGeometry(rectf);
        QRect rect = rectf.toRect();
//  this->captureLabel->setGeometry(rectf.x()/2, rectf.y()/2, rectf.x(), rectf.y());
    this->stackedWidget->setGeometry(rectf.toRect());
//    this->captureLabel->setGeometry(rect.width() / 2, rect.height()/2, rect.width(), rect.height());
        this->captureLabel->setGeometry(0,0, rect.width()/2, rect.height() / 2);
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

void Camera::processCapturedImage(int requestId, QImage img)
{
    qDebug()<<"processCapturedImage"<<requestId;
//    int id = map.key(requestId, -1);
//    if (id == -1)
//        return;
    qDebug()<<"label size"<<captureLabel->size();
    qDebug()<<"widget size"<<stackedWidget->size();

    QImage scaledImage = img.scaled(stackedWidget->size()/2,
                                      Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);
    this->captureLabel->setPixmap(QPixmap::fromImage(scaledImage));
}

void Camera::handleCaptureDisplay(bool state)
{
    this->captureLabel->setVisible(state);
}
