import QtQuick 2.12
import QtQuick.Controls 2.12  // 必须导入此模块
import QtQuick.Layouts 1.12

import com.example.backup 1.0
import com.example.group 1.0
import com.example.settings 1.0
import "qrc:/qt/qml/typicalbackup/resource"



ApplicationWindow {
    id: rootWindow
    visible: true
    width: 500
    height: 800
    // 设置最大尺寸
    maximumWidth: 500
    maximumHeight: 800
    // 可选：设置最小尺寸
    minimumWidth: 500
    minimumHeight: 800
    title: settings.applicationWindowTitleName

    menuBar: MenuBar {
        font.family: "Microsoft YaHei"
        font.pixelSize: 14

        // 文件菜单
        Menu {
            title: "文件"

//            MenuItem {
//                text: "新建(&N)"
//                onTriggered: console.log("新建被点击")
//            }

//            MenuItem {
//                text: "打开(&O)"
//                onTriggered: console.log("打开被点击")
//            }

            //MenuSeparator {}

            MenuItem {
                text: "退出"
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: "设置"

            MenuItem {
                text: "首选项"
                onTriggered: {
                    var component = Qt.createComponent("settingWindow.qml")
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(rootWindow)
                        dialog.open()
                    } else {
                        console.error("Error loading settings:", component.errorString())
                    }
                }
            }
        }

        Menu {
            title: "帮助"

            MenuItem {
                text: "关于"
                onTriggered: {
                    settings.help()
                }
            }
        }
    }

    // 使用 Loader 动态加载 settingWindow.qml
    Loader {
        id: settingsWindow
        source: "settingWindow.qml"
        onLoaded: {
            // 设置加载完成后的回调
            console.log("设置窗口已加载")
        }
    }

    function delay(duration) {
        var start = new Date().getTime()
        while (new Date().getTime() - start < duration) {
            // 阻塞等待
        }
    }

    // Main
    ColumnLayout {
        anchors.fill: parent
        spacing: 2  // 垂直间距

        // ListView: backup
        ColumnLayout {
            height: 380 
            Layout.fillWidth: true  // 添加此行

            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                    font.bold: true
                    font.pointSize: 12
                    text: "备份项"
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                Button {
                    id: buttonBackupAdd
                    text: "添加"
                    font.family: "Microsoft YaHei"
                    font.pointSize: 14
                    font.bold: true
                    Layout.preferredHeight: 34
                    Layout.preferredWidth: 80

                    onClicked: {
                        settings.logDebug("qml: Clicked: buttonBackupAdd")

                        settings.backupModel.addBackup(
                            "新备份项",
                            "",
                            "",
                            "",
                            true,
                            true,
                            -1
                        )
                        settings.backupModel.forceLayout()
                        listViewBackup.currentIndex = listViewBackup.model.getRowCount() - 1
                    }
                    onPressed: {
                        buttonBackupAddBackground.color = "black"
                    }
                    onReleased: {
                        buttonBackupAddBackground.color = "white"
                    }
                    background: Rectangle {
                                id: buttonBackupAddBackground
                                color: "white"
                                border.color: "black"
                                border.width: 1
                                radius: 4
                    }
                }

                Button {
                    id: buttonBackupDel
                    text: "删除"
                    font.family: "Microsoft YaHei"
                    font.pointSize: 14
                    font.bold: true
                    Layout.preferredHeight: 34
                    Layout.preferredWidth: 80

                    onClicked: {
                        settings.logDebug("qml: Clicked: buttonBackupDel")
                        var tempIndex = listViewBackup.currentIndex
                        if (tempIndex === listViewBackup.model.getRowCount() - 1) {
                            settings.backupModel.removeBackup(tempIndex)
                            settings.backupModel.forceLayout()
                            listViewBackup.currentIndex = tempIndex - 1
                        }
                        else {
                            settings.backupModel.removeBackup(tempIndex)
                            settings.backupModel.forceLayout()
                            listViewBackup.currentIndex = tempIndex
                        }
                    }
                    onPressed: {
                        buttonBackupDelBackground.color = "black"
                    }
                    onReleased: {
                        buttonBackupDelBackground.color = "red"
                    }
                    background: Rectangle {
                                id: buttonBackupDelBackground
                                color: "red"
                                border.color: "black"
                                border.width: 1
                                radius: 4
                    }
                }
            }

            // 分隔线
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#cccccc"
            }

            ListView {
                id: listViewBackup

                height: 400 

                clip: true    // 超出部分裁剪
                Layout.fillWidth: true
                Layout.fillHeight: true

                focus: true

                //model: settings.getBackupList()
                model: settings.backupModel

                Component.onCompleted: {
                    settings.logDebug("qml: BackupList: onCompleted")
//                    var bakcupListSize = settings.getBackupList().length
//                    if (bakcupListSize > 1000) {
//                        settings.logDebug("qml: BackupList: BackupList大小[" + bakcupListSize +"]超出范围[1000]!")
//                        return
//                    }
                }

                onCurrentIndexChanged: {
                    var currentItem = listViewBackup.model.get(listViewBackup.currentIndex)
                    if (currentItem) {
                        textFieldOperateName.text = currentItem.m_operateName
                        textFieldSourceFile.text = currentItem.m_sourceFile
                        textFieldDestinationPath.text = currentItem.m_destinationPath
                        textFieldErrorMessage.text = currentItem.m_errorMessage
                        //comboBoxMode.currentIndex = comboBoxMode.find(currentItem.m_shellOperate)
                        checkBoxStartBackup.checked = currentItem.m_startBackup
                        checkBoxSetPermissions.checked = currentItem.m_setPermissions
                        progressBarBackupItemTask.value = currentItem.m_progress

                        backup.visible = true

                    } else {
                        backup.visible = false
                    }
                }

                delegate: Rectangle {
                    width: listViewBackup.width
                    height: 50
                    color: ListView.isCurrentItem ? "#d0eaff" : "white"
                    border.color: "#cccccc"
                    border.width: 1

                    Row {
                        spacing: 10
                        anchors.centerIn: parent

                        Text {
                            //text: settings.getBackupList()[listViewBackup.currentIndex].getOperateName()
                            text: model.getOperateName()
                            font.pointSize: 10
                            font.family: "Microsoft YaHei"
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Button {
                            id: buttonBackupItem
                            text: "备份"
                            font.family: "Microsoft YaHei"
                            font.pointSize: 14
                            font.bold: true
                            Layout.preferredHeight: 34
                            Layout.preferredWidth: 80

                            onClicked: {
                                settings.operateCopyFile(model)
                            }
                            onPressed: {
                                buttonBackupItemBackground.color = "black"
                            }
                            onReleased: {
                                buttonBackupItemBackground.color = "white"
                            }
                            background: Rectangle {
                                        id: buttonBackupItemBackground
                                        color: "white"
                                        border.color: "black"
                                        border.width: 1
                                        radius: 4
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            settings.logDebug("qml: BackupList currentIndex: " + listViewBackup.currentIndex)
                            listViewBackup.currentIndex = listViewBackup.currentIndex
                        }
                    }
                }

                highlight: Rectangle {
                    color: "green"
                }

                section {
                    //property: "modelData.m_menuButton"
                    property: "startBackup"
                    criteria: ViewSection.FullString
                    delegate: sectionBackup
                }
            }
        }

        Component {
            id: sectionBackup
            Rectangle{
                width: listViewBackup.width
                color: "#dddeee"
                height: 24
                Text {
                    anchors.fill: parent
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    text: section === "true" ? "备份项" : "启动时备份项"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                }
            }
        }

        // 分隔线
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#cccccc"
        }

        // 配置: backup
        ColumnLayout {
            id: backup
            visible: false
            height: 180 
            Layout.fillWidth: true  // 添加此行
            spacing: 5  // 垂直间距
            
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft    
                font.bold: true
                font.pointSize: 12
                text: "备份项配置"
                font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    font.pointSize: 10
                    text: "操作名:"
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 文件: 文本编辑框
                TextField {
                    id: textFieldOperateName
                    placeholderText: "例: 备份D:/Test到F盘"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                    selectByMouse: true  // 启用鼠标选区
                    text: {
                        return model.m_operateName;
                    }

                    onTextChanged:  {
                        model.m_operateName = text
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight    
                    font.bold: false
                    font.pointSize: 10
                    text: "源文件(夹):"
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 文件: 文本编辑框
                TextField {
                    id: textFieldSourceFile
                    placeholderText: "例: D:/Test"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                    selectByMouse: true  // 启用鼠标选区
                    text: {
                        return model.m_sourceFile;
                    }

                    onTextChanged: {
                        model.m_sourceFile = text
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight    
                    font.bold: false
                    text: "目的地路径:"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 参数: 文本编辑框
                TextField {
                    id: textFieldDestinationPath
                    placeholderText: "例: -Test -K -C"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                    selectByMouse: true  // 启用鼠标选区
                    text: {
                        return model.m_destinationPath;
                    }
                    onTextChanged: {
                        model.m_destinationPath = text
                    }
                }
            }


            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                visible: {
                    return model.m_errorMessage
                }

                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    font.bold: false
                    text: "错误信息:"
                    font.pointSize: 10

                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 参数: 文本编辑框
                TextField {
                    id: textFieldErrorMessage
                    placeholderText: "例: -Test -K -C"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                    selectByMouse: true  // 启用鼠标选区
                    text: {
                        return model.m_errorMessage;
                    }
                    onTextChanged: {
                        model.m_errorMessage = text
                    }
                }
            }

//            RowLayout {
//                Layout.fillWidth: true
//                spacing: 10

//                Text {
//                    Layout.fillWidth: true
//                    horizontalAlignment: Text.AlignRight
//                    font.bold: false
//                    font.pointSize: 10
//                    text: "模式:"
//                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
//                }

//                // 模式: 下拉列表框
//                ComboBox {
//                    id: comboBoxMode
//                    model: ["打开文件", "打开文件夹", "管理员运行"]
//                    font.family: "Microsoft YaHei"
//                    font.pointSize: 10
//                    Layout.preferredWidth: 400

//                    delegate: ItemDelegate {
//                        width: comboBoxMode.width
//                        contentItem: Text {
//                            text: modelData
//                            font.family: "Microsoft YaHei"
//                            font.pointSize: 12
//                            font.bold: true
//                            color: "#000000"
//                            elide: Text.ElideRight
//                            verticalAlignment: Text.AlignVCenter
//                        }
//                    }

//                    onActivated: {
//
//                        if (listViewBackup.currentIndex !== -1) {
//                            var item = settings.backupModel.get(listViewBackup.currentIndex);
//                            if (item) {
//                                item.setShellOperate(currentText);
//                            }
//                        }

//                        settings.backupModel.forceLayout()
//                    }
//                }
//            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // 启动时备份: 复选框
                CheckBox {
                    id: checkBoxStartBackup
                    Layout.leftMargin: 94  // 设置偏移量
                    text: "启动时备份"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    checked: {
                        return model.m_startBackup
                    }
                    onToggled: {
                        model.m_startBackup = checked
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // 设置权限: 复选框
                CheckBox {
                    id: checkBoxSetPermissions
                    Layout.leftMargin: 94  // 设置偏移量
                    text: "设置权限"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    checked: {
                        return model.m_setPermissions
                    }
                    onToggled: {
                        model.m_setPermissions = checked
                        settings.backupModel.forceLayout()
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                ProgressBar {
                    id: progressBarBackupItemTask
                    width: 300
                    from: 0
                    to: 100
                    value: model.m_progress
                    visible: {  // 当进度值为 -1 时隐藏进度条
                        model.m_progress >= 0
                    }

                    background: Rectangle {
                        color: "white"
                        border.color: "black"
                        border.width: 1
                        radius: 5
                    }

                    contentItem: Rectangle {
                        color: "green"
                        radius: 5
                        width: progressBarBackupItemTask.visualPosition * progressBarBackupItemTask.width
                        height: progressBarBackupItemTask.height
                    }
                }
            }
        }
        
        // 按钮: 保存/退出
        ColumnLayout {
            Layout.fillWidth: true  // 添加此行
            spacing: 5  // 垂直间距

            RowLayout {
                Layout.leftMargin: 240  // 设置偏移量
                Layout.fillWidth: true
                Layout.fillHeight: true  // 添加此行
                spacing: 5
                    
                Button {
                    id: buttonSettingSave
                    text: "保存"
                    font.family: "Microsoft YaHei"
                    font.pointSize: 14
                    font.bold: true
                    Layout.preferredHeight: 34
                    Layout.preferredWidth: 110

                    onClicked: {
                        settings.logDebug("qml: Clicked: buttonSettingSave")
                        settings.backupModel.forceLayout(true)
                        listViewBackup.currentIndex = -1

                        settings.updateConfig()
                        settings.saveData();
                        settings.loadBackup(true)
                        settings.loadToolsMenu()
                    }
                    onPressed: {
                        buttonSettingSaveBackground.color = "black"
                    }
                    onReleased: {
                        buttonSettingSaveBackground.color = "white"
                    }
                    background: Rectangle {
                                id: buttonSettingSaveBackground
                                color: "white"
                                border.color: "black"
                                border.width: 1
                                radius: 4
                    }
                }

                Button {
                    id: buttonSettingQuit
                    text: "退出"
                    font.family: "Microsoft YaHei"
                    font.pointSize: 14
                    font.bold: true
                    Layout.preferredHeight: 34
                    Layout.preferredWidth: 110

                    onClicked: {
                        settings.logDebug("qml: Clicked: buttonSettingQuit")
                        settings.quit()
                    }
                    onPressed: {
                        buttonSettingQuitBackground.color = "black"
                    }
                    onReleased: {
                        buttonSettingQuitBackground.color = "white"
                    }
                    background: Rectangle {
                                id: buttonSettingQuitBackground
                                color: "white"
                                border.color: "black"
                                border.width: 1
                                radius: 4
                    }
                }
            }
        }
    }
}
