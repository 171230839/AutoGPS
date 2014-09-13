import QtQuick 1.1
import "../Controls"
import "../Panels/Player"

Item{
    property Stack stack
    property Panel panel
    signal userSelectProjectClicked(string user)
       signal getCroplandsClicked()
     id: player
     panel: playerPanel
    objectName: "player"

    PlayerPanel
    {
        id: playerPanel
        stack: player.stack
        anchors.fill: parent
        Component.onCompleted: {
            userSelectProjectClicked.connect(player.userSelectProjectClicked);
            getCroplandsClicked.connect(player.getCroplandsClicked);
        }
    }
}
