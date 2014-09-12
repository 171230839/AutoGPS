import QtQuick 1.1
import "../Controls"
import "../Panels/Player"

Item{
    property Stack stack
    property Panel panel

     id: player
     panel: playerPanel
    objectName: "player"

    PlayerPanel
    {
        id: playerPanel
        stack: player.stack
        anchors.fill: parent
    }
}
