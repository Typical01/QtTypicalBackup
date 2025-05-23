import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.12  // 必须导入此模块
import QtQuick.Layouts 1.12

import com.example.shell 1.0
import com.example.settings 1.0


Window {
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
    //title: settings.applicationWindowTitleName
    flags: Qt.Dialog | Qt.WindowMinMaxButtonsHint | Qt.WindowSystemMenuHint | Qt.WindowTitleHint

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

        // 区域: 一般设置
        ColumnLayout {
            height: 80 
            Layout.fillWidth: true  // 添加此行
            spacing: 5  // 垂直间距
            
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft
                font.bold: true
                font.pointSize: 12
                text: "基础设置"
                font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
            }

            RowLayout {
                implicitWidth: 500; implicitHeight: 50 
                Layout.fillWidth: true
                Layout.leftMargin: 16  // 设置偏移量
                spacing: 5  // 水平间距
                
                CheckBox {
                    id: settingOption
                    text: "开机自启动"   // 单选框文本
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    checked: settings.bIsSelfAutoStarting
                    onToggled: settings.bIsSelfAutoStarting = checked
                }
            }
        }

        // ListView: ShellConfigItem 
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
                    text: "Shell项"
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                Button {
                    id: buttonShellConfigAdd
                    text: "添加"
                    font.family: "Microsoft YaHei"
                    font.pointSize: 14
                    font.bold: true
                    Layout.preferredHeight: 34
                    Layout.preferredWidth: 80

                    onClicked: {
                        settings.logDebug("qml: Clicked: buttonShellConfigAdd")

                        settings.shellConfigModel.addShellConfig(
                            "新项",
                            "打开文件",
                            "",
                            "",
                            true,
                            true
                        )
                        settings.shellConfigModel.forceLayout()
                        listViewShellConfig.currentIndex = listViewShellConfig.model.getRowCount() - 1

//                        settings.shellConfigListAppend(
//                            "新项",
//                            "打开文件",
//                            "",
//                            "",
//                            true,
//                            true
//                        )
//                        listViewShellConfig.model = []; // 清空模型
//                        listViewShellConfig.model = settings.getShellConfigList(); // 重新绑定模型
                    }
                    onPressed: {
                        buttonShellConfigAddBackground.color = "black"
                    }
                    onReleased: {
                        buttonShellConfigAddBackground.color = "white"
                    }
                    background: Rectangle {
                                id: buttonShellConfigAddBackground
                                color: "white"
                                border.color: "black"
                                border.width: 1
                                radius: 4
                    }
                }

                Button {
                    id: buttonShellConfigDel
                    text: "删除"
                    font.family: "Microsoft YaHei"
                    font.pointSize: 14
                    font.bold: true
                    Layout.preferredHeight: 34
                    Layout.preferredWidth: 80

                    onClicked: {
                        settings.logDebug("qml: Clicked: buttonShellConfigDel")
                        var tempIndex = listViewShellConfig.currentIndex
                        if (tempIndex === listViewShellConfig.model.getRowCount() - 1) {
                            settings.shellConfigModel.removeShellConfig(tempIndex)
                            settings.shellConfigModel.forceLayout()
                            listViewShellConfig.currentIndex = tempIndex - 1
                        }
                        else {
                            settings.shellConfigModel.removeShellConfig(tempIndex)
                            settings.shellConfigModel.forceLayout()
                            listViewShellConfig.currentIndex = tempIndex
                        }

//                        settings.shellConfigListRemove(listViewShellConfig.currentIndex)
//                        listViewShellConfig.model = []; // 清空模型
//                        listViewShellConfig.model = settings.getShellConfigList(); // 重新绑定模型
                    }
                    onPressed: {
                        buttonShellConfigDelBackground.color = "black"
                    }
                    onReleased: {
                        buttonShellConfigDelBackground.color = "red"
                    }
                    background: Rectangle {
                                id: buttonShellConfigDelBackground
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
                id: listViewShellConfig

                height: 400 

                clip: true    // 超出部分裁剪
                Layout.fillWidth: true
                Layout.fillHeight: true

                focus: true

                //model: settings.getShellConfigList()
                model: settings.shellConfigModel

                Component.onCompleted: {
                    settings.logDebug("qml: ShellConfigList: onCompleted")
//                    var shellConfigListSize = settings.getShellConfigList().length
//                    if (shellConfigListSize > 1000) {
//                        settings.logDebug("qml: ShellConfigList: ShellConfigList大小[" + shellConfigListSize +"]超出范围[1000]!")
//                        return
//                    }
                }

//                onCurrentIndexChanged: {
//                    var ItemIndex = listViewShellConfig.currentIndex
//                    if (ItemIndex != -1) {
//                        shellConfigItem.visible = true
//                    }
//                    else {
//                        shellConfigItem.visible = false
//                        return
//                    }

//                    var currentItem = settings.getShellConfigList()[ItemIndex]
//                    textFieldOperateName.text = currentItem.getOperateName()
//                    comboBoxMode.currentIndex = comboBoxMode.find(currentItem.getShellOperate())
//                    textFieldFile.text = currentItem.getFile()
//                    textFieldArg.text = currentItem.getArg()
//                    checkBoxShowWindow.checked = currentItem.getWindowShow() === true
//                    checkBoxMenuButton.checked = currentItem.getMenuButton() === true
//                }
                onCurrentIndexChanged: {
                    var currentItem = listViewShellConfig.model.get(listViewShellConfig.currentIndex)
                    if (currentItem) {
                        textFieldOperateName.text = currentItem.m_operateName
                        comboBoxMode.currentIndex = comboBoxMode.find(currentItem.m_shellOperate)
                        textFieldFile.text = currentItem.m_file
                        textFieldArg.text = currentItem.m_arg
                        checkBoxShowWindow.checked = currentItem.m_windowShow
                        checkBoxMenuButton.checked = currentItem.m_menuButton

                        shellConfigItem.visible = true

                    } else {
                        shellConfigItem.visible = false
                    }
                }

                delegate: Rectangle {
                    width: listViewShellConfig.width
                    height: 40
                    color: ListView.isCurrentItem ? "#d0eaff" : "white"
                    border.color: "#cccccc"
                    border.width: 1

                    Row {
                        spacing: 10
                        anchors.centerIn: parent

                        Image {
                            width: 32
                            height: 32
                            source: {
                                    switch (model.shellOperate) {
                                        case "open":
                                            return "qrc:/qt/qml/typicaltools/resource/Icon/File.ico";
                                        case "explore":
                                            return "qrc:/qt/qml/typicaltools/resource/Icon/Folder.ico";
                                        case "runas":
                                            return "qrc:/qt/qml/typicaltools/resource/Icon/Administrator.ico";
                                        case "打开文件":
                                            return "qrc:/qt/qml/typicaltools/resource/Icon/File.ico";
                                        case "打开文件夹":
                                            return "qrc:/qt/qml/typicaltools/resource/Icon/Folder.ico";
                                        case "管理员运行":
                                            return "qrc:/qt/qml/typicaltools/resource/Icon/Administrator.ico";
                                        default:
                                            settings.logDebug("图标: " + model.m_shellOperate)
                                            return "qrc:/qt/qml/typicaltools/resource/Icon/DeerAndPipe.ico";
                                    }
                                }
                        }

                        Text {
                            //text: settings.getShellConfigList()[index].getOperateName()
                            text: model.operateName
                            font.pointSize: 10
                            font.family: "Microsoft YaHei"
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            settings.logDebug("qml: ShellConfigList currentIndex: " + index)
                            listViewShellConfig.currentIndex = index
                        }
                    }
                }

                highlight: Rectangle {
                    color: "green"
                }

                section {
                    //property: "modelData.m_menuButton"
                    property: "menuButton"
                    criteria: ViewSection.FullString
                    delegate: sectionShellConfig
                }
            }
        }

        Component {
            id: sectionShellConfig
            Rectangle{
                width: listViewShellConfig.width
                color: "#dddeee"
                height: 24
                Text {
                    anchors.fill: parent
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    text: section === "true" ? "菜单项" : "启动项"
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

        // 配置: ShellConfigItem 
        ColumnLayout {
            id: shellConfigItem
            visible: false
            height: 180 
            Layout.fillWidth: true  // 添加此行
            spacing: 5  // 垂直间距
            
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft    
                font.bold: true
                font.pointSize: 12
                text: "Shell项配置"
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
                    placeholderText: "例: 打开cmd.exe"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                    selectByMouse: true  // 启用鼠标选区
                    text: {
                        var index = listViewShellConfig.currentIndex;
                        if (index !== -1) {
                            var item = settings.shellConfigModel.get(index);
                            if (item && item.m_operateName !== undefined) {
                                return item.m_operateName;
                            }
                        }
                        return "";
                    }
                    //settings.getShellConfigList()[listViewShellConfig.currentIndex].m_operateName : ""
                    onTextChanged:  {
                        if (listViewShellConfig.currentIndex !== -1) {
                            settings.shellConfigModel.get(listViewShellConfig.currentIndex).m_operateName = text
                            settings.shellConfigModel.forceLayout()
                            //settings.getShellConfigList()[listViewShellConfig.currentIndex].m_operateName = text
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
                    text: "文件路径:"
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 文件: 文本编辑框
                TextField {
                    id: textFieldFile
                    placeholderText: "例: C:/Windows/System32/cmd.exe"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                    selectByMouse: true  // 启用鼠标选区
                    text: {
                        var index = listViewShellConfig.currentIndex;
                        if (index !== -1) {
                            var item = settings.shellConfigModel.get(index);
                            if (item && item.m_file !== undefined) {
                                return item.m_file;
                            }
                        }
                        return "";
                    }
                    //settings.getShellConfigList()[listViewShellConfig.currentIndex].m_file : ""
                    onTextChanged: {
                        if (listViewShellConfig.currentIndex !== -1) {
                           settings.shellConfigModel.get(listViewShellConfig.currentIndex).m_file = text
                            //settings.getShellConfigList()[listViewShellConfig.currentIndex].m_file = text
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
                    text: "参数:"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 参数: 文本编辑框
                TextField {
                    id: textFieldArg
                    placeholderText: "例: -Test -K -C"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    Layout.preferredWidth: 400
                    selectByMouse: true  // 启用鼠标选区
                    text: {
                        var index = listViewShellConfig.currentIndex;
                        if (index !== -1) {
                            var item = settings.shellConfigModel.get(index);
                            if (item && item.m_arg !== undefined) {
                                return item.m_arg;
                            }
                        }
                        return "";
                    }
                    //settings.getShellConfigList()[listViewShellConfig.currentIndex].m_arg : ""
                    onTextChanged: {
                        if (listViewShellConfig.currentIndex !== -1) {
                           settings.shellConfigModel.get(listViewShellConfig.currentIndex).m_arg = text
                            //settings.getShellConfigList()[listViewShellConfig.currentIndex].m_arg = text
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
                    text: "模式:"
                    font.family: "Microsoft YaHei"  // 直接使用系统已安装的字体名称
                }

                // 模式: 下拉列表框
                ComboBox {
                    id: comboBoxMode
                    model: ["打开文件", "打开文件夹", "管理员运行"]
                    font.family: "Microsoft YaHei"
                    font.pointSize: 10
                    Layout.preferredWidth: 400

                    delegate: ItemDelegate {
                        width: comboBoxMode.width
                        contentItem: Text {
                            text: modelData
                            font.family: "Microsoft YaHei"
                            font.pointSize: 12
                            font.bold: true
                            color: "#000000"
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    onActivated: {
                        var index = listViewShellConfig.currentIndex;
                        if (index !== -1) {
                            var item = settings.shellConfigModel.get(index);
                            if (item) {
                                item.setShellOperate(currentText);
                            }
                        }

                        settings.shellConfigModel.forceLayout()
                        //settings.getShellConfigList()[listViewShellConfig.currentIndex].setShellOperate(currentText);
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // 显示窗口: 复选框
                CheckBox {
                    id: checkBoxShowWindow
                    Layout.leftMargin: 94  // 设置偏移量
                    text: "显示窗口"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    checked: {
                        var index = listViewShellConfig.currentIndex;
                        if (index !== -1) {
                            var item = settings.shellConfigModel.get(index);
                            if (item) {
                                return item.m_windowShow;
                            }
                        }
                        return false;
                    }
                         //settings.getShellConfigList()[listViewShellConfig.currentIndex].m_windowShow : true
                    onToggled: {
                        if (listViewShellConfig.currentIndex !== -1) {
                            settings.shellConfigModel.get(listViewShellConfig.currentIndex).m_windowShow = checked
//                            settings.getShellConfigList()[listViewShellConfig.currentIndex].m_windowShow = checked
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                // 菜单按钮: 复选框
                CheckBox {
                    id: checkBoxMenuButton
                    Layout.leftMargin: 94  // 设置偏移量
                    text: "菜单按钮"
                    font.pointSize: 10
                    font.family: "Microsoft YaHei"
                    checked: {
                        var index = listViewShellConfig.currentIndex;
                        if (index !== -1) {
                            var item = settings.shellConfigModel.get(index);
                            if (item) {
                                return item.m_menuButton;
                            }
                        }
                        return false;
                    }
                    onToggled: {
                        if (listViewShellConfig.currentIndex !== -1) {
                            settings.shellConfigModel.get(listViewShellConfig.currentIndex).m_menuButton = checked
                            settings.shellConfigModel.forceLayout()
//                    checked: settings.getShellConfigList()[listViewShellConfig.currentIndex].m_menuButton
//                    onToggled: settings.getShellConfigList()[listViewShellConfig.currentIndex].m_menuButton = checked
                        }
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
                        settings.shellConfigModel.forceLayout(true)
                        listViewShellConfig.currentIndex = -1

                        settings.updateConfig()
                        settings.saveData();
                        settings.loadShellConfig(true)
                        settings.loadToolsMenu()


//                        listViewShellConfig.model = []; // 清空模型
//                        listViewShellConfig.model = settings.getShellConfigList(); // 重新绑定模型
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
                        close()
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
