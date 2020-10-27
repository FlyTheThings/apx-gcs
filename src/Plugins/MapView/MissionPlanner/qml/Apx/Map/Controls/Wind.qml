/*
 * APX Autopilot project <http://docs.uavos.com>
 *
 * Copyright (c) 2003-2020, Aliaksei Stratsilatau <sa@uavos.com>
 * All rights reserved
 *
 * This file is part of APX Ground Control.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick          2.12

import Apx.Common 1.0

Item {
    id: control

    readonly property bool m_status: mandala.est.wind.status.value > 0
    readonly property real m_wspd: mandala.est.wind.speed.value
    readonly property real m_whdg: mandala.est.wind.heading.value

    implicitWidth: 100
    implicitHeight: implicitWidth

    readonly property int spacing: 0
    readonly property int fontSize: 28*ui.scale

    //internal
    readonly property int arrowSize: height-textItem.height-spacing

    readonly property color color: m_status?"#fff":"yellow"

    property bool smoothMove: true
    Timer {
        id: smoothMoveTimer
        running: true
        interval: 1000
        onTriggered: smoothMove=true
    }
    Connections {
        target: apx.vehicles
        function onVehicleSelected() {
            smoothMove=false
            smoothMoveTimer.restart()
        }
    }

    Text {
        id: textItem
        color: control.color
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: m_wspd.toFixed(1)
        font.family: font_narrow
        font.pixelSize: fontSize
        font.bold: true
    }
    Text {
        id: textHdg
        color: control.color
        text: (Math.round(apx.angle360(image.v+180)/5)*5).toFixed(0)
        font.family: font_narrow
        font.pixelSize: fontSize*0.6
        rotation: image.rotation
        anchors.centerIn: image
    }
    SvgImage {
        id: image
        color: "#fff" //"#fd6"
        source: "../icons/wind-arrow.svg"
        sourceSize.height: Math.round(arrowSize)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        rotation: v-map.bearing
        property real v: m_whdg
        Behavior on v { enabled: ui.smooth && control.smoothMove; RotationAnimation {duration: 1000; direction: RotationAnimation.Shortest; } }
    }
}
