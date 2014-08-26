import QtQuick 1.1
import "../Controls"
import "../Panels/Record"

Item{
    signal startRecordClicked()
    signal  stopAndSaveClicked()
    signal translateToXmlClicked()

    signal xmlStartRecordClicked()
    signal  xmlStopAndSaveClicked()
    signal selectXmlFileClicked()
    signal playInSimulatorClicked()
    signal paintGeometryClicked()
    property Panel panel
    property Stack stack


    id: record
    panel: recordPanel
    objectName: "record"
    RecordPanel
    {
        id: recordPanel
        stack: record.stack
        anchors.fill: parent
        serialRecordPanel: serialRecordPanel
        xmlRecordPanel:  xmlRecordPanel
    }

    SerialRecordPanel
    {
        id: serialRecordPanel
        stack: record.stack
        anchors.fill:  parent
        xmlRecordPanel: xmlRecordPanel
        Component.onCompleted:
        {
            startRecordClicked.connect(record.startRecordClicked)
            stopAndSaveClicked.connect(record.stopAndSaveClicked)
            translateToXmlClicked.connect(record.translateToXmlClicked)
        }
    }
    XmlRecordPanel
    {
        id: xmlRecordPanel
        stack: record.stack
        anchors.fill:  parent
        Component.onCompleted:
        {
            xmlStartRecordClicked.connect(record.xmlStartRecordClicked)
            xmlStopAndSaveClicked.connect(record.xmlStopAndSaveClicked)
            selectXmlFileClicked.connect(record.selectXmlFileClicked)
            playInSimulatorClicked.connect(record.playInSimulatorClicked)
            paintGeometryClicked.connect(record.paintGeometryClicked)
        }
    }
}
