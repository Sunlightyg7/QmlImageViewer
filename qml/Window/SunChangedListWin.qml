import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import "../Control"

Window {
    id: rootWin
    title: "Changed List"
    width: 260
    height: 300
    visible: true

    property alias listChanged: listView.model
    property int nCurrSelected: -1
    signal listItemRemove(var index)
    signal listItemMove(var from, var to)
    signal listItemSelect(var index)

    Rectangle {
        id: listTitle
        width: rootWin.width
        height: 20
        color: "lightgray"

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
        width: rootWin.width
        height: rootWin.height - listTitle.height - operBtn.height

        delegate: Rectangle {
            width: parent.width
            height: 20
            color: modelData.bSelected ? "lightblue" : "white"

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

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    modelData.bSelected = true;
                    nCurrSelected = model.index;
                    listItemSelect(nCurrSelected);
                }
            }
        }

        onModelChanged: {
            for (var i = 0; i < listView.count; i++) {
                var item = listView.model[i];
                if(item.bSelected)
                    nCurrSelected = i;
            }
        }
    }

    RowLayout  {
        id: operBtn
        height: 30
        width: rootWin.width
        anchors.top: listView.bottom
        spacing: 0

        Button {
            id: btn1
            text: "修改值"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Button {
            height: parent.height
            text: "删除"
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                listItemRemove(nCurrSelected);
            }
        }

        Button {
            height: parent.height
            text: "上移"
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                listItemMove(nCurrSelected, nCurrSelected - 1);
            }
        }

        Button {
            height: parent.height
            text: "下移"
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                listItemMove(nCurrSelected, nCurrSelected + 1);
            }
        }
    }
}
