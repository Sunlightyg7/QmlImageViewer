import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "../Control"
import "../Template"

SunConfigWin {

    Rectangle {
        anchors.fill:parent
        anchors.margins: 10  // 设置内边距

        SunBar {
            id: grayBar
            label: "灰度"

        }

        SunOk {
            id: sunok
        }
    }

    Connections {
        target: sunok
        function onConfirmClicked() {
            console.log("Confirm button clicked!");
            confirmClicked();
        }

        function onCancelClicked() {
            console.log("Cancel button clicked!");
            cancelClicked();
        }
    }

    Connections {
        target: grayBar
        function onValueChanged() {
            propertyChanged("gray", grayBar.value);
        }
    }
}
