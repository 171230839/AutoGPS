#include "mycoordinate.h"
#include "mapcontroller.h"
#include "simplegraphicoverlay.h"
#include <GeometryEngine.h>
#include <SimpleMarkerSymbol.h>
#include <Polyline.h>
#include <Polygon.h>
#include <SimpleFillSymbol.h>
#include <MapGraphicsView.h>
#include <Map.h>
#include <QFileDialog>
#include <GraphicsLayer.h>
#include <Line.h>
#include <Point.h>



const   double   PI   =   3.141592653589793;
static const double  carWidth = 2.0;

namespace AutoGPSNAMESPACE{

using namespace EsriRuntimeQt;

MapController::MapController(Map* inputMap,
                             MapGraphicsView* inputGraphicsView,
                             QWidget* widget):
    map(inputMap),
    mapGraphicsView(inputGraphicsView),
    widget(widget),
    pointsLayer(NULL),
    ownshipStartingMapPoint(NULL),
    currentMapPoint(NULL),
    showOwnship(true),
    followOwnship(false),
    isMapReady(false),
    drawingOverlay(NULL),
    bPoints(false),
    readyPointList(false),
    graphicId(0),
    bSelectPoints(false),
    bTiledLayerVisible(true),
    paintLayer(NULL),
    cropLandGraphicId(0),
    bSelectStartPoint(false),
    startPoint(NULL)
{
}

MapController::~MapController()
{

    qDeleteAll(pointList);
    pointList.clear();
    qDeleteAll(cropLandPointList);
    cropLandPointList.clear();
    qDebug()<<"~MapController";
}

void MapController::onMapReady()
{
    isMapReady = true;
    pointsLayer.reset(new EsriRuntimeQt::GraphicsLayer());
    map->addLayer(*pointsLayer);
    map->grid().setType(GridType::Mgrs);
    map->grid().setVisible(false);
    qDebug()<<"minScale"<<map->minScale();
    qDebug()<<"maxScale"<<map->maxScale();
    qDebug()<<"scale"<<map->scale();
    qDebug()<<"map referce"<<map->spatialReference().toJson();
    map->setScale(6010.79);
}




void MapController::handleHomeClicked()
{
    if (!ownshipStartingMapPoint->isEmpty())
    {
        handleToggleFollowMe(false); // or it will just snap right back in simulation

        map->setExtent(*originalExtent);
        map->setScale(originalScale);
        map->setRotation(0);
        map->panTo(*ownshipStartingMapPoint);
    }
}

void MapController::handleToggleShowMe(bool state)
{
    qDebug()<<"handleToggleShowMe"<<state;
    showOwnship = state;
}

void MapController::handleToggleFollowMe(bool state)
{
    qDebug()<<"handleToggleFollowMe:"<<state;
    followOwnship = state;
}

void MapController::handleZoomIn()
{
    //    map->zoom(0.5);
    map->setScale(map->scale() / 2);
    qDebug()<<"map  scaled zoomin"<<map->scale();
    if (map->scale() < 100)
    {
        if (bTiledLayerVisible)
        {
            QStringList layerNames = map->layerNames();
            if (layerNames.contains("tiledLayer"))
            {
                Layer layer = map->layer("tiledLayer");
                layer.setVisible(false);
            }
            bTiledLayerVisible = false;
        }
    }


    QMatrix matrix = mapGraphicsView->matrix();
    qDebug()<<"m11 hbox scale"<<matrix.m11();
    qDebug()<<"m21 vbox scale"<< matrix.m22();

}

void MapController::handleZoomOut()
{
    //    map->zoom(2);
    map->setScale(map->scale() *  2);
    if (map->scale() >= 100)
    {
        if (!bTiledLayerVisible)
        {
            QStringList layerNames = map->layerNames();
            if (layerNames.contains("tiledLayer"))
            {
                Layer layer = map->layer("tiledLayer");
                layer.setVisible(true);
            }
            bTiledLayerVisible = true;
        }
    }
    qDebug()<<"map  scaled zoomout"<<map->scale();
    QMatrix matrix = mapGraphicsView->matrix();
    qDebug()<<"m11 hbox scale"<<matrix.m11();
    qDebug()<<"m21 vbox scale"<< matrix.m22();
}

void MapController::handlePan(QString direction)
{
    if (followOwnship)
    {
        followOwnship = false;
        map->setRotation(0);
    }
    Envelope extent = map->extent();

    double width = extent.width();
    double height = extent.height();

    double centerX = extent.centerX();
    double centerY = extent.centerY();

    const double PAN_INCREMENT = 0.25;

    if (direction.compare("up") == 0)
        centerY += height * PAN_INCREMENT;
    else if (direction.compare("down") == 0)
        centerY -= height * PAN_INCREMENT;
    else if (direction.compare("left") == 0)
        centerX -= width * PAN_INCREMENT;
    else if (direction.compare("right") == 0)
        centerX += width * PAN_INCREMENT;

    Envelope newExtent(Point(centerX, centerY), width, height);
    map->panTo(newExtent);
}

void MapController::handleResetMap()
{
    qDebug()<<"handleResetMap()";
    handleToggleFollowMe(false); // or it will just snap right back in simulation

    map->setExtent(*originalExtent);
    map->setScale(originalScale);
    map->setRotation(0);
}

void MapController::onAvaliblePosition(double lat, double lon, double heading)
{
    //    qDebug()<<"onAvaliblePosition"<<lat<<lon<<heading;
    if (!isMapReady ||  mapGraphicsView == 0)
        return;
    Point mapPoint = GeometryEngine::project(lon, lat, map->spatialReference());
    currentMapPoint.reset(new Point(mapPoint));
    if (ownshipStartingMapPoint->isEmpty())
    {
        originalExtent.reset(new Envelope(map->extent()));
        originalScale = map->scale();
        ownshipStartingMapPoint.reset(new Point(mapPoint)); // originalExtent.center();
    }

    if (drawingOverlay == 0)
    {
        qDebug()<<"new SimpleGraphicOverlay";
        drawingOverlay.reset(new SimpleGraphicOverlay());

        QPixmap ownshipPixmap(":/Resources/icons/Ownship.png");
        QImage ownshipImage = ownshipPixmap.toImage();
        drawingOverlay->setImage(ownshipImage);
        drawingOverlay->setGraphicsView(mapGraphicsView);
    }
    drawingOverlay->setVisible(showOwnship);
    if (showOwnship)
    {
        //                qDebug()<<"on showOwnship";
        drawingOverlay->setPosition(mapPoint);
        drawingOverlay->setAngle(heading);
    }

    if (followOwnship)
    {
        //        qDebug()<<"on     followOwnShip";
        map->setRotation(heading);
        map->panTo(mapPoint);
    }
}

void MapController::handlePointsToggled(bool state)
{
    bPoints = state;
    if (!state && (graphicId != 0))
    {
        pointList.pop_back();
        pointsLayer->removeGraphic(graphicId);
    }
}

void MapController::handleToLinesClicked()
{
    qDebug()<<"handleToLinesClicked()"<< pointList.size();
    if (isMapReady)
    {
        //        pointList.append(pointList.at(0));
        if (pointList.size() <= 1)
            return;
        if (bPoints&& (graphicId != 0))
        {
            pointsLayer->removeGraphic(graphicId);
            pointList.pop_back();
        }
        if (!readyPointList)
        {
            pointList.append(pointList.first());
            readyPointList = true;
        }
        else
        {
            pointsLayer->removeAll();
        }
        QList<Point> temp;
        foreach(Point* point, pointList)
        {
            temp.append(*point);
        }
        QList<QList<EsriRuntimeQt::Point> > tmpList;
        tmpList.append(temp);
        EsriRuntimeQt::Polyline line1(tmpList);
        EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(0,0,255), 3);
        EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
        pointsLayer->addGraphic(graphic1);
        temp.clear();
        tmpList.clear();

    }
}

//void MapController::handleOkClicked()
//{
//    qDebug()<<"handleOkClicked()";
//    if (bPoints&& (graphicId != 0))
//    {
//        pointsLayer.removeGraphic(graphicId);
//        pointList.pop_back();
//        bPoints = false;
//    }
//    //    map->removeLayer("tiledLayer");
//    if (!readyPointList)
//    {
//        pointList.append(pointList.first());
//        readyPointList = true;
//    }
//    preparePaths();
//}

void MapController::handleToPolygonClicked()
{
    if (isMapReady)
    {
        if (pointList.size() <= 1)
            return;
        if (bPoints&& (graphicId != 0))
        {
            pointsLayer->removeGraphic(graphicId);
            pointList.pop_back();
        }
        if (!readyPointList)
        {
            pointList.append(pointList.first());
            readyPointList = true;
        }
        else
        {
            pointsLayer->removeAll();
        }
        QList<Point> temp;
        foreach(Point* point, pointList)
        {
            temp.append(*point);
        }
        QList<QList<EsriRuntimeQt::Point> > tmpList;
        tmpList.append(temp);
        EsriRuntimeQt::Polygon polygon(tmpList);
        pointsLayer->addGraphic(EsriRuntimeQt::Graphic(polygon, EsriRuntimeQt::SimpleFillSymbol(QColor("Green"))));
        temp.clear();
        tmpList.clear();
    }
}

void MapController::mousePress(QMouseEvent mouseEvent)
{
    if (!isMapReady)
        return;
    QPointF mousePoint = QPointF(mouseEvent.pos().x(), mouseEvent.pos().y());
    Point* mapPoint = new Point(map->toMapPoint(mousePoint.x(), mousePoint.y()));
    if (bPoints)
    {
        pointList.append(mapPoint);
        SimpleMarkerSymbol smsSymbol(Qt::red, 5, SimpleMarkerSymbolStyle::Circle);
        Graphic mouseClickGraphic(*mapPoint, smsSymbol);
        graphicId = pointsLayer->addGraphic(mouseClickGraphic);
        return;
    }
    if (bSelectPoints)
    {
        QList<qint64> hitGraphicIDs = paintLayer->graphicIds(mousePoint.x(), mousePoint.y(), 3);

        foreach( qint64 id, hitGraphicIDs)
        {
            Graphic graphic = paintLayer->graphic(id);
            Geometry geometry = graphic.geometry();
            if (Geometry::isPoint(geometry.type()))
            {
                paintLayer->select(id);
                cropLandPointList.append(new Point(geometry));
                return;
            }
        }
    }
    if (bSelectStartPoint)
    {
        QList<qint64> hitGraphicIDs = paintLayer->graphicIds(mousePoint.x(), mousePoint.y(), 3);

        foreach( qint64 id, hitGraphicIDs)
        {
            Graphic graphic = paintLayer->graphic(id);
            Geometry geometry = graphic.geometry();
            if (Geometry::isPoint(geometry.type()))
            {
                paintLayer->clearSelection();
                paintLayer->select(id);
                startPoint.reset(new Point(geometry));
                return;
            }
        }
    }
}



void MapController::onClearClicked()
{
    pointList.clear();
    pointsLayer->removeAll();
    readyPointList = false;
}

//void MapController::preparePaths(const QList<Point> &pointList)
//{
//    qDebug()<<"preparePaths-size:"<<pointList.size();

//    foreach (Point point , pointList)
//    {
//        Point temp = GeometryEngine::project(point, map->spatialReference(), WGS84);
//        qDebug()<<QString("firstP x: %1 y: %2").arg(temp.x(), 0, 'g', 14).arg(temp.y(), 0, 'g', 14);
//        wgsList.append(temp);
//    }
//    wgsList.append(wgsList.first());
//    for (int i = 1; i < wgsList.size(); ++i)
//    {
//        GeodesicDistanceResult  distance = GeometryEngine::geodesicDistanceBetweenPoints(wgsList.at(i - 1), wgsList.at(i), WGS84);
//        qDebug()<<"distance :"<<distance.distance() << "  angle:"<<distance.azimuthFrom1To2();
//        distanceList.append(distance.distance());
//        azimuthList.append(distance.azimuthFrom1To2());
//    }
//    wgsList.pop_back();
//}


void MapController::handleSelectPointsToggled(bool state)
{
    bSelectPoints = state;
    if (state)
    {
        cropLandPointList.clear();
    }
}

void MapController::handlePaintCropLandClicked()
{
    if (cropLandPointList.isEmpty())
        return;
    //    pointList.append(pointList.first());
    QList<Point> temp;
    foreach(Point* point, cropLandPointList)
    {
        temp.append(*point);
    }
    QList<QList<EsriRuntimeQt::Point> > tmpList;
    tmpList.append(temp);
    EsriRuntimeQt::Polygon polygon(tmpList);
    cropLandGraphicId = paintLayer->addGraphic(EsriRuntimeQt::Graphic(polygon, EsriRuntimeQt::SimpleFillSymbol(QColor(0, 180, 0, 200))));
    temp.clear();
    tmpList.clear();
}

void MapController::handleGetPathClicked()
{
    qDebug()<<"handleGetPathClicked()"<<cropLandPointList.size();

    if (startPoint->isEmpty())
        return;
    if (cropLandPointList.size() < 2)
        return;

    int order = cropLandPointList.indexOf(startPoint.data());
    //    qDebug()<<"start Point x"<<startPoint.x()<<" Y:"<<startPoint.y();
    qDebug()<<"order"<<order;

    if (order != -1)
    {

        Point* x = getXAxisPoint(cropLandPointList, order);
        MyCoordinate coor(startPoint.data(), x, carWidth);
        connect(&coor, SIGNAL(paintLineList(QList<Line>)), this, SLOT(onPaintLineList(QList<Line>)));
        connect(&coor, SIGNAL(paintPathList(QList<Line>)), this, SLOT(onPaintPathList(QList<Line>)));
        connect(&coor, SIGNAL(paintCornerList(QList<Line>)), this, SLOT(onPaintCornerList(QList<Line>)));
        coor.paintGrid(cropLandPointList);
    }
}

Point * MapController::getXAxisPoint(const QList<Point*>& list, int order)
{
    Point* returnPoint;
    QList<double> distanceList;

    for (int i = 1; i <= list.size(); ++i)
    {
        if (i != list.size())
        {
            GeodesicDistanceResult  distance = GeometryEngine::geodesicDistanceBetweenPoints(*list.at(i - 1), *list.at(i), map->spatialReference());
            distanceList.append(distance.distance());
        }
        else
        {
            GeodesicDistanceResult  distance = GeometryEngine::geodesicDistanceBetweenPoints(*list.at(i - 1), *list.first(), map->spatialReference());
            distanceList.append(distance.distance());
        }
    }
    double behindDistance ;
    double frontDistance ;
    if (order == 0)
    {
        behindDistance = distanceList.at(0);
        frontDistance = distanceList.back();
    }
    else if (order < list.size())
    {
        behindDistance = distanceList.at(order);
        frontDistance = distanceList.at(order -1);
    }
    if (behindDistance <= frontDistance)
    {
        if (order == 0)
        {
            returnPoint = cropLandPointList.back();
        }
        else
        {
            returnPoint = cropLandPointList.at(order - 1);
        }
    }
    else
    {
        if (order == (list.size() - 1))
        {
            returnPoint = cropLandPointList.first();
        }
        else
        {
            returnPoint = cropLandPointList.at(order + 1);
        }
    }
    distanceList.clear();
    return returnPoint;
}

void MapController::handleUnSelectClicked()
{
    paintLayer->clearSelection();
    if (cropLandGraphicId)
    {
        paintLayer->removeGraphic(cropLandGraphicId);
    }
    bSelectStartPoint = false;
    cropLandPointList.clear();
    startPoint->clear();
}

void MapController::onMouseWheel(QWheelEvent e)
{
    if (e.delta() > 0)
    {
        if (map->scale() <= 100)
        {
            if (bTiledLayerVisible)
            {
                QStringList layerNames = map->layerNames();
                if (layerNames.contains("tiledLayer"))
                {
                    Layer layer = map->layer("tiledLayer");
                    layer.setVisible(false);
                }
                bTiledLayerVisible = false;
            }
        }
    }
    else
    {
        if  (map->scale() >100)
        {
            if (!bTiledLayerVisible)
            {
                QStringList layerNames = map->layerNames();
                if (layerNames.contains("tiledLayer"))
                {
                    Layer layer = map->layer("tiledLayer");
                    layer.setVisible(true);
                }
                bTiledLayerVisible = true;
            }
        }
    }
}

void MapController::onPaintGeometry(const QList<QPointF*> &pointFList)
{
    qDebug()<<"onPaintGeometry"<<pointFList.size();
    if (pointFList.size() == 0)
        return;

    map->removeAll();
    map->grid().setVisible(true);
    paintLayer.reset(new GraphicsLayer());
    map->addLayer(*paintLayer);
    qDebug()<<"addLayer";
    QList<Point> pointList;
    foreach(QPointF* pointF, pointFList)
    {
        Point mapPoint = GeometryEngine::project(pointF->y(), pointF->x(), map->spatialReference());
        pointList.append(mapPoint);
        SimpleMarkerSymbol smsSymbol(Qt::red, 5, SimpleMarkerSymbolStyle::Circle);
        Graphic graphic(mapPoint, smsSymbol);
        paintLayer->addGraphic(graphic);
    }
    map->panTo(pointList.first());
    map->setScale(300);

    QList<QList<EsriRuntimeQt::Point> > tmpList;
    tmpList.append(pointList);
    EsriRuntimeQt::Polyline line1(tmpList);
    EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(0,0,255), 3);
    EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
    paintLayer->addGraphic(graphic1);


    QStringList mgrsList = pointListToMGRS(pointList);

    QStringList newMgrsList;
    foreach (QString str, mgrsList)
    {
        if (!newMgrsList.contains(str))
        {
            newMgrsList.append(str);
        }
    }
    qDebug()<<"newMgrsList size"<<newMgrsList.size();
    foreach(QString str, newMgrsList)
    {
        paintMgrsGrid(str);
    }
    pointList.clear();
    tmpList.clear();
    mgrsList.clear();
    newMgrsList.clear();
}

QStringList MapController::pointListToMGRS(const QList<Point> &pointList)
{
    QStringList returnMgrsList;

    if (!isMapReady) // no SR until map ready
        return returnMgrsList;

    SpatialReference sr = map->spatialReference();
    if (sr.id() < 0)
    {
        qDebug() << "FAIL: No SR in mapPointToMGRS";
        return returnMgrsList;
    }
    const MgrsConversionMode method = MgrsConversionMode::Automatic;
    const int digits = 3;
    returnMgrsList = sr.toMilitaryGrid(method, digits, false, true, pointList);

    return returnMgrsList;
}

void MapController:: paintMgrsGrid(QString & mgrs)
{
    QStringList list = mgrs.split(' ', QString::SkipEmptyParts);
    if (list.size() != 4)
        return;
    QString slon = list.at(2) + "00";
    QString slat = list.at(3) + "00";
    mgrs.replace(list.at(2), slon);
    mgrs.replace(list.at(3), slat);
    int ilon = slon.toInt();
    int ilat = slat.toInt();
    QStringList latBottomList;
    QStringList latTopList;
    QStringList lonLeftList;
    QStringList lonRightList;
    for (int i = 2; i < 100; i = i + 2)
    {
        int lon = ilon + i;
        QString newLon = QString::number(lon);
        QString lontemp = mgrs;
        lontemp.replace(slon, newLon);
        latBottomList.append(lontemp);
        int lat = ilat + i;
        QString newLat = QString::number(lat);
        QString lattemp = mgrs;
        lattemp.replace(slat, newLat);
        lonLeftList.append(lattemp);
    }
    QString rightBottomMgrs = mgrs;
    QString rightBottomLon = QString::number(slon.toInt() + 100);
    rightBottomMgrs.replace(slon, rightBottomLon);
    for (int i = 2; i < 100; i = i + 2)
    {
        int lat = ilat + i;
        QString newLat = QString::number(lat);
        QString lattemp = rightBottomMgrs;
        lattemp.replace(slat, newLat);
        lonRightList.append(lattemp);
    }
    //    qDebug()<<"rightBottomMgrs"<<rightBottomMgrs;
    QString leftTopMgrs = mgrs;
    QString leftTopLat = QString::number(slat.toInt() + 100);
    leftTopMgrs.replace(slat, leftTopLat);
    for (int i = 2; i < 100; i = i + 2)
    {
        int lon = ilon  + i;
        QString newLon = QString::number(lon);
        QString lontemp = leftTopMgrs;
        lontemp.replace(slon, newLon);
        latTopList.append(lontemp);
    }
    //    qDebug()<<"leftTopMgrs"<<leftTopMgrs;
    mgrsListToLines(latBottomList, latTopList);
    mgrsListToLines(lonLeftList, lonRightList);
}
Point MapController::MGRSToMapPoint(const QString & mgrs)
{
    Point returnPoint;

    if (!isMapReady) // no SR until map ready
        return returnPoint;
    SpatialReference sr = map->spatialReference();
    if (sr.id() < 0)
    {
        qDebug() << "FAIL: No SR in mapPointToMGRS";
        return returnPoint;
    }

    QStringList mgrsList;
    mgrsList.append(mgrs);

    QList<Point> list = sr.fromMilitaryGrid(mgrsList, MgrsConversionMode::Automatic);

    if (list.size() < 1)
        return returnPoint;
    returnPoint = list.at(0);
    return returnPoint;
}

void MapController::mgrsListToLines(const QStringList & list1,const QStringList& list2)
{
    Q_ASSERT(list1.size() == list2.size());
    int size = list1.size();
    //    qDebug()<<"size"<<size;
    QList<QList<EsriRuntimeQt::Point> > tmpList;
    for (int i = 0; i < size; ++i)
    {
        QList<Point> pointList;
        QString temp1 = list1.at(i);
        Point point1 = MGRSToMapPoint(temp1);
        pointList.append(point1);
        QString temp2 = list2.at(i);
        Point point2 = MGRSToMapPoint(temp2);
        pointList.append(point2);

        tmpList.append(pointList);

    }
    EsriRuntimeQt::Polyline line1(tmpList);
    EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(0,255,0, 50), 1);
    EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
    paintLayer->addGraphic(graphic1);
}

void MapController::handleSelectStartPointClicked()
{
    qDebug()<<"handleSelectStartPointClicked()";

    this->bSelectPoints = false;
    this->bSelectStartPoint = true;
    paintLayer->clearSelection();
}

void MapController::onPaintLineList(QList<Line*> lineList)
{
    qDebug()<<"onPaintLineList"<<lineList.size();
    QList<QList<Point> > tmpList;
    foreach (Line* line, lineList)
    {
        QList<Point> pointList;
        pointList.append(line->startXY());
        pointList.append(line->endXY());
        tmpList.append(pointList);
    }
    EsriRuntimeQt::Polyline line1(tmpList);
    EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(255,0,0, 140), 1, SimpleLineSymbolStyle::Dot);
    EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
    paintLayer->addGraphic(graphic1);
}

void MapController::onPaintPathList(QList<Line*> lineList)
{
    qDebug()<<"onPaintPathList"<<lineList.size();
    QList<QList<Point> > tmpList;
    foreach (Line* line, lineList)
    {
        QList<Point> pointList;
        pointList.append(line->startXY());
        pointList.append(line->endXY());
        tmpList.append(pointList);
        //        QList<Point> arrow =
    }
    EsriRuntimeQt::Polyline line1(tmpList);
    EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(255,0,255, 200), 1);
    EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
    paintLayer->addGraphic(graphic1);
}

void MapController::onPaintCornerList(QList<Line*> lineList)
{
    QList<QList<Point> > tmpList;
    foreach (Line* line, lineList)
    {
        QList<Point> pointList;
        pointList.append(line->startXY());
        pointList.append(line->endXY());
        tmpList.append(pointList);
        //        QList<Point> arrow =
    }
    EsriRuntimeQt::Polyline line1(tmpList);
    EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(255,0,255, 200), 1);
    EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
    paintLayer->addGraphic(graphic1);
}

void MapController::onToCroplandClicked()
{
    qDebug()<<"onTOcropLandClicked()";
    QString dirPath = QCoreApplication::applicationDirPath();
    QDir dir(dirPath);
    if (!dir.exists("Projects"))
    {
        dir.mkdir("Projects");
    }
    qDebug()<<"dirPath"<<dirPath;
    projectName = QFileDialog::getExistingDirectory(widget, tr("Open Directory"),
                                                    dirPath + "/Projects",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    qDebug()<<"projectName"<<projectName;
    if ((projectName == dirPath) || (!projectName.contains(dirPath)))
    {
        projectName = "";
    }
}


void MapController::setSimpleVisible(bool state)
{
    if (drawingOverlay)
    {
        drawingOverlay->setVisible(state);
    }
}

void MapController::handleCreateProjectClicked()
{
    qDebug()<<"handleCreateProjectClicked()";
    QString dirPath = QCoreApplication::applicationDirPath();
    QDir dir(dirPath);
    if (!dir.exists("Projects"))
    {
        dir.mkdir("Projects");
    }
    QString project = QFileDialog::getSaveFileName(widget, tr("Create Project"),
                                                    dirPath + "/Projects");
    qDebug()<<"project";
    if (!dir.exists(project))
    {
          dir.mkdir(project);
    }

}

}

