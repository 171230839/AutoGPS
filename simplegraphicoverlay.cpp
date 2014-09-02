
#include "simplegraphicoverlay.h"
#include <QPainter>
#include <Point.h>
#include <MapGraphicsView.h>
#include <QGraphicsView>

namespace AutoGPSNAMESPACE{
using namespace EsriRuntimeQt;

SimpleGraphicOverlay::SimpleGraphicOverlay():
    m_pMapGraphicsView(0)
    {
        rotation = 0.0;
        screenX = -1;
        screenY = -1;
        visible = true;

        setAttribute(Qt::WA_NoBackground);
        setAutoFillBackground(false);
        this->setObjectName("SimpleGraphicOverlay");
    }

 SimpleGraphicOverlay::~SimpleGraphicOverlay()
{

}

QRectF SimpleGraphicOverlay::boundingRect() const
{
    if (m_pMapGraphicsView)
    {
        return QRect(0, 0, m_pMapGraphicsView->width(), m_pMapGraphicsView->height());
    }
    return QRectF();
}

void SimpleGraphicOverlay::setImage(QImage imageIn)
{
    image = imageIn;
}

void SimpleGraphicOverlay::setAngle(double rotationln)
{
    rotation = rotationln;
}

void SimpleGraphicOverlay::setPosition(Point position)
{
    if (!m_pMapGraphicsView)
        return;

    Point screenPoint = m_pMapGraphicsView->map().toScreenPoint(position);
    screenX = screenPoint.x();
    screenY = screenPoint.y();
}

void SimpleGraphicOverlay::setGraphicsView(MapGraphicsView* pGraphicView)
{
    m_pMapGraphicsView = pGraphicView;
    QGraphicsScene *scene = m_pMapGraphicsView->scene();
    scene->addItem(this);
}

void SimpleGraphicOverlay::setVisible(bool visibleIn)
{
    visible = visibleIn;
}

void SimpleGraphicOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if ((!visible) || (!m_pMapGraphicsView) || image.isNull() || (screenX == -1) || (screenY == -1))
        return;

    double mapRotation = m_pMapGraphicsView->map().rotation();
    QSize size = image.size();
    double halfImageWidth = size.width() / 2.0;
    double halfImageHeight = size.height() / 2.0;

    painter->translate(screenX, screenY);
    painter->rotate(rotation - mapRotation);
    painter->drawImage(QPoint(-halfImageWidth, -halfImageHeight), image);

}

}
