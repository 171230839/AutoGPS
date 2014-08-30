
// Copyright 2014 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "AutoGPS.h"
#include <QApplication>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("AutoGPS");
    app.setApplicationName("AutoGPS");


    QTranslator qtTranslator;
    qtTranslator.load("myapp.qm");
    app.installTranslator(&qtTranslator);

    AutoGPS applicationWindow;

    applicationWindow.show();

    return app.exec();
}

