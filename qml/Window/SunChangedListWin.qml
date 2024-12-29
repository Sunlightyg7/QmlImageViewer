import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "../Control"

Window {
    title: "Changed List"
    width: 260
    height: 300
    visible: true

    property alias listChanged: listView.model

    Rectangle {
        id: listTitle
        width: parent.width
        height: 20
        color: "lightgray"
        border.color: "black"

        Row {
            spacing: 0
            width: parent.width
            anchors.centerIn: parent

            Text {
                text: "名称"
                width: 100
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                text: "值"
                width: 80
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                text: "优先级"
                width: 80
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    ListView {
        id: listView
        anchors.top: listTitle.bottom
        width: parent.width
        height: parent.height

        delegate: Rectangle {
            width: parent.width
            height: 20

            Row {
                spacing: 0
                width: parent.width
                anchors.centerIn: parent

                Text {
                    text: modelData.szFuncName
                    width: 100
                    horizontalAlignment: Text.AlignHCenter
                }

                Text {
                    text: modelData.varValue
                    width: 80
                    horizontalAlignment: Text.AlignHCenter
                }

                Text {
                    text: modelData.nPriority
                    width: 80
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        onModelChanged: {

        }
    }

    Button {
        text: "-"
        anchors.bottom: parent.bottom
    }
}
