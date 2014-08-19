/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "masterthread.h"



#include <QTime>
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QSettings>

MasterThread::MasterThread(QObject *parent)
    : QThread(parent),
      waitTimeout(0),
      quit(false)
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

    while (!quit) {
        //![6] //! [7]
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                           .arg(portName).arg(serial.error()));
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
            //! [12]
            Decoding(response);
            //            emit this->response(response);
            //! [10] //! [11] //! [12]
        } else {
            emit timeout(tr("Wait read response timeout %1")
                         .arg(QTime::currentTime().toString()));
        }

        mutex.lock();
        //        cond.wait(&mutex);
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
        mutex.unlock();
    }
    //! [13]
}




QStringList MasterThread::portList()
{
    m_portList.clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        m_portList.append(info.portName());
    return m_portList;
}



//void MasterThread::setContext(QDeclarativeContext *context)
//{
//    this->context = context;
//}

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

void MasterThread::Decoding(QString comdata)
{
    QString header = comdata.left(6);
    if (header == "$GPGGA")
    {
        QStringList list = comdata.split(',');
        QString time = list.at(1);
        int time_hour = time.mid(0, 2).toInt();
        int time_minute = time.mid(2, 2).toInt();
        int time_second = time.mid(4, 2).toInt();
        int time_msec = time.right(3).toInt();
        QString latlon = list.at(2) + ',' +list.at(4).mid(1);
        qDebug()<<"latLon"<<latlon;
        QString timeStr = QString("%1:%2:%3:%4").arg(time_hour).arg(time_minute).arg(time_second).arg(time_msec);
        qDebug()<<"timeStr"<<timeStr;
        emit positionChanged(QVariant::fromValue(latlon));
        emit timeChanged(QVariant::fromValue(timeStr));
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
        emit timeChanged(QVariant::fromValue(timeStr));
        QString state = list.at(2);
        QString lat = DMTODMS(list.at(3));
        QString lon = DMTODMS(list.at(5));
        QString latlon = lat + "," + lon;
        QString speed = QString::number(list.at(7).toDouble() * 1.852) + "km/h";
        QString heading = list.at(8);
//        emit stateChanged(QVariant::fromValue(state));
        emit positionChanged(QVariant::fromValue(latlon));
        emit speedChanged(QVariant::fromValue(speed));
        emit headingChanged(QVariant::fromValue(heading));

        double dLat = DMTodecimalDegrees(list.at(3));
        double dLon = DMTodecimalDegrees(list.at(5));
        emit avaliblePosition(dLat, dLon, heading.toDouble());

    }
}

//QString MasterThread::decimalDegreesToDMS(double coord)
//{
//  int lonDegrees = floor(coord);
//  double degreesRemainder = coord - static_cast<double>(lonDegrees);
//  int lonMinutes = floor(degreesRemainder * 60.0);
//  double minutesRemainder = (degreesRemainder * 60) - static_cast<double>(lonMinutes);
//  int lonSeconds = floor(minutesRemainder * 60);

//  return QString::number(lonDegrees) + (char) 0x00B0 + " " + QString::number(lonMinutes) + "'" + " " + QString::number(lonSeconds) + "\"";
//}

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

//QString MasterThread::getDM(QString dm)
//{
//    double ddm = dm.toDouble();
//        int ds = floor(ddm);
//        int degree = floor(ds/100);
//        int minute = ds%100;
//        double tMin = static_cast<double>(minute) +  ddm - static_cast<double>(ds);
//        return QString::number(degree) + (char) 0x00B0 + " " + QString::number(tMin) + "'";
//}


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
