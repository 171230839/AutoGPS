#include "GPSSimulator.h"

GPSSimulator::GPSSimulator():
	m_PlaybackMultiplier(1),
	isStarted(false)
{
    timer = new QTimer(this);
	gpxReader = new QXmlStreamReader();
    connect(timer, SIGNAL(timeout()), this, SLOT(handleTimerEvent()));
}

GPSSimulator::GPSSimulator(QString fileName, int updateInterval):
	m_TimerInterval(updateInterval),
	m_PlaybackMultiplier(1),
	isStarted(false)
{
	timer = new QTimer(this);
	gpxReader = new QXmlStreamReader();
    connect(timer, SIGNAL(timeout()), this, SLOT(handleTimerEvent()));
    if (!setGpxFile(fileName))
	{
		m_GpxFile.setFileName("");
	}
}

GPSSimulator::~GPSSimulator()
{
	delete gpxReader;
	delete timer;
}

void GPSSimulator::startSimulation()
{
    if (!initializeInterpolationValues())
	{
		return;
	}
    timer->start(m_TimerInterval);
	isStarted = true;
}

void GPSSimulator::pauseSimulation()
{
	timer->stop();
}

void GPSSimulator::resumeSimulation()
{
	timer->start();
}

bool GPSSimulator::initializeInterpolationValues()
{
	QPointF pt1;
	QPointF pt2;
	QPointF pt3;
	getNextPoint(pt1, segmentStartTime);
	getNextPoint(pt2, segmentEndTime);
    getNextPoint(pt3, nextSegmentEndTime);

	if (pt1.isNull() || pt2.isNull() || pt3.isNull())
	{
		return false;
	}

	currentSegment.setPoints(pt1, pt2);
	nextSegment.setPoints(pt2, pt3);
	startOrientationDelta = 0;
	endOrientationDelta = currentSegment.angleTo(nextSegment);

	if (endOrientationDelta > 180.0)
	{
		endOrientationDelta -= 360.0;
	}

	currentTime = segmentStartTime;
	return true;
}


void GPSSimulator::getNextPoint(QPointF&  point, QTime& time)
{
	if (!gotoNextPositionElement())
	{
		point.setX(0.0);
		point.setY(0.0);
		return;
	}

    QXmlStreamAttributes attrs = gpxReader->attributes();
	point.setY(attrs.value("lat").toString().toDouble());
	point.setX(attrs.value("lon").toString().toDouble());

	if (point == latestPoint)
	{
		gpxReader->readNext();
		getNextPoint(point, time);
	}

	gpxReader->readNextStartElement();
	while (gpxReader->name().compare("trkpt", Qt::CaseInsensitive) != 0 && !gpxReader->atEnd())
	{
		if (gpxReader->isStartElement())
		{
			// if (gpxReader->name().compare("ele", Qt::CaseInsensitive) == 0)
			// {}
            if (gpxReader->name().compare("time", Qt::CaseInsensitive) == 0)
			{
				QString timeString = gpxReader->readElementText();
				int hours = timeString.section(":", 0, 0).right(2).toInt();
				int minutes = timeString.section(":", 1, 1).toInt();
				int seconds = timeString.section(":", 2, 2).left(2).toInt();
				time.setHMS(hours, minutes, seconds, 0);
			}
		}
		gpxReader->readNext();
	}

    latestPoint.setX(point.x());
    latestPoint.setY(point.y());
}

bool GPSSimulator::gotoNextPositionElement()
{
	while (!gpxReader->atEnd() && !gpxReader->hasError())
	{
		if (gpxReader->isStartElement())
		{
			if (gpxReader->name().compare("trkpt") == 0)
			{
				return true;
			}
		}
		gpxReader->readNext();
	}
    return false;
}

int GPSSimulator::timerInterval()
{
	return m_TimerInterval;
}

void GPSSimulator::setTimerInterval(int ms)
{
	m_TimerInterval = ms;
}

int GPSSimulator::playbackMultiplier()
{
    return m_PlaybackMultiplier;
}

void GPSSimulator::setPlaybackMultiplier(int val)
{
	m_PlaybackMultiplier = val;
}

QString GPSSimulator::gpxFile()
{
	return m_GpxFile.fileName();
}

bool GPSSimulator::setGpxFile(QString fileName)
{
	if (!QFile::exists(fileName))
		return false;
	if (m_GpxFile.isOpen())
		m_GpxFile.close();
	m_GpxFile.setFileName(fileName);
	if (!m_GpxFile.open(QFile::ReadOnly | QFile::Text))
		return false;

	gpxData = m_GpxFile.readAll();
	gpxReader->clear();
	gpxReader->addData(gpxData);
	m_GpxFile.close();
	isStarted = false;
	return true;
}



void GPSSimulator::handleTimerEvent()
{
	currentTime = currentTime.addMSecs(timer->interval() * m_PlaybackMultiplier);
    if (currentTime > segmentEndTime)
	{
		if (!updateInterpolationParameters())
		{
			gpxReader->clear();
			gpxReader->addData(gpxData);
            initializeInterpolationValues();
			return;
		}
	}

	double val1 = static_cast<double>(segmentStartTime.msecsTo(currentTime));
	double val2 = static_cast<double>(segmentStartTime.msecsTo(segmentEndTime));
	double normalizedTime = val1 / val2;

	QPointF currentPosition = currentSegment.pointAt(normalizedTime);
    double currentOrientation = getInterpolatedOrientation(currentPosition, normalizedTime);

	emit positionUpdateAvailable(currentPosition, currentOrientation);
}

bool GPSSimulator::updateInterpolationParameters()
{
	QPointF newPt;
	segmentStartTime = segmentEndTime;
	segmentEndTime = nextSegmentEndTime;
	getNextPoint(newPt, nextSegmentEndTime);

	if (newPt.isNull())
	{
		return false;
	}

	currentSegment = nextSegment;
    nextSegment.setPoints(currentSegment.p2(), newPt);
	startOrientationDelta = endOrientationDelta;
	endOrientationDelta = currentSegment.angleTo(nextSegment);

	if (endOrientationDelta > 180.0)
	{
		endOrientationDelta -= 360.0;
	}
	return true;
}

double GPSSimulator::getInterpolatedOrientation(const QPointF& currentPosition, double normalizedTime)
{
	QLineF segment;
	double transitionAngle;

	if (normalizedTime < 0.1)
	{
		segment.setPoints(currentSegment.p1(), currentPosition);
		transitionAngle = ((startOrientationDelta / 2.0) * (((currentSegment.length() * 0.1) - 
			segment.length()) / (currentSegment.length() * 0.1)));
		return currentSegment.angle() - transitionAngle;
	}
	else if (normalizedTime > 0.9)
	{
		QPointF tempPt = currentSegment.pointAt(0.9);
		segment.setPoints(tempPt, currentPosition);
		transitionAngle = ((endOrientationDelta / 2.0) * segment.length()) / (currentSegment.length() * 0.1);
		return currentSegment.angle() + transitionAngle;
	}
	else
	{
		return currentSegment.angle();
	}
}

bool GPSSimulator::started()
{
    return isStarted;
}

bool GPSSimulator::isActive()
{
  return timer->isActive();
}
