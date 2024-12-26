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

    onClosing: {
        console.log("Window destroy!");
        destroy();
    }
}
