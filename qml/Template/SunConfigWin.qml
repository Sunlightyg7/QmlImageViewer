import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "../Control"

Window {
    width: 400
    flags: Qt.Window|Qt.WindowStaysOnTopHint|Qt.WindowTitleHint|
           Qt.WindowSystemMenuHint|Qt.WindowMinMaxButtonsHint|
           Qt.WindowCloseButtonHint|Qt.WindowFullscreenButtonHint

    signal confirmClicked()
    signal cancelClicked()
    signal propertyChanged(string name, variant value)
    // 防止关闭窗口时重复触发cancelClicked
    property bool bWinCloseBtn: true

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
