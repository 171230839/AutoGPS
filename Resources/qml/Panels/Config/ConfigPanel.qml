import QtQuick 1.1
import "../../Controls"

Panel
{
    signal showMeToggled(bool state)
     signal followMeToggled(bool state)
      signal resetMapClicked()

    property Panel serialConfig

    id: configPanel
    title: "serialPort"
    visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:
    [
        ModelObject { text: "SerialPort Conifg"; onNoArgModelSignal: stack.addPanel(serialConfig); },
         ModelObject { text: "Show Me"; startEnabled: true; type: "toggle"; Component.onCompleted: modelSignal.connect(configPanel.showMeToggled); },
         ModelObject { text: "Follow Me"; startEnabled: false; type: "toggle"; Component.onCompleted: modelSignal.connect(configPanel.followMeToggled); },
        ModelObject { text: "Reset Map"; Component.onCompleted: noArgModelSignal.connect(configPanel.resetMapClicked); }
    ]
}
