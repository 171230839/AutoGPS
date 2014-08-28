#include "mapcontroller.h"
#include <GeometryEngine.h>
#include <SimpleMarkerSymbol.h>
#include <Polyline.h>
#include <Polygon.h>
#include <SimpleFillSymbol.h>
#include <MapGraphicsView.h>
#include <Map.h>
#include <Line.h>

const   double   PI   =   3.141592653589793;
static const double  carWidth = 2.0;

MapController::MapController(Map* inputMap,
                             MapGraphicsView* inputGraphicsView,
                             QObject* parent):
    map(inputMap),
    mapGraphicsView(inputGraphicsView),
    QObject(parent),
    showOwnship(true),
    followOwnship(false),
    isMapReady(false),
    drawingOverlay(0),
    bPoints(false),
    readyPointList(false),
    graphicId(0),
    bSelectPoints(false),
    bTiledLayerVisible(true),
    cropLandGraphicId(0),
    bSelectStartPoint(false)
{
}

MapController::~MapController()
{

}

void MapController::onMapReady()
{
    isMapReady = true;
    map->addLayer(pointsLayer);
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
    if (!ownshipStartingMapPoint.isEmpty())
    {
        handleToggleFollowMe(false); // or it will just snap right back in simulation

        map->setExtent(originalExtent);
        map->setScale(originalScale);
        map->setRotation(0);
        map->panTo(ownshipStartingMapPoint);
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

    map->setExtent(originalExtent);
    map->setScale(originalScale);
    map->setRotation(0);
}

void MapController::onAvaliblePosition(double lat, double lon, double heading)
{
    //    qDebug()<<"onAvaliblePosition"<<lat<<lon<<heading;
    if (!isMapReady ||  mapGraphicsView == 0)
        return;
    Point mapPoint = GeometryEngine::project(lon, lat, map->spatialReference());
    currentMapPoint = mapPoint;
    if (ownshipStartingMapPoint.isEmpty())
    {
        originalExtent = map->extent();
        originalScale = map->scale();
        ownshipStartingMapPoint = mapPoint; // originalExtent.center();
    }

    if (drawingOverlay == 0)
    {
        qDebug()<<"new SimpleGraphicOverlay";
        drawingOverlay = new SimpleGraphicOverlay();

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
        pointsLayer.removeGraphic(graphicId);
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
            pointsLayer.removeGraphic(graphicId);
            pointList.pop_back();
        }
        if (!readyPointList)
        {
            pointList.append(pointList.first());
            readyPointList = true;
        }
        else
        {
            pointsLayer.removeAll();
        }
        QList<QList<EsriRuntimeQt::Point> > tmpList;
        tmpList.append(pointList);
        EsriRuntimeQt::Polyline line1(tmpList);
        EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(0,0,255), 3);
        EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
        pointsLayer.addGraphic(graphic1);

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
            pointsLayer.removeGraphic(graphicId);
            pointList.pop_back();
        }
        if (!readyPointList)
        {
            pointList.append(pointList.first());
            readyPointList = true;
        }
        else
        {
            pointsLayer.removeAll();
        }
        QList<QList<EsriRuntimeQt::Point> > tmpList;
        tmpList.append(pointList);
        EsriRuntimeQt::Polygon polygon(tmpList);
        pointsLayer.addGraphic(EsriRuntimeQt::Graphic(polygon, EsriRuntimeQt::SimpleFillSymbol(QColor("Green"))));
    }
}

void MapController::mousePress(QMouseEvent mouseEvent)
{
    if (!isMapReady)
        return;
    QPointF mousePoint = QPointF(mouseEvent.pos().x(), mouseEvent.pos().y());
    Point mapPoint = map->toMapPoint(mousePoint.x(), mousePoint.y());
    if (bPoints)
    {

        pointList.append(mapPoint);
        SimpleMarkerSymbol smsSymbol(Qt::red, 5, SimpleMarkerSymbolStyle::Circle);
        Graphic mouseClickGraphic(mapPoint, smsSymbol);
        graphicId = pointsLayer.addGraphic(mouseClickGraphic);
        return;
    }
    if (bSelectPoints)
    {
        QList<qint64> hitGraphicIDs = paintLayer.graphicIds(mousePoint.x(), mousePoint.y(), 3);

        foreach( qint64 id, hitGraphicIDs)
        {
            Graphic graphic = paintLayer.graphic(id);
            Geometry geometry = graphic.geometry();
            if (Geometry::isPoint(geometry.type()))
            {
                paintLayer.select(id);
                startPoint = geometry;
                return;
            }
        }
    }
    if (bSelectStartPoint)
    {
        QList<qint64> hitGraphicIDs = paintLayer.graphicIds(mousePoint.x(), mousePoint.y(), 3);

        foreach( qint64 id, hitGraphicIDs)
        {
            Graphic graphic = paintLayer.graphic(id);
            Geometry geometry = graphic.geometry();
            if (Geometry::isPoint(geometry.type()))
            {
                paintLayer.clearSelection();
                paintLayer.select(id);
                return;
            }
        }
    }
}



void MapController::onClearClicked()
{
    pointList.clear();
    pointsLayer.removeAll();
    readyPointList = false;
}

void MapController::preparePaths(const QList<Point> &pointList)
{
    qDebug()<<"preparePaths-size:"<<pointList.size();

    foreach (Point point , pointList)
    {
        Point temp = GeometryEngine::project(point, map->spatialReference(), WGS84);
        qDebug()<<QString("firstP x: %1 y: %2").arg(temp.x(), 0, 'g', 14).arg(temp.y(), 0, 'g', 14);
        wgsList.append(temp);
    }
    wgsList.append(wgsList.first());
    for (int i = 1; i < wgsList.size(); ++i)
    {
        GeodesicDistanceResult  distance = GeometryEngine::geodesicDistanceBetweenPoints(wgsList.at(i - 1), wgsList.at(i), WGS84);
        qDebug()<<"distance :"<<distance.distance() << "  angle:"<<distance.azimuthFrom1To2();
        distanceList.append(distance.distance());
        azimuthList.append(distance.azimuthFrom1To2());
    }
    wgsList.pop_back();
}


void MapController::handleSelectPointsToggled(bool state)
{
    bSelectPoints = state;
}

void MapController::handlePaintCropLandClicked()
{
    QList<qint64> selectGraphicsId = paintLayer.selectionIds();
    cropLandPointList.clear();
    foreach( qint64 id, selectGraphicsId)
    {
        Graphic graphic = paintLayer.graphic(id);
        Geometry geometry = graphic.geometry();
        if (!Geometry::isPoint(geometry.type()))
            return;
        Point point = geometry;
        cropLandPointList.append(point);
    }
    if (cropLandPointList.isEmpty())
        return;
    //    pointList.append(pointList.first());
    QList<QList<EsriRuntimeQt::Point> > tmpList;
    tmpList.append(cropLandPointList);
    EsriRuntimeQt::Polygon polygon(tmpList);
    cropLandGraphicId = paintLayer.addGraphic(EsriRuntimeQt::Graphic(polygon, EsriRuntimeQt::SimpleFillSymbol(QColor(0, 180, 0, 200))));
}

void MapController::handleGetPathClicked()
{
    qDebug()<<"handleGetPathClicked()"<<cropLandPointList.size();

    if (startPoint.isEmpty())
        return;
    if (cropLandPointList.size() < 2)
        return;

    int order = cropLandPointList.indexOf(startPoint);
    qDebug()<<"start Point x"<<startPoint.x()<<" Y:"<<startPoint.y();
    qDebug()<<"order"<<order;
    if (order != -1)
    {
        preparePaths(cropLandPointList);
        getPath(order);
    }
}



void MapController::getPath(int order)
{
    if (order < 0)
        return;
    //    boolcompareDistance(order);
    int index = cropLandPointList.size();
    qDebug()<<"order:"<<order;
    qDebug()<<"index:"<<index;
    double behindDistance ;
    double frontDistance ;
    double behindAngle;
    double frontAngle;
    if (order == 0)
    {
        behindDistance = distanceList.at(0);
        behindAngle = azimuthList.at(0);
        frontDistance = distanceList.back();
        frontAngle = azimuthList.back();
    }
    else if (order <= index)
    {
        qDebug()<<"order<=";
        behindDistance = distanceList.at(order);
        behindAngle = azimuthList.at(order);
        frontDistance = distanceList.at(order -1);
        frontAngle = azimuthList.at(order - 1);
    }

    qDebug()<<"behindD"<<behindDistance;
    qDebug()<<"frontD"<<frontDistance;

    if (behindDistance <= frontDistance)
    {
//        getFrontPath(order, behindAngle, frontAngle);
        Point x;
        if (order == 0)
        {
            x = cropLandPointList.back();
        }
        else
        {
            x = cropLandPointList.at(order);
        }
        MyCoordinate coor(startPoint, x);
        QList<Point> pointList = coor.mapPointsToMyCoordinate(cropLandPointList);

    }
    else
    {
        //    getBehindPath(order, behindAngle, frontAngle);
        Point x;
        if (order == index)
        {
            x = cropLandPointList.first();
        }
        else
        {
            x = cropLandPointList.at(order + 1);
        }
        MyCoordinate coor(startPoint, x);
//        coor.paintGrid();
        QList<Point> pointList = coor.mapPointsToMyCoordinate(cropLandPointList);
        QList<Line> lineList = coor.pointListToLines(pointList);
        QList<Point> getList = coor.getPointListFromLines(lineList);
//        QList<Point> getList;
//        foreach( Line line, lineList)
//        {
//            QList<Point> tempList = coor.getPointsFromLine(line);
//            getList.append(tempList);
//        }

    }

}

void MapController::getFrontPath(int order, double frontAngle, double)
{
    qDebug()<<"getFrontPath frontAngle:"<<frontAngle;
}

void MapController::getBehindPath(int order, double behindAngle, double frontAngle)
{
    qDebug()<<"getBehindPath behindAngle: " << behindAngle<< "frontAngle"<<frontAngle;
    double angle = 0;
    if ((behindAngle * frontAngle) > 0)
    {
        angle = abs(behindAngle - frontAngle);
    }
    else
    {
        if ((abs(behindAngle) + abs(frontAngle)) > 180)
        {
            angle = abs(behindAngle) + abs(frontAngle) - 180;
        }
        else
        {
            angle = 180 - abs(behindAngle) - abs(frontAngle);
        }
    }
    qDebug()<<"angle"<<angle;
    double degree = angle * PI / 180;
    qDebug()<<"degree"<<degree;
    double l = asin(degree) * carWidth;
    qDebug()<<"l"<<l;
    double x = l * sin(frontAngle);
    double y = l * cos(frontAngle);
    Point point = cropLandPointList.at(order);
    qDebug()<<"point x"<<point.x() << " y:"<<point.y();
    Point nextPoint = cropLandPointList.at(order + 1);
    qDebug()<<"x:"<<x<<" y:"<<y;
    QList<Point> pointList;
    int times = abs((nextPoint.x() - point.x()) / x);
    for (int i = 0; i < times; ++i)
    {
        Point temp;
        temp.setX(point.x() - i * x);
        temp.setY(point.y() - i * y);
        pointList.append(temp);
    }

}

void MapController::handleUnSelectClicked()
{
    paintLayer.clearSelection();
    if (cropLandGraphicId)
    {
        paintLayer.removeGraphic(cropLandGraphicId);
    }
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

void MapController::onPaintGeometry(const QList<QPointF> &pointFList)
{
    qDebug()<<"onPaintGeometry"<<pointFList.size();
    if (pointFList.size() == 0)
        return;

    map->removeAll();
    map->grid().setVisible(true);
    map->addLayer(paintLayer);
    QList<Point> pointList;
    foreach(QPointF pointF, pointFList)
    {

        Point mapPoint = GeometryEngine::project(pointF.y(), pointF.x(), map->spatialReference());
        pointList.append(mapPoint);
        SimpleMarkerSymbol smsSymbol(Qt::red, 5, SimpleMarkerSymbolStyle::Circle);
        Graphic graphic(mapPoint, smsSymbol);
        paintLayer.addGraphic(graphic);

    }
    map->panTo(pointList.first());
    map->setScale(100);

    QList<QList<EsriRuntimeQt::Point> > tmpList;
    tmpList.append(pointList);
    EsriRuntimeQt::Polyline line1(tmpList);
    EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(0,0,255), 3);
    EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
    paintLayer.addGraphic(graphic1);

    //    Point point = pointList.at(0);
    //    qDebug()<<QString("foreach lat: %1 lon: %2").arg(point.x(), 0, 'g', 11).arg(point.y(), 0, 'g', 12);
    QStringList mgrsList = pointListToMGRS(pointList);

    QStringList newMgrsList;
    foreach (QString str, mgrsList)
    {
        if (!newMgrsList.contains(str))
        {
            newMgrsList.append(str);
        }
    }
    //    qDebug()<<"newMgrsList size"<<newMgrsList.size();
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
    QStringList mgrsList = sr.toMilitaryGrid(method, digits, false, true, pointList);

    return mgrsList;
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
    EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(0,255,0, 180), 1);
    EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
    paintLayer.addGraphic(graphic1);
}

void MapController::handleSelectStartPointClicked()
{
    qDebug()<<"handleSelectStartPointClicked()";

    this->bSelectPoints = false;
    this->bSelectStartPoint = true;
    paintLayer.clearSelection();
}

MyCoordinate::MyCoordinate(const Point& origin, const Point &horizontal)
    :origin(origin),
      horizontal(horizontal)
{

    Point temp;
    temp.setX(horizontal.x() - origin.x());
    temp.setY(horizontal.y() - origin.y());
    horAngle = atan(temp.y() / temp.x());
}

Point MyCoordinate::mapPointToMyCoordinate(const Point& point)
{
    Point returnPoint;
    Point temp;
    temp.setX(point.x() - origin.x());
    temp.setY(point.y() - origin.y());
    double angle = atan(temp.y() / temp.x());
    qDebug()<<"angle"<<angle;
    double newAngle = angle - horAngle;
    qDebug()<<"horAngle"<<horAngle;
    qDebug()<<"newAngle"<<newAngle;
//    double length = temp.calculateLength2D();
    Line line;
    line.setStart(Point(0,0));
    line.setEnd(temp);
    double length = line.calculateLength2D();
    qDebug()<<"length"<<length;
    returnPoint.setY(length * sin(newAngle));
    returnPoint.setX(length * cos(newAngle));
    return returnPoint;
}

QList<Point>  MyCoordinate::mapPointsToMyCoordinate(const QList<Point>& pointList)
{
    QList<Point> returnPointList;
    foreach (Point point, pointList)
    {
        Point temp = mapPointToMyCoordinate(point);
        returnPointList.append(temp);
    }

    return returnPointList;
}

QList<Line> MyCoordinate::pointListToLines(const QList<Point> &pointList)
{
    QList<Line> returnLineList;
    for (int i = 1; i < pointList.size(); ++i)
    {
        Line line;
        line.setStart(pointList.at(i-1));
        if (i != (pointList.size() - 1))
        {
            line.setEnd(pointList.at(i));
        }
        else
            line.setEnd(pointList.first());
       returnLineList.append(line);
    }
    return returnLineList;
}

QList<Point> MyCoordinate::getPointListFromLines(const QList<Line>& lineList)
{
    QList<Point> returnPointList;
    foreach (Line line, lineList)
    {
        QList<Point> list = getPointListFromLine(line);
        returnPointList.append(list);
    }
    return returnPointList;
}

QList<Point> MyCoordinate::getPointListFromLine(const Line & line)
{
    QList<Point> returnPointList;
    return returnPointList;
}
