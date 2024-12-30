import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "../Control"
import "../Template"

SunConfigWin {

    // alias手动指定双向绑定，将值绑定到滑动条
    property alias gamma: sunbar.sliderValue
    winName: "gamma"

    Rectangle {
        anchors.fill:parent
        anchors.margins: 10  // 设置内边距

        SunBar {
            id: sunbar
            label: "gamma"
            minValue: 1
        }
    }

    onGammaChanged: {
        propertyChanged("gamma", gamma);
    }
}
