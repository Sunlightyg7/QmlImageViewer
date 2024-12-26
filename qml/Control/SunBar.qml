import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    width: parent.width

    property alias value: slider.value
    property string label: "名称"  // 可以动态更改标签
    property real minValue: -100
    property real maxValue: 100
    property real defaultValue: 0
    property real scaleValue: 0.9

    Rectangle {
        id: title
        width: parent.width * scaleValue
        height: valueInput.height
        anchors.horizontalCenter: parent.horizontalCenter

        // 组件标题
        Text {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: label
        }

        TextField {
            id: valueInput
            width: 50
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: slider.value.toFixed(0)

            background: Rectangle {
                border.width: 1
            }

            validator: IntValidator {
                bottom: slider.from       // 设置最小值
                top: slider.to        // 设置最大值
            }

            // 当文本变化时，更新 slider 的值
            onTextChanged: {
                // 获取输入框的文本，并将其转换为数字
                var newValue = parseInt(valueInput.text);
                if (!isNaN(newValue)) {
                    // 确保值在范围内
                    newValue = Math.min(Math.max(newValue, slider.from), slider.to);
                    slider.value = newValue;
                }
            }
        }
    }

    // 调整值的滑块
    Slider {
        id: slider
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * scaleValue
        from: minValue
        to: maxValue
        value: defaultValue
        stepSize: 1
        anchors.top: title.bottom  // 滑块紧接在输入框下方
        anchors.topMargin: 10  // 控制滑块与输入框之间的距离

        onValueChanged: {
            valueInput.text = slider.value.toFixed(0);
        }
    }
}
