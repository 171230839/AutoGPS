import QtQuick 1.1
import "../../Controls"

Panel{


    id: playerPanel


    title: "playerPanel"

  visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
        ModelObject { text: "ssss"; }

       ]
}
