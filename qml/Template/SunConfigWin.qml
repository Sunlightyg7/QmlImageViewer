import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "../Control"

Window {
    width: 400
    // flags: Qt.Window|Qt.WindowStaysOnTopHint|Qt.WindowTitleHint|
    //        Qt.WindowSystemMenuHint|Qt.WindowMinMaxButtonsHint|
    //        Qt.WindowCloseButtonHint|Qt.WindowFullscreenButtonHint

    signal confirmClicked()
    signal cancelClicked()
    // 当窗口内的值变化时发出，由C++反射调用相应图像处理函数
    signal propertyChanged(string name, variant value)
    // 防止关闭窗口时重复触发cancelClicked
    property bool bWinCloseBtn: true
    property string winName: ""

    title: winName

    SunOk {
        id: sunok
        z: 1
    }

    Shortcut {
        id:escShortcut
        sequences: ["Esc"]

        onActivated: {
            bWinCloseBtn = false;
            cancelClicked();
            close();
        }
    }

    Connections {
        target: sunok
        function onConfirmClicked() {
            bWinCloseBtn = false;
            confirmClicked();
            close();
        }

        function onCancelClicked() {
            bWinCloseBtn = false;
            cancelClicked();
            close();
        }
    }

    onClosing: {
        if(bWinCloseBtn)
        {
            cancelClicked();
        }

        console.log("Window destroy!");
        destroy();
    }
}
