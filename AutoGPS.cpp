#include "AutoGPS.h"
#include "mapcontroller.h"
#include "masterthread.h"
#include "camera.h"
//#include <QApplication>
#include <ArcGISLocalTiledLayer.h>
#include <ArcGISRuntime.h>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeContext>
#include <QMessageBox>
#include <QGraphicsLinearLayout>
#include <QtDeclarative/QDeclarativeEngine>
#include <QGraphicsWidget>
#include <MapGraphicsView.h>
#include <QFileDialog>



namespace AutoGPSNAMESPACE{
using namespace EsriRuntimeQt;
static const QString UI_OVERLAY_PATH("qrc:/Resources/qml/MainOverlay.qml");

AutoGPS::AutoGPS (QWidget *parent):
    QMainWindow(parent),
    map(new Map()),
    mapGraphicsView(NULL),
    mapController(NULL),
    engine(NULL),
    overlayWidget(NULL),
    overlayUI(NULL),
    context(NULL),
    thread(new MasterThread(this)),
    camera(NULL)
{

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->showMaximized();
    EsriRuntimeQt::ArcGISRuntime::setRenderEngine(EsriRuntimeQt::RenderEngine::OpenGL);

    mapGraphicsView = EsriRuntimeQt::MapGraphicsView::create(*map);
    if (!mapGraphicsView)
    {
        qCritical() << "Unable to create map->";
        return;
    }
    map->setWrapAroundEnabled(false);
    map->setEsriLogoVisible(false);
    map->setMinScale(10);

    this->setCentralWidget(mapGraphicsView);
    QString tpkPath = QCoreApplication::applicationDirPath() + "/AutoGPS.tpk";
//    QString tpkPath = "AutoGPS.tpk";
    ArcGISLocalTiledLayer tiledLayer(tpkPath);
    tiledLayer.setName("tiledLayer");
    qDebug()<<" items size"<<mapGraphicsView->items().size();
    map->addLayer(tiledLayer);
    qDebug()<<" items size"<<mapGraphicsView->items().size();
    //    ArcGISTiledMapServiceLayer tiledLayer("http://services.arcgisonline.com/ArcGIS/rest/services/NatGeo_World_Map/MapServer");
    //    map->addLayer(tiledLayer);
    //    ArcGISLocalTiledLayer tiledLayer("D:/Qt/QtSampleApplication_10.2.3_win32/sdk/samples/data/tpks/Topographic.tpk");
    //    map->addLayer(tiledLayer);

    mapController.reset(new MapController(map.data(), mapGraphicsView, this));
    camera.reset(new Camera(mapGraphicsView));


    engine.reset(new QDeclarativeEngine());
    context.reset(new QDeclarativeContext(engine->rootContext()));
    context->setContextProperty("serialPortThread", thread.data());
    context->setContextProperty("cameraObject", camera.data());


    QDeclarativeComponent component(engine.data(), QUrl(UI_OVERLAY_PATH));
    overlayUI.reset(component.create(context.data()));
    //    thread.setContext(context);
    if (!overlayUI)
    {
        qDebug() << "Failed to load UI overlay";
        qDebug() << component.errorString();
        QMessageBox box;
        box.setText("Error:\n" + component.errorString());
        box.exec();
        return;
    }



    // Create a container for the UML UI and add it to the scene
    overlayWidget = new QGraphicsWidget();
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(overlayWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    QGraphicsLayoutItem* qmlUILayout = qobject_cast<QGraphicsLayoutItem*>(overlayUI.data());
    layout->addItem(qmlUILayout);
    overlayWidget->setLayout(layout);

    mapGraphicsView->scene()->addItem(overlayWidget);


    connect(overlayUI.data(), SIGNAL(homeClicked()), mapController.data(), SLOT(handleHomeClicked()));
    connect(overlayUI.data(), SIGNAL(zoomInClicked()), mapController.data(), SLOT(handleZoomIn()));
    connect(overlayUI.data(), SIGNAL(zoomOutClicked()), mapController.data(), SLOT(handleZoomOut()));
    connect(overlayUI.data(), SIGNAL(panClicked(QString)), mapController.data(), SLOT(handlePan(QString)));
    connect(mapController.data(), SIGNAL(positionChanged(QVariant)), overlayUI.data(), SLOT(updateLocation(QVariant)));
    connect(mapController.data(), SIGNAL(speedChanged(QVariant)), overlayUI.data(), SLOT(updateSpeed(QVariant)));
    connect(mapController.data(), SIGNAL(headingChanged(QVariant)), overlayUI.data(), SLOT(updateHeading(QVariant)));
   connect(mapController.data(), SIGNAL(error(QVariant)), overlayUI.data(), SLOT(error(QVariant)));
    connect(thread.data(), SIGNAL(positionChanged(QVariant)), overlayUI.data(), SLOT(updateLocation(QVariant)));
    connect(thread.data(), SIGNAL(avaliblePosition(double, double, double)), mapController.data(), SLOT(onAvaliblePosition(double, double, double)));
    connect(thread.data(), SIGNAL(timeChanged(QVariant)), overlayUI.data(), SLOT(updateTime(QVariant)));
    connect(thread.data(), SIGNAL(speedChanged(QVariant)), overlayUI.data(), SLOT(updateSpeed(QVariant)));
    connect(thread.data(), SIGNAL(headingChanged(QVariant)), overlayUI.data(), SLOT(updateHeading(QVariant)));
    connect(overlayUI.data(), SIGNAL(basemapChanged(QString)), this, SLOT(handleBasemapChanged(QString)));
    connect(overlayUI.data(), SIGNAL(cameraIndexChanged(int)), camera.data(), SLOT(handleCameraIndexChanged(int)));
    connect(overlayUI.data(), SIGNAL(captureDisplay(bool)), camera.data(), SLOT(handleCaptureDisplay(bool)));
    connect(overlayUI.data(), SIGNAL(captureStart(bool)), camera.data(), SLOT(handleCaptureStart(bool)));
    connect(thread.data(), SIGNAL(error(QVariant)), overlayUI.data(), SLOT(error(QVariant)));
    connect(thread.data(), SIGNAL(paintGeometry(const QList<QPointF*>&)), mapController.data(), SLOT(onPaintGeometry(const QList<QPointF*>&)));
    connect(thread.data(), SIGNAL(paintProject(const QList<EsriRuntimeQt::Point*>&, QString, QString)), mapController.data(), SLOT(onPaintProject(const QList<EsriRuntimeQt::Point*>&, QString, QString)));
    connect(mapController.data(), SIGNAL(processProject(QString)), thread.data(), SLOT(onProcessProject(QString)));


    QObject * mainMenuUI = overlayUI->findChild<QObject*>("mainMenu");
    if (mainMenuUI)
    {
        connect(mainMenuUI, SIGNAL(exitClicked()), this, SLOT(close()));
        connect(mainMenuUI, SIGNAL(userSelectProjectClicked(QString )), thread.data(), SLOT(onSelectProjectClicked(QString)));
        connect(mapController.data(), SIGNAL(gotoPlayerPanel()), mainMenuUI, SLOT(onGotoPlayerPanel()));
    }

    QObject * serialConfig = overlayUI->findChild<QObject*>("serialConfig");
    if (serialConfig)
    {
        connect(serialConfig, SIGNAL(readyOpenSerialPort(QVariant)), thread.data(), SLOT(onReadyOpenSerialPort(QVariant)));
    }

    QObject *config = overlayUI->findChild<QObject*>("config");
    if (config)
    {
        connect(config, SIGNAL(showMeToggled(bool)), mapController.data(), SLOT(handleToggleShowMe(bool)));
        connect(config, SIGNAL(followMeToggled(bool)), mapController.data(), SLOT(handleToggleFollowMe(bool)));
        connect(config, SIGNAL(resetMapClicked()), mapController.data(), SLOT(handleResetMap()));
    }
    QObject *worker = overlayUI->findChild<QObject*>("worker");
    if (worker)
    {
        connect(worker, SIGNAL(createProjectClicked()), mapController.data(), SLOT(handleCreateProjectClicked()));
        connect(worker, SIGNAL(pointsToggled(bool)), mapController.data(), SLOT(handlePointsToggled(bool)));
        connect(worker, SIGNAL(toLinesClicked()), mapController.data(), SLOT(handleToLinesClicked()));
        connect(worker, SIGNAL(toPolygonClicked()), mapController.data(), SLOT(handleToPolygonClicked()));
        connect(worker, SIGNAL(clearClicked()), mapController.data(), SLOT(onClearClicked()));
        connect(worker, SIGNAL(saveProjectClicked()), mapController.data(), SLOT(handleSaveProjectClicked()));
    }


   QObject * record = overlayUI->findChild<QObject*>("record");
    if (record)
    {
        connect(record, SIGNAL(startRecordClicked()), thread.data(), SLOT(onStartRecordClicked()));
        connect(record, SIGNAL(stopAndSaveClicked()), thread.data(), SLOT(onStopAndSaveClicked()));
        connect(record, SIGNAL(translateToXmlClicked()), thread.data(), SLOT(onTranslateToXmlClicked()));
        connect(record, SIGNAL(selectLogFileClicked()), thread.data(), SLOT(onSelectLogFileClicked()));
        connect(record, SIGNAL(xmlStartRecordClicked()), thread.data(), SLOT(onXmlStartRecordClicked()));
        connect(record, SIGNAL(xmlStopAndSaveClicked()), thread.data(), SLOT(onXmlStopAndSaveClicked()));
        connect(record, SIGNAL(selectXmlFileClicked()), thread.data(), SLOT(onSelectXmlFileClicked()));
//        connect(record, SIGNAL(playInSimulatorClicked()), thread.data(), SLOT(onPlayInSimulatorClicked()));
//        connect(record, SIGNAL(paintGeometryClicked()), thread.data(), SLOT(onPaintGeometryClicked()));
        connect(record, SIGNAL(selectProjectClicked(QString)), thread.data(), SLOT(onSelectProjectClicked(QString)));
        connect(mapController.data(), SIGNAL(addCroplandPanel()), record, SLOT(onAddCropLandPanel()));


        connect(record, SIGNAL(selectPointsToggled(bool)), mapController.data(), SLOT(handleSelectPointsToggled(bool)));
         connect(record, SIGNAL(paintCropLandClicked()), mapController.data(), SLOT(handlePaintCropLandClicked()));
        connect(record, SIGNAL(unSelectClicked()), mapController.data(), SLOT(handleUnSelectClicked()));
        connect(record, SIGNAL(selectStartPointClicked()), mapController.data(), SLOT(handleSelectStartPointClicked()));
        connect(record, SIGNAL(getPathClicked()), mapController.data(), SLOT(handleGetPathClicked()));
        connect(record, SIGNAL(pathSaveProjectClicked()), mapController.data(), SLOT(handlePathSaveProjectClicked()));
    }

    connect(map.data(), SIGNAL(mapReady()), mapController.data(), SLOT(onMapReady()));
    connect(map.data(), SIGNAL(mouseWheel(QWheelEvent)), mapController.data(), SLOT(onMouseWheel(QWheelEvent)));

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateNorthArrow()));
    timer->start(1000 / 24);

    // hook up the handler for the mouse click
    connect(map.data(), SIGNAL(mousePress(QMouseEvent)), mapController.data(), SLOT(mousePress(QMouseEvent)));


//    QWidget * widget = new QWidget();
//    widget->setLayoutDirection(leftToRight);
     thread->start();
}

AutoGPS::~AutoGPS()
{
    qDebug()<<"~AutoGPS";
}

void AutoGPS::updateNorthArrow()
{
    QMetaObject::invokeMethod(overlayUI.data(), "updateMapRotation", Q_ARG(QVariant, map->rotation()));
}

void AutoGPS::resizeEvent(QResizeEvent* event)
{
    if (map->isInitialized())
    {
        QRectF sceneRect = mapGraphicsView->sceneRect();
//        qDebug()<<"sceneRect height"<<sceneRect.height();
        overlayWidget->setGeometry(sceneRect);
        camera->setGeometry(sceneRect);
    }
    QMainWindow::resizeEvent(event);
}

void AutoGPS::closeEvent(QCloseEvent *event)
{

    thread->storeSerialConfig();
    event->accept();
}

void AutoGPS::handleBasemapChanged(QString basemap)
{
    if (basemap == "Map")
    {
        setBasemapFirst();
    }
    else if ( basemap == "Camera")
    {
        setBasemapSecond();
    }
    else if (basemap == "3D")
    {
        setBasemapThird();
    }
}

void AutoGPS::setBasemapFirst()
{
    qDebug()<<"setBasemapFirst";
    QStringList layerNames = map->layerNames();
    if (layerNames.contains("tiledLayer"))
    {
        Layer layer = map->layer("tiledLayer");
        layer.setVisible(true);
    }
    if (mapController)
    {
         mapController->setSimpleVisible(true);
    }
    if (camera)
    {
       camera->setVisible(false);
    }
}

void AutoGPS::setBasemapSecond()
{
    qDebug()<<"setBasemapSecond";
    QStringList layerNames = map->layerNames();
    if (layerNames.contains("tiledLayer"))
    {
        Layer layer = map->layer("tiledLayer");
        layer.setVisible(false);
    }
    if (mapController)
    {
            mapController->setSimpleVisible(true);
    }

    if (camera)
    {
        camera->setVisible(true);
    }

}

void AutoGPS::setBasemapThird()
{

}


}
