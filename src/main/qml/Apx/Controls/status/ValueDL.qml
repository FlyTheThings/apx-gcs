﻿import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

import APX.Vehicles 1.0
import Apx.Common 1.0

ValueButton {
    id: control

    fact: apx.datalink

    text: qsTr("DL")
    toolTip: apx.datalink.descr+":"
           + "\n" + qsTr("Received packets")
           + "\n" + qsTr("Stream errors")
           + "\n" + qsTr("Transmitted packets")


    active: !(apx.datalink.ports.active || apx.datalink.hosts.active)
    warning: apx.datalink.readonly.value

    property bool light: active||warning

    enabled: true
    onPressAndHold: apx.vehicles.current.errcnt=0

    readonly property int errcnt: apx.vehicles.current.protocol.errcnt

    readonly property color cGreen: light?Material.color(Material.Yellow):Material.color(Material.LightGreen)
    readonly property color cRed: light?Material.color(Material.Yellow):Material.color(Material.DeepOrange)
    readonly property color cYellow: Material.color(Material.Amber)
    readonly property color cCyan: light?"#fff":Material.color(Material.Cyan)
    readonly property color cGrey: Material.color(Material.Grey)

    valueC: Component {
        Row {
            spacing: 0
            layoutDirection: Qt.RightToLeft
            Text {
                height: parent.height
                font.family: font_narrow
                font.pixelSize: valueSize
                verticalAlignment: Text.AlignVCenter
                text: "0%1".arg(apx.datalink.stats.uplink.cnt.value % 100).slice(-2)+" "
                color: apx.datalink.hbeat.value?cGreen:cGrey
            }
            Text {
                height: parent.height
                font.family: font_narrow
                font.pixelSize: valueSize
                verticalAlignment: Text.AlignVCenter
                property int value: errcnt%10
                text: value+" "
                color: errcnt>1?(errTimer.running?cRed:cYellow):cGrey
                Behavior on color { enabled: ui.smooth; ColorAnimation {duration: 250} }
                Timer {
                    id: errTimer
                    interval: 5000
                    repeat: false
                }
                onValueChanged: errTimer.restart()
            }
            Text {
                height: parent.height
                font.family: font_narrow
                font.pixelSize: valueSize
                verticalAlignment: Text.AlignVCenter
                text: "0%1".arg(apx.datalink.stats.dnlink.cnt.value%100).slice(-2)+" "
                color: apx.datalink.online?(apx.vehicles.current.protocol.streamType===ProtocolVehicle.TELEMETRY?cGreen:cCyan):cRed
            }
        }
    }
}
