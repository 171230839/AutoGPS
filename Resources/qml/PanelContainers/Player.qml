import QtQuick 1.1
import "../Controls"
import "../Panels/Player"

Item{
    property Stack stack
    property Panel panel
    signal playerSelectProjectClicked(string user)
    signal getCroplandsClicked()

    signal selectPointsClicked()
    signal paintCropLandClicked()
    signal unSelectClicked()
    signal selectStartPointClicked()
    signal getPathClicked()
    signal pathSaveProjectClicked()
    signal croplandGoBackClicked()

    id: player
    panel: playerPanel
    objectName: "player"
    function onAddPlayerCroplandPanel()
    {
        stack.addPanel(playerCroplandPanel)
    }

    PlayerPanel
    {
        id: playerPanel
        stack: player.stack
        anchors.fill: parent
        croplandPanel: playerCroplandPanel
        Component.onCompleted: {
            playerSelectProjectClicked.connect(player.playerSelectProjectClicked);
            getCroplandsClicked.connect(player.getCroplandsClicked);
        }
    }
    PlayerCroplandPanel
    {
        id: playerCroplandPanel
        stack: player.stack
        anchors.fill:  parent
        Component.onCompleted: {
            selectPointsClicked.connect(player.selectPointsClicked)
            paintCropLandClicked.connect(player.paintCropLandClicked)
            unSelectClicked.connect(player.unSelectClicked)
            selectStartPointClicked.connect(player.selectStartPointClicked)
            getPathClicked.connect(player.getPathClicked)
            pathSaveProjectClicked.connect(player.pathSaveProjectClicked)
            croplandGoBackClicked.connect(player.croplandGoBackClicked)
        }
    }
}
