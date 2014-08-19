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

#ifndef MASTERTHREAD_H
#define MASTERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QStringList>
#include <QVariant>

//! [0]
class MasterThread : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QStringList portList READ portList NOTIFY portListChanged)
    Q_PROPERTY(QString qPortName READ qPortName)
    Q_PROPERTY(QString qBaudRate READ qBaudRate)
    Q_PROPERTY(QString qDataBits READ qDataBits)
    Q_PROPERTY(QString qStopBits READ qStopBits)
    Q_PROPERTY(QString qParity READ qParity)
    Q_PROPERTY(QString qTimeout READ qTimeout)
public:
    MasterThread(QObject *parent = 0);
    ~MasterThread();
    void run();

    QStringList portList();
    QString qPortName() { return portName;}
    QString qBaudRate() { return QString::number(baudRate);}
    QString qDataBits() { return QString::number(dataBits);}
    QString qStopBits() { return QString::number(stopBits);}
    QString qParity() {
        if (parity == 0)
            return "NONE";
        else if (parity == 2)
            return "ODD";
        else if (parity == 3)
            return "ENEV";
        return "";
    }
    QString qTimeout() { return QString::number(waitTimeout);}
    void storeSerialConfig();
    void init();
signals:
    void response(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);
    void portListChanged();
    void positionChanged(QVariant);
    void timeChanged(QVariant);
    void speedChanged(QVariant);
    void headingChanged(QVariant);
//    void stateChanged(QVariant);
    void avaliblePosition(double, double, double);
private:
    QString portName;
    //    QString request;
    int baudRate;
    int stopBits;
    int dataBits;
    int parity;
    int waitTimeout;
    QMutex mutex;
    //    QWaitCondition cond;
    bool quit;
    QStringList m_portList;

    void Decoding(QString);
//    QString decimalDegreesToDMS(double coord);
    QString DMTODMS(QString );
//    QString getDM(QString);
    double DMTodecimalDegrees(QString);
public slots:
    void onReadyOpenSerialPort(QVariant);
};
//! [0]

#endif // MASTERTHREAD_H
