import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import Qt.labs.settings 1.1
import io.root 1.0
import "qml"
import "qml/Window"

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 970
    height: 600
    minimumWidth: 485
    minimumHeight: 300
    title: "Image Processer"

    property Component imgConfigWin: null

    SunChangedListWin {
        id: changedListWin

        Component.onCompleted: {
            changedListWin.x = rootWindow.x + rootWindow.width - width
            changedListWin.y = rootWindow.y + rootWindow.height - height - rootWindow.footer.height
        }

        onListItemRemove: {
            imageView.removeListItem(index);
        }

        onListItemMove: {
            imageView.moveListItem(from, to);
        }

        onListItemSelect: {
            imageView.selectedListItem(index);
        }
    }

    function createWindow(qmlPath) {
        if(null !== imgConfigWin) {
            console.log(qmlPath + "创建失败,已创建其他窗口.")
            return;
        }

        imgConfigWin = Qt.createComponent(qmlPath);

        if (imgConfigWin.status === Component.Ready) {
            var compIns = imgConfigWin.createObject(rootWindow);

            if (compIns) {
                compIns.show();

                initProperty(compIns, compIns.winName);

                // 连接转发的 confirmClicked 信号
                compIns.confirmClicked.connect(function() {
                    console.log("Confirm button clicked! (Handled in parent)");
                    imageView.applyImgConfig(compIns.winName);
                });

                // 连接转发的 cancelClicked 信号
                compIns.cancelClicked.connect(function() {
                    console.log("Cancel button clicked! (Handled in parent)");
                    imageView.cancelImgConfig(compIns.winName);
                });

                // 连接调整图像的propertyChanged事件
                compIns.propertyChanged.connect(function(name, value) {
                    propertyChangedEvent(name, value);

                });

                compIns.onClosing.connect(function(message) {
                    imgConfigWin = null;
                });

            } else {
                console.log("无法创建窗口实例: " + qmlPath);
                imgConfigWin = null;
            }
        } else if (imgConfigWin.status === Component.Error) {
            console.log("创建组件失败: " + imgConfigWin.errorString());
            imgConfigWin = null;
        }
    }

    function initProperty(compIns, winName) {
        if("gamma" === winName) {
            compIns.gamma = imageView.gamma;
        }
        else if("brightness" === winName) {
            compIns.brightness = imageView.brightness;
        }
        else {
            console.log("initProperty failed, " + winName + " cannot found.")
        }
    }

    // 调整图像属性时收到的事件
    function propertyChangedEvent(propName, value) {
        //console.log("property: " + propName + ", value: " + value);
        imageView.addImgConfig(propName, value);
    }

    ImageViewModel {
        id: viewModel
        onImageChanged: {
            imageView.initImage(this);
        }
    }

    // 创建设置对象
    Settings {
        id: settings
        property string lastOpenFolder: settings.value("lastOpenFolder", "C:") // 默认路径
    }

    // FileDialog 用于打开文件选择框
    FileDialog {
        id: fileDialog
        title: "选择图片"
        folder: settings.lastOpenFolder
        nameFilters: ["图片文件 (*.png *.jpg *.jpeg *.bmp)"]  // 过滤器，只显示图片文件
        onAccepted: {
            // 文件选择对话框关闭后，获取选择的文件路径
            viewModel.openImage(fileDialog.fileUrl);
            settings.lastOpenFolder = fileDialog.folder;
        }
    }

    // 创建菜单栏
    menuBar: MenuBar {
        id: menuBar

        Menu {
            title: qsTr("文件")

            MenuItem {
                text: qsTr("打开")
                onTriggered: {
                    fileDialog.open();
                }
            }

            MenuItem {
                text: qsTr("保存")
                onTriggered: {
                }
            }
        }

        Menu {
            title: qsTr("调整")

            MenuItem {
                text: qsTr("gamma")
                onTriggered: {
                    createWindow("qml/Window/SunGammaWin.qml");
                }
            }
            MenuItem {
                text: qsTr("亮度")
                onTriggered: {
                    createWindow("qml/Window/SunBrightnessWin.qml");
                }
            }
        }

        Menu {
            title: qsTr("窗口")

            MenuItem {
                text: qsTr("修改记录")
                onTriggered: {
                    changedListWin.visible = true;
                }
            }
        }
    }

    Column  {
        anchors.fill: parent

        ImageView {
            id: imageView
            width: parent.width
            height: parent.height

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                cursorShape: mouseArea.pressed ? Qt.PointingHandCursor : Qt.ArrowCursor
                hoverEnabled: true

                property double fOriX: 0
                property double fOriY: 0

                // 处理滚轮事件
                onWheel: {
                    // 判断滚轮方向，向上缩放
                    if (wheel.angleDelta.y > 0) {
                        imageView.zoomIn(mouseArea.mouseX, mouseArea.mouseY);
                        // 向下缩放
                    } else {
                        imageView.zoomOut(mouseArea.mouseX, mouseArea.mouseY);
                    }

                }

                onPressed: {
                    fOriX = mouseArea.mouseX
                    fOriY = mouseArea.mouseY
                    console.log("press at: ", fOriX, fOriY)
                }

                onPositionChanged: {
                    if (mouseArea.pressed && imageView.isImageOpened()) {
                        // 计算偏移
                        var dx = mouseArea.mouseX - fOriX
                        var dy = mouseArea.mouseY - fOriY
                        imageView.baseOffsetX += dx
                        imageView.baseOffsetY += dy
                        // 更新初始位置
                        fOriX = mouseArea.mouseX
                        fOriY = mouseArea.mouseY
                    }
                }
            }

            Component.onCompleted: {
                console.log("Window size onCompleted: " + parent.width + "x" + parent.height);
                imageView.winWidth = width;
                imageView.winHeight = height;
            }

            onWidthChanged: {
                imageView.winWidth = width;
            }

            onHeightChanged: {
                imageView.winHeight = height;
            }

            onChangedListChanged: {
                changedListWin.listChanged = imageView.getStepsList();
            }
        }
    }

    // 使用 footer 作为状态栏
    footer: Rectangle {
        width: parent.width
        height: 20
        color: "#2f4f4f" // 状态栏背景颜色
        Text {
            anchors.centerIn: parent
            color: "white"
            text: "Scale: " + Math.round(imageView.zoomFactor * 100) + "%"
        }
    }

    Component.onCompleted: {

    }
}
