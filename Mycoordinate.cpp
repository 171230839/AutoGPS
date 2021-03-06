#include "mycoordinate.h"
#include <QDebug>
#include <QLineF>
//using namespace EsriRuntimeQt;
#include <Point.h>
#include <Line.h>
#include <QPointF>

namespace  AutoGPSNAMESPACE {

MyCoordinate::MyCoordinate(const EsriRuntimeQt::Point* origin, const EsriRuntimeQt::Point* horizontal, double gridWidth, QString orientation)
    : origin(new QPointF(origin->toQPointF())),
      horizontal(new QPointF(horizontal->toQPointF())),
      gridWidth(gridWidth),
      orientation(orientation)
{
    qDebug()<<QString("origin x: %1 y: %2").arg(origin->x(), 0, 'g', 14).arg(origin->y(), 0, 'g', 14);

    qDebug()<<QString("horizontal x: %1 y: %2").arg(horizontal->x(), 0, 'g', 14).arg(horizontal->y(), 0, 'g', 14);

    QPointF temp;
    temp.setX(horizontal->x() - origin->x());
    temp.setY(horizontal->y() - origin->y());
    qDebug()<<"temp  x"<<temp.x()<<" Y"<<temp.y();
    horAngle = atan2(temp.y() , temp.x());
    //    xAxisLine.setP1(QPointF(0,0));
    //    xAxisLine.setP2(temp);
}
MyCoordinate::~MyCoordinate()
{

    yAxisList.clear();
    xAxisList.clear();
    qDebug()<<"~MyCoordinate()";
}

void MyCoordinate::paintGrid(const QList<EsriRuntimeQt::Point*>& pointList)
{
    qDebug()<<"paintGrid"<<pointList.size();
    QList<QPointF*> pointFList;
    foreach(EsriRuntimeQt::Point* point, pointList)
    {
        qDebug()<<"point x"<< point->x() << " y " << point->y();
        QPointF* pointF = new QPointF(point->toQPointF());
        pointFList.append(pointF);
    }

    QList<QPointF*> returnPointList = mapPointsToMyCoordinate(pointFList);

    QLineF yAxisLine = getYAxisLineFromList(this->yAxisList);
    QLineF xAxisLine = getXAxisLineFromList(this->xAxisList);

    QList<QLineF*> lineList = pointListToLines(returnPointList);
        qDebug()<<" lineList size"<<lineList.size();
    paintLines(lineList, yAxisLine, xAxisLine);

    qDeleteAll(pointFList);
    qDeleteAll(returnPointList);
    qDeleteAll(lineList);
}

QList<QPointF*>  MyCoordinate::mapPointsToMyCoordinate(const QList<QPointF*>& pointList)
{
    QList<QPointF*> returnPointList;
    foreach (QPointF* point, pointList)
    {
        QPointF* temp = mapPointToMyCoordinate(point);
        returnPointList.append(temp);
        qDebug()<<"Mycoordinate point x:"<<temp->x()<<" y: "<<temp->y();
    }
    return returnPointList;
}

QPointF* MyCoordinate::mapPointToMyCoordinate( QPointF* point)
{
    qDebug()<<" map point to my Coordinate" << point->x()<< point->y();
    QPointF* returnPoint = new QPointF();
    QPointF temp;
    temp.setX(point->x() - origin->x());
    temp.setY(point->y() - origin->y());
    double angle = atan2(temp.y(), temp.x());
    qDebug()<<"angle"<<angle;
    double newAngle = angle - horAngle;

    qDebug()<<"horAngle"<<horAngle;
    qDebug()<<"newAngle"<<newAngle;
    //    double length = temp.calculateLength2D();
    EsriRuntimeQt::Line line;
    line.setStart(EsriRuntimeQt::Point(0,0));
    line.setEnd(temp);
    double length = line.calculateLength2D();
    qDebug()<<"length"<<length;
    double y = length * sin(newAngle);
    returnPoint->setY(y);
    qDebug()<<"y "<<y;

    this->yAxisList.append(y);
    double x = length * cos(newAngle);
    returnPoint->setX(x);
    this->xAxisList.append(x);
    return returnPoint;
}





QList<QLineF*> MyCoordinate::pointListToLines(const QList<QPointF*> &pointList)
{
    QList<QLineF*> returnLineList;
    qDebug()<<"pointList size"<<pointList.size();
    for (int i = 1; i <= pointList.size(); ++i)
    {
        QLineF* line = new QLineF();
        line->setP1(*pointList.at(i-1));
        if (i != (pointList.size()))
        {
            line->setP2(*pointList.at(i));
        }
        else
            line->setP2(*pointList.first());
        returnLineList.append(line);
    }
    return returnLineList;
}

bool pathLineListSort(const QLineF* line1, const QLineF* line2)
{
    double d1 = abs(line1->p1().y());
    double d2 = abs(line2->p1().y());
    return d1 < d2;
}

void MyCoordinate::paintLines(const QList<QLineF*>& lineList, const QLineF& yAxisLine, const QLineF& xAxisLine)
{
    //    QList<QPointF> returnPointList;
    qDebug()<<"getPointListFromLines size"<<lineList.size();
    qDebug()<<"yAxisLine p1 " << yAxisLine.x1() << yAxisLine.y1();
    qDebug()<<"yAxisLine p2" << yAxisLine.x2()<<yAxisLine.y2();
    qDebug()<<"xAxisLine p1"<< xAxisLine.x1() << xAxisLine.y1();
    qDebug()<<"xAxisLine p2"<< xAxisLine.x2() << xAxisLine.y2();
    QLineF * firstLine = lineList.first();
    qDebug()<<"firstLine p1 " << firstLine->p1().x()<<firstLine->p1().y();
    qDebug()<<"firstLine p2" << firstLine->p2().x() << firstLine->p2().y();

    QList<QPointF*> yPointList;
    QList<QPointF*> xPointList;
    QHash<QString, double> yMap;
    //    Mymap yMap;
    QHash<QString, double> xMap;
    foreach (QLineF* line, lineList)
    {
        QList<QPointF*> ylist = getYPointListFromLine(line, yAxisLine, xAxisLine);
        yPointList.append(ylist);
        QList<QPointF*> xlist = getXPointListFromLine(line, yAxisLine, xAxisLine);
        xPointList.append(xlist);

    }
    QList<QLineF*> tempList;
    qDebug()<<"yPointList size"<<yPointList.size();
    foreach (QPointF* point, yPointList)
    {
        //        qDebug()<<"yMap insertMulti"<< point.y() << point.x();
        if (yMap.contains(QString::number(point->y())))
        {
            //            qDebug()<<"contains"<< point.y();
            double v1 = yMap.value(QString::number(point->y()));
            QPointF start(point->x(), point->y());
            QPointF end(v1, point->y());
            QLineF * line = new QLineF(start, end);
            tempList.append(line);
            continue;
        }
        yMap.insert(QString::number(point->y()), point->x());
    }
    qDebug()<<"lineList y "<< tempList.size();
    foreach (QPointF* point, xPointList)
    {
//                  qDebug()<<"xMap insertMulti"<< point->x() << point->y();
        if (xMap.contains(QString::number(point->x())))
        {
            double v1 = xMap.value(QString::number(point->x()));
            QPointF start(point->x(), v1);
            QPointF end(point->x(), point->y());

            QLineF *line = new QLineF(start, end);
            tempList.append(line);
            continue;
        }
        xMap.insert(QString::number(point->x()), point->y());

    }
    qDebug()<<"tempList size()"<<tempList.size();
    QList<EsriRuntimeQt::Line*> drawLineList = myLinesToMapLines(tempList);
    qDebug()<<"drawLineList size"<<drawLineList.size();
    emit paintLineList(drawLineList);


    ///////////// paint path
    QList<QPointF*> pathList;
    foreach (QLineF *line, lineList)
    {
        QList<QPointF*> ylist = getPathListFromLine(line, yAxisLine, xAxisLine);
        pathList.append(ylist);
    }
    QHash<QString, double> pathMap;
    QList<QLineF*> pathLineList;
    foreach (QPointF* point, pathList)
    {
        //        qDebug()<<"yMap insertMulti"<< point.y() << point.x();
        if (pathMap.contains(QString::number(point->y())))
        {
            //            qDebug()<<"contains"<< point.y();
            double v1 = pathMap.value(QString::number(point->y()));
            //            QPointF start(v1, point.y());
            //            QPointF end(point.x(), point.y());
            QPointF start(point->x(), point->y());
            QPointF end(v1, point->y());
            qDebug()<<"orientation--------"<<this->orientation;
            if (this->orientation == "behind")
            {
                QLineF *line = new QLineF(start, end);
                pathLineList.append(line);
            }
            else
            {
                QLineF *line = new QLineF(end, start);
                pathLineList.append(line);
            }

            continue;
        }
        pathMap.insert(QString::number(point->y()), point->x());
    }

    std::sort(pathLineList.begin(), pathLineList.end(), pathLineListSort);


    QList<QLineF*> cornerList = getCornerListFromPathLineList(pathLineList, yAxisLine, xAxisLine);
    pathLineList.append(cornerList);
    QList<EsriRuntimeQt::Line*> drawPathList = myLinesToMapLines(pathLineList);
    emit paintPathList(drawPathList);
    //无聊的时候再画圆吧。
    //    QList<QLineF*> circleList = getCircleListFromPathLineList(pathLineList, yAxisLine, xAxisLine);

    qDebug()<<" qdelete all start";
    qDeleteAll(yPointList);
    qDeleteAll(xPointList);
    qDeleteAll(tempList);
        qDeleteAll(drawLineList);
    qDeleteAll(pathList);
    qDeleteAll(pathLineList);
//        qDeleteAll(drawPathList);
}



//void MyCoordinate::pathLineBubbleSort(QList<QLineF*> pathLineList)
//{
//    QLineF * temp;
//    int size = pathLineList.size();
//    for (int i = 1; i < size; ++i)
//    {
//        for (int j = size - 1; j >= i; --j)
//        {
//            QLineF* line1 = pathLineList.at(j);
//            QLineF* line2 = pathLineList.at(j - 1);
//            if (line1->p1().y() < line2->p1().y())
//            {

//            }
//        }
//    }
//}

QList<QPointF*> MyCoordinate::getYPointListFromLine( QLineF *line, const QLineF& yAxisLine, const QLineF& xAxisLine)
{
    QList<QPointF*> returnPointList;

    //    qDebug()<<"get X PointListFromLine line 1"<< line.x1() << line.y1();
    //    qDebug()<<"line 2"<< line.x2()<<line.y2();
    double ytime = abs(yAxisLine.y1() - yAxisLine.y2()) / gridWidth;
    for (int i = 1; i < ytime; ++i)
    {
        //        double y = i * gridWidth + yAxisLine.y1();
        double y;
        if (yAxisLine.y2() > 0)
            y = i * gridWidth;
        else
            y = - i * gridWidth;
        QLineF tempLine = xAxisLine;
        tempLine.translate(0, y);
        //        qDebug()<<"tempLine"<<tempLine.x1()<<tempLine.y1();
        //        qDebug()<<" p2"<<tempLine.x2() << tempLine.y2();
        QPointF *intersectPoint = new QPointF();
        QLineF::IntersectType intersectType =  line->intersect(tempLine, intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
        {
            //            qDebug()<<"intersectPoint x"<<intersectPoint.x() << " y"<<intersectPoint.y();
            returnPointList.append(intersectPoint);
        }
    }
    qDebug()<<"resultPointList size"<<returnPointList.size();
    return returnPointList;
}

QList<QPointF*> MyCoordinate::getXPointListFromLine(QLineF *line, const QLineF& yAxisLine, const QLineF& xAxisLine)
{
    QList<QPointF*> returnPointList;

    //    qDebug()<<"get Y PointListFromLine line 1"<< line.x1() << line.y1();
    //    qDebug()<<"line 2"<< line.x2()<<line.y2();

    if (xAxisLine.x1() < -2)
    {
        double xtime1 = -xAxisLine.x1() / gridWidth;
        for (int i = 1; i < xtime1; ++i)
        {
            double x = -i * gridWidth;
            QLineF tempLine = yAxisLine;
            tempLine.translate(x, 0);
            //            qDebug()<<"tempLine"<<tempLine.x1()<<tempLine.y1();
            //            qDebug()<<" p2"<<tempLine.x2() << tempLine.y2();
            QPointF *intersectPoint = new QPointF();
            QLineF::IntersectType intersectType =  line->intersect(tempLine, intersectPoint);
            if (intersectType == QLineF::BoundedIntersection)
            {
                //                qDebug()<<"intersectPoint x"<<intersectPoint.x() << " y"<<intersectPoint.y();
                returnPointList.append(intersectPoint);
            }
        }
    }
    double xtime2 = xAxisLine.x2() / gridWidth;
    for (int i = 0; i < xtime2; ++i)
    {
        double x = i * gridWidth;
        QLineF tempLine = yAxisLine;
        tempLine.translate(x, 0);
        //        qDebug()<<"tempLine"<<tempLine.x1()<<tempLine.y1();
        //        qDebug()<<" p2"<<tempLine.x2() << tempLine.y2();
        QPointF *intersectPoint = new QPointF();
        QLineF::IntersectType intersectType =  line->intersect(tempLine, intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
        {
            //            qDebug()<<"intersectPoint x"<<intersectPoint.x() << " y"<<intersectPoint.y();
            returnPointList.append(intersectPoint);
        }
    }

    qDebug()<<"resultPointList size"<<returnPointList.size();

    return returnPointList;
}

QLineF MyCoordinate::getYAxisLineFromList(const QVector<double> & list)
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

QLineF MyCoordinate::getXAxisLineFromList(const QVector<double>  &list)
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

QList<EsriRuntimeQt::Line*> MyCoordinate::myLinesToMapLines(const QList<QLineF*> &lineList)
{
    QList<EsriRuntimeQt::Line*> returnList;
    foreach( QLineF* line, lineList)
    {
        QPointF pointP1 = line->p1();
        QPointF pointP2 = line->p2();
        EsriRuntimeQt::Point p1 = myPointToMapPoint(pointP1);
        EsriRuntimeQt::Point p2 = myPointToMapPoint(pointP2);
        EsriRuntimeQt::Line * tempLine = new EsriRuntimeQt::Line();
        tempLine->setStart(p1);
        tempLine->setEnd(p2);
        returnList.append(tempLine);
    }
    return returnList;
}

EsriRuntimeQt::Point MyCoordinate::myPointToMapPoint(const QPointF & point)
{
    double myAngle = atan2(point.y() , point.x());
    double angle = horAngle  + myAngle;

    EsriRuntimeQt::Point returnpoint;
    double length = sqrt(point.x() * point.x() + point.y() * point.y());
    returnpoint.setX(length * cos(angle) + origin->x());
    returnpoint.setY(length * sin(angle) + origin->y());

    return returnpoint;
}
QList<QPointF*> MyCoordinate::getPathListFromLine(QLineF *line, const QLineF& yAxisLine, const QLineF& xAxisLine)
{
    QList<QPointF*> returnPointList;

    //    qDebug()<<"get X PointListFromLine line 1"<< line.x1() << line.y1();
    //    qDebug()<<"line 2"<< line.x2()<<line.y2();
    double ytime = abs(yAxisLine.y1() - yAxisLine.y2()) / gridWidth;
    for (int i = 1; i < ytime; ++i)
    {
        //        double y = i * gridWidth + yAxisLine.y1();
        double y;
        if (yAxisLine.y2() > 0)
            y = (i - 0.5) * gridWidth ;
        else
            y = - (i - 0.5) * gridWidth;
        QLineF tempLine = xAxisLine;
        tempLine.translate(0, y);
        //        qDebug()<<"tempLine"<<tempLine.x1()<<tempLine.y1();
        //        qDebug()<<" p2"<<tempLine.x2() << tempLine.y2();
        QPointF *intersectPoint = new QPointF();
        QLineF::IntersectType intersectType =  line->intersect(tempLine, intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
        {
            //            qDebug()<<"intersectPoint x"<<intersectPoint.x() << " y"<<intersectPoint.y();
            returnPointList.append(intersectPoint);
        }
    }
    qDebug()<<"resultPointList size"<<returnPointList.size();
    return returnPointList;
}

QList<QLineF*> MyCoordinate::getCornerListFromPathLineList(const QList<QLineF*>& lineList, const QLineF& yAxisLine, const QLineF& xAxisLine)
{
    QList<QLineF*> returnList;
    int size = lineList.size();
    qDebug()<<"size"<<size;

    for (int i =1; i < size; ++i)
    {
        QLineF* first = lineList.at(i - 1);
        QPointF fp1 = first->p1();
        QPointF fp2 = first->p2();

        QPointF firstCenter((fp1.x() + fp2.x()) / 2, fp1.y());
        QLineF* second = lineList.at(i);
        QPointF sp1 = second->p1();
        QPointF sp2 = second->p2();
        if (i % 2 != 0)
        {
            qDebug()<< " i"<<i;
            qDebug()<<" fp2 x"<<fp2.x() << " sp2. x"<<sp2.x();
            qDebug()<<" fp1 x" << fp1.x() << "  sp1 x" << sp1.x();
            qDebug()<<"firstCenter x"<<firstCenter.x() << " y"<<firstCenter.y();

            double x = 0;
            if (fp2.x() < sp2.x())
            {
                x = sp2.x() + (sp2.x() - fp2.x()) / 2;
            }
            else if (fp2.x() > sp2.x())
            {
                x = fp2.x() + (fp2.x() - sp2.x()) / 2;
            }
            if (x == 0)
                continue;

            QPointF point1(x, fp2.y());
            QLineF *xline1 = new QLineF(fp2, point1);
            returnList.append(xline1);
            QPointF point2(x, sp2.y());
            QLineF *xline2 = new QLineF(sp2, point2);
            returnList.append(xline2);
            QPointF corner(x + gridWidth / 2, (fp2.y() + sp2.y()) / 2);
           qDebug()<<"corner  x"<<corner.x() << " y "<<corner.y();
            QLineF *yline1 = new QLineF(point1, corner);
            returnList.append(yline1);
            QLineF *yline2 = new QLineF(point2, corner);
            returnList.append(yline2);

            QPointF arrowUp(firstCenter.x() - gridWidth/2, firstCenter.y() + gridWidth/4);
            QPointF arrowDown(firstCenter.x() - gridWidth/2, firstCenter.y() - gridWidth/4);
            QLineF *arrowUpLine = new QLineF(arrowUp, firstCenter);
            QLineF *arrowDownLine = new QLineF(arrowDown, firstCenter);
            returnList.append(arrowUpLine);
            returnList.append(arrowDownLine);
        }
        else
        {
            qDebug()<< " i"<<i;
            qDebug()<<" fp2 x"<<fp2.x() << " sp2. x"<<sp2.x();
            qDebug()<<" fp1 x" << fp1.x() << "  sp1 x" << sp1.x();
            qDebug()<<"firstCenter x"<<firstCenter.x() << " y"<<firstCenter.y();

            double x = 0;
            if (fp1.x() < sp1.x())
            {
                x = fp1.x() - (sp1.x() - fp1.x()) / 2;
            }
            else if (fp1.x() > sp1.x())
            {
                x = sp1.x() - (fp1.x() - sp1.x()) / 2;
            }
            if (x == 0)
                continue;
            QPointF point1(x, fp2.y());
            QLineF *xline1 = new QLineF(fp2, point1);
            returnList.append(xline1);
            QPointF point2(x, sp2.y());
            QLineF *xline2 = new QLineF(sp2, point2);
            returnList.append(xline2);
            QPointF corner(x - gridWidth / 2, (fp2.y() + sp2.y()) / 2);
             qDebug()<<"corner  x"<<corner.x() << " y "<<corner.y();
            QLineF *yline1 = new QLineF(point1, corner);
            returnList.append(yline1);
            QLineF *yline2 = new QLineF(point2, corner);
            returnList.append(yline2);

            QPointF arrowUp(firstCenter.x() + gridWidth/2, firstCenter.y() + gridWidth/4);
            QPointF arrowDown(firstCenter.x() + gridWidth/2, firstCenter.y() - gridWidth/4);
            QLineF *arrowUpLine = new QLineF(arrowUp, firstCenter);
            QLineF *arrowDownLine = new QLineF(arrowDown, firstCenter);
            returnList.append(arrowUpLine);
            returnList.append(arrowDownLine);
        }
    }
    return returnList;
}

//QList<QLineF*> MyCoordinate::getCircleListFromPathLineList(const QList<QLineF*>&, const QLineF&, const QLineF&)
//{
//    QList<QLineF*> returnList;


//    return returnList;
//}

//QList<QLineF*> MyCoordinate::getArrowListFromPathLineList(const QList<QLineF*>&lineList, const QLineF&yAxisLine, const QLineF&xAxisLine)
//{
//    QList<QLineF*> returnList;
//    foreach(QLineF* line, lineList)
//    {
//        QPointF fp1 = line->p1();
//        QPointF fp2 = line->p2();
//        QPointF pointF((fp1.x() + fp2.x()) / 2, fp1.y());

//    }

//    return returnList;
//}

}
