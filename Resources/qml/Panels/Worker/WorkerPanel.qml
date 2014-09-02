import QtQuick 1.1
import "../../Controls"

Panel{
    signal createProjectClicked()
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
        ModelObject { text: "Create Project"; Component.onCompleted: noArgModelSignal.connect(workerPanel.createProjectClicked); onNoArgModelSignal: { stack.addPanel(geometryPanel)}}
//        ModelObject { text: "get Paths"; onNoArgModelSignal: { stack.addPanel(pathsPanel)}}
    ]


}
