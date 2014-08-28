import QtQuick 1.1
import "../../Controls"

Panel{


    id: routePanel


    title: "Route"

  visible: false;
    onGoBack: stack.removePanel()
    delegate: MultiDelegate{}
    model:[
       ]
}
