import QtQuick 1.1
import "../Panels/MainMenu"
import "../Controls"

Item
{
    signal closeMenu()
    signal exitClicked()


    property Stack stack
    property Panel panel

    id: mainMenu
    objectName: "mainMenu"
    panel: mainPanel

    MainPanel
    {
        id: mainPanel;
        //    reportsPanel: reports.panel
        configPanel: config.panel
        workerPanel: worker.panel
        anchors.fill: parent
        stack: mainMenu.stack;
        visible: false
        onGoBack: mainMenu.closeMenu()
        Component.onCompleted:
        {
            // WARNING: There is a known issue that when
            // this project's QML is run in the QML debugger, it does initialize
            // in the correct order and the following properties are null
            // For now, you will need to disable the QML debugger to run the debugger
            // as a workaround (in Projects | Run | Debugger Settings)
            stack.addPanel(mainPanel);
            mainPanel.exitClicked.connect(mainMenu.exitClicked);
        }
    }

    Config
    {
        id: config
        stack: mainMenu.stack
        anchors.fill: parent
    }

    Worker
    {
        id: worker
        stack: mainMenu.stack
        anchors.fill: parent
    }

//     Camera
//     {
//         id: camera
//         stack: mainMenu.stack
//         anchors.fill: parent
//     }
}
