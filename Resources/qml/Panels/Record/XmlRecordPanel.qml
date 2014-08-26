import QtQuick 1.1
import "../../Controls"

Panel{

    signal xmlStartRecordClicked()
    signal  xmlStopAndSaveClicked()
    signal selectXmlFileClicked()
    signal playInSimulatorClicked()
    signal paintGeometryClicked()
    id: xmlRecordPanel

    property Panel pathsPanel

    title: "Xml Direct Record"

    visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
        //        ModelObject { text: "Start record"; startEnabled: false; type: "toggle"; Component.onCompleted: modelSignal.connect(geometryPanel.pointsToggled); },
        ModelObject { text: "Start record";  Component.onCompleted: noArgModelSignal.connect(xmlRecordPanel.xmlStartRecordClicked); },
        ModelObject { text: "Stop and Save"; Component.onCompleted: noArgModelSignal.connect(xmlRecordPanel.xmlStopAndSaveClicked);},
        ModelObject { text: "Select Xml File"; Component.onCompleted: noArgModelSignal.connect(xmlRecordPanel.selectXmlFileClicked);},
        ModelObject { text: "Paint geometry"; Component.onCompleted: noArgModelSignal.connect(xmlRecordPanel.paintGeometryClicked);},
        ModelObject { text: "Play in Simulator "; Component.onCompleted:  noArgModelSignal.connect(xmlRecordPanel.playInSimulatorClicked);}

        //        ModelObject { text: "OK"; Component.onCompleted: noArgModelSignal.connect(geometryPanel.okClicked);onNoArgModelSignal: {geometryPanel.goBack(); stack.addPanel(pathsPanel) }}
    ]
}
