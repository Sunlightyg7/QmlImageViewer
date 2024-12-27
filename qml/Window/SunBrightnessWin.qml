import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "../Control"
import "../Template"

SunConfigWin {

    // alias手动指定双向绑定，将值绑定到滑动条
    property alias brightness: sunbar.sliderValue
    property string winName: "brightness"

    Rectangle {
        anchors.fill:parent
        anchors.margins: 10  // 设置内边距

        SunBar {
            id: sunbar
            label: "亮度"
        }
    }

    onBrightnessChanged: {
        propertyChanged("brightness", brightness);
    }


}
