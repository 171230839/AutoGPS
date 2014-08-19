

#ifndef GPSSIMULATOR_H
#define GPSSIMULATOR_H


#include <QDomDocument>
#include <QXmlStreamReader>
#include <QDomElement>
#include <QTimer>
#include <QFile>
#include <QLineF>
#include <QTime>
#include <QPointF>

class GPSSimulator : public QObject
{
	Q_OBJECT
public:
	GPSSimulator();
	GPSSimulator(QString fileName, int updateInterval = 20);
	~GPSSimulator();

	void startSimulation();
	void pauseSimulation();
	void resumeSimulation();

	bool isActive();
	bool started();

	QString gpxFile();
	bool setGpxFile(QString fileName);

    int timerInterval();
    void setTimerInterval(int );

    int playbackMultiplier();
    void setPlaybackMultiplier(int multiplier);

private:
	QFile m_GpxFile;
	QByteArray gpxData;
	QXmlStreamReader* gpxReader;
	QTimer* timer;
	int m_TimerInterval;
    int m_PlaybackMultiplier;
	QLineF currentSegment;
	QLineF nextSegment;
	QPointF latestPoint;
	double startOrientationDelta;
	double endOrientationDelta;
	QTime currentTime;
	QTime segmentStartTime;
    QTime segmentEndTime;
	QTime nextSegmentEndTime;
	bool isStarted;

	bool gotoNextPositionElement();
	void getNextPoint(QPointF& point, QTime& time);
	bool updateInterpolationParameters();
    bool initializeInterpolationValues();

	double getInterpolatedOrientation(const QPointF& currentPosition, double normalizedTime);
    private slots:
    void handleTimerEvent();

	signals:
	void positionUpdateAvailable(QPointF pos, double origentation);

};

#endif // GPSSIMULATOR_H
