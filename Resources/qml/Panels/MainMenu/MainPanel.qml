import QtQuick 1.1
import "../../Controls"

Panel
{
    signal exitClicked()

    property Panel configPanel
    property Panel workerPanel

    id: mainPanel
    title: "Main Menu"
    delegate: MultiDelegate { }
    model:
        [
          ModelObject { text: "Start Work"; onNoArgModelSignal: stack.addPanel(workerPanel);},
        ModelObject { text: "Config"; onNoArgModelSignal: stack.addPanel(configPanel); }

    ]

    PushButton
    {
        id: btnExit
        buttonText: "Quit Application"
        buttonDefaultIcon: "../../icons/Menu-Button-Normal.png"
        buttonActiveIcon: "../../icons/Menu-Button-Pressed.png"
        width: 150
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: 10
        onItemClicked:
        {
            mainPanel.exitClicked()
        }
    }
}
