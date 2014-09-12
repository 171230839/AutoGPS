import QtQuick 1.1
import "../../Controls"

Panel{

    signal xmlStartRecordClicked()
    signal  xmlStopAndSaveClicked()
    signal selectXmlFileClicked()
//    signal toCroplandClicked()
//    signal paintGeometryClicked()
    signal selectProjectClicked(string user)
    id: xmlRecordPanel

//    property Panel cropLandPanel
//    property Panel routePanel
    title: "Xml Direct Record"

    visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
        ModelObject { text: "Start record";  Component.onCompleted: noArgModelSignal.connect(xmlRecordPanel.xmlStartRecordClicked); },
        ModelObject { text: "Stop and Save"; Component.onCompleted: noArgModelSignal.connect(xmlRecordPanel.xmlStopAndSaveClicked);},
        ModelObject { text: "Select Xml File"; Component.onCompleted: noArgModelSignal.connect(xmlRecordPanel.selectXmlFileClicked);},
//        ModelObject { text: "Paint geometry"; Component.onCompleted: noArgModelSignal.connect(xmlRecordPanel.paintGeometryClicked);},
        ModelObject { text: "Select Project "; onNoArgModelSignal: xmlRecordPanel.selectProjectClicked("worker");}
//            onNoArgModelSignal: {stack.addPanel(cropLandPanel)}}
//        ModelObject { text: "To Route"; onNoArgModelSignal: { stack.addPanel(routePanel)}}
     ]
}
