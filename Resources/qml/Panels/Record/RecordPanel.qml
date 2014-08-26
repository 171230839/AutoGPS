import QtQuick 1.1
import "../../Controls"

Panel{
    id: recordPanel
    property Stack stack
    property Panel serialRecordPanel
    property Panel xmlRecordPanel


    title: "Record"
    visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
        ModelObject { text: "SerialPort Record"; onNoArgModelSignal: { stack.addPanel(serialRecordPanel)}},
        ModelObject { text: "Xml Record"; onNoArgModelSignal: {stack.addPanel(xmlRecordPanel) }}
    ]
}
