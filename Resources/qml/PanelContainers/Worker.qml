import QtQuick 1.1
import "../Panels/Worker"
import "../Controls"


Item{
     signal createProjectClicked()
    signal saveProjectClicked()
    signal pointsToggled(bool state)
    signal toLinesClicked()
    signal toPolygonClicked()
    signal clearClicked()
    property Panel panel
    property Stack stack
    id: worker
    panel: workerPanel
    objectName: "worker"

    WorkerPanel
    {
        id: workerPanel
        stack: worker.stack
         anchors.fill: parent
        geometryPanel: geometryPanel
        Component.onCompleted:
        {
            createProjectClicked.connect(worker.createProjectClicked)
        }
    }

    GeometryPanel
    {
        id: geometryPanel
        stack: worker.stack
         anchors.fill: parent
         Component.onCompleted:
         {
             saveProjectClicked.connect(worker.saveProjectClicked)
            pointsToggled.connect(worker.pointsToggled)
             toLinesClicked.connect(worker.toLinesClicked)
             toPolygonClicked.connect(worker.toPolygonClicked)
            clearClicked.connect(worker.clearClicked)
         }
    }

//    PathsPanel
//    {
//        id: pathsPanel
//        stack: worker.stack
//        anchors.fill:parent
//    }

}
