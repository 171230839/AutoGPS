#ifndef CAMERA_H
#define CAMERA_H


#include <QObject>
#include <QStringList>
#include <QImage>
#include <QMap>

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
using namespace EsriRuntimeQt;




class Camera : public QObject
{
    Q_OBJECT
    //    Q_PROPERTY(QStringList descriptionList READ descriptionList )
public:
    Camera(MapGraphicsView * view, QObject* parent = 0);
    Q_INVOKABLE QStringList getCameraList();
    void setGeometry(const QRectF&  );
    void setVisible(bool);

signals:

public slots:
    void handleCameraIndexChanged(int  index);
//    void processCapturedImage(int,QImage);
    void onTimeout();
private:
    MapGraphicsView* mapGraphicsView;
    QStringList cameraList;
//    QStackedWidget* widget;
    QGraphicsProxyWidget* proxyWidget;
      QList<QCameraImageCapture*> cameraImageCaptureList;
      QMap<int, int> map;
       QLabel* captureLabel;
       QStackedWidget * stackedWidget;
       int currentIndex;
};



#endif // CAMERA_H
