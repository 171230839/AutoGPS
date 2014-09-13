import QtQuick 1.1
import "../../Controls"

Panel{
    signal selectPointsClicked()
    signal paintCropLandClicked()
    signal unSelectClicked()
    signal selectStartPointClicked()
    signal getPathClicked()
    signal pathSaveProjectClicked()
    id: cropLandPanel

    title: "CropLand  "

  visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
//        ModelObject { text: "Select Points"; startEnabled: false; type: "toggle"; Component.onCompleted:modelSignal.connect(cropLandPanel.selectPointsToggled); },
        ModelObject { text: "Select Points"; Component.onCompleted: noArgModelSignal.connect(cropLandPanel.selectPointsClicked);},

        ModelObject { text: "Paint CropLand"; Component.onCompleted: noArgModelSignal.connect(cropLandPanel.paintCropLandClicked);},
        ModelObject { text: "Select Start Point"; Component.onCompleted:  noArgModelSignal.connect(cropLandPanel.selectStartPointClicked);},
        ModelObject { text: "Get Path"; Component.onCompleted:  noArgModelSignal.connect(cropLandPanel.getPathClicked);},
         ModelObject { text: "Clear"; Component.onCompleted: noArgModelSignal.connect(cropLandPanel.unSelectClicked);},
        ModelObject { text: "Save"; Component.onCompleted: noArgModelSignal.connect(cropLandPanel.pathSaveProjectClicked);}
        ]
}
