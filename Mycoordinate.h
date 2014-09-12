#ifndef MYCOORDINATE_H
#define MYCOORDINATE_H


#include <QObject>
#include <QScopedPointer>
#include <QVector>
QT_BEGIN_NAMESPACE
class QPointF;
namespace EsriRuntimeQt
{
class Point;
class Line;
}
QT_END_NAMESPACE

namespace AutoGPSNAMESPACE{

class MyCoordinate : public QObject
{
    Q_OBJECT
public:
    MyCoordinate(const EsriRuntimeQt::Point* origin, const EsriRuntimeQt::Point* horizontal, double gridWidth, QString orientation);
    ~MyCoordinate();
    void paintGrid(const QList<EsriRuntimeQt::Point*>& pointList);
private:
    QPointF* mapPointToMyCoordinate(QPointF* );
    QList<QPointF*> mapPointsToMyCoordinate(const QList<QPointF*>&);
    QList<QLineF*> pointListToLines(const QList<QPointF*> &);
    void  paintLines(const QList<QLineF*>& lineList, const QLineF &, const QLineF&);
    QList<QPointF*> getYPointListFromLine(QLineF* , const QLineF&, const QLineF&);
    QList<QPointF*> getXPointListFromLine(QLineF* , const QLineF&, const QLineF&);
    QLineF getXAxisLineFromList(const QVector<double>&);
    QLineF getYAxisLineFromList(const QVector<double>&);
    QList<EsriRuntimeQt::Line*> myLinesToMapLines(const QList<QLineF*> &lineList);
    EsriRuntimeQt::Point myPointToMapPoint(const QPointF & point);
    QList<QPointF*> getPathListFromLine(QLineF*, const QLineF& yAxisLine, const QLineF& xAxisLine);
    QList<QLineF*> getCornerListFromPathLineList(const QList<QLineF*>&, const QLineF&, const QLineF&);
//    QList<QLineF*> getCircleListFromPathLineList(const QList<QLineF*>&, const QLineF&, const QLineF&);

private:
    QScopedPointer<QPointF> origin;
    QScopedPointer<QPointF> horizontal;
    //    QLineF xAxisLine;
    double gridWidth;
    double horAngle;
    double yAxisMax;
   QVector<double> yAxisList;
   QVector<double> xAxisList;
   QString orientation;

signals:
    void paintLineList(const QList<EsriRuntimeQt::Line*>&);
    void paintPathList(const QList<EsriRuntimeQt::Line*>&);
//    void paintCornerList(QList<EsriRuntimeQt::Line*>);
};

}
#endif // COORDINATE_H
