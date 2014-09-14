import QtQuick 1.1
import "../../Controls"

Panel{
    signal selectPointsClicked()
    signal paintCropLandClicked()
    signal unSelectClicked()
    signal selectStartPointClicked()
    signal getPathClicked()
    signal pathSaveProjectClicked()
    signal croplandGoBackClicked()

    id: playerCroplandPanel
    title: "playerCroplandPanel"

    visible: false;
      onGoBack: {stack.removePanel(); croplandGoBackClicked();}
      delegate: MultiDelegate{}
      model:[
  //        ModelObject { text: "Select Points"; startEnabled: false; type: "toggle"; Component.onCompleted:modelSignal.connect(playerCroplandPanel.selectPointsToggled); },
          ModelObject { text: "Select Points"; Component.onCompleted: noArgModelSignal.connect(playerCroplandPanel.selectPointsClicked)},
          ModelObject { text: "Paint CropLand"; Component.onCompleted: noArgModelSignal.connect(playerCroplandPanel.paintCropLandClicked);},
          ModelObject { text: "Select Start Point"; Component.onCompleted:  noArgModelSignal.connect(playerCroplandPanel.selectStartPointClicked);},
          ModelObject { text: "Get Path"; Component.onCompleted:  noArgModelSignal.connect(playerCroplandPanel.getPathClicked);},
           ModelObject { text: "Clear"; Component.onCompleted: noArgModelSignal.connect(playerCroplandPanel.unSelectClicked);},
          ModelObject { text: "Save"; Component.onCompleted: noArgModelSignal.connect(playerCroplandPanel.pathSaveProjectClicked); onNoArgModelSignal:{playerCroplandPanel.goBack();}}
          ]
}
