// -*- coding: utf-8 -*-

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
    // 可选：设置最小尺寸
    font.family: "Microsoft YaHei"
    minimumWidth: 260
    minimumHeight: 400
    title: settings.applicationWindowTitleName


    onClosing: {
        // 执行必要的清理操作
        console.log("窗口即将关闭，执行清理操作。");
        settings.quit(); // 退出应用程序
    }

    // 在根元素或适当的作用域中定义
    property alias g_currentBackupItem: currentBackupItem.item
    QtObject {
        id: currentBackupItem
        property var item: null
    }

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

    Popup {
        id: popupProgressMain
        anchors.centerIn: parent
        width: 400
        height: 80
        modal: true
        focus: true
        closePolicy: Popup.NoAutoClose

        Column {
            anchors.fill: parent
            spacing: 10
            //padding: 10

            Label {
                id: lableMain
                text: "处理中..."
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ProgressBar {
                id: progressBarMain
                width: parent.width
                height: 30
                //indeterminate: true  // 不确定进度模式
                value: {
                    lableMain.text = "处理中..."
                    if (g_currentBackupItem) {
                        return g_currentBackupItem.m_progress
                    }
                    return 0.00
                }

                // 当进度变化时检查是否完成
                onValueChanged: {
                    if (value >= 1.00) {
                        Qt.callLater(function() {
                            g_currentBackupItem.m_progress = 0.00
                            popupProgressMain.close()
                            // 这里添加任务完成后的处理逻辑
                            console.log("popupProgressMain Task completed!")
                        })
                    }
                    else if (value === 0.00) {
                        popupProgressMain.close()
                        console.log("popupProgressMain close!")
                    }
                }

                contentItem: Rectangle {
                    width: progressBarMain.width * progressBarMain.value
                    height: 30
                    color: "#05ff00"
                    radius: 4
                }

                background: Rectangle {
                    color: "#e0e0e0"
                    radius: 4
                }
            }
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
                Layout.alignment: Qt.AlignRight  // 靠右对齐
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

                    onReleased: {
                        settings.logDebug("qml: Clicked: buttonBackupAdd")

                        settings.backupModel.addBackup(
                            "新备份项",
                            "",
                            "",
                            false,
                            false,
                            0.0
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

                    onReleased: {
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
                    settings.autoStartBackupItem()

                    // 延迟打开进度条，确保数据已加载
                    Qt.callLater(function() {
                        if (settings.bIsSelfAutoStarting && settings.bAutoStartingToQuitGame) {
                            lableMain.text = "[启动备份项] 处理中..."
                            popupProgressMain.open()
                            settings.logDebug("qml: popupProgressMain: open (delayed)")
                        }
                        })
                }

                onCurrentIndexChanged: {
                    g_currentBackupItem = listViewBackup.model.get(listViewBackup.currentIndex)
                    if (g_currentBackupItem) {
                        textFieldOperateName.text = g_currentBackupItem.m_operateName
                        textFieldSourceFile.text = g_currentBackupItem.m_sourceFile
                        textFieldDestinationPath.text = g_currentBackupItem.m_destinationPath
                        checkBoxStartBackup.checked = g_currentBackupItem.m_startBackup
                        checkBoxSetPermissions.checked = g_currentBackupItem.m_setPermissions
                        //settings.backupModel.forceLayout()

                        backup.visible = true

                    } else {
                        backup.visible = false
                    }
                }

                delegate: Rectangle {
                    width: listViewBackup.width
                    height: 38
                    color: mouseAreaListView.containsMouse ? "#ddf0ff" : ListView.isCurrentItem ? "#c1e3ff" : "white"

                    MouseArea {
                        id: mouseAreaListView
                        width: parent.width - 80  // 覆盖除按钮外的区域
                        height: parent.height
                        anchors.left: parent.left
                        hoverEnabled: true
                        //propagateComposedEvents: true  // 允许事件传播
                        onReleased: {
                            if (listViewBackup.currentIndex !== index) {
                                settings.logDebug("qml: BackupList currentIndex: " + index)
                                listViewBackup.currentIndex = index
                                g_currentBackupItem = listViewBackup.model.get(listViewBackup.currentIndex)
                                //mouse.accepted = false
                            }
                        }
                    }

                    ColumnLayout {
                        width: listViewBackup.width
                        spacing: 0
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
                                    color: "black"
                                }
                                Layout.alignment: Qt.AlignHCenter  // 水平居中
                                font.bold: true
                                width: 80
                                height: 34
                                enabled: {
                                    if (!settings.bIsClickSaveButton) {
                                        // 有错误时: 无效
                                        if (model.m_sourceFileError || model.m_destinationPathError) {
                                            return false
                                        }

                                        //进度条约束
                                        if (model.m_progress >= 1.00 || model.m_progress < 0.0) {
                                            model.m_progress = 0.0
                                            return true
                                        }

                                        if (model.m_progress === 0.0) return true

                                        return true
                                    }
                                    else {
                                        return false
                                    }
                                }
                                hoverEnabled: true  // 启用悬停检测

                                onReleased: {
                                    settings.logDebug("qml: Clicked: buttonBackupItem: 备份")

                                    listViewBackup.currentIndex = index
                                    g_currentBackupItem = listViewBackup.model.get(index)
                                    if (g_currentBackupItem) {
                                        popupProgressMain.open()
                                        settings.runBackupTask(g_currentBackupItem)
                                    }
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
                                color: {
                                    if (model.m_sourceFileError || model.m_destinationPathError) {
                                        return "red"
                                    }
                                    else {
                                        return "black"
                                    }
                                }
                            }
                        }

//                        ProgressBar {
//                            id: progressBarBackupItemTask
//                            Layout.fillWidth: true
//                            height: 4
//                            from: 0
//                            to: 100
//                            value: model.m_progress
//                            visible: {
//                                if (model.m_progress >= 1.00 || model.m_progress <= 0.00) {
//                                    model.m_progress = 0.0
//                                    return false
//                                }
//                                return true
//                            }

//                            background: Rectangle {
//                                color: "white"
//                                border.color: "gray"
//                                border.width: 1
//                                radius: 5
//                            }

//                            contentItem: Rectangle {
//                                color: "#1ad800"
//                                radius: 5
//                                width: progressBarBackupItemTask.visualPosition * progressBarBackupItemTask.width
//                                height: progressBarBackupItemTask.height
//                            }
//                        }

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
                Layout.alignment: Qt.AlignLeft  // 靠左对齐
                spacing: 10

                Text {
                    //Layout.fillWidth: true
                    font.pointSize: 10
                    text: " 操作名:"
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 文件: 文本编辑框
                TextField {
                    id: textFieldOperateName
                    Layout.fillWidth: true
                    placeholderText: "例: 备份D:/Test到F盘"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                    selectByMouse: true  // 启用鼠标选区
                    text: {
                        if (g_currentBackupItem) {
                            return g_currentBackupItem.m_operateName
                        }
                        return ""
                    }

                    onTextChanged:  {
                        if (g_currentBackupItem) {
                            g_currentBackupItem.m_operateName = text
                            //settings.backupItemManage(currentItem)
                        }
                        //settings.backupModel.forceLayout()
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft  // 靠左对齐
                spacing: 10


                Text {
                    //Layout.fillWidth: true
                    font.bold: false
                    font.pointSize: 10
                    text: " 源文件(夹):"
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                    color: {
                        if (g_currentBackupItem) {
                            return g_currentBackupItem.m_sourceFileError ? "red" : "black"
                        }
                        return "black"
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 140

                    // 文件: 文本编辑框
                    TextArea {
                        id: textFieldSourceFile
                        Layout.fillWidth: true
                        placeholderText: "例: D:/Test"
                        font.pointSize: 10
                        font.family: "Microsoft YaHei"
                        Layout.preferredHeight: 140
                        selectByMouse: true  // 启用鼠标选区
                        wrapMode: Text.Wrap  // 自动换行
                        text: {
                            if (g_currentBackupItem) {
                                return g_currentBackupItem.m_sourceFile
                            }
                            return ""
                        }
                        color: {
                            if (g_currentBackupItem) {
                                return g_currentBackupItem.m_sourceFileError ? "red" : "black"
                            }
                            return "black"
                        }

                        onTextChanged: {
                            if (g_currentBackupItem) {
                                g_currentBackupItem.m_sourceFile = text
                                //settings.backupItemManage(currentItem)
                            }
                            //settings.backupModel.forceLayout()
                        }
                    }

                    background: Rectangle {
                        Layout.fillWidth: true
                        height: 140
                        color: "white"
                        border.color: "gray"
                        border.width: 1
                        radius: 0
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft  // 靠左对齐
                spacing: 10

                Text {
                    //Layout.fillWidth: true
                    font.bold: false
                    text: " 目的地路径:"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                    wrapMode: Text.Wrap  // 自动换行
                    color: {
                        if (g_currentBackupItem) {
                            return g_currentBackupItem.m_destinationPathError ? "red" : "black"
                        }
                        return "black"
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80

                    // 参数: 文本编辑框
                    TextArea {
                        id: textFieldDestinationPath
                        Layout.fillWidth: true
                        placeholderText: "例: F:/"
                        font.pointSize: 10
                        Layout.preferredHeight: 80
                        font.family: "Microsoft YaHei"
                        Layout.preferredWidth: 400
                        selectByMouse: true  // 启用鼠标选区
                        text: {
                            if (g_currentBackupItem) {
                                return g_currentBackupItem.m_destinationPath
                            }
                            return ""
                        }
                        color: {
                            if (g_currentBackupItem) {
                                return g_currentBackupItem.m_destinationPathError ? "red" : "black"
                            }
                            return "black"
                        }

                        onTextChanged: {
                            if (g_currentBackupItem) {
                                g_currentBackupItem.m_destinationPath = text
                                //settings.backupItemManage(currentItem)
                            }
                            //settings.backupModel.forceLayout()
                        }
                    }

                    background: Rectangle {
                        Layout.fillWidth: true
                        height: 80
                        color: "white"
                        border.color: "gray"
                        border.width: 1
                        radius: 0
                    }
                }
            }


            RowLayout {
                id: modelErrorMessage
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft  // 靠左对齐
                spacing: 10
                visible: {
                    if (g_currentBackupItem) {
                        return g_currentBackupItem.m_errorMessageList.length <= 0 ? false : true
                    }
                    return false
                }

                Text {
                    //Layout.fillWidth: true
                    font.bold: true
                    text: " 错误信息:"
                    font.pointSize: 10
                    color: "red"  // 设置输入文本的颜色为红色
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120

                    // 参数: 文本编辑框
                    TextArea  {
                        id: textFieldErrorMessage
                        Layout.fillWidth: true
                        placeholderText: "例: 路径错误/文件(夹)无效"
                        font.pointSize: 10
                        font.family: "Microsoft YaHei"
                        font.bold: true
                        selectByMouse: true  // 启用鼠标选区
                        readOnly: true
                        color: "red"  // 设置输入文本的颜色为红色
                        text: {
                            if (g_currentBackupItem) {
                                var errorText = ""
                                for (var i = 0; i < g_currentBackupItem.m_errorMessageList.length; ++i) {
                                    errorText += g_currentBackupItem.m_errorMessageList[i] + "\n"
                                }
                                return errorText
                            }
                            return ""
                        }
                    }

                    background: Rectangle {
                        Layout.fillWidth: true
                        height: 120
                        color: "white"
                        border.color: "gray"
                        border.width: 1
                        radius: 0
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // 启动时备份: 复选框
                CheckBox {
                    id: checkBoxStartBackup
                    Layout.alignment: Qt.AlignLeft  // 靠左对齐
                    text: " 启动时备份"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    checked: {
                        if (g_currentBackupItem) {
                            return g_currentBackupItem.m_startBackup
                        }
                        return false
                    }
                    onToggled: {
                        if (g_currentBackupItem) {
                            g_currentBackupItem.m_startBackup = checked
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
                    Layout.alignment: Qt.AlignLeft  // 靠左对齐
                    text: " 设置权限(所有人)"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    checked: {
                        if (g_currentBackupItem) {
                            return g_currentBackupItem.m_setPermissions
                        }
                        return false
                    }
                    onToggled: {
                        if (g_currentBackupItem) {
                            g_currentBackupItem.m_setPermissions = checked
                        }
                    }
                }
            }

//            RowLayout {
//                Layout.fillWidth: true

//                ProgressBar {
//                    id: progressBarMainTask
//                    width: 300
//                    from: 0
//                    to: 100
//                    value: {
//                        if (listViewBackup.currentIndex !== -1) {
//                            var currentItem = settings.backupModel.get(listViewBackup.currentIndex)
//                            if (currentItem) {
//                                return currentItem.m_progress
//                            }
//                        }
//                        return -1
//                    }
//                    visible: {  // 当进度值为 -1 时隐藏进度条
//                        if (listViewBackup.currentIndex !== -1) {
//                            var currentItem = settings.backupModel.get(listViewBackup.currentIndex)
//                            if (currentItem) {
//                                return currentItem.m_progress >= 0
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
//                        width: progressBarMainTask.visualPosition * progressBarMainTask.width
//                        height: progressBarMainTask.height
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
                Layout.alignment: Qt.AlignRight  // 靠右对齐
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
                    enabled: !settings.bIsClickSaveButton
                    hoverEnabled: true  // 启用悬停检测

                    onReleased: {
                        settings.bIsClickSaveButton = true

                        settings.logDebug("qml: Clicked: buttonSettingSave")
                        listViewBackup.currentIndex = -1

                        settings.backupModel.forceLayout(true)
                        settings.updateConfig()
                        settings.saveData()
                        settings.runBackupItemManage()
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

                    onReleased: {
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
