import QtQuick 2.12
import QtQuick.Controls 2.12  // 必须导入此模块
import QtQuick.Layouts 1.12

import com.example.backup 1.0
//import com.example.group 1.0
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
        font.pixelSize: 16

        // 文件菜单
        Menu {
            title: "文件"
            font.family: "Microsoft YaHei"
            font.pixelSize: 16

            //MenuSeparator {}

            MenuItem {
                text: "退出"
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: "设置"
            font.family: "Microsoft YaHei"
            font.pixelSize: 16

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
            font.family: "Microsoft YaHei"
            font.pixelSize: 16

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
        //spacing: 2  // 垂直间距

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
                    hoverEnabled: true  // 启用悬停检测

                    onClicked: {
                        settings.logDebug("qml: Clicked: buttonBackupAdd")

                        settings.backupModel.addBackup(
                            "新备份项",
                            "",
                            "",
                            false,
                            false,
                            -1
                        )

                        settings.logDebug("qml: Clicked: buttonBackupAdd: 添加备份项")
                        settings.backupModel.forceLayout()
                        listViewBackup.currentIndex = listViewBackup.model.getRowCount() - 1
                    }

                    background: Rectangle {
                                id: buttonBackupAddBackground
                                color: !buttonBackupAdd.enabled ? "#aaaaaa" :
                                       buttonBackupAdd.pressed ? "black" :
                                       buttonBackupAdd.hovered ? "gray" : "white"
                                border.color: "gray"
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
                    hoverEnabled: true  // 启用悬停检测

                    onClicked: {
                        settings.logDebug("qml: Clicked: buttonBackupDel")
                        var tempIndex = listViewBackup.currentIndex
                        if (tempIndex === listViewBackup.model.getRowCount() - 1) {
                            settings.backupModel.removeBackup(tempIndex)
                            settings.logDebug("qml: Clicked: buttonBackupDel: 删除备份项")
                            settings.backupModel.forceLayout()
                            listViewBackup.currentIndex = tempIndex - 1
                        }
                        else {
                            settings.backupModel.removeBackup(tempIndex)
                            settings.logDebug("qml: Clicked: buttonBackupDel: 删除备份项")
                            settings.backupModel.forceLayout()
                            listViewBackup.currentIndex = tempIndex
                        }
                    }

                    background: Rectangle {
                                id: buttonBackupDelBackground
                                color: !buttonBackupDel.enabled ? "#aaaaaa" :
                                       buttonBackupDel.pressed ? "black" :
                                       buttonBackupDel.hovered ? "gray" : "white"
                                border.color: "gray"
                                border.width: 1
                                radius: 4
                    }
                }
            }

            // 分隔线
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "gray"
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
                }

                onCurrentIndexChanged: {
                    //Qt.callLater(function() {
                        var currentItem = listViewBackup.model.get(listViewBackup.currentIndex)
                        if (currentItem) {
                            textFieldOperateName.text = currentItem.m_operateName
                            textFieldSourceFile.text = currentItem.m_sourceFile
                            textFieldDestinationPath.text = currentItem.m_destinationPath
                            textFieldErrorMessage.text = currentItem.m_errorMessage
                            checkBoxStartBackup.checked = currentItem.m_startBackup
                            checkBoxSetPermissions.checked = currentItem.m_setPermissions
                            //settings.backupModel.forceLayout()

                            backup.visible = true

                        } else {
                            backup.visible = false
                        }
                    //})
                }

                delegate: Rectangle {
                    width: listViewBackup.width
                    height: 46
                    color: mouseAreaListView.containsMouse ? "#ddf0ff" : ListView.isCurrentItem ? "#c1e3ff" : "white"

                    MouseArea {
                        id: mouseAreaListView
                        width: parent.width - 80  // 覆盖除按钮外的区域
                        height: parent.height
                        anchors.left: parent.left
                        hoverEnabled: true
                        //propagateComposedEvents: true  // 允许事件传播
                        onClicked: {
                            settings.logDebug("qml: BackupList currentIndex: " + index)
                            listViewBackup.currentIndex = index
                            //mouse.accepted = false
                        }
                    }

                    ColumnLayout {
                        width: listViewBackup.width
                        //Layout.fillWidth: true  // 添加此行
                        RowLayout {
                            width: listViewBackup.width
                            //Layout.fillWidth: true  // 添加此行

//                            Item {
//                                Layout.fillWidth: true
//                            }

                            Button {
                                id: buttonBackupItem
                                // 设置文本颜色
                                contentItem: Text {
                                    text: "备份"
                                    font.family: "Microsoft YaHei"
                                    font.pointSize: 10
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter

                                    color: model.m_errorMessage !== "" ? "red" : "black"
                                }
                                font.bold: true
                                width: 80
                                height: 34
                                enabled: {
                                    if (settings.bIsClickSaveButton) {
                                        if (model.m_errorMessage !== "") {
                                            return false
                                        }
                                        return true
                                    }
                                    else {
                                        return false
                                    }
                                }
                                hoverEnabled: true  // 启用悬停检测

                                onClicked: {
                                    settings.logDebug("qml: Clicked: buttonBackupItem: 备份")
                                    settings.runBackupTask(listViewBackup.model.get(index))
                                }

                                background: Rectangle {
                                            id: buttonBackupItemBackground
                                            color: !buttonBackupItem.enabled ? "#aaaaaa" :
                                                   buttonBackupItem.pressed ? "black" :
                                                   buttonBackupItem.hovered ? "gray" : "white"
                                            border.color: "gray"
                                            border.width: 1
                                            radius: 4
                                }
                            }

                            Text {
                                text: model.m_operateName
                                //horizontalAlignment: Text.AlignHCenter
                                //verticalAlignment: Text.AlignVCenter
                                font.pointSize: 14
                                font.family: "Microsoft YaHei"
                                color: model.m_errorMessage !== "" ? "red" : "black"
                            }
                        }

                        ProgressBar {
                            id: progressBarBackupItemTask
                            Layout.fillWidth: true
                            height: 4
                            from: 0
                            to: 100
                            value: model.m_progress
                            visible: model.m_progress >= 0.0 ? true : false

                            background: Rectangle {
                                color: "white"
                                border.color: "gray"
                                border.width: 1
                                radius: 5
                            }

                            contentItem: Rectangle {
                                color: "#1ad800"
                                radius: 5
                                width: progressBarBackupItemTask.visualPosition * progressBarBackupItemTask.width
                                height: progressBarBackupItemTask.height
                            }
                        }

                        // 分隔线
                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: "gray"
                        }
                    }
                }


                highlight: Rectangle {
                    color: "green"
                }

                section {
                    //property: "modelData.m_menuButton"
                    property: "m_startBackup"
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
                    text: section === "true" ? "启动时备份项" : "备份项"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                }
            }
        }

        // 配置: backup
        ColumnLayout {
            id: backup
            visible: false
            height: 180 
            Layout.fillWidth: true  // 添加此行
            //spacing: 5  // 垂直间距

            // 分隔线
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "gray"
            }

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
                        if (listViewBackup.currentIndex !== -1) {
                            var item = settings.backupModel.get(listViewBackup.currentIndex)
                            if (item) {
                                return item.m_operateName
                            }
                        }
                        return ""
                    }

                    onTextChanged:  {
                        if (listViewBackup.currentIndex !== -1) {
                            settings.backupModel.get(listViewBackup.currentIndex).m_operateName = text
                            //settings.backupModel.forceLayout()
                        }
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
                        if (listViewBackup.currentIndex !== -1) {
                            var item = settings.backupModel.get(listViewBackup.currentIndex)
                            if (item) {
                                return item.m_sourceFile
                            }
                        }
                        return ""
                    }

                    onTextChanged: {
                        if (listViewBackup.currentIndex !== -1) {
                            settings.backupModel.get(listViewBackup.currentIndex).m_sourceFile = text
                            //settings.backupModel.forceLayout()
                        }
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
                    placeholderText: "例: F:/"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                    selectByMouse: true  // 启用鼠标选区
                    text: {
                        if (listViewBackup.currentIndex !== -1) {
                            var item = settings.backupModel.get(listViewBackup.currentIndex)
                            if (item) {
                                return item.m_destinationPath
                            }
                        }
                        return ""
                    }
                    onTextChanged: {
                        if (listViewBackup.currentIndex !== -1) {
                            settings.backupModel.get(listViewBackup.currentIndex).m_destinationPath = text
                            //settings.backupModel.forceLayout()
                        }
                    }
                }
            }


            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                visible: {
                    if (listViewBackup.currentIndex !== -1) {
                        return settings.backupModel.get(listViewBackup.currentIndex).m_errorMessage !== "" ? true : false
                    }
                    return false
                }

                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    font.bold: true
                    text: "错误信息:"
                    font.pointSize: 10
                    color: "red"  // 设置输入文本的颜色为红色
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 参数: 文本编辑框
                TextField  {
                    id: textFieldErrorMessage
                    placeholderText: "例: 路径错误/文件(夹)无效"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                    font.bold: true
                    selectByMouse: true  // 启用鼠标选区
                    color: "red"  // 设置输入文本的颜色为红色
                    text: {
                        if (listViewBackup.currentIndex !== -1) {
                            var item = settings.backupModel.get(listViewBackup.currentIndex)
                            if (item) {
                                return item.m_errorMessage
                            }
                        }
                        return ""
                    }
                    onTextChanged: {
                        if (listViewBackup.currentIndex !== -1) {
                            settings.backupModel.get(listViewBackup.currentIndex).m_errorMessage = text
                            //settings.backupModel.forceLayout()
                        }
                    }
                }
            }

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
                        if (listViewBackup.currentIndex !== -1) {
                            var item = settings.backupModel.get(listViewBackup.currentIndex)
                            if (item) {
                                return item.m_startBackup
                            }
                        }
                        return false
                    }
                    onToggled: {
                        if (listViewBackup.currentIndex !== -1) {
                            settings.backupModel.get(listViewBackup.currentIndex).m_startBackup = checked
                            //settings.backupModel.forceLayout()
                        }
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
                        if (listViewBackup.currentIndex !== -1) {
                            var item = settings.backupModel.get(listViewBackup.currentIndex)
                            if (item) {
                                return item.m_setPermissions
                            }
                        }
                        return false
                    }
                    onToggled: {
                        if (listViewBackup.currentIndex !== -1) {
                            settings.backupModel.get(listViewBackup.currentIndex).m_setPermissions = checked
                        }
                    }
                }
            }

//            RowLayout {
//                Layout.fillWidth: true

//                ProgressBar {
//                    id: progressBarBackupItemTask
//                    width: 300
//                    from: 0
//                    to: 100
//                    value: {
//                        if (listViewBackup.currentIndex !== -1) {
//                            var item = settings.backupModel.get(listViewBackup.currentIndex)
//                            if (item) {
//                                return item.m_progress
//                            }
//                        }
//                        return -1
//                    }
//                    visible: {  // 当进度值为 -1 时隐藏进度条
//                        if (listViewBackup.currentIndex !== -1) {
//                            var item = settings.backupModel.get(listViewBackup.currentIndex)
//                            if (item) {
//                                return item.m_progress >= 0
//                            }
//                        }
//                        return false
//                    }

//                    background: Rectangle {
//                        color: "white"
//                        border.color: "gray"
//                        border.width: 1
//                        radius: 5
//                    }

//                    contentItem: Rectangle {
//                        color: "green"
//                        radius: 5
//                        width: progressBarBackupItemTask.visualPosition * progressBarBackupItemTask.width
//                        height: progressBarBackupItemTask.height
//                    }
//                }
//            }
        }
        
        // 按钮: 保存/退出
        ColumnLayout {
            Layout.fillWidth: true  // 添加此行
            spacing: 2  // 垂直间距

            // 分隔线
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "gray"
            }

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
                    enabled: settings.bIsClickSaveButton
                    hoverEnabled: true  // 启用悬停检测

                    onClicked: {
                        settings.logDebug("qml: Clicked: buttonSettingSave")
                        listViewBackup.currentIndex = -1

                        settings.updateConfig()
                        settings.saveData()
                        settings.backupItemManage()
                        settings.backupModel.forceLayout(true)
                    }

                    background: Rectangle {
                                id: buttonSettingSaveBackground
                                color: !buttonSettingSave.enabled ? "#aaaaaa" :
                                       buttonSettingSave.pressed ? "black" :
                                       buttonSettingSave.hovered ? "gray" : "white"
                                border.color: "gray"
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
                    hoverEnabled: true  // 启用悬停检测

                    onClicked: {
                        settings.logDebug("qml: Clicked: buttonSettingQuit")
                        settings.quit()
                    }

                    onPressed: {
                        buttonSettingQuitBackground.color = "black"
                    }
//                    onReleased: {
//                        buttonSettingQuitBackground.color = buttonSettingQuit.hovered ? "gray" : "white"
//                    }
                    background: Rectangle {
                                id: buttonSettingQuitBackground
                                color: !buttonSettingQuit.enabled ? "#aaaaaa" :
                                       buttonSettingQuit.pressed ? "black" :
                                       buttonSettingQuit.hovered ? "gray" : "white"
                                border.color: "gray"
                                border.width: 1
                                radius: 4
                    }
                }
            }
        }
    }
}
