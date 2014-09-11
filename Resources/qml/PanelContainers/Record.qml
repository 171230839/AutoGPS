 import QtQuick 1.1
import "../Controls"
import "../Panels/Record"

Item{
    signal startRecordClicked()
    signal  stopAndSaveClicked()
    signal translateToXmlClicked()
    signal selectLogFileClicked()

    signal xmlStartRecordClicked()
    signal  xmlStopAndSaveClicked()
    signal selectXmlFileClicked()

//    signal paintGeometryClicked()
    signal selectProjectClicked()

    signal selectPointsToggled(bool state)
    signal paintCropLandClicked()
    signal unSelectClicked()
    signal selectStartPointClicked()
    signal getPathClicked()
    property Panel panel
    property Stack stack

    function onAddCropLandPanel()
    {
        stack.addPanel(cropLandPanel)
    }

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
            selectLogFileClicked.connect(record.selectLogFileClicked)
        }
    }
    XmlRecordPanel
    {
        id: xmlRecordPanel
        stack: record.stack
        anchors.fill:  parent
//        cropLandPanel:  cropLandPanel
//        routePanel:  routePanel
        Component.onCompleted:
        {
            xmlStartRecordClicked.connect(record.xmlStartRecordClicked)
            xmlStopAndSaveClicked.connect(record.xmlStopAndSaveClicked)
            selectXmlFileClicked.connect(record.selectXmlFileClicked)
//            playInSimulatorClicked.connect(record.playInSimulatorClicked)
//            paintGeometryClicked.connect(record.paintGeometryClicked)
            selectProjectClicked.connect(record.selectProjectClicked)
        }
    }

    CropLandPanel
    {
        id: cropLandPanel
        stack: record.stack
        anchors.fill:  parent
        Component.onCompleted:
        {
            selectPointsToggled.connect(record.selectPointsToggled)
            paintCropLandClicked.connect(record.paintCropLandClicked)
            unSelectClicked.connect(record.unSelectClicked)
            selectStartPointClicked.connect(record.selectStartPointClicked)
            getPathClicked.connect(record.getPathClicked)

        }
    }
//    RoutePanel
//    {
//        id: routePanel
//        stack: record.stack
//        anchors.fill:  parent
//    }
}
