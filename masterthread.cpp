
#include "masterthread.h"
#include <QTime>
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QSettings>
#include <QTime>
//#include <QFile>
#include <QDir>
#include <QApplication>
#include <QXmlStreamWriter>
#include <QFileDialog>

MasterThread::MasterThread(QObject*parent, QWidget * window)
    : QThread(parent),
      widget(widget),
      waitTimeout(0),
      quit(false),
      bStartRecord(false),
      bXmlStartRecord(false),
      bXmlStopAndSave(false)
{
    init();
}

//! [0]
MasterThread::~MasterThread()
{
    mutex.lock();
    quit = true;
    //    cond.wakeOne();
    mutex.unlock();
    wait();
}

void MasterThread::init()
{
    QMutexLocker locker(&mutex);
    QSettings settings("AutoGPS", "AutoGPS");
    portName = settings.value("SerialPortName", "Com1").toString();
    baudRate = settings.value("SerialBaudRate", "9600").toString().toInt();
    dataBits = settings.value("SerialDataBits", "8").toString().toInt();
    stopBits = settings.value("SerialStopBits", "1").toString().toInt();
    parity = settings.value("SerialParity", "0").toString().toInt();
    waitTimeout =   settings.value("SerialTimeOut",  "1000").toString().toInt();
    //    qDebug()<<"init --------";
    //    qDebug()<<"baudRate: "<<baudRate;
    //    qDebug()<<"dataBits"<<dataBits;
    //    qDebug()<<"portName"<<portName;
    //    qDebug()<<"stopBits"<<stopBits;
    //    qDebug()<<"parity"<<parity;
    //    qDebug()<<"waitTimeout"<<waitTimeout;
}

void MasterThread::run()
{
    //    qDebug()<<"run()";
    bool currentPortNameChanged = false;
    bool currentBaudRateChanged = false;
    bool currentStopBitsChanged = false;
    bool currentDataBitsChanged = false;
    bool currentParityChanged = false;
    bool currentXmlStartRecord = false;
    //    bool currentXmlStopAndSave = false;
    mutex.lock();
    //! [4] //! [5]
    QString currentPortName;
    if (currentPortName != portName) {
        currentPortName = portName;
        currentPortNameChanged = true;
    }
    int currentBaudRate = this->baudRate;
    int currentStopBits = this->stopBits;
    int currentDataBits = this->dataBits;
    int currentParity = this->parity;
    int currentWaitTimeout =  this->waitTimeout;

    mutex.unlock();
    //! [5] //! [6]
    QSerialPort serial;
    QByteArray logData;
    QXmlStreamWriter logXml(&logData);
    logXml.setAutoFormatting(true);

    while (!quit) {
        //![6] //! [7]
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(QVariant::fromValue(tr("Can't open %1, error code %2")
                                               .arg(portName).arg(serial.error())));
                return;
            }
        }
        if (currentBaudRateChanged) {
            serial.setBaudRate(currentBaudRate);
        }
        if (currentDataBitsChanged) {
            serial.setDataBits(QSerialPort::DataBits(currentDataBits));
        }
        if (currentStopBitsChanged) {
            serial.setStopBits(QSerialPort::StopBits(currentStopBits));
        }
        if (currentParityChanged) {
            serial.setParity(QSerialPort::Parity(currentParity));
        }

        if (serial.waitForReadyRead(currentWaitTimeout)) {
            QByteArray responseData = serial.readAll();
            while (serial.waitForReadyRead(10))
                responseData += serial.readAll();
            QString response(responseData);
            record(response);
            if (!currentXmlStartRecord)
            {
                decoding(response, true, NULL);
            }
            else
            {
                qDebug()<<" xml start  decoding";
                decoding(response, true, &logXml);
            }
        } else {
            emit timeout(tr("Wait read response timeout %1")
                         .arg(QTime::currentTime().toString()));
        }
        mutex.lock();
        if (currentPortName != portName) {
            currentPortName = portName;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        if (currentBaudRate != baudRate) {
            currentBaudRate = baudRate;
            currentBaudRateChanged = true;
        }
        else {
            currentBaudRateChanged = false;
        }
        if (currentDataBits != dataBits)
        {
            currentDataBits = dataBits;
            currentDataBitsChanged = true;
        } else {
            currentDataBitsChanged = false;
        }
        if (currentStopBits != stopBits)
        {
            currentStopBits = stopBits;
            currentStopBitsChanged = true;
        }
        else
        {
            currentStopBitsChanged = false;
        }
        if (currentParity != parity)
        {
            currentParity = parity;
            currentParityChanged = true;
        }
        else
        {
            currentParityChanged = false;
        }

        currentWaitTimeout = waitTimeout;
        if (this->bXmlStopAndSave)
        {
            logXml.writeEndElement();
            logXml.writeEndDocument();


            QString dirPath = QApplication::applicationDirPath();
            QDir dir(dirPath);
            if (!dir.exists("logFile"))
            {
                dir.mkdir("logFile");
            }
            QString filePath = dirPath + "/logFile/" + getCurrentDateTimeString() + ".xml";
            xmlFileName = filePath;
            QFile file(filePath);
            if (!file.open(QIODevice::WriteOnly))
            {
                qDebug()<<"file cannot open"<< filePath;
                emit error(QVariant::fromValue(tr("Can't open xml file %1, error code %2")
                                               .arg(filePath).arg(file.error())));
                return;
            }
            file.write(logData);
            logData.clear();
            currentXmlStartRecord = false;
            this->bXmlStopAndSave = false;
        }
        if (this->bXmlStartRecord)
        {
            logXml.writeStartDocument();
            logXml.writeStartElement("logDatas");
            this->bXmlStartRecord = false;
            currentXmlStartRecord = true;
        }
        mutex.unlock();
    }
}

QStringList MasterThread::portList()
{
    m_portList.clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        m_portList.append(info.portName());
    return m_portList;
}

void MasterThread::onReadyOpenSerialPort(QVariant data)
{
    qDebug()<<"onReadyOpenSerialPort";
    QObject* serialPortData = qobject_cast<QObject*>(data.value<QObject*>());
    if (!serialPortData)
        return;
    QVariant portNo = serialPortData->property("portNo");
    QVariant baudRate = serialPortData->property("baudRate");
    QVariant dataBits = serialPortData->property("dataBits");
    QVariant stopBits = serialPortData->property("stopBits");
    QVariant parity = serialPortData->property("parity");
    QVariant timeOut = serialPortData->property("timeOut");

    QMutexLocker locker(&mutex);
    this->portName = portNo.toString();
    this->baudRate = baudRate.toString().toInt();
    this->stopBits = stopBits.toString().toInt();
    this->dataBits = dataBits.toString().toInt();
    QString temp = parity.toString();
    if (temp == "NONE")
        this->parity = 0;
    else if (temp == "ODD")
        this->parity = 2;
    else if (temp == "EVEN")
        this->parity = 3;
    this->waitTimeout = timeOut.toString().toInt();
    qDebug()<<"-- --------";
    qDebug()<<"baudRate: "<<baudRate;
    qDebug()<<"dataBits"<<dataBits;
    qDebug()<<"portName"<<portName;
    qDebug()<<"stopBits"<<stopBits;
    qDebug()<<"parity"<<parity;
    qDebug()<<"waitTimeout"<<waitTimeout;
    if (!isRunning())
        start();
}

void MasterThread::storeSerialConfig()
{
    QMutexLocker locker(&mutex);
    QSettings settings("AutoGPS", "AutoGPS");
    settings.setValue("SerialPortName", portName);
    settings.setValue("SerialBaudRate", baudRate);
    settings.setValue("SerialDataBits", dataBits);
    settings.setValue("SerialStopBits", stopBits);
    settings.setValue("SerialParity", parity);
    settings.setValue("SerialTimeOut", waitTimeout);
}

void MasterThread::decoding(QString &comdata, bool bEmit,QXmlStreamWriter* stream)
{
    if (comdata.contains('\n'))
    {
        QStringList dataList = comdata.split('\n',  QString::SkipEmptyParts);
        foreach(QString data, dataList)
        {
            itemDecoding(data, bEmit, stream);
        }
    }
    else
    {
        itemDecoding(comdata, bEmit, stream);
    }
}

void MasterThread::itemDecoding(QString &comdata, bool bEmit,QXmlStreamWriter* stream)
{
    QString header = comdata.left(6);
    QString latlon;
    QString timeStr;
    QString speed;
    QString heading;
    double dLat = 0;
    double dLon = 0;
    if (header == "$GPGGA")
    {
        QStringList list = comdata.split(',');
        QString time = list.at(1);
        int time_hour = time.mid(0, 2).toInt();
        int time_minute = time.mid(2, 2).toInt();
        int time_second = time.mid(4, 2).toInt();
        int time_msec = time.right(3).toInt();
        latlon = list.at(2) + ',' +list.at(4).mid(1);
        //        qDebug()<<"latLon"<<latlon;
        timeStr = QString("%1:%2:%3:%4").arg(time_hour).arg(time_minute).arg(time_second).arg(time_msec);
        //        qDebug()<<"timeStr"<<timeStr;
        dLat = DMTodecimalDegrees(list.at(2));
        dLon = DMTodecimalDegrees(list.at(4));

    }
    else if (header == "$GPRMC")
    {
        QStringList list = comdata.split(',');

        QString time = list.at(1);
        int time_hour = time.mid(0, 2).toInt();
        int time_minute = time.mid(2, 2).toInt();
        int time_second = time.mid(4, 2).toInt();
        int time_msec = time.right(3).toInt();
        QString timeStr = QString("%1:%2:%3:%4").arg(time_hour).arg(time_minute).arg(time_second).arg(time_msec);

        QString state = list.at(2);
        QString lat = DMTODMS(list.at(3));
        QString lon = DMTODMS(list.at(5));
        latlon = lat + "," + lon;
        speed = QString::number(list.at(7).toDouble() * 1.852) + "km/h";
        heading = list.at(8);
        //        emit stateChanged(QVariant::fromValue(state));
        dLat = DMTodecimalDegrees(list.at(3));
        dLon = DMTodecimalDegrees(list.at(5));
    }

    if (bEmit)
    {
        if (!timeStr.isEmpty())
        {
            emit timeChanged(QVariant::fromValue(timeStr));
        }
        if (!latlon.isEmpty())
        {
            emit positionChanged(QVariant::fromValue(latlon));
        }
        if (!speed.isEmpty())
        {
            emit speedChanged(QVariant::fromValue(speed));
        }
        if (!heading.isEmpty())
        {
            qDebug()<<" !heading. empty"<<dLat<<dLon;
            emit headingChanged(QVariant::fromValue(heading));
            emit avaliblePosition(dLat, dLon, heading.toDouble());
        }
        else
        {
            qDebug()<<"empty"<<dLat<<dLon;
            emit avaliblePosition(dLat, dLon, 0.0);
        }
    }

    if (stream)
    {
        qDebug()<<"stream"<<stream;
        stream->writeStartElement("log");
        stream->writeAttribute("lat", QString::number(dLat));
        stream->writeAttribute("lon", QString::number(dLon));
        stream->writeTextElement("time", timeStr);
        stream->writeEndElement();
    }
}


//度分转度分秒
QString MasterThread::DMTODMS(QString dm)
{
    double ddm = dm.toDouble();
    int ds = floor(ddm);
    int degree = floor(ds/100);
    int minute = ds%100;
    double second = ddm - static_cast<double>(ds);
    int rSecond = floor( second * 60);
    return QString::number(degree) + (char) 0x00B0 + " " + QString::number(minute) + "'" + " " + QString::number(rSecond) + "\"";
}


//DM即度分格式 转成 小数度格式
double MasterThread::DMTodecimalDegrees(QString dm)
{
    double ddm = dm.toDouble();
    int ds = floor(ddm);
    int degree = floor(ds/100);
    int minute = ds%100;
    double tMin = static_cast<double>(minute) +  ddm - static_cast<double>(ds);
    double degrees = (static_cast<double>(degree) + tMin/60);
    return degrees;
}

void MasterThread::onStartRecordClicked()
{
    qDebug()<<"startRecord Clicked";
    QMutexLocker locker(&mutex);
    bStartRecord = true;
    recordString.clear();

}

void MasterThread::onStopAndSaveClicked()
{
    QString dirPath = QApplication::applicationDirPath();
    QDir dir(dirPath);
    if (!dir.exists("logFile"))
    {
        dir.mkdir("logFile");
    }
    QString filePath = dirPath + "/logFile/" + getCurrentDateTimeString() + ".log";
    logFileName = filePath;
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"file cannot open"<< filePath;
        emit error(QVariant::fromValue(tr("Can't open logfile %1, error code %2")
                                       .arg(filePath).arg(file.error())));
        return;
    }
    QMutexLocker locker(&mutex);
    qDebug()<<"bStopAndSave"<<recordString;
    file.write(recordString.toLatin1());

    bStartRecord = false;
}

void MasterThread::onTranslateToXmlClicked()
{
    qDebug()<<"onTranslateToxmlCLicked";
    if (recordString.isEmpty())
        return;
    if (logFileName.isEmpty())
        return;
    xmlFileName = logFileName.replace(".log", ".xml");
    QFile file(xmlFileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        emit error(QVariant::fromValue(tr("Can't open xml file %1, error code %2")
                                       .arg(xmlFileName).arg(file.error())));
        return;
    }

    QByteArray logData;
    QXmlStreamWriter logXml(&logData);
    logXml.setAutoFormatting(true);
    logXml.writeStartDocument();
    logXml.writeStartElement("logDatas");
    decoding(recordString, false, &logXml);

    logXml.writeEndElement();
    //    logXml.writeAttribute();
    logXml.writeEndDocument();
    file.write(logData);
    qDebug()<<"logData"<<logData;
}

void MasterThread::record(QString &comdata)
{
    QMutexLocker locker(&mutex);
    if (bStartRecord)
    {
        if (!recordString.isEmpty())
        {
            recordString += "\r\n";
        }
        recordString.append(comdata);
    }
}

void MasterThread::onXmlStartRecordClicked()
{
    QMutexLocker locker(&mutex);
    bXmlStartRecord = true;
}

void MasterThread::onXmlStopAndSaveClicked()
{
    QMutexLocker locker(&mutex);
    bXmlStopAndSave = true;
}

void MasterThread::onSelectXmlFileClicked()
{
    QString dirPath = QApplication::applicationDirPath();
    QDir dir(dirPath);
    if (!dir.exists("logFile"))
    {
        dir.mkdir("logFile");
    }
    qDebug()<<"dirPath"<<dirPath;
    QString fileName = QFileDialog::getOpenFileName(widget, "Select Xml file", dirPath + "/logFile" , "Image Files (*.xml)");
    QFile file(fileName);
    QByteArray byteArray;
    if (! file.open(QIODevice::ReadOnly))
    {
        emit error(QVariant::fromValue(tr("Can't open select xml file %1, error code %2")
                                       .arg(fileName).arg(file.error())));
        return;
    }
    byteArray = file.readAll();
    reader.clear();
    reader.addData(byteArray);

}

QString MasterThread::getCurrentDateTimeString()
{
    QDate date = QDate::currentDate();
    QString dateString = date.toString("yyyy-MM-dd");
    QTime time = QTime::currentTime();
    QString timeString = time.toString("hh-mm-ss");
    QString dirPath = QApplication::applicationDirPath();
    return (dateString + " " + timeString);
}

void MasterThread::onPlayInSimulatorClicked()
{

}

void MasterThread::onPaintGeometryClicked()
{
    if (reader.device() != 0)
    {
        readPointFList.clear();
        while (!reader.atEnd() && !reader.hasError())
        {
            if (reader.isStartElement())
            {
                if (reader.name().compare("log") == 0)
                {
                    QXmlStreamAttributes attrs = reader.attributes();
                    double dLat = attrs.value("lat").toString().toDouble();
                    double dLon = attrs.value("lon").toString().toDouble();
                    qDebug()<<"dLat"<<dLat << "dLon"<<dLon;
                    QString time = reader.readElementText();
                    qDebug()<<"time";
                }
            }

            reader.readNext();
        }
    }

}


