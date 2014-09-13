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





namespace AutoGPSNAMESPACE{
const   double   PI   =   3.141592653589793;
static const double  carWidth = 2.0;

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
    bSelectStartPoint(false),
    startPoint(NULL),
    bSelectProject(false),
    projectLayer(NULL),
    pathLayer(NULL)
{
}

MapController::~MapController()
{
    if (pointList.size() > 0)
    {
        if (pointList.count(pointList.first()) > 1)
        {
            pointList.pop_front();
        }
    }
    qDeleteAll(pointList);
    //    if (paintLineList.size() > 0)
    //    {
    //        qDeleteAll(paintLineList);
    //        paintLineList.clear();
    //    }
    //    if (paintPathList.size() > 0)
    //    {
    //        qDeleteAll(paintPathList);
    //        paintPathList.clear();
    //    }
    qDeleteAll(croplandPointList);
    qDebug()<<"~MapController ok";
}

void MapController::onMapReady()
{
    isMapReady = true;
    pointsLayer.reset(new EsriRuntimeQt::GraphicsLayer());
    map->addLayer(*pointsLayer);
    projectLayer.reset(new GraphicsLayer());
    map->addLayer(*projectLayer);
    pathLayer.reset(new GraphicsLayer());
    map->addLayer(*pathLayer);

    map->grid().setType(GridType::Mgrs);
    map->grid().setVisible(false);
    qDebug()<<"minScale"<<map->minScale();
    qDebug()<<"maxScale"<<map->maxScale();
    qDebug()<<"scale"<<map->scale();
    qDebug()<<"map reference"<<map->spatialReference().toJson();
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
    if (bSelectProject)
    {
        qDebug()<<"bselectProject";

        QList<qint64> hitGraphicIDs = projectLayer->graphicIds(mousePoint.x(), mousePoint.y(), 3);
        foreach(qint64 id, hitGraphicIDs)
        {
            Graphic graphic = projectLayer->graphic(id);
            Geometry geometry = graphic.geometry();
            if (Geometry::isMultipath(geometry.type()))
            {
                //                projectLayer->clearSelection();
                //                projectLayer->select(id);

                this->projectName = projectMap.key(id);
                qDebug()<<"projectName"<<projectName;
                if (this->projectUser.isEmpty())
                    return;

                if (this->projectUser == "worker")
                {
                    emit addCroplandPanel();
                    emit processProject(projectName);
                    bSelectProject = false;
                }
                else if (this->projectUser == "player")
                {
                    qDebug()<<"player ----"<<projectName;
                    projectLayer->select(id);

                    projectList.append(projectName);
                }

                return;
            }
        }
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
                croplandPointList.append(new Point(geometry));
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
                this->bSelectStartPoint = false;
                return;
            }
        }
    }
}



void MapController::onClearClicked()
{
    qDeleteAll(pointList);
    pointList.clear();
    pointsLayer->removeAll();
    readyPointList = false;
}


void MapController::handleSelectPointsClicked()
{
        qDeleteAll(croplandPointList);
        croplandPointList.clear();
        bSelectPoints = true;
}

void MapController::handlePaintCropLandClicked()
{
    if (croplandPointList.isEmpty())
        return;
    //    pointList.append(pointList.first());
    qDebug()<<"handlePaintCroplandclicked"<<croplandPointList.size();
    paintCropland(this->croplandPointList);
}

void MapController::paintCropland(const QList<EsriRuntimeQt::Point*>& croplandPointList)
{
    QList<Point> temp;
    foreach(Point* point, croplandPointList)
    {
        temp.append(*point);
    }
    QList<QList<EsriRuntimeQt::Point> > tmpList;
    tmpList.append(temp);
    EsriRuntimeQt::Polygon polygon(tmpList);
    pathLayer->addGraphic(EsriRuntimeQt::Graphic(polygon, EsriRuntimeQt::SimpleFillSymbol(QColor(0, 180, 0, 200))));

}

void MapController::handleGetPathClicked()
{
    qDebug()<<"handleGetPathClicked()"<<croplandPointList.size();

    if (startPoint->isEmpty())
        return;
    if (croplandPointList.size() < 2)
        return;
    int order = -1;
    //    foreach(Point*point, croplandPointList)
    for (int i = 0; i < croplandPointList.size(); ++i)
    {
        Point start = * startPoint;
        Point temp = *(croplandPointList.at(i));
        if ((start.x() == temp.x())&&(start.y() == temp.y()))
        {
            order = i;
        }
    }

    qDebug()<<"order"<<order;

    if (order != -1)
    {


        Point* x = getXAxisPoint(croplandPointList, order);
//        for (int i = 0; i < croplandPointList.size(); ++i)
//        {
//            if (i < order)
//            {
//                Point* point = croplandPointList.takeAt(i);
//                croplandPointList.append(point);
//            }
//        }
        MyCoordinate coor(startPoint.data(), x, carWidth, this->orientation);
        connect(&coor, SIGNAL(paintLineList(const QList<EsriRuntimeQt::Line*>&)), this, SLOT(onPaintLineList(const QList<EsriRuntimeQt::Line*>&)));
        connect(&coor, SIGNAL(paintPathList(const QList<EsriRuntimeQt::Line*>&)), this, SLOT(onPaintPathList(const QList<EsriRuntimeQt::Line*>&)));
        //        connect(&coor, SIGNAL(paintCornerList(QList<EsriRuntimeQt::Line*>)), this, SLOT(onPaintCornerList(QList<EsriRuntimeQt::Line*>)));
        coor.paintGrid(croplandPointList);
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
        this->orientation = "front";
        if (order == 0)
        {
            returnPoint = croplandPointList.back();
        }
        else
        {
            returnPoint = croplandPointList.at(order - 1);
        }
    }
    else
    {
        this->orientation = "behind";
        if (order == (list.size() - 1))
        {
            returnPoint = croplandPointList.first();
        }
        else
        {
            returnPoint = croplandPointList.at(order + 1);
        }
    }
    distanceList.clear();
    return returnPoint;
}

void MapController::handleUnSelectClicked()
{
    paintLayer->clearSelection();
    //    if (cropLandGraphicId)
    //    {
    //        paintLayer->removeGraphic(cropLandGraphicId);
    //    }
    pathLayer->removeAll();

    bSelectStartPoint = false;
    qDeleteAll(croplandPointList);
    croplandPointList.clear();
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
    pathLayer.reset(new GraphicsLayer());
    map->addLayer(*pathLayer);
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

void MapController::onPaintLineList(const QList<Line*> &lineList)
{
    qDebug()<<"onPaintLineList"<<lineList.size();
    //    if (paintLineList.size() > 0)
    //    {
    //        qDeleteAll(paintLineList);
    //        paintLineList.clear();
    //    }
    //    paintLineList.append(lineList);
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
    pathLayer->addGraphic(graphic1);
    //    qDeleteAll(lineList);
    qDebug()<<"qDelete all ";
}

void MapController::onPaintPathList(const QList<Line*> &lineList)
{
    qDebug()<<"onPaintPathList"<<lineList.size();
    //    if (paintPathList.size() > 0)
    //    {
    //        qDeleteAll(paintPathList);
    //        paintPathList.clear();
    //    }
    //    paintPathList.append(lineList);
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
    pathLayer->addGraphic(graphic1);
    //    qDeleteAll(lineList);
    qDebug()<<"onPaintPathList over ";
}

//void MapController::onPaintCornerList(QList<Line*> lineList)
//{
//    QList<QList<Point> > tmpList;
//    foreach (Line* line, lineList)
//    {
//        QList<Point> pointList;
//        pointList.append(line->startXY());
//        pointList.append(line->endXY());
//        tmpList.append(pointList);
//    }
//    EsriRuntimeQt::Polyline line1(tmpList);
//    EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(255,0,255, 200), 1);
//    EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
//    paintLayer->addGraphic(graphic1);
//}




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
    projectPath = QFileDialog::getSaveFileName(widget, tr("Create Project"),
                                               dirPath + "/Projects");
    qDebug()<<"project"<<projectPath;
    if ((projectPath == dirPath) || (!projectPath.contains(dirPath)))
    {
        emit error(QVariant::fromValue(tr("Warning unable to create project with path %1")
                                       .arg(projectPath)));
        projectPath = "";
        return;
    }
    if (!dir.exists(projectPath))
    {
        dir.mkdir(projectPath);
    }
}

void MapController::handleSaveProjectClicked()
{
    if (projectPath.isEmpty())
        return;
    QString filePath = projectPath + "/geometry.xml";
    QByteArray logData;
    QXmlStreamWriter logXml(&logData);
    logXml.setAutoFormatting(true);
    logXml.writeStartDocument();
    logXml.writeStartElement("Geometrys");
    pointList.pop_back();
    foreach(Point* point, pointList)
    {
        logXml.writeStartElement("Point");
        logXml.writeAttribute("x", QString("%1").arg(point->x(), 0, 'g', 11));
        logXml.writeAttribute("y", QString("%1").arg(point->y(), 0, 'g', 11));
        logXml.writeEndElement();
    }
    logXml.writeEndElement();
    logXml.writeEndDocument();
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"file cannot open"<< filePath;
        emit error(QVariant::fromValue(tr("Can't open xml file %1")
                                       .arg(filePath)));
        return;
    }
    file.write(logData);
    file.close();

}

void MapController::onPaintProject(const QList<Point*> &pointList, QString projectName, QString user)
{
    qDebug()<<"onPaintProject"<<pointList.size();
    this->bSelectProject = true;
    this->projectUser = user;
    QList<Point> tempList;
    foreach(Point * point, pointList)
    {
        tempList.append(Point(point->x(), point->y()));
    }

    QList<QList<EsriRuntimeQt::Point> > tmpList;
    tmpList.append(tempList);
    EsriRuntimeQt::Polygon polygon(tmpList);
    qint64 id = projectLayer->addGraphic(EsriRuntimeQt::Graphic(polygon, EsriRuntimeQt::SimpleFillSymbol(QColor("Green"))));
    projectMap.insert(projectName, id);

    //    qDeleteAll(pointList);
}

void MapController::handlePathSaveProjectClicked()
{
    qDebug()<<"handlePathSaveProjectClicked())";
    QString dirPath = QCoreApplication::applicationDirPath();
    QString str = dirPath  + "/Projects/" + projectName;
    qDebug()<<"str "<<str;
    QDir dir(str);
    if (!dir.exists())
    {
        return;
    }
    QString fileName = dir.absoluteFilePath("path.xml");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        emit error(QVariant::fromValue(tr("Can't open xml file %1")
                                       .arg(fileName)));
        return;
    }
    QByteArray array;
    QXmlStreamWriter writer(&array);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    //    writer.writeStartElement("Start");
    writer.writeStartElement("CroplandPoints");
    foreach(Point* point, croplandPointList)
    {
        writer.writeStartElement("Point");
        writer.writeAttribute("x", QString("%1").arg(point->x(), 0, 'g', 11));
        writer.writeAttribute("y", QString("%1").arg(point->y(), 0, 'g', 11));
        writer.writeEndElement();
    }
    writer.writeEndElement();
    //    writer.writeStartElement("PaintLines");
    //    foreach(Line* line, paintLineList)
    //    {
    //        writer.writeStartElement("Line");
    //        Point start = line->startXY();
    //        Point end = line->endXY();
    //        writer.writeStartElement("StartPoint");
    //        writer.writeAttribute("x", QString("%1").arg(start.x(), 0, 'g', 11));
    //        writer.writeAttribute("y", QString("%1").arg(start.y(), 0, 'g', 11));
    //        writer.writeEndElement();
    //        writer.writeStartElement("EndPoint");
    //        writer.writeAttribute("x", QString("%1").arg(end.x(), 0, 'g', 11));
    //        writer.writeAttribute("y", QString("%1").arg(end.y(), 0, 'g', 11));
    //        writer.writeEndElement();
    //        writer.writeEndElement();
    //    }
    //    writer.writeEndElement();
    //    writer.writeStartElement("PaintPaths");
    //    foreach(Line* line, paintPathList)
    //    {
    //        writer.writeStartElement("Path");
    //        Point start = line->startXY();
    //        Point end = line->endXY();
    //        writer.writeStartElement("StartPoint");
    //        writer.writeAttribute("x", QString("%1").arg(start.x(), 0, 'g', 11));
    //        writer.writeAttribute("y", QString("%1").arg(start.y(), 0, 'g', 11));
    //        writer.writeEndElement();
    //        writer.writeStartElement("EndPoint");
    //        writer.writeAttribute("x", QString("%1").arg(end.x(), 0, 'g', 11));
    //        writer.writeAttribute("y", QString("%1").arg(end.y(), 0, 'g', 11));
    //        writer.writeEndElement();
    //        writer.writeEndElement();
    //    }
    //    writer.writeEndElement();
    //    writer.writeEndElement();
    writer.writeEndDocument();

    //    qDeleteAll(paintLineList);
    //    paintLineList.clear();
    //    qDeleteAll(paintPathList);
    //    paintPathList.clear();
    file.write(array);
    file.close();

}


void MapController::readAndPaintPathXMLFile(QString projectName)
{
    qDebug()<<"readAndPaintPathXmlfile"<<projectName;


    QString dirPath = QCoreApplication::applicationDirPath();
    QString str = dirPath + "/Projects/" + projectName;
    qDebug()<<"str "<<str;
    QDir dir(str);
    if (!dir.exists())
    {
        return;
    }
    QString fileName = dir.absoluteFilePath("path.xml");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error(QVariant::fromValue(tr("Can't open xml file %1")
                                       .arg(fileName)));
        return;
    }
    QByteArray array = file.readAll();
    QXmlStreamReader reader(array);
    QList<Point*> croplandPointList;
    //    QList<EsriRuntimeQt::Line*> paintLineList;
    //    QList<EsriRuntimeQt::Line*> paintPathList;
    qDebug()<<"start read";
    while (!reader.atEnd() && !reader.hasError())
    {
        reader.readNext();
        if (reader.isStartElement())
        {
            if (reader.name().compare("Point") == 0)
            {
                QXmlStreamAttributes attrs = reader.attributes();
                double x = attrs.value("x").toString().toDouble();
                double y = attrs.value("y").toString().toDouble();
                Point *point = new Point(x, y);
                croplandPointList.append(point);
            }
            //            else if (reader.name().compare("Line") == 0)
            //            {
            //                Line *line = new Line();
            //                while (!(reader.isEndElement() && (reader.name().compare("Line") == 0)))
            //                {
            //                    reader.readNext();
            //                    if (reader.isStartElement())
            //                    {
            //                        if (reader.name().compare("StartPoint"))
            //                        {
            //                            QXmlStreamAttributes attrs = reader.attributes();
            //                            double x = attrs.value("x").toString().toDouble();
            //                            double y = attrs.value("y").toString().toDouble();
            //                            line->setStartXY(x , y);
            //                        }
            //                        else if (reader.name().compare("EndPoint"))
            //                        {
            //                            QXmlStreamAttributes attrs = reader.attributes();
            //                            double x = attrs.value("x").toString().toDouble();
            //                            double y = attrs.value("y").toString().toDouble();
            //                            line->setEnd(Point(x, y));
            //                        }
            //                    }

            //                }
            //                paintLineList.append(line);
            //            }
            //            else if (reader.name().compare("Path") == 0)
            //            {
            //                Line *line = new Line();
            //                while (!(reader.isEndElement() && (reader.name().compare("Path") == 0)))
            //                {
            //                    reader.readNext();
            //                    if (reader.isStartElement())
            //                    {
            //                        if (reader.name().compare("StartPoint"))
            //                        {
            //                            QXmlStreamAttributes attrs = reader.attributes();
            //                            double x = attrs.value("x").toString().toDouble();
            //                            double y = attrs.value("y").toString().toDouble();
            //                            line->setStartXY(x , y);
            //                        }
            //                        else if (reader.name().compare("EndPoint"))
            //                        {
            //                            QXmlStreamAttributes attrs = reader.attributes();
            //                            double x = attrs.value("x").toString().toDouble();
            //                            double y = attrs.value("y").toString().toDouble();
            //                            line->setEnd(Point(x, y));
            //                        }
            //                    }

            //                }
            //                paintPathList.append(line);
            //            }
        }
    }
    if (croplandPointList.size() == 0)
        return;
    qDebug()<<"croplandPointlIst size"<< croplandPointList.size();
    //    qDebug()<<"paintLineList size"<<paintLineList.size();
    //    qDebug()<<"paintPathList.szie"<<paintPathList.size();
    map->panTo(*croplandPointList.first());
    map->setScale(300);

    this->paintCropland(croplandPointList);
    //    this->onPaintLineList(paintLineList);
    //    this->onPaintPathList(paintPathList);

    qDeleteAll(croplandPointList);
    croplandPointList.clear();
    //    qDeleteAll(paintPathList);
    //    paintPathList.clear();
    //    qDeleteAll(paintLineList);
    //    paintLineList.clear();
    file.close();
}

void MapController::onGetCroplandsClicked()
{
    if (projectList.size()  < 1)
        return;
    qDebug()<<"onGetCroplandsCLicked"<<projectList.size();
    map->removeAll();
    map->grid().setVisible(true);
    pathLayer.reset(new GraphicsLayer());
    map->addLayer(*pathLayer);

    foreach(QString pro, projectList)
    {
        readAndPaintPathXMLFile(pro);
    }

    projectList.clear();
    this->bSelectProject = false;
}


}

