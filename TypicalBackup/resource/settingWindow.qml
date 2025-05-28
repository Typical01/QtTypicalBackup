import QtQuick 2.12
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import com.example.settings 1.0


Dialog {
    id: settingsDialog
    width: 350
    height: 200
    title: "设置"
    modal: Qt.ApplicationModal
    font.family: "Microsoft YaHei"
    font.pixelSize: 14
    standardButtons: Dialog.Ok | Dialog.Cancel
    parent: ApplicationWindow.contentItem  // 绑定到主窗口内容区

    // 居中计算
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    contentItem: Column {
         spacing: 10
         padding: 10

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
                 font.family: "Microsoft YaHei"
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
                     checked: settings.getIsSelfAutoStarting()
                     onToggled: settings.setIsSelfAutoStarting(checked)
                 }
             }
         }

         // 添加更多设置控件
    }

    onAccepted: {
        settings.logDebug("设置已保存")
    }

    onRejected: {
        settings.logDebug("设置已取消")
    }
}
