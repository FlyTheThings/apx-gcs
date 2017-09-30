﻿import QtQuick 2.2
import QtGraphicalEffects 1.0
import "."

Rectangle {
    id: root
    property string text
    property color colorBG: doOpt?"gray":"#478fff"
    property color colorFG: (doOpt&&option)?"#8f8":"white"
    property variant option: "undefined"
    property string toolTip
    property string icon
    property bool effects: true
    property alias font: textItem.font

    property alias active: mouseArea.enabled

    property bool doOpt: option!=="undefined"

    signal clicked
    opacity: 0.5
    Behavior on opacity { NumberAnimation { duration: 100 } }
    implicitWidth: textItem.width+implicitHeight
    implicitHeight: 18
    radius: 4
    color: colorBG
    //color: "#225d9d"
    border.width: 0


    Rectangle {
        id: rect
        anchors.fill: parent
        anchors.margins: 1
        radius: root.radius
        color: "transparent"
        border.width: 1
        border.color: colorFG

    }
    Text {
        id: textItem
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 1
        text: root.text
        font.pixelSize: parent.height-2
        font.family: font_narrow
        color: colorFG
    }
    Image {
        id: img
        anchors.centerIn: parent
        source: icon
        sourceSize.height: parent.height*0.9
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: enabled
        visible: enabled
        propagateComposedEvents: true
        onEntered: root.opacity=1
        onExited:  root.opacity=0.5
        onClicked: {
            root.clicked()
        }
        ToolTipArea {
            text: root.toolTip
        }
    }

    FastBlur {
        anchors.fill: root
        transparentBorder: true
        source: img
        radius: img.height/2
        visible: effects && icon && (mouseArea.containsMouse || mouseArea.pressed)
    }

    FastBlur {
        anchors.fill: textItem
        transparentBorder: true
        source: textItem
        radius: textItem.height/2
        visible: effects && root.text && (mouseArea.containsMouse || mouseArea.pressed)
    }

    FastBlur {
        anchors.fill: rect
        transparentBorder: true
        source: rect
        radius: rect.height/2
        visible: effects && (mouseArea.containsMouse || mouseArea.pressed)
    }

}

