import QtQuick 2.5;
import QtQuick.Layouts 1.3
import QtPositioning 5.6

import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2

import APX.Vehicles 1.0
import APX.Mission 1.0

import Apx.Common 1.0
import Apx.Controls 1.0
//import Apx.Map 1.0

ColumnLayout {
    spacing: 4
    property Vehicle vehicle: apx.vehicles.current
    property Mission mission: vehicle.mission
    property bool empty: mission?mission.empty:true

    function focusOnMap(fact)
    {
        fact.trigger()
    }

    ListView {
        id: missionListView
        Layout.fillHeight: true
        model: mission.listModel
        implicitWidth: contentItem.childrenRect.width
        orientation: ListView.Vertical
        spacing: 4 * ui.scale
        clip: true
        snapMode: ListView.SnapToItem
        visible: !empty


        delegate: RowLayout{
            FactButton {
                id: missionItem

                size: 24 * ui.scale
                //titleSize: textSize * 0.8
                descrSize: textSize * 0.7
                descrColor: textColor
                descrFontFamily: font.family

                fact: modelData
                toolTip: fact?fact.descr:""
                value: ""
                descr: {
                    if(!fact)return ""
                    return fact.descr.split(',').map(function(s){
                        var i=s.indexOf('=')
                        if(i<0)return s
                        return s.slice(0,Math.min(3,i)).toUpperCase()
                    }).join(' ')
                }
                showIcon: false
                showEditor: false
                showValue: false
                showNext: (fact && fact.selected)?true:false

                active: false

                noFactTrigger: true
                onTriggered: {
                    if(fact.selected)fact.trigger()
                    else fact.selected=true
                }

                color: {
                    switch(fact.missionItemType){
                    default: return Material.primaryColor
                    case Mission.RunwayType:
                        return Material.color(Material.Blue, Material.Shade900) //Style.cListRunway
                    case Mission.WaypointType:
                        return Material.color(Material.BlueGrey, Material.Shade500) //Style.cListWaypoint
                    case Mission.PoiType:
                        return Material.color(Material.Teal, Material.Shade700) //Style.cListPoint
                    case Mission.TaxiwayType:
                        return Material.color(Material.Grey, Material.Shade800) //Style.cListTaxiway
                    }
                }
            }
            Loader {
                active: modelData.active
                Layout.fillHeight: true
                sourceComponent: Component {
                    MaterialIcon {
                        verticalAlignment: Text.AlignVCenter
                        size: Math.max(8,height)
                        name: "chevron-double-left"
                        color: Material.iconColor
                    }
                }
            }
        }

        //! [transitions]
        remove: Transition {
            SequentialAnimation {
                PauseAnimation { duration: 125 }
                NumberAnimation { property: "height"; to: 0; easing.type: Easing.InOutQuad }
            }
        }

        displaced: Transition {
            SequentialAnimation {
                PauseAnimation { duration: 125 }
                NumberAnimation { property: "y"; easing.type: Easing.InOutQuad }
            }
        }
        //! [transitions]
    }
}
