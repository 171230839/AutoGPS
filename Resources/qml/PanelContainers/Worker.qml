import QtQuick 1.1
import "../Panels/Worker"
import "../Controls"


Item{
    property Panel panel
    property Stack stack
    id: worker
    panel: workerPanel

    WorkerPanel
    {
        id: workerPanel
        stack: worker.stack
         anchors.fill: parent
        geometryPanel: geometryPanel
        pathsPanel: pathsPanel
    }

    GeometryPanel
    {
        id: geometryPanel
        stack: worker.stack
         anchors.fill: parent
         pathsPanel: pathsPanel
    }

    PathsPanel
    {
        id: pathsPanel
        stack: worker.stack
        anchors.fill:parent
    }

}
