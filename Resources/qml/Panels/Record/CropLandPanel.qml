import QtQuick 1.1
import "../../Controls"

Panel{
    signal selectPointsToggled(bool state)
    signal paintCropLandClicked()
    signal unSelectClicked()
    signal selectStartPointClicked()
    signal getPathClicked()
    id: cropLandPanel

    title: "CropLand  "

  visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
        ModelObject { text: "Select Points"; startEnabled: false; type: "toggle"; Component.onCompleted:modelSignal.connect(cropLandPanel.selectPointsToggled); },
         ModelObject { text: "Paint CropLand"; Component.onCompleted: noArgModelSignal.connect(cropLandPanel.paintCropLandClicked);},
        ModelObject { text: "unSelect"; Component.onCompleted: noArgModelSignal.connect(cropLandPanel.unSelectClicked);},
        ModelObject { text: "Select Start Point"; Component.onCompleted:  noArgModelSignal.connect(cropLandPanel.selectStartPointClicked);},
        ModelObject { text: "Get Path"; Component.onCompleted:  noArgModelSignal.connect(cropLandPanel.getPathClicked);}
        ]
}
