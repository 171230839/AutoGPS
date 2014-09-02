
#ifndef SIMPLEGRAPHICOVERLAY_H
#define SIMPLEGRAPHICOVERLAY_H

#include <QGraphicsWidget>
#include <QPointer>
QT_BEGIN_NAMESPACE
namespace EsriRuntimeQt
{
class Point;
class MapGraphicsView;
}
class QGraphicsView;
QT_END_NAMESPACE

namespace AutoGPSNAMESPACE{
class SimpleGraphicOverlay : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit SimpleGraphicOverlay();
    ~SimpleGraphicOverlay();
    void setImage(QImage imageIn);
    void setAngle(double rotationIn);
    void setPosition(EsriRuntimeQt::Point);
    void setGraphicsView(EsriRuntimeQt::MapGraphicsView* pGraphicView);
    void setVisible(bool visibleIn);

    QRectF boundingRect() const;

signals:
    
public slots:
    
protected :

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QImage image;
    double rotation;
    double screenX, screenY;
    bool visible;
    QPointer<EsriRuntimeQt::MapGraphicsView> m_pMapGraphicsView;
};

}

#endif // SIMPLEGRAPHICOVERLAY_H
