#pragma once
#ifndef BACKEND_H
#define BACKEND_H


#include <QApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSystemTrayIcon>
#include <QMenu>

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QQuickView>

#include <QSharedMemory>
#include <QMessageBox>
#include <QAbstractListModel>

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>


#include <Backup.h>
#include <ShellConfigModel.h>


#include <TypicalTool/Tool.h>



using namespace tytl;


namespace QtTypicalTool {
    class Settings : public QObject
    {
        Q_OBJECT
    
        Q_PROPERTY(bool bIsSelfAutoStarting READ getIsSelfAutoStarting WRITE setIsSelfAutoStarting NOTIFY isSelfAutoStartingChanged)
        Q_PROPERTY(ShellConfigModel* backupModel READ getShellConfigModel WRITE setShellConfigModel NOTIFY backupModelChanged)

    public:
        Settings(QObject* parent = nullptr);
        ~Settings();

    public:
        QApplication* Application;
        QQmlApplicationEngine* QmlApplicationEngine;

    public:
        QString applicationName;
        QString applicationDirPath;
        QString applicationWindowTitleName = TEXT("TypicalBackup v1.1");
        QString applicationConfigPath;

        Json::Value rootConfig;
        JsonManage jsonManage;
        FileSystem fileSystem;

        ShellConfigModel* backupModel;
        std::vector<Backup*> ExeRunItem; //程序启动项
        std::map<int32_t, Backup*> ExeMenuItem; //程序菜单项

        bool bIsSelfAutoStarting = false;
        inline static int32_t IntId = 1000;

    public:
        int32_t GetIntId();
        //void ShellOperate(QMenu* _Menu, QVariantList& _ShellConfig);
        void ShellOperate(QMenu* _Menu, ShellConfigModel* _backupModel);
        void ExeRunItemShell();
        void ExeMenuItemShell(int32_t _MenuItemID);
        void ExecuteAnalyze(QString OperateName, QString ShellOperate, QString ShellFile, QString ShellArg = TEXT(""), bool WindowShow = true);

    public:
        void Initialize(QApplication* _Application, const QString& _applicationName, const QString& _applicationDirPath);

        static void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    public:
        Q_INVOKABLE ShellConfigModel* getShellConfigModel() const { return backupModel; }
        Q_INVOKABLE void setShellConfigModel(ShellConfigModel* model) {
            if (backupModel != model) {
                backupModel->setData(*model);
                emit backupModelChanged();
            }
        }
        Q_INVOKABLE bool getIsSelfAutoStarting() const { return bIsSelfAutoStarting; }
        Q_INVOKABLE void setIsSelfAutoStarting(const bool& IsSelfAutoStarting) {
            if (bIsSelfAutoStarting != IsSelfAutoStarting) {
                bIsSelfAutoStarting = IsSelfAutoStarting;
                emit isSelfAutoStartingChanged();
            }
        }

    signals:
        void backupListChanged();
        void backupModelChanged();
        void isSelfAutoStartingChanged();

    public slots:

        void loadBaseConfig();
        void updateConfig();
        void loadShellConfig(bool reLoad = false);
        void loadToolsMenu();

        void logDebug(const QString& message)
        {
            qDebug() << message;
        }

        void test() {
            logDebug(Printf(TEXT("test: Printf!")).str().c_str());
        }

        void help() {
            QMessageBox::warning(nullptr, "TypicalBackup: 帮助", 
                "作者: Typical01\nGithub: https://github.com/Typical01\n\n本程序由Qt框架制作.\nQt: https://download.qt.io/archive/qt/5.12/5.12.0/single/qt-everywhere-src-5.12.0.zip");
        }

        void openSettingWindow() {
            offLoadEngine();
            if (QmlApplicationEngine) {
                QmlApplicationEngine->load(QUrl(QStringLiteral("qrc:/qt/qml/typicalbackup/resource/main.qml")));

                if (QmlApplicationEngine->rootObjects().isEmpty()) {
                    QMessageBox::critical(nullptr, "TypicalBackup", "加载 QML 文件失败！");
                    delete QmlApplicationEngine;
                    QmlApplicationEngine = nullptr;
                }
            }
        }

        void onLoadEngine() {
            QmlApplicationEngine = new QQmlApplicationEngine();
            QmlApplicationEngine->rootContext()->setContextProperty("settings", this);
        }

        void offLoadEngine() {
            if (QmlApplicationEngine) {
                for (QObject* object : QmlApplicationEngine->rootObjects()) {
                    QWindow* window = qobject_cast<QWindow*>(object);
                    if (window) {
                        window->close();
                    }
                }

                if (!QmlApplicationEngine->rootObjects().isEmpty()) {
                    QmlApplicationEngine->rootObjects().first()->deleteLater();
                }
            }
        }

        void saveData() {
            qDebug() << "saveData: QmlList[listViewShellConfig] Item Sun: " << backupModel->rowCount();
            rootConfig.clear();

            Json::Value jsonBase;
            jsonBase["注册表开机自启动"] = bIsSelfAutoStarting;
            rootConfig[TEXT("基本设置")] = jsonBase;

#ifdef QVariantList
            for (int index = 0; index < backupList.size(); ++index) {
                qDebug() << "saveData: QmlList[listViewShellConfig] Item Count: " << index;

                // 处理获取到的数据
                Backup* config = qvariant_cast<Backup*>(backupList[index]); 

                Json::Value json;
                json["模式"] = config->getShellOperate().toStdString();
                qDebug() << "saveData: shellOperate: " << config->getShellOperate();
                json["文件"] = config->getFile().toStdString();
                qDebug() << "saveData: file:         " << config->getFile();
                json["参数"] = config->getArg().toStdString();
                qDebug() << "saveData: arg:          " << config->getArg();
                json["显示窗口"] = config->getWindowShow();
                qDebug() << "saveData: windowShow:   " << config->getWindowShow();
                json["菜单按键"] = config->getMenuButton();
                qDebug() << "saveData: menuButton:   " << config->getMenuButton();

                rootConfig[config->getOperateName().toStdString()] = json;
                qDebug() << "saveData: operateName:  " << config->getOperateName();
            }
#else
            for (int index = 0; index < backupModel->rowCount(); ++index) {
                QModelIndex modelIndex = backupModel->index(index, 0);
                QString operateName = backupModel->data(modelIndex, ShellConfigModel::OperateNameRole).toString();
                QString shellOperate = backupModel->data(modelIndex, ShellConfigModel::ShellOperateRole).toString();
                QString file = backupModel->data(modelIndex, ShellConfigModel::FileRole).toString();
                QString arg = backupModel->data(modelIndex, ShellConfigModel::ArgRole).toString();
                bool windowShow = backupModel->data(modelIndex, ShellConfigModel::WindowShowRole).toBool();
                bool menuButton = backupModel->data(modelIndex, ShellConfigModel::MenuButtonRole).toBool();

                Json::Value json;
                json["模式"] = shellOperate.toStdString();
                qDebug() << "saveData: shellOperate: " << shellOperate;
                json["文件"] = file.toStdString();
                qDebug() << "saveData: file: " << file;
                json["参数"] = arg.toStdString();
                qDebug() << "saveData: arg: " << arg;
                json["显示窗口"] = windowShow;
                qDebug() << "saveData: windowShow: " << windowShow;
                json["菜单按键"] = menuButton;
                qDebug() << "saveData: menuButton: " << menuButton;

                rootConfig[operateName.toStdString()] = json;
                qDebug() << "saveData: operateName: " << operateName;
            }
#endif

            jsonManage.SetJsonValue(rootConfig);
            jsonManage.ToStreamString();
            jsonManage.WriteJsonFile();
            qDebug() << "saveData: 完成!";
        }
    };
}

#endif
