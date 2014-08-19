import QtQuick 1.1
import "../../Controls"

Panel
{
    signal selectPointToggled(bool state)
    signal getPathClicked()
    signal unSelectClicked()

    id: pathsPanel
    title: "get Paths"
    objectName: "pathsPanel"
    visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
        ModelObject { text: "select point"; startEnabled: false; type: "toggle"; Component.onCompleted: modelSignal.connect(pathsPanel.selectPointToggled); },
        ModelObject { text: "get Path"; Component.onCompleted: noArgModelSignal.connect(pathsPanel.getPathClicked);},
        ModelObject { text: "unSelect"; Component.onCompleted: noArgModelSignal.connect(pathsPanel.unSelectClicked);}
    ]
}
