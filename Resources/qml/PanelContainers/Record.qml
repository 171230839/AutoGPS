import QtQuick 1.1
import "../Controls"
import "../Panels/Record"

Item{
    signal startRecordClicked()
    signal  stopAndSaveClicked()
    signal playInSimulatorClicked()


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
    }

    SerialRecordPanel
    {
        id: serialRecordPanel
        stack: record.stack
        anchors.fill:  parent
        Component.onCompleted:
        {
            startRecordClicked.connect(record.startRecordClicked)
            stopAndSaveClicked.connect(record.stopAndSaveClicked)
            playInSimulatorClicked.connect(record.playInSimulatorClicked)
        }
    }
}
