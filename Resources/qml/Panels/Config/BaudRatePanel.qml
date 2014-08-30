import QtQuick 1.1
import "../../Controls"

Panel
{
    signal contentUpdated()

    property string selectedContent: serialPortThread.qBaudRate()
    function updateSelectedContent(updatedText)
    {
        console.log("updateSelectedContent: " + updatedText );
        selectedContent = updatedText
        stack.removePanel()
        contentUpdated()
    }
    id: baudRatePanel
    title: qsTr("BaudRate")
    onGoBack: stack.removePanel()
    anchors.fill:  parent
    visible: false
    delegate: MultiDelegate{}
    model: [
        ModelObject { text: "1200";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "2400";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "4800";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "9600";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "19200";onModelSignal: updateSelectedContent(text); }
    ]
}
