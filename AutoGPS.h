// Copyright 2014 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef AUTOGPS_H
#define AUTOGPS_H


#include <QMainWindow>


QT_BEGIN_NAMESPACE
class QDeclarativeEngine;
class QGraphicsWidget;
class QDeclarativeContext;
namespace EsriRuntimeQt
{
class MapGraphicsView;
class Map;
}
namespace AutoGPSNAMESPACE
{
class MapController;
class MasterThread;
class Camera;
}

QT_END_NAMESPACE
//using AutoGPSNAMESPACE;
namespace AutoGPSNAMESPACE{


class AutoGPS : public QMainWindow
{
    Q_OBJECT
public:
    explicit  AutoGPS (QWidget *parent = 0);
    ~AutoGPS ();

private slots:
    void updateNorthArrow();
public slots:
    void  handleBasemapChanged(QString);

private:
    QScopedPointer<EsriRuntimeQt::Map> map;
     EsriRuntimeQt::MapGraphicsView* mapGraphicsView;
//    ArcGISLocalTiledLayer tiledLayer;
    QScopedPointer<MapController> mapController;
    QScopedPointer<QDeclarativeEngine> engine;
    QGraphicsWidget* overlayWidget;
    QScopedPointer<QObject> overlayUI;
    QScopedPointer<QDeclarativeContext> context ;
    QScopedPointer<MasterThread> thread;
    QScopedPointer<Camera> camera;
    void setBasemapFirst();
    void setBasemapSecond();
    void setBasemapThird();
protected:
    void resizeEvent(QResizeEvent* event);
    void closeEvent(QCloseEvent *event);

};

}
#endif // AUTOGPS_H

