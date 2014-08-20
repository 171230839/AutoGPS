#ifndef CAMERA_H
#define CAMERA_H


#include <QObject>
#include <QStringList>

class QCamera;
class QCameraImageCapture;
class QMediaRecorder;
class QCameraViewfinder;
namespace EsriRuntimeQt
{
class MapGraphicsView;
}
using namespace EsriRuntimeQt;
class Camera : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(QStringList descriptionList READ descriptionList )
public:
    explicit Camera(MapGraphicsView *inputGraphicsView,QObject *parent = 0);
    Q_INVOKABLE QString getCameraRightD();
    Q_INVOKABLE QString getCameraLeftD();
    QWidget* getWidget();
signals:

public slots:

private:
    MapGraphicsView* mapGraphicsView;
    QCamera *cameraL;
    QCameraImageCapture *imageCaptureLeft;
    QMediaRecorder* mediaRecorderLeft;

    QCamera *cameraR;
    QCameraImageCapture *imageCaptureRight;
    QMediaRecorder* mediaRecorderRight;

    QString m_cameraLeft;
    QString m_cameraRight;
    void setCameraLeft(QByteArray);
    void setCameraRight(QByteArray);

    QCameraViewfinder* viewfinderR;
    QCameraViewfinder* viewfinderL;
    QWidget * widget;
};

#endif // CAMERA_H
