#ifndef CAMERA_H
#define CAMERA_H


#include <QObject>
#include <QStringList>
#include <QImage>
#include <QMap>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QCamera;
class QCameraImageCapture;
class QMediaRecorder;
class QCameraViewfinder;
class QGraphicsProxyWidget;
class QStackedWidget;
class QLabel;
namespace EsriRuntimeQt
{
class MapGraphicsView;
}
QT_END_NAMESPACE
//using namespace EsriRuntimeQt;

namespace AutoGPSNAMESPACE{


class Camera : public QObject
{
    Q_OBJECT
    //    Q_PROPERTY(QStringList descriptionList READ descriptionList )
public:
    Camera(EsriRuntimeQt::MapGraphicsView * view);
    ~Camera();
    Q_INVOKABLE QStringList getCameraDescriptionList();
    void setGeometry(const QRectF&  );
    void setVisible(bool);

signals:

public slots:
    void handleCameraIndexChanged(int  index);
    void processCapturedImage(int,QImage);
    void onTimeout();
    void handleCaptureDisplay(bool);
    void handleCaptureStart(bool);
private:
    QPointer<EsriRuntimeQt::MapGraphicsView> mapGraphicsView;
    QStringList cameraDescriptionList;
    //    QStackedWidget* widget;
    QList<QCamera*> cameraList;
    QGraphicsProxyWidget* proxyWidget;
    QList<QCameraImageCapture*> cameraImageCaptureList;
    QMap<int, int> map;
    QLabel* captureLabel;
    QStackedWidget * stackedWidget;
    int currentIndex;
    QTimer *timer;
};


}
#endif // CAMERA_H
