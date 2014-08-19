import QtQuick 1.1
import "../Panels/Config"
import "../Controls"


Item{

    signal showMeToggled(bool state)
     signal followMeToggled(bool state)
     signal resetMapClicked()

    property Stack stack
    property Panel panel

     id: config
     panel: configPanel
    objectName: "config"

     ConfigPanel
     {
         id: configPanel
         stack: config.stack
         anchors.fill: parent
         serialConfig: serialConfig.panel
         Component.onCompleted:
         {
            followMeToggled.connect(config.followMeToggled)
            showMeToggled.connect(config.showMeToggled)
             resetMapClicked.connect(config.resetMapClicked)
         }

     }
     SerialConfig
     {
         id: serialConfig
         stack: config.stack
         anchors.fill:  parent
     }
}
