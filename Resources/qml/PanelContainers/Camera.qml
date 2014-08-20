import QtQuick 1.1
import "../Controls"

LayoutItem{
    property int windowWidth
    property int windowHeight
    id :camera

    Rectangle{
        id: cameraLeft
        color: Qt.rgba(0.5, 0.5, 0.5, 0.75)
        border.color:  Qt.rgba(0, 0, 0, 0.5)
        border.width: 2
        width: windowWidth
        height: windowHeight
        anchors.left: parent.left
        radius: 10
        state: "open"

          Text
          {
            id: nameLeft
            height: 20
            text: cameraObject.getCameraLeftD()
//            text: "ssssssfr"
            anchors.top: parent.top
             anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignBottom
            font.pixelSize: 26
            font.bold: true
            color: "white"
          }

    //    states:
    //    [
    //      State
    //      {
    //        name: "open"
    //        PropertyChanges { target: camera; width: windowWidth; height: windowHeight; visible: true }
    //        AnchorChanges {
    //          target: camera;
    //          anchors.left:  parent.left;
    //          anchors.right: parent.right;
    //          anchors.top: parent.top;
    //          anchors.bottom: undefined;
    //          anchors.horizontalCenter: parent.horizontalCenter
    //        }
    //      },

    //      State
    //      {
    //        name: "closed"
    //        PropertyChanges { target: camera; width: 50; height: 50; visible: false }
    //        AnchorChanges { target: camera; anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.top; anchors.top: undefined;  }
    //      }
    //    ]

    //    transitions:
    //    [
    //      Transition
    //      {
    //        to: "open"
    //        ParallelAnimation
    //        {
    //          AnchorAnimation { duration: 250 }
    //          NumberAnimation { target: camera; property: "width"; to: windowWidth; duration: 250 }
    //          NumberAnimation { target: camera; property: "height"; to: windowHeight; duration: 250 }
    //        }
    //      },

    //      Transition
    //      {
    //        to: "closed"

    //        SequentialAnimation
    //        {
    //          ParallelAnimation
    //          {
    //            AnchorAnimation { duration: 250 }
    //            NumberAnimation { target: camera; property: "width"; to: 50; duration: 250 }
    //            NumberAnimation { target: camera; property: "height"; to: 50; duration: 250 }
    //         }

    //         PropertyAction { target: camera; property: "visible"; value: false }
    //       }
    //      }
    //    ]
    }
    Rectangle{
        id: cameraRight
        color: Qt.rgba(0.5, 0.5, 0.5, 0.75)
        border.color:  Qt.rgba(0, 0, 0, 0.5)
        border.width: 2
        width: windowWidth
        height: windowHeight
        radius: 10
        state: "open"
        anchors.right: parent.right
        Text
        {
          id: nameRight
          height: 20
          text: cameraObject.getCameraRightD()
          anchors.top: parent.top
           anchors.horizontalCenter: parent.horizontalCenter
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignBottom
          font.pixelSize: 26
          font.bold: true
          color: "white"
        }
    }
}



