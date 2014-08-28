#include "AutoGPS.h"
#include <QApplication>
#include <ArcGISLocalTiledLayer.h>
#include <ArcGISRuntime.h>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeContext>
#include <QMessageBox>
#include <QGraphicsLinearLayout>
#include <QtDeclarative/QDeclarativeEngine>
#include <QGraphicsWidget>
#include <MapGraphicsView.h>



static const QString UI_OVERLAY_PATH("qrc:/Resources/qml/MainOverlay.qml");

AutoGPS::AutoGPS (QWidget *parent):
    QMainWindow(parent),
    mapGraphicsView(NULL),
    mapController(NULL),
    engine(NULL),
    overlayWidget(NULL),
    overlayUI(NULL),
    mainMenuUI(NULL),
    context(NULL),
    thread(this, this),
    camera(NULL)
{

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->showMaximized();
    EsriRuntimeQt::ArcGISRuntime::setRenderEngine(EsriRuntimeQt::RenderEngine::OpenGL);

    mapGraphicsView = EsriRuntimeQt::MapGraphicsView::create(map, this);
    if (!mapGraphicsView)
    {
        qCritical() << "Unable to create map.";
        return;
    }
    map.setWrapAroundEnabled(false);
    map.setEsriLogoVisible(false);
    map.setMinScale(10);

    this->setCentralWidget(mapGraphicsView);
    QString tpkPath = QApplication::applicationDirPath() + QDir::separator() + "AutoGPS.tpk";
    ArcGISLocalTiledLayer tiledLayer(tpkPath);
    tiledLayer.setName("tiledLayer");
    qDebug()<<" items size"<<mapGraphicsView->items().size();
    map.addLayer(tiledLayer);
    qDebug()<<" items size"<<mapGraphicsView->items().size();
    //    ArcGISTiledMapServiceLayer tiledLayer("http://services.arcgisonline.com/ArcGIS/rest/services/NatGeo_World_Map/MapServer");
    //    map.addLayer(tiledLayer);
    //    ArcGISLocalTiledLayer tiledLayer("D:/Qt/QtSampleApplication_10.2.3_win32/sdk/samples/data/tpks/Topographic.tpk");
    //    map.addLayer(tiledLayer);

    mapController = new MapController(&map, mapGraphicsView, this);
     camera = new Camera(mapGraphicsView, this);


    engine = new QDeclarativeEngine(this);
    context = new QDeclarativeContext(engine->rootContext());
    context->setContextProperty("serialPortThread", &thread);
    context->setContextProperty("cameraObject", camera);


    QDeclarativeComponent component(engine, QUrl(UI_OVERLAY_PATH), engine);
    overlayUI = component.create(context);
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
    QGraphicsLayoutItem* qmlUILayout = qobject_cast<QGraphicsLayoutItem*>(overlayUI);
    layout->addItem(qmlUILayout);
    overlayWidget->setLayout(layout);

    mapGraphicsView->scene()->addItem(overlayWidget);


    connect(overlayUI, SIGNAL(homeClicked()), mapController, SLOT(handleHomeClicked()));
    connect(overlayUI, SIGNAL(zoomInClicked()), mapController, SLOT(handleZoomIn()));
    connect(overlayUI, SIGNAL(zoomOutClicked()), mapController, SLOT(handleZoomOut()));
    connect(overlayUI, SIGNAL(panClicked(QString)), mapController, SLOT(handlePan(QString)));
    connect(mapController, SIGNAL(positionChanged(QVariant)), overlayUI, SLOT(updateLocation(QVariant)));
    connect(mapController, SIGNAL(speedChanged(QVariant)), overlayUI, SLOT(updateSpeed(QVariant)));
    connect(mapController, SIGNAL(headingChanged(QVariant)), overlayUI, SLOT(updateHeading(QVariant)));
    connect(&thread, SIGNAL(positionChanged(QVariant)), overlayUI, SLOT(updateLocation(QVariant)));
    connect(&thread, SIGNAL(avaliblePosition(double, double, double)), mapController, SLOT(onAvaliblePosition(double, double, double)));
    connect(&thread, SIGNAL(timeChanged(QVariant)), overlayUI, SLOT(updateTime(QVariant)));
    connect(&thread, SIGNAL(speedChanged(QVariant)), overlayUI, SLOT(updateSpeed(QVariant)));
    connect(&thread, SIGNAL(headingChanged(QVariant)), overlayUI, SLOT(updateHeading(QVariant)));
    connect(overlayUI, SIGNAL(basemapChanged(QString)), this, SLOT(handleBasemapChanged(QString)));
    connect(overlayUI, SIGNAL(cameraIndexChanged(int)), camera, SLOT(handleCameraIndexChanged(int)));
    connect(overlayUI, SIGNAL(captureDisplay(bool)), camera, SLOT(handleCaptureDisplay(bool)));
    connect(overlayUI, SIGNAL(captureStart(bool)), camera, SLOT(handleCaptureStart(bool)));
    connect(&thread, SIGNAL(error(QVariant)), overlayUI, SLOT(error(QVariant)));
    connect(&thread, SIGNAL(paintGeometry(QList<QPointF>)), mapController, SLOT(onPaintGeometry(QList<QPointF>)));
    mainMenuUI = overlayUI->findChild<QObject*>("mainMenu");
    if (mainMenuUI)
    {
        connect(mainMenuUI, SIGNAL(exitClicked()), this, SLOT(close()));
    }

    QObject * serialConfig = overlayUI->findChild<QObject*>("serialConfig");
    if (serialConfig)
    {
        connect(serialConfig, SIGNAL(readyOpenSerialPort(QVariant)), &thread, SLOT(onReadyOpenSerialPort(QVariant)));
    }

    QObject *config = overlayUI->findChild<QObject*>("config");
    if (config)
    {
        connect(config, SIGNAL(showMeToggled(bool)), mapController, SLOT(handleToggleShowMe(bool)));
        connect(config, SIGNAL(followMeToggled(bool)), mapController, SLOT(handleToggleFollowMe(bool)));
        connect(config, SIGNAL(resetMapClicked()), mapController, SLOT(handleResetMap()));
    }
    QObject *geometryPanel = overlayUI->findChild<QObject*>("geometryPanel");
    if (geometryPanel)
    {
        connect(geometryPanel, SIGNAL(pointsToggled(bool)), mapController, SLOT(handlePointsToggled(bool)));
        connect(geometryPanel, SIGNAL(toLinesClicked()), mapController, SLOT(handleToLinesClicked()));
//        connect(geometryPanel, SIGNAL(okClicked()), mapController, SLOT(handleOkClicked()));
        connect(geometryPanel, SIGNAL(toPolygonClicked()), mapController, SLOT(handleToPolygonClicked()));
        connect(geometryPanel, SIGNAL(clearClicked()), mapController, SLOT(onClearClicked()));
    }

    QObject *workerPanel = overlayUI->findChild<QObject*>("workerPanel");
    if (workerPanel)
    {
        //        connect(workerPanel, SIGNAL(selectPointToggled(bool)), mapController, SLOT(handleSelectPointToggled(bool)));
    }
//    QObject *pathsPanel = overlayUI->findChild<QObject*>("pathsPanel");
//    if (pathsPanel)
//    {
//        connect(pathsPanel, SIGNAL(selectPointToggled(bool)), mapController, SLOT(handleSelectPointToggled(bool)));
//        connect(pathsPanel, SIGNAL(getPathClicked()), mapController, SLOT(handleGetPathClicked()));
//        connect(pathsPanel, SIGNAL(unSelectClicked()), mapController, SLOT(handleUnSelectClicked()));
//    }

    QObject *record = overlayUI->findChild<QObject*>("record");
    if (record)
    {
        connect(record, SIGNAL(startRecordClicked()), &thread, SLOT(onStartRecordClicked()));
        connect(record, SIGNAL(stopAndSaveClicked()), &thread, SLOT(onStopAndSaveClicked()));
        connect(record, SIGNAL(translateToXmlClicked()), &thread, SLOT(onTranslateToXmlClicked()));
        connect(record, SIGNAL(selectLogFileClicked()), &thread, SLOT(onSelectLogFileClicked()));
        connect(record, SIGNAL(xmlStartRecordClicked()), &thread, SLOT(onXmlStartRecordClicked()));
        connect(record, SIGNAL(xmlStopAndSaveClicked()), &thread, SLOT(onXmlStopAndSaveClicked()));
        connect(record, SIGNAL(selectXmlFileClicked()), &thread, SLOT(onSelectXmlFileClicked()));
//        connect(record, SIGNAL(playInSimulatorClicked()), &thread, SLOT(onPlayInSimulatorClicked()));
        connect(record, SIGNAL(paintGeometryClicked()), &thread, SLOT(onPaintGeometryClicked()));
        connect(record, SIGNAL(selectPointsToggled(bool)), mapController, SLOT(handleSelectPointsToggled(bool)));
         connect(record, SIGNAL(paintCropLandClicked()), mapController, SLOT(handlePaintCropLandClicked()));
        connect(record, SIGNAL(unSelectClicked()), mapController, SLOT(handleUnSelectClicked()));
        connect(record, SIGNAL(selectStartPointClicked()), mapController, SLOT(handleSelectStartPointClicked()));
        connect(record, SIGNAL(getPathClicked()), mapController, SLOT(handleGetPathClicked()));
    }

    connect(&map, SIGNAL(mapReady()), mapController, SLOT(onMapReady()));
    connect(&map, SIGNAL(mouseWheel(QWheelEvent)), mapController, SLOT(onMouseWheel(QWheelEvent)));

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateNorthArrow()));
    timer->start(1000 / 24);

    // hook up the handler for the mouse click
    connect(&map, SIGNAL(mousePress(QMouseEvent)), mapController, SLOT(mousePress(QMouseEvent)));


//    QWidget * widget = new QWidget();
//    widget->setLayoutDirection(leftToRight);


     thread.start();

}

AutoGPS::~AutoGPS()
{
}

void AutoGPS::updateNorthArrow()
{
    QMetaObject::invokeMethod(overlayUI, "updateMapRotation", Q_ARG(QVariant, map.rotation()));
}

void AutoGPS::resizeEvent(QResizeEvent* event)
{
    if (map.isInitialized())
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

    thread.storeSerialConfig();
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
    QStringList layerNames = map.layerNames();
    if (layerNames.contains("tiledLayer"))
    {
        Layer layer = map.layer("tiledLayer");
        layer.setVisible(true);
    }
    if (mapController)
    {
        if (mapController->getSimpleGraphic())
        {
            mapController->getSimpleGraphic()->setVisible(true);
        }
    }
    if (camera)
    {
       camera->setVisible(false);
    }
}

void AutoGPS::setBasemapSecond()
{
    qDebug()<<"setBasemapSecond";
    QStringList layerNames = map.layerNames();
    if (layerNames.contains("tiledLayer"))
    {
        Layer layer = map.layer("tiledLayer");
        layer.setVisible(false);
    }
    if (mapController)
    {
        if (mapController->getSimpleGraphic())
        {
            mapController->getSimpleGraphic()->setVisible(false);
        }
    }

    if (camera)
    {
        camera->setVisible(true);
    }

}

void AutoGPS::setBasemapThird()
{

}

