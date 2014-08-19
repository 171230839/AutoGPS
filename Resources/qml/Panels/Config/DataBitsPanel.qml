
import QtQuick 1.1
import "../../Controls"

Panel
{
    signal contentUpdated()

    property string selectedContent: serialPortThread.qDataBits
    function updateSelectedContent(updatedText)
    {
        console.log("updateSelectedContent: " + updatedText );
        selectedContent = updatedText
        stack.removePanel()
        contentUpdated()
    }
    id: dataBitsPanel
    title: "DataBits"
    onGoBack: stack.removePanel()
    anchors.fill:  parent
    visible: false
    delegate: MultiDelegate{}
    model: [
        ModelObject { text: "6";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "7";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "8";onModelSignal: updateSelectedContent(text); }
    ]
}
