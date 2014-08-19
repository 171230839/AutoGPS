import QtQuick 1.1
import "../../Controls"

Panel
{
     signal openSerialPort()

    property Panel portNoPanel
    property Panel baudRatePanel
    property Panel dataBitsPanel
    property Panel stopBitsPanel
    property Panel parityPanel
    property Panel timeOutPanel

//    property list<QtObject> portNoPanelModel
    id: serialConfigPanel
    title: "SerialPort Config"
    anchors.fill:  parent
    visible: false
    onGoBack:  stack.removePanel()
    delegate: MultiDelegate{}

    function openSerialPortPanel()
    {
//        var modelData = new Array();
//        for(var i=0; i < serialPortNo.length; ++i)
//        {
//            var str = serialPortNo[i];
//            console.log("str:" + str);
//            var  componentTemp = Qt.createComponent("../../Controls/ModelObject.qml");

//             var object = componentTemp.createObject(serialConfigPanel, { text: "ssss"});
////            object.text = "sssss";
//                modelData[i] = object;
//        }
        console.log("model:" + model);
        var component = Qt.createComponent("PortNoPanel.qml");
         var panel = component.createObject(serialConfigPanel);
        stack.addPanel(panel);
    }

    model:
        [
        ModelObject {  text: "SerialPort No: " + portNoPanel.selectedContent ; onNoArgModelSignal: {stack.addPanel(portNoPanel)} },
        ModelObject {  text: "BaudRate:  " + baudRatePanel.selectedContent ; onNoArgModelSignal: {stack.addPanel(baudRatePanel)} },
        ModelObject { text: "DataBits: " + dataBitsPanel.selectedContent; onNoArgModelSignal: {stack.addPanel(dataBitsPanel)}},
        ModelObject {  text: "StopBits:  " + stopBitsPanel.selectedContent ; onNoArgModelSignal: {stack.addPanel(stopBitsPanel)} },
        ModelObject {  text: "Parity:  " + parityPanel.selectedContent ; onNoArgModelSignal: {stack.addPanel(parityPanel)} },
        ModelObject { text: "TimeOut: " + timeOutPanel.selectedContent; onNoArgModelSignal: { stack.addPanel(timeOutPanel)}}
    ]

    PushButton
    {
        id: btnOpen
        width: 150
        buttonText: "Open"
        buttonDefaultIcon: "../../icons/Menu-Button-Normal.png"
        buttonActiveIcon: "../../icons/Menu-Button-Pressed.png"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: 10
        onItemClicked: {
            console.log("serialCOnfig panel pushbutton clicked");
            serialConfigPanel.openSerialPort(); serialConfigPanel.goBack(); }
    }
}
