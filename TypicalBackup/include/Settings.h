#pragma once
#ifndef BACKEND_H
#define BACKEND_H


#include <QApplication>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QQmlContext>

#include <QQmlApplicationEngine>

#include <QObject>
#include <QDir>
#include <QQuickView>

#include <QSharedMemory>
#include <QMessageBox>
#include <QPushButton>
#include <QAbstractListModel>

#include <QThread>
#include <QMetaObject>


#include <QtLog.h>
#include <Backup.h>
//#include <Group.h>
#include <BackupModel.h>
//#include <GroupModel.h>


#include <TypicalTool/Tool.h>



using namespace tytl;
using namespace qtytl;


namespace QtTypicalTool {
    class Settings : public QObject
    {
        Q_OBJECT
    
        Q_PROPERTY(bool bIsSelfAutoStarting READ getIsSelfAutoStarting WRITE setIsSelfAutoStarting NOTIFY isSelfAutoStartingChanged)
        Q_PROPERTY(bool bIsClickSaveButton READ getIsClickSaveButton WRITE setIsClickSaveButton NOTIFY isClickSaveButtonChanged)
        Q_PROPERTY(BackupModel* backupModel READ getShellConfigModel WRITE setShellConfigModel NOTIFY backupModelChanged)
        Q_PROPERTY(QString applicationWindowTitleName READ getApplicationWindowTitleName WRITE setApplicationWindowTitleName NOTIFY applicationWindowTitleNameChanged)

    public:
        Settings(QObject* parent = nullptr);
        ~Settings();

    public:
        QApplication* application;
        QQmlApplicationEngine* qmlApplicationEngine;

    public:
        QString applicationName;
        QString applicationDirPath;
        QString applicationWindowTitleName = TEXT("TypicalBackup v1.1");
        QString applicationConfigPath;

        Json::Value rootConfig;
        JsonManage jsonManage;
        FileSystem fileSystem;

        BackupModel* backupModel;

        std::atomic<bool> bIsErrorMessage = false; // 是否有错误
        bool bIsSelfAutoStarting = false; // 开机自启动
        bool bIsClickSaveButton = true; // 是否点击了[保存]按钮
        bool bAutoStartingToQuitGame = false; // 自启动后退出

    public:
        void Initialize(QApplication* _application, const QString& _applicationName, const QString& _applicationDirPath);

    public:
        Q_INVOKABLE BackupModel* getShellConfigModel() const { return backupModel; }
        Q_INVOKABLE void setShellConfigModel(BackupModel* model) {
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
        Q_INVOKABLE bool getIsClickSaveButton() const { return bIsClickSaveButton; }
        Q_INVOKABLE void setIsClickSaveButton(const bool& IsClickSaveButton) {
            if (bIsClickSaveButton != IsClickSaveButton) {
                bIsClickSaveButton = IsClickSaveButton;
                emit isClickSaveButtonChanged();
            }
        }
        Q_INVOKABLE QString getApplicationWindowTitleName() const { return applicationWindowTitleName; }
        Q_INVOKABLE void setApplicationWindowTitleName(const QString& _applicationWindowTitleName) {
            if (applicationWindowTitleName != _applicationWindowTitleName) {
                applicationWindowTitleName = _applicationWindowTitleName;
                emit applicationWindowTitleNameChanged();
            }
        }

    signals:
        void backupModelChanged();
        void isSelfAutoStartingChanged();
        void isClickSaveButtonChanged();
        void applicationWindowTitleNameChanged();

    public slots:
        //数据加载/保存
        void loadData();
        void saveData();
        void updateConfig();
        void logDebug(const QString& message);

        //其他
        void help();
        void quit();
        void output();

        //qml
        void openMainWindow();
        void onLoadEngine();
        void offLoadEngine();

        //backup
        void backupItemManage(Backup* bakcup = nullptr); //备份项处理
        bool IsVaildFilePath(const QString& _FilePath);
        QStringList pathManage(QString& path); //路径处理
        bool filePathManage(const QString& path, QString& errorMessage); //文件路径处理
        void operateCopyFile(Backup* backup);
        void groupManage(); //分组处理
        void runBackupTask(Backup* backup); //开始备份任务
    };
}

#endif
