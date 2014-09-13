import QtQuick 1.1
import "../../Controls"

Panel{


    id: playerPanel
    signal userSelectProjectClicked(string user)
    signal getCroplandsClicked()
    title: "playerPanel"

  visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
        ModelObject { text: "Select projects"; onNoArgModelSignal: userSelectProjectClicked("player") ;},
//        ModelObject { text: "Select projects"; startEnabled: false; type: "toggle"; Component.onCompleted: modelSignal.connect(geometryPanel.pointsToggled); },
    ModelObject { text: "Get Croplands"; onNoArgModelSignal: getCroplandsClicked(); }
       ]
}
