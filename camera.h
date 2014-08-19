#ifndef CAMERA_H
#define CAMERA_H


#include <QObject>
#include <QStringList>
class QCamera;
class QCameraImageCapture;
class QMediaRecorder;

class Camera : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList descriptionList READ descriptionList )
public:
    explicit Camera(QObject *parent = 0);
    QStringList descriptionList();
signals:

public slots:

private:
    QCamera *camera;
    QCameraImageCapture *imageCapture;
    QMediaRecorder* mediaRecorder;
    QStringList m_descriptionList;
};

#endif // CAMERA_H
