#include "mapcontroller.h"
#include <GeometryEngine.h>
#include <SimpleMarkerSymbol.h>
#include <Polyline.h>
#include <Polygon.h>
#include <SimpleFillSymbol.h>
#include <MapGraphicsView.h>
#include <Map.h>





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
                cropLandPointList.append(Point(geometry));
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
                startPoint = geometry;
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
    foreach( Point temp, cropLandPointList)
    {
        qDebug()<<QString("temp x: %1 y: %2").arg(temp.x(), 0, 'g', 14).arg(temp.y(), 0, 'g', 14);
    }

    if (order != -1)
    {

        Point x = getXAxisPoint(cropLandPointList, order);
        MyCoordinate coor(startPoint, x, carWidth);
        connect(&coor, SIGNAL(paintLineList(QList<Line>)), this, SLOT(onPaintLineList(QList<Line>)));
        coor.paintGrid(cropLandPointList);
    }
}

Point MapController::getXAxisPoint(const QList<Point>& list, int order)
{
    Point returnPoint;
    QList<double> distanceList;

    for (int i = 1; i <= list.size(); ++i)
    {
        if (i != list.size())
        {
        GeodesicDistanceResult  distance = GeometryEngine::geodesicDistanceBetweenPoints(list.at(i - 1), list.at(i), map->spatialReference());
        distanceList.append(distance.distance());
        }
        else
        {
            GeodesicDistanceResult  distance = GeometryEngine::geodesicDistanceBetweenPoints(list.at(i - 1), list.first(), map->spatialReference());
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
    return returnPoint;
}

void MapController::handleUnSelectClicked()
{
    paintLayer.clearSelection();
    if (cropLandGraphicId)
    {
        paintLayer.removeGraphic(cropLandGraphicId);
    }
    bSelectStartPoint = false;
    cropLandPointList.clear();
    startPoint.clear();
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

void MapController::onPaintLineList(QList<Line> lineList)
{
    qDebug()<<"onPaintLineList"<<lineList.size();
    QList<QList<Point> > tmpList;
    foreach (Line line, lineList)
    {
        QList<Point> pointList;
        pointList.append(line.startXY());
        pointList.append(line.endXY());
        tmpList.append(pointList);
    }
    EsriRuntimeQt::Polyline line1(tmpList);
    EsriRuntimeQt::SimpleLineSymbol lineSym1(QColor(255,0,0, 180), 1);
    EsriRuntimeQt::Graphic graphic1(line1, lineSym1);
    paintLayer.addGraphic(graphic1);
}

MyCoordinate::MyCoordinate(const Point& origin, const Point &horizontal, double gridWidth, QObject* parent)
    : origin(origin.toQPointF()),
      horizontal(horizontal.toQPointF()),
      gridWidth(gridWidth),
      QObject(parent)
{
    qDebug()<<QString("origin x: %1 y: %2").arg(origin.x(), 0, 'g', 14).arg(origin.y(), 0, 'g', 14);

    qDebug()<<QString("horizontal x: %1 y: %2").arg(horizontal.x(), 0, 'g', 14).arg(horizontal.y(), 0, 'g', 14);

    QPointF temp;
    temp.setX(horizontal.x() - origin.x());
    temp.setY(horizontal.y() - origin.y());
    qDebug()<<"temp  x"<<temp.x()<<" Y"<<temp.y();
    horAngle = atan2(temp.y() , temp.x());
    //    xAxisLine.setP1(QPointF(0,0));
    //    xAxisLine.setP2(temp);
}

void MyCoordinate::paintGrid(const QList<Point>& pointList)
{
    QList<QPointF> pointFList;
    foreach(Point point, pointList)
    {
        pointFList.append(point.toQPointF());
    }

    QList<QPointF> returnPointList = mapPointsToMyCoordinate(pointFList);
    //    this->farthestPoint = getFarthestPoint(returnPointList);
    QLineF yAxisLine = getYAxisLineFromList(this->yAxisList);
    QLineF xAxisLine = getXAxisLineFromList(this->xAxisList);
    QList<QLineF> lineList = pointListToLines(returnPointList);
    paintLines(lineList, yAxisLine, xAxisLine);

}

QList<QPointF>  MyCoordinate::mapPointsToMyCoordinate(const QList<QPointF>& pointList)
{
    QList<QPointF> returnPointList;
    foreach (QPointF point, pointList)
    {
        QPointF temp = mapPointToMyCoordinate(point);
        returnPointList.append(temp);
        qDebug()<<"Mycoordinate point x:"<<temp.x()<<" y: "<<temp.y();
    }
    return returnPointList;
}

QPointF MyCoordinate::mapPointToMyCoordinate(const QPointF& point)
{
    QPointF returnPoint;
    QPointF temp;
    temp.setX(point.x() - origin.x());
    temp.setY(point.y() - origin.y());
    double angle = atan2(temp.y(), temp.x());
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
    double y = length * sin(newAngle);
    returnPoint.setY(y);
    qDebug()<<"y "<<y;
    yAxisList.append(y);
    double x = length * cos(newAngle);
    returnPoint.setX(x);
    xAxisList.append(x);
    return returnPoint;
}





QList<QLineF> MyCoordinate::pointListToLines(const QList<QPointF> &pointList)
{
    QList<QLineF> returnLineList;
    qDebug()<<"pointList size"<<pointList.size();
    for (int i = 1; i <= pointList.size(); ++i)
    {
        QLineF line;
        line.setP1(pointList.at(i-1));
        if (i != (pointList.size()))
        {
            line.setP2(pointList.at(i));
        }
        else
            line.setP2(pointList.first());
        returnLineList.append(line);
    }
    return returnLineList;
}

void MyCoordinate::paintLines(const QList<QLineF>& lineList, const QLineF& yAxisLine, const QLineF& xAxisLine)
{
    //    QList<QPointF> returnPointList;
    qDebug()<<"getPointListFromLines size"<<lineList.size();
    qDebug()<<"yAxisLine p1 " << yAxisLine.x1() << yAxisLine.y1();
    qDebug()<<"yAxisLine p2" << yAxisLine.x2()<<yAxisLine.y2();
    qDebug()<<"xAxisLine p1"<< xAxisLine.x1() << xAxisLine.y1();
    qDebug()<<"xAxisLine p2"<< xAxisLine.x2() << xAxisLine.y2();
    QList<QPointF> yPointList;
    QList<QPointF> xPointList;
    QHash<QString, double> yMap;
    //    Mymap yMap;
    QHash<QString, double> xMap;
    foreach (QLineF line, lineList)
    {
        QList<QPointF> ylist = getYPointListFromLine(line, yAxisLine, xAxisLine);
        yPointList.append(ylist);
        QList<QPointF> xlist = getXPointListFromLine(line, yAxisLine, xAxisLine);
        xPointList.append(xlist);
    }
    QList<QLineF> tempList;
    qDebug()<<"yPointList size"<<yPointList.size();
    foreach (QPointF point, yPointList)
    {
        qDebug()<<"yMap insertMulti"<< point.y() << point.x();
        if (yMap.contains(QString::number(point.y())))
        {
            qDebug()<<"contains"<< point.y();
            double v1 = yMap.value(QString::number(point.y()));
            QPointF start(v1, point.y());
            QPointF end(point.x(), point.y());
            QLineF line(start, end);
            tempList.append(line);
            continue;
        }
        yMap.insert(QString::number(point.y()), point.x());
    }
    qDebug()<<"lineList y "<< tempList.size();
    foreach (QPointF point, xPointList)
    {
        if (xMap.contains(QString::number(point.x())))
        {
            double v1 = xMap.value(QString::number(point.x()));
            QPointF start(point.x(), v1);
            QPointF end(point.x(), point.y());
            QLineF line(start, end);
            tempList.append(line);
            continue;
        }
        xMap.insert(QString::number(point.x()), point.y());

    }

    QList<Line> drawLineList = myLinesToMapLines(tempList);
    qDebug()<<"drawLineList size"<<drawLineList.size();
    emit paintLineList(drawLineList);


    //    return returnPointList;
}

QList<QPointF> MyCoordinate::getYPointListFromLine( const QLineF &line, const QLineF& yAxisLine, const QLineF& xAxisLine)
{
    QList<QPointF> returnPointList;

    qDebug()<<"get X PointListFromLine line 1"<< line.x1() << line.y1();
    qDebug()<<"line 2"<< line.x2()<<line.y2();
    double ytime = abs(yAxisLine.y1() - yAxisLine.y2()) / gridWidth;
    //    qDebug()<<"yMax"<<yMax;
    //    double ytime = abs( line.y1() - line.y2()) / gridWidth;
    qDebug()<<"ytime"<<ytime;

    for (int i = 1; i < ytime; ++i)
    {
        double y = i * gridWidth + yAxisLine.y1();
        QLineF tempLine = xAxisLine;
        tempLine.translate(0, y);
        qDebug()<<"tempLine"<<tempLine.x1()<<tempLine.y1();
        qDebug()<<" p2"<<tempLine.x2() << tempLine.y2();
        QPointF intersectPoint;
        QLineF::IntersectType intersectType =  line.intersect(tempLine, &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
        {
            qDebug()<<"intersectPoint x"<<intersectPoint.x() << " y"<<intersectPoint.y();
            returnPointList.append(intersectPoint);
        }
    }
    qDebug()<<"resultPointList size"<<returnPointList.size();

    return returnPointList;
}

QList<QPointF> MyCoordinate::getXPointListFromLine( const QLineF &line, const QLineF& yAxisLine, const QLineF& xAxisLine)
{
    QList<QPointF> returnPointList;

    qDebug()<<"get Y PointListFromLine line 1"<< line.x1() << line.y1();
    qDebug()<<"line 2"<< line.x2()<<line.y2();
    double xtime = abs(xAxisLine.x1() - xAxisLine.x2()) / gridWidth;
    //    qDebug()<<"yMax"<<yMax;
    //    double ytime = abs( line.y1() - line.y2()) / gridWidth;
    qDebug()<<"xtime"<<xtime;

    for (int i = 1; i < xtime; ++i)
    {
        double x = i * gridWidth + xAxisLine.x1();
        QLineF tempLine = yAxisLine;
        tempLine.translate(x, 0);
        qDebug()<<"tempLine"<<tempLine.x1()<<tempLine.y1();
        qDebug()<<" p2"<<tempLine.x2() << tempLine.y2();
        QPointF intersectPoint;
        QLineF::IntersectType intersectType =  line.intersect(tempLine, &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
        {
            qDebug()<<"intersectPoint x"<<intersectPoint.x() << " y"<<intersectPoint.y();
            returnPointList.append(intersectPoint);
        }
    }
    qDebug()<<"resultPointList size"<<returnPointList.size();

    return returnPointList;
}

QLineF MyCoordinate::getYAxisLineFromList(const QList<double> & list)
{
    QLineF returnLine;
    if (list.isEmpty())
        return returnLine;
    double max = list.first();
    double min = list.first();
    foreach(double d, list)
    {
        if (d > max)
        {
            max = d;
        }
        if (d < min)
        {
            min = d;
        }
    }

    returnLine.setP1(QPointF(0, min));
    returnLine.setP2(QPointF(0, max));
    return returnLine;
}

QLineF MyCoordinate::getXAxisLineFromList(const QList<double> & list)
{
    QLineF returnLine;
    if (list.isEmpty())
        return returnLine;
    double max = list.first();
    double min = list.first();
    foreach(double d, list)
    {
        if (d > max)
        {
            max = d;
        }
        if (d < min)
        {
            min = d;
        }
    }

    returnLine.setP1(QPointF(min, 0));
    returnLine.setP2(QPointF(max, 0));
    return returnLine;
}

QList<Line> MyCoordinate::myLinesToMapLines(const QList<QLineF> &lineList)
{
    QList<Line> returnList;
    foreach( QLineF line, lineList)
    {
        QPointF pointP1 = line.p1();
        QPointF pointP2 = line.p2();
        Point p1 = myPointToMapPoint(pointP1);
        Point p2 = myPointToMapPoint(pointP2);
        Line tempLine;
        tempLine.setStart(p1);
        tempLine.setEnd(p2);
        returnList.append(tempLine);
    }
    return returnList;
}

Point MyCoordinate::myPointToMapPoint(const QPointF & point)
{
    double myAngle = atan2(point.y() , point.x());
    double angle = horAngle  + myAngle;

    Point returnpoint;
    double length = sqrt(point.x() * point.x() + point.y() * point.y());
    returnpoint.setX(length * cos(angle) + origin.x());
    returnpoint.setY(length * sin(angle) + origin.y());

    return returnpoint;
}
