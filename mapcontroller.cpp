#include "mapcontroller.h"
#include <GeometryEngine.h>
#include <SimpleMarkerSymbol.h>
#include <Polyline.h>
#include <Polygon.h>
#include <SimpleFillSymbol.h>
#include <MapGraphicsView.h>
#include <Map.h>


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
    bSelectPoint(false),
    bTiledLayerVisible(true)
{
}

MapController::~MapController()
{

}

void MapController::onMapReady()
{
    isMapReady = true;
    map->addLayer(pointsLayer);
    qDebug()<<"minScale"<<map->minScale();
    qDebug()<<"maxScale"<<map->maxScale();
    qDebug()<<"scale"<<map->scale();

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

void MapController::handleOkClicked()
{
    qDebug()<<"handleOkClicked()";
    if (bPoints&& (graphicId != 0))
    {
        pointsLayer.removeGraphic(graphicId);
        pointList.pop_back();
        bPoints = false;
    }
    //    map->removeLayer("tiledLayer");
    if (!readyPointList)
    {
        pointList.append(pointList.first());
        readyPointList = true;
    }
    preparePaths();

}

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
    if (bSelectPoint)
    {
        QList<qint64> hitGraphicIDs = pointsLayer.graphicIds(mousePoint.x(), mousePoint.y(), 3);
        if (hitGraphicIDs.length() > 0)
        {
            pointsLayer.clearSelection();
            pointsLayer.select(hitGraphicIDs.at(0));
            return;
        }

    }

}



void MapController::onClearClicked()
{
    pointList.clear();
    pointsLayer.removeAll();
    readyPointList = false;
}

void MapController::preparePaths()
{
    qDebug()<<"preparePaths-size:"<<pointList.size();

    foreach (Point point , pointList)
    {
        Point temp = GeometryEngine::project(point, map->spatialReference(), WGS84);
        qDebug()<<QString("firstP x: %1 y: %2").arg(temp.x(), 0, 'g', 14).arg(temp.y(), 0, 'g', 14);
        wgsList.append(temp);
    }

    for (int i = 1; i < wgsList.size(); ++i)
    {
        GeodesicDistanceResult  distance = GeometryEngine::geodesicDistanceBetweenPoints(wgsList.at(i - 1), wgsList.at(i), WGS84);
        qDebug()<<"distance :"<<distance.distance() << "  angle:"<<distance.azimuthFrom1To2();
        distanceList.append(distance.distance());
        azimuthList.append(distance.azimuthFrom1To2());
    }
    pointList.pop_back();
    wgsList.pop_back();
}


void MapController::handleSelectPointToggled(bool state)
{
    bSelectPoint = state;
}

void MapController::handleGetPathClicked()
{
    qint64 graphicID = getSelectedGraphicId();
    if (graphicID != -1)
    {
        Graphic graphic = pointsLayer.graphic(graphicID);
        Geometry geometry = graphic.geometry();
        if (!Geometry::isPoint(geometry.type()))
            return;
        Point gPoint = geometry;
        int order = -1;
        for (int i = 0; i < pointList.size(); ++i)
        {
            if (gPoint == pointList.at(i))
            {
                order = i;
            }
        }
        getPath(order);
    }
}

qint64 MapController::getSelectedGraphicId()
{
    QList<qint64> hitGraphicIDs = pointsLayer.graphicIds();

    for (int i = 0; i < hitGraphicIDs.length(); i++)
    {
        if (pointsLayer.isGraphicSelected(hitGraphicIDs.at(i)))
            return hitGraphicIDs.at(i);
    }
    return -1;
}

void MapController::getPath(int order)
{
    if (order < 0)
        return;
    //    boolcompareDistance(order);
    int index = wgsList.size() - 1;
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
        getFrontPath(order, frontAngle);
    }
    else
        getBehindPath(order, behindAngle);
}

void MapController::getFrontPath(int order, double frontAngle)
{
    qDebug()<<"getFrontPath frontAngle:"<<frontAngle;
}

void MapController::getBehindPath(int order, double behindAngle)
{
    qDebug()<<"getBehindPath behindAngle: " << behindAngle;
    if ( (0 < behindAngle) && ( behindAngle <= 90))
    {
        double y = asin(behindAngle) * carWidth;
        qDebug()<<"y:"<<y;
    }

}

void MapController::handleUnSelectClicked()
{
    pointsLayer.clearSelection();
}

void MapController::onMouseWheel(QWheelEvent e)
{
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
    else if  (map->scale() >= 100)
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
