import QtQuick 1.1

import "CompositeControls"
import "PanelContainers"
import "Controls"

LayoutItem
{
    signal basemapChanged(string name)
    signal homeClicked()
    signal zoomInClicked()
    signal zoomOutClicked()
    signal panClicked(string direction)

    property string currentItem : "Map"

    function updateLocation(newLocation)
    {
        statusBar.currentLocation = "Location: " + newLocation
    }

    function updateSpeed(newSpeed)
    {
        //TODO we should use the actual speed, as follows:
        //statusBar.currentSpeed = "   Speed: " + (Math.round(10.0 * newSpeed) / 10.0);
        //However, the app provides speeds that are unrealistically fast and highly variable.
        //Therefore, fake it as follows:
        //        var rand = Math.random();
        //        if (rand < 0.005)
        //            lastFakeSpeed--;
        //        else if (rand > 0.995)
        //            lastFakeSpeed++;
        statusBar.currentSpeed = "      Speed: " + newSpeed;
    }


    function updateHeading(newHeading)
    {
        statusBar.currentHeading = " Heading: " + newHeading
    }

    function updateMapRotation(newRotation)
    {
        northArrow.rotation = 360 - newRotation;
    }

    function updateTime(newTime)
    {
        statusBar.currentTime = "Time: " + newTime
    }

    //    function updateBasemapList(newList)
    //    {
    //        btnBasemaps.scrollList = newList
    //    }



    id: window
    width: 1024
    height: 768
    minimumSize: "600x400"
    preferredSize: "800x600"
    onCurrentItemChanged:
    {
        if (currentItem === "Map")
            classificationBar.lblClassification = "AutoGPS - Map View"
        else if (currentItem === "Camera")
            classificationBar.lblClassification = "AutoGPS - Camera View"
        else if (currentItem === "3D")
            classificationBar.lblClassification = "AutoGPS - 3D View"
    }

    ClassificationBar
    {
        id: classificationBar
        width: parent.width
        lblClassification: "AutoGPS - Map View"
        classificationColor: Qt.rgba(0, 1, 0, 1)
        ScrollButton
        {
            id: btnBasemaps
            scrollButtonDefaultIcon: "../../icons/Basemap-Normal.png"
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.verticalCenter:  parent.verticalCenter
            //        visible: menu.state != "open"
            Component.onCompleted:
            {
                itemChanged.connect(window.basemapChanged);
            }
            onItemChanged:
            {
                window.currentItem = name;
            }
        }
    }

    ToggleButton
    {
        id: btnMenu
        buttonDefaultIcon: "../../icons/Menu-Normal.png"
        buttonActiveIcon: "../../icons/Menu-Pressed.png"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 3
        visible: window.currentItem === "Map"
        Component.onCompleted:
        {
            itemClicked.connect(menu.invoke)
        }
        z: 2

        Menu
        {
            id: menu
            windowHeight: window.height - classificationBar.height - btnMenu.height - 15
            windowWidth: window.width / 4
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.bottom
            z: 2
            onMenuClosed:
            {
                while (stack.count() > 1)
                {
                    stack.removePanel()
                }
            }

            Stack
            {
                id: stack
            }

            MainMenu
            {
                id: mainMenu
                stack: stack
                anchors.fill: parent
                onCloseMenu: btnMenu.setToggled(false)
            }
        }
    }


    PushButton
    {
        id: btnHome
        buttonDefaultIcon: "../../icons/Home-Normal.png"
        buttonActiveIcon: "../../icons/Home-Pressed.png"
        anchors.left: btnMenu.right
        anchors.bottom: parent.bottom
        anchors.margins: 3
        visible: window.currentItem === "Map"
        Component.onCompleted:
        {
            itemClicked.connect(window.homeClicked)
            itemClicked.connect(mainMenu.clicked)
        }
    }



    // Layer toggling, add back in if desired:


    StatusBar
    {
        id: statusBar
        width: 400
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: 3
        visible: window.currentItem === "Map"
    }

    NavigationControl2
    {
        id: navControl
        objectName: "navControl"
        anchors.bottom:parent.bottom
        anchors.right: parent.right
        anchors.margins: 3
        visible: window.currentItem === "Map"
        Component.onCompleted:
        {
            zoomInClicked.connect(window.zoomInClicked)
            zoomOutClicked.connect(window.zoomOutClicked)
            panClicked.connect(window.panClicked)
            clicked.connect(mainMenu.clicked)
        }
    }
    Image {
        id: northArrow
        source: "qrc:/Resources/icons/North-Arrow.png"
        anchors.bottomMargin: 25
        anchors.bottom: navControl.top
        anchors.horizontalCenter: navControl.horizontalCenter
        width: navControl.width * .75
        height: navControl.width * .75
        smooth: true
        fillMode: Image.PreserveAspectFit
        visible: window.currentItem === "Map"
    }
     Camera
     {
         id: camera
         anchors.top: classificationBar.bottom
         anchors.horizontalCenter: parent.horizontalCenter
         windowWidth: window.width / 2
         windowHeight: window.height /4
         visible: window.currentItem === "Camera"
         width: window.width
         height:  window.height / 4
     }


}
