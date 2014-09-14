import QtQuick 1.1
import "../../Controls"

Panel{
    signal saveProjectClicked()
    signal pointsToggled(bool state)
    signal toLinesClicked()
    signal toPolygonClicked()
    signal clearClicked()
    signal geometryGoBackClicked()

//    property Panel pathsPanel
    id: geometryPanel
    title: "get Geometry"
    objectName: "geometryPanel"
    visible: false;
    onGoBack: {stack.removePanel(); geometryGoBackClicked();}
    delegate: MultiDelegate{}
    model:[
        ModelObject { text: "Points"; startEnabled: false; type: "toggle"; Component.onCompleted: modelSignal.connect(geometryPanel.pointsToggled); },
        ModelObject { text: "toLines";  Component.onCompleted: noArgModelSignal.connect(geometryPanel.toLinesClicked); },
        ModelObject { text: "toPolygon"; Component.onCompleted: noArgModelSignal.connect(geometryPanel.toPolygonClicked);},
        ModelObject { text: "Clear"; Component.onCompleted: noArgModelSignal.connect(geometryPanel.clearClicked);},
        ModelObject { text: "SaveProject"; Component.onCompleted: noArgModelSignal.connect(geometryPanel.saveProjectClicked); onNoArgModelSignal: {geometryPanel.goBack();}}
//        ModelObject { text: "OK"; Component.onCompleted: noArgModelSignal.connect(geometryPanel.okClicked);onNoArgModelSignal: {geometryPanel.goBack(); stack.addPanel(pathsPanel) }}
]
}
