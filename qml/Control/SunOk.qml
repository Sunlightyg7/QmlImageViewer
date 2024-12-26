import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    width: parent.width
    height: 30
    anchors.bottom: parent.bottom

    focus: true
    Keys.onPressed: {
        // 按下 ESC 键时退出
        if (event.key === Qt.Key_Escape) {
            cancelClicked();
        }
    }

    signal confirmClicked()
    signal cancelClicked()

    // 背景矩形
    Rectangle {
        anchors.fill: parent

        // 确认按钮
        Button {
            id: confirmButton
            text: "确认"
            width: 50
            height: parent.height
            anchors.right: cancelButton.left
            onClicked: {
                confirmClicked();
            }
        }

        // 取消按钮
        Button {
            id: cancelButton
            text: "取消"
            width: 50
            height: parent.height
            anchors.right: parent.right
            onClicked: {
                cancelClicked();
            }
        }
    }
}
