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
class QXmlStreamWriter;
#include <QXmlStreamReader>
class QWidget;
//! [0]
class MasterThread : public QThread
{
    Q_OBJECT
    //    Q_PROPERTY(QStringList portList READ portList NOTIFY portListChanged)
    //    Q_PROPERTY(QString qPortName READ qPortName NOTIFY qPortNameChanged)
    //    Q_PROPERTY(QString qBaudRate READ qBaudRate NOTIFY qBaudRateChanged)
    //    Q_PROPERTY(QString qDataBits READ qDataBits NOTIFY qDataBitsChanged)
    //    Q_PROPERTY(QString qStopBits READ qStopBits NOTIFY qStopBitsChanged)
    //    Q_PROPERTY(QString qParity READ qParity NOTIFY qParityChanged)
    //    Q_PROPERTY(QString qTimeout READ qTimeout NOTIFY qTimeoutChanged)
public:
    MasterThread(QObject*parent = 0, QWidget* widget= 0);
    ~MasterThread();
    void run();

    Q_INVOKABLE QStringList portList();
    Q_INVOKABLE QString qPortName() { return portName;}
    Q_INVOKABLE QString qBaudRate() { return QString::number(baudRate);}
    Q_INVOKABLE  QString qDataBits() { return QString::number(dataBits);}
    Q_INVOKABLE QString qStopBits() { return QString::number(stopBits);}
    Q_INVOKABLE QString qParity() {
        if (parity == 0)
            return "NONE";
        else if (parity == 2)
            return "ODD";
        else if (parity == 3)
            return "ENEV";
        return "";
    }
    Q_INVOKABLE QString qTimeout() { return QString::number(waitTimeout);}
    void storeSerialConfig();
    void init();
signals:

    void error(const QVariant &s);
    void timeout(const QString &s);

    void positionChanged(const QVariant&);
    void timeChanged(const QVariant&);
    void speedChanged(const QVariant&);
    void headingChanged(const QVariant&);
    //    void stateChanged(QVariant);
    void avaliblePosition(double, double, double);

    //     void portListChanged();
    //    void qPortNameChanged();
    //    void qBaudRateChanged();
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

    void decoding(QString&, bool, QXmlStreamWriter*);
    //    QString decimalDegreesToDMS(double coord);
    QString DMTODMS(QString );
    //    QString getDM(QString);
    double DMTodecimalDegrees(QString);
    bool bStartRecord;
    void itemDecoding(QString&, bool,QXmlStreamWriter*);
    void record(QString&);
    QString recordString;
    QString logFileName;

    bool bXmlStartRecord;
    bool bXmlStopAndSave;
    QString xmlFileName;
    QWidget * widget;
    QString getCurrentDateTimeString();
    QXmlStreamReader reader;
    QList<QPointF> readPointFList;
public slots:
    void onReadyOpenSerialPort(QVariant);
    void onStartRecordClicked();
    void onStopAndSaveClicked();
    void onTranslateToXmlClicked();
    void onXmlStartRecordClicked();
    void onXmlStopAndSaveClicked();
    void onSelectXmlFileClicked();
    void onPlayInSimulatorClicked();
    void onPaintGeometryClicked();
};
//! [0]

#endif // MASTERTHREAD_H
