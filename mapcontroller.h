#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QObject>
#include "simplegraphicoverlay.h"
#include <GraphicsLayer.h>
#include <Point.h>

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

    //    bool filterMessages(const QString& strMessage);
    //    void transmitMessages(QByteArray datagram);
    //    void setMessagesStream(const QString& stream);
    //    void setMessagesStream(QXmlStreamReader& reader);
    //    bool readMessages(QXmlStreamReader& reader);
    //    bool readMessage(QXmlStreamReader& reader);
    //    void skipUnknownElement(QXmlStreamReader& reader);
    //    QString getReaderValue(QXmlStreamReader& reader);
    //    void readToElementEnd(QXmlStreamReader& reader);
    //    void showHideMe(bool show, Point atPoint, double withHeading);



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

    void preparePaths();
    static const  int WGS84 = 4326;

    bool bSelectPoint;
    QList<Point> wgsList;
    QList<double> distanceList;
    QList<double> azimuthList;
    qint64 getSelectedGraphicId();
    void getPath(int);
    void getFrontPath(int, double);
    void getBehindPath(int, double);
    bool bTiledLayerVisible;
//    bool bTiledLayerVisibleGreater;
    GraphicsLayer paintLayer;
    QStringList pointListToMGRS(const QList<Point>&);
    void paintMgrsGrid(QString &);
    Point MGRSToMapPoint(const QString&);
//    void paintGridItem(const QList<Point>&);
    void mgrsListToLines(const QStringList&, const QStringList&);
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
    void handleOkClicked();
    void handleToPolygonClicked();
    void onClearClicked();
    void mousePress(QMouseEvent);
    void handleSelectPointToggled(bool);
    void handleGetPathClicked();
    void handleUnSelectClicked();
    void onMouseWheel(QWheelEvent);
    void onPaintGeometry(const QList<QPointF> &);
};

#endif // MAPCONTROLLER_H
