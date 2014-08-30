#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>
#include "simplegraphicoverlay.h"
#include <GraphicsLayer.h>
#include <Point.h>

#include <Line.h>
using namespace EsriRuntimeQt;
namespace EsriRuntimeQt
{
class Point;
class MapGraphicsView;
class Map;
class GraphicsLayer;

}
class MapController : public QObject
{
    Q_OBJECT
public:
    explicit MapController(Map* inputMap, MapGraphicsView *inputGraphicsView, QObject* parent = 0);
    ~MapController();

    SimpleGraphicOverlay*  getSimpleGraphic() { return this->drawingOverlay;}
private:
    Map* map;
    MapGraphicsView* mapGraphicsView;

    GraphicsLayer  pointsLayer;
    Point ownshipStartingMapPoint;
    Point currentMapPoint;
    bool showOwnship, followOwnship;
    bool isMapReady;

    SimpleGraphicOverlay* drawingOverlay;

    double originalScale;
    Envelope originalExtent;

    bool bPoints;
    QList<Point> pointList;
    bool readyPointList;

    qint64 graphicId;

    void preparePaths(const QList<Point>&);
    static const  int WGS84 = 4326;

    bool bSelectPoints;
    QList<Point> wgsList;
    QList<double> distanceList;
    QList<double> azimuthList;
//    qint64 getSelectedGraphicId();
    void getPath(int);
    void getFrontPath(int, double, double);
    void getBehindPath(int, double, double);
    bool bTiledLayerVisible;
//    bool bTiledLayerVisibleGreater;
    GraphicsLayer paintLayer;
    QStringList pointListToMGRS(const QList<Point>&);
    void paintMgrsGrid(QString &);
    Point MGRSToMapPoint(const QString&);
//    void paintGridItem(const QList<Point>&);
    void mgrsListToLines(const QStringList&, const QStringList&);
    qint64 cropLandGraphicId;
    bool bSelectStartPoint;
    QList<Point> cropLandPointList;
    Point startPoint;

signals:
    void headingChanged(QVariant newHeading);
    void positionChanged(QVariant newPosition);
    void speedChanged(QVariant newSpeed);
public slots:
    void onMapReady();

    void handleHomeClicked();
    void handlePan(QString direction);
    //    void handlePositionAvailable(QPointF pos, double orientation);

    void handleToggleShowMe(bool state);
    void handleToggleFollowMe(bool state);
    void handleZoomIn();
    void handleZoomOut();
    void handleResetMap();


    void onAvaliblePosition(double, double, double);
    void handlePointsToggled(bool);
    void handleToLinesClicked();
//    void handleOkClicked();
    void handleToPolygonClicked();
    void onClearClicked();
    void mousePress(QMouseEvent);
    void handleSelectPointsToggled(bool);
    void handleGetPathClicked();
    void handlePaintCropLandClicked();
    void handleUnSelectClicked();
    void onMouseWheel(QWheelEvent);
    void onPaintGeometry(const QList<QPointF> &);
    void handleSelectStartPointClicked();
    void onPaintLineList(QList<Line>);
};

class MyCoordinate : public QObject
{
    Q_OBJECT
public:
    MyCoordinate(const Point& origin, const Point& horizontal, double gridWidth, QObject * parent = 0);

    void paintGrid(const QList<Point>& pointList);
private:
    QPointF mapPointToMyCoordinate(const QPointF& );
    QList<QPointF> mapPointsToMyCoordinate(const QList<QPointF>&);
     QList<QLineF> pointListToLines(const QList<QPointF> &);
    void  paintLines(const QList<QLineF>& lineList, const QLineF &, const QLineF&);
    QList<QPointF> getYPointListFromLine(const QLineF& , const QLineF&, const QLineF&);
    QList<QPointF> getXPointListFromLine(const QLineF& , const QLineF&, const QLineF&);
    QLineF getXAxisLineFromList(const QList<double> &);
    QLineF getYAxisLineFromList(const QList<double>&);
    QList<Line> myLinesToMapLines(const QList<QLineF> &lineList);
    Point myPointToMapPoint(const QPointF & point);
private:
    QPointF origin;
    QPointF horizontal;
//    QLineF xAxisLine;
    double gridWidth;
   double horAngle;
   double yAxisMax;
   QList<double> yAxisList;
   QList<double> xAxisList;
signals:
    void paintLineList(QList<Line>);
};

#endif // MAPCONTROLLER_H
