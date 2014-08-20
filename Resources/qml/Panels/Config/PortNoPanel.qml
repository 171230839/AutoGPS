import QtQuick 1.1
import "../../Controls"

Panel
{
    signal contentUpdated()

    property string selectedContent: serialPortThread.qPortName()

    function updateSelectedContent(updatedText)
    {
        console.log("updateSelectedContent: " + updatedText );
        selectedContent = updatedText
        stack.removePanel()
        contentUpdated()
    }

    function getModelData()
    {
       var component = Qt.createComponent("../../Controls/ModelObject.qml");
        var model = new Array();
        var serialPortNo = serialPortThread.portList();
//        console.log("serialPortNo: " + serialPortNo);
        for(var i=0; i < serialPortNo.length; ++i)
        {
            var str = serialPortNo[i];
//            console.log("str:  " + str);
            var object = component.createObject(portNoPanel, {text: str});
            object.modelSignal.connect(updateSelectedContent);
            model[i] = object
        }
        return model;
    }

    id: portNoPanel
    title: "SerialPort Number"
    onGoBack: stack.removePanel()
    anchors.fill:  parent
    visible: false
    delegate: MultiDelegate{}
    model: getModelData()

}
