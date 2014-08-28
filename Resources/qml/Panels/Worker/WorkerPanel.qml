import QtQuick 1.1
import "../../Controls"

Panel{

    property Stack stack
    property Panel geometryPanel
//    property Panel pathsPanel
    id: workerPanel
    objectName: "workerPanel"
    title: "Start Work"
    visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
        ModelObject { text: "get Geometry "; onNoArgModelSignal: { stack.addPanel(geometryPanel)}}
//        ModelObject { text: "get Paths"; onNoArgModelSignal: { stack.addPanel(pathsPanel)}}
    ]


}
