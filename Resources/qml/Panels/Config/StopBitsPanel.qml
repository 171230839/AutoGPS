import QtQuick 1.1
import "../../Controls"

Panel
{
    signal contentUpdated()

    property string selectedContent: serialPortThread.qStopBits()
    function updateSelectedContent(updatedText)
    {
        console.log("updateSelectedContent: " + updatedText );
        selectedContent = updatedText
        stack.removePanel()
        contentUpdated()
    }
    id: stopBitsPanel
    title: "StopBits"
    onGoBack: stack.removePanel()
    anchors.fill:  parent
    visible: false
    delegate: MultiDelegate{}
    model: [
        ModelObject { text: "1";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "2";onModelSignal: updateSelectedContent(text); }
    ]
}
