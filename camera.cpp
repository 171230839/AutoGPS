#include "camera.h"
#include <QCameraViewfinder>
#include <QMediaRecorder>
#include <QCamera>
#include <QCameraImageCapture>

Camera::Camera(QObject *parent) :
    QObject(parent),
    camera(0),
    imageCapture(0),
    mediaRecorder(0)
{
    QByteArray cameraDevice;
    QStringList descriptionList;
    foreach(const QByteArray &deviceName, QCamera::availableDevices()) {
        QString description = camera->deviceDescription(deviceName);
        descriptionList.append(description);
    }
}

 QStringList Camera::descriptionList()
 {
    m_descriptionList.clear();
     foreach(const QByteArray &deviceName, QCamera::availableDevices()) {
         QString description = camera->deviceDescription(deviceName);
         m_descriptionList.append(description);
     }
     return m_descriptionList;
 }
