import QtQuick 1.1
import "../../Controls"

Panel
{
    signal contentUpdated()

    property string selectedContent: serialPortThread.qTimeout()
    function updateSelectedContent(updatedText)
    {
        console.log("updateSelectedContent: " + updatedText );
        selectedContent = updatedText
        stack.removePanel()
        contentUpdated()
    }
    id: timeOutPanel
    title: "timeOut"
    onGoBack: stack.removePanel()
    anchors.fill:  parent
    visible: false
    delegate: MultiDelegate{}
    model: [
        ModelObject { text: "200";onModelSignal: updateSelectedContent(text); },
         ModelObject { text: "300";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "500";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "800";onModelSignal: updateSelectedContent(text); },
        ModelObject { text: "1000";onModelSignal: updateSelectedContent(text); }
    ]
}
