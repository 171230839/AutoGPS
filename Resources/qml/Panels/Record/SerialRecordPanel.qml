import QtQuick 1.1
import "../../Controls"

Panel{

    signal startRecordClicked()
    signal  stopAndSaveClicked()
    signal translateToXmlClicked()
    id: serialRecordPanel

    property Panel xmlRecordPanel


    title: "SerialPort Record"

    visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
//        ModelObject { text: "Start record"; startEnabled: false; type: "toggle"; Component.onCompleted: modelSignal.connect(geometryPanel.pointsToggled); },
        ModelObject { text: "Start record";  Component.onCompleted: noArgModelSignal.connect(serialRecordPanel.startRecordClicked); },
        ModelObject { text: "Stop and Save"; Component.onCompleted: noArgModelSignal.connect(serialRecordPanel.stopAndSaveClicked);},
        ModelObject { text: "TranslateToXml"; Component.onCompleted: noArgModelSignal.connect(serialRecordPanel.translateToXmlClicked); onNoArgModelSignal: {serialRecordPanel.goBack(); stack.addPanel(xmlRecordPanel)}}
//        ModelObject { text: "OK"; Component.onCompleted: noArgModelSignal.connect(geometryPanel.okClicked);onNoArgModelSignal: {geometryPanel.goBack(); stack.addPanel(pathsPanel) }}
]
}
