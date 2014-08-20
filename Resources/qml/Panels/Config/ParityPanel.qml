import QtQuick 1.1
import "../../Controls"

Panel
{
    signal contentUpdated()

    property string selectedContent: serialPortThread.qParity()
    function updateSelectedContent(updatedText)
    {
        console.log("updateSelectedContent: " + updatedText );
        selectedContent = updatedText
        stack.removePanel()
        contentUpdated()
    }
    id: parityPanel
    title: "Parity"
    onGoBack: stack.removePanel()
    anchors.fill:  parent
    visible: false
    delegate: MultiDelegate{}
    model: [
        ModelObject { text: "NONE";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "ODD";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "EVEN";onModelSignal: updateSelectedContent(text); }
    ]
}
