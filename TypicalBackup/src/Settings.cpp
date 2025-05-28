#include "Settings.h"


QtTypicalTool::Settings::Settings(QObject* parent)
    : QObject(parent), application(nullptr), qmlApplicationEngine(nullptr)
    , backupModel(new BackupModel(this))
{
    onLoadEngine();
}

QtTypicalTool::Settings::~Settings()
{
    logDebug(TEXT("Settings: 程序结束!"));
}

void QtTypicalTool::Settings::Initialize(QApplication* _application, const QString& _applicationName, const QString& _applicationDirPath)
{
    application = _application;
    applicationName = _applicationName;
    applicationDirPath = _applicationDirPath;

    tytl::Win::WindowDPI();

#ifdef _DEBUG
    Log::SetDebug(true);
#endif
    Log::SetLogFileWrite(true);
    Log::SetLogWriteLevel(LogTip);
    LogDebug.SetLogFileWriteConfig(TEXT("TypicalBackup"), Printf(TEXT("%s/Log"), applicationDirPath.toStdString()));

    qRegisterMetaType<Backup*>("Backup*");
    qRegisterMetaType<BackupModel*>("BackupModel*");
    //qRegisterMetaType<Group*>("Group*");
    //qRegisterMetaType<GroupModel*>("GroupModel*");
    qmlRegisterType<Backup>("com.example.backup", 1, 0, "Backup");
    qmlRegisterType<BackupModel>("com.example.backup", 1, 0, "BackupModel");
    //qmlRegisterType<Group>("com.example.group", 1, 0, "Group");
    //qmlRegisterType<GroupModel>("com.example.group", 1, 0, "GroupModel");
    qmlRegisterType<Settings>("com.example.settings", 1, 0, "Settings");

    logDebug(QString("Settings::Initialize: applicationName[%1]").arg(applicationName));
    logDebug(QString("Settings::Initialize: applicationDirPath[%1]").arg(applicationDirPath));
}

void QtTypicalTool::Settings::loadData()
{
    logDebug(TEXT("Settings::loadData: Load."));

    //先创建文件夹(否则后面的文件不能创建): \\Tools\\Config
    QString ConfigDirectory = Printf(TEXT("%s\\Config"), applicationDirPath.toStdString()).cstr();
    applicationConfigPath = Printf(TEXT("%s%s"), ConfigDirectory.toStdString(), TEXT("\\Config.ini")).cstr();
    logDebug(QString("Settings::loadData: applicationConfigPath[%1]").arg(applicationConfigPath));

    fileSystem.SetPath(ConfigDirectory.toStdString());
    if (!fileSystem.CreateDirectorys()) {
        logDebug(Printf(TEXT("Settings::loadData: 文件夹[%s]已存在!"), ConfigDirectory.toStdString()).cstr());
    }

    //文件不存在时，创建
    if (!jsonManage.Init(Printf(TEXT("%s"), applicationConfigPath.toStdString()).cstr(), true)) {
        logDebug(Printf(TEXT("Settings::loadData: 开始创建Json文件[%s]!"), applicationConfigPath.toStdString()).cstr());
        logDebug(Printf(TEXT("Settings::loadData: 配置文件初始化[%s]"), jsonManage.GetJsonFilePath()).cstr());

        // 基本设置
        Json::Value BaseConfig;
        BaseConfig[TEXT("注册表开机自启动")] = false;

        rootConfig[TEXT("基本设置")] = BaseConfig;

        Json::Value jsonBase;
        jsonBase["注册表开机自启动"] = bIsSelfAutoStarting;
        logDebug(QString("saveData: bIsSelfAutoStarting: [%1]").arg(bIsSelfAutoStarting));
        rootConfig[TEXT("基本设置")] = jsonBase;

        Json::Value jsonBackupItem;
        for (int index = 0; index < 3; ++index) {
            QString operateName = QString("Test%1").arg(index);
            QString sourceFile = QString("%1").arg(index);
            QString destinationPath = QString("%1").arg(index);
            bool startBackup = false;
            bool setPermissions = false;
            float progress = -1.f;

            Json::Value json;
            json["源文件/夹"] = sourceFile.toStdString();
            json["目标路径"] = destinationPath.toStdString();
            json["开始备份"] = startBackup;
            json["设置权限"] = setPermissions;
            json["进度"] = progress;

            jsonBackupItem[operateName.toStdString()] = json;
        }
        rootConfig["备份项"] = jsonBackupItem;

        jsonManage.SetJsonValue(rootConfig);
        jsonManage.WriteJsonFile();
    }
    else {
        logDebug(Printf(TEXT("Settings::loadData: 读取文件[%s]成功!"), applicationConfigPath.toStdString()).cstr());
        logDebug(TEXT("Settings::loadData: jsonManage ToStreamString Start:"));
        jsonManage.ToStreamString();
        logDebug(TEXT("Settings::loadData: jsonManage ToStreamString End!"));

        rootConfig = jsonManage.GetJsonValue();
        if (rootConfig.isObject() && rootConfig.isMember(TEXT("基本设置"))) {
            bIsSelfAutoStarting = rootConfig[TEXT("基本设置")][(TEXT("注册表开机自启动"))].asBool();
        }
        else {
            logDebug(TEXT("Settings::loadData: 没有 rootConfig!"));
        }
        logDebug(Printf(TEXT("Settings::loadData: 设置 开机自启动[%s]"), ToStr(bIsSelfAutoStarting)).cstr());
        logDebug(Printf(TEXT("Settings::loadData: rootConfig的配置数量[%s]"), ToStr(rootConfig.size())).cstr());
    }

    //loadData
    if (rootConfig.isMember("备份项")) {
        Json::Value backupItems = rootConfig["备份项"];
        if (backupItems.isObject()) {
            for (Json::Value::const_iterator it = backupItems.begin(); it != backupItems.end(); ++it) {
                std::string operateName = it.key().asString();
                const Json::Value& item = *it;
                logDebug(QString("Settings::loadData: 备份项[%1]").arg(operateName.c_str()));

                QString sourceFile = item.get("源文件/夹", "").asString().c_str();
                logDebug(QString("\t 源文件/夹 [%1]").arg(sourceFile));
                QString destinationPath = item.get("目标路径", "").asString().c_str();
                logDebug(QString("\t 目标路径  [%1]").arg(destinationPath));
                bool startBackup = item.get("开始备份", false).asBool();
                logDebug(QString("\t 开始备份  [%1]").arg(startBackup));
                bool setPermissions = item.get("设置权限", false).asBool();
                logDebug(QString("\t 设置权限  [%1]").arg(setPermissions));
                float progress = item.get("进度", 0).asFloat();
                logDebug(QString("\t 进度      [%1]").arg(progress));

                // 创建 Backup 对象并设置属性
                Backup* backup = new Backup();
                backup->setOperateName(QString::fromStdString(operateName));
                backup->setSourceFile(sourceFile);
                backup->setDestinationPath(destinationPath);
                backup->setStartBackup(startBackup);
                backup->setSetPermissions(setPermissions);
                backup->setProgress(progress);

                // 将 Backup 对象添加到模型中
                backupModel->addBackup(backup);
            }
        }
    }
}

void QtTypicalTool::Settings::saveData()
{
    logDebug(QString("saveData: QmlList[listViewShellConfig] Item Sun: [%1]").arg(backupModel->rowCount()));
    rootConfig.clear();

    Json::Value jsonBase;
    jsonBase["注册表开机自启动"] = bIsSelfAutoStarting;
    logDebug(QString("saveData: bIsSelfAutoStarting: [%1]").arg(bIsSelfAutoStarting));
    rootConfig[TEXT("基本设置")] = jsonBase;

    Json::Value jsonBackupItem;
    for (int index = 0; index < backupModel->rowCount(); ++index) {
        QModelIndex modelIndex = backupModel->index(index, 0);
        QString operateName = backupModel->data(modelIndex, BackupModel::OperateNameRole).toString();
        QString sourceFile = backupModel->data(modelIndex, BackupModel::SourceFileRole).toString();
        QString destinationPath = backupModel->data(modelIndex, BackupModel::DestinationPathRole).toString();
        bool startBackup = backupModel->data(modelIndex, BackupModel::StartBackupRole).toBool();
        bool setPermissions = backupModel->data(modelIndex, BackupModel::SetPermissionsRole).toBool();
        float progress = backupModel->data(modelIndex, BackupModel::ProgressRole).toFloat();
        /*QVector<QVariant> sourceFileList = backupModel->data(modelIndex, BackupModel::SourceFileListRole).toList().toVector();
        QVector<QVariant> destinationPathList = backupModel->data(modelIndex, BackupModel::DestinationPathListRole).toList().toVector();*/

        Json::Value json;
        json["源文件/夹"] = sourceFile.toStdString();
        logDebug(QString("saveData: sourceFile [%1]").arg(sourceFile));
        json["目标路径"] = destinationPath.toStdString();
        logDebug(QString("saveData: destinationPath [%1]").arg(destinationPath));
        json["开始备份"] = startBackup;
        logDebug(QString("saveData: startBackup [%1]").arg(startBackup));
        json["设置权限"] = setPermissions;
        logDebug(QString("saveData: setPermissions [%1]").arg(setPermissions));
        json["进度"] = progress;
        logDebug(QString("saveData: progress [%1]").arg(progress));

        //// 构建源文件列表的 JSON 数组
        //Json::Value jsonSourceFileList(Json::arrayValue);
        //for (const QVariant& item : sourceFileList) {
        //    jsonSourceFileList.append(item.toString().toStdString());
        //    logDebug(QString("saveData: sourceFileList Item [%1]").arg(item.toString()));
        //}
        //json["源文件列表"] = jsonSourceFileList;

        //// 构建目标路径列表的 JSON 数组
        //Json::Value jsonDestinationPathList(Json::arrayValue);
        //for (const QVariant& item : destinationPathList) {
        //    jsonDestinationPathList.append(item.toString().toStdString());
        //    logDebug(QString("saveData: destinationPathList Item [%1]").arg(item.toString()));
        //}
        //json["目标路径列表"] = jsonDestinationPathList;

        jsonBackupItem[operateName.toStdString()] = json;
        logDebug(QString("saveData: operateName [%1]").arg(operateName));
    }
    rootConfig["备份项"] = jsonBackupItem;

    jsonManage.SetJsonValue(rootConfig);
    jsonManage.ToStreamString();
    logDebug(QString("saveData: Json File Path[%1]").arg(jsonManage.GetJsonFilePath().c_str()));
    jsonManage.WriteJsonFile();
    logDebug("saveData: 完成!");
}

void QtTypicalTool::Settings::updateConfig()
{
    logDebug(TEXT("Typical_Tool::WindowsSystem::WindowShell::updateConfig"));

    if (jsonManage.GetJsonValue().empty()) { //修改配置后, 重新加载
        logDebug(TEXT("Settings::UpdateConfig: jsonManage为空!"));
        return;
    }
    //获取更新后的 rootConfig
    logDebug(TEXT("Settings::UpdateConfig: jsonManage ToStreamString Start:"));
    jsonManage.ToStreamString();
    logDebug(TEXT("Settings::UpdateConfig: jsonManage ToStreamString End!"));

    //更新 开机自启动
    if (bIsSelfAutoStarting != true) {
        logDebug(TEXT("Settings::UpdateConfig: 注册开机自启动[false]"));

        if (Win::SetSelfStarting(applicationName.toStdString(), Printf(TEXT("\"%s\\%s.exe\""), 
            QDir::toNativeSeparators(applicationDirPath).toStdString(), QDir::toNativeSeparators(applicationName).toStdString()).str(), TEXT(""), false)) {
            logDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 删除成功!"));
        }
        else {
            logDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 删除失败!"));
        }
    }
    else {
        logDebug(TEXT("Settings::UpdateConfig: 注册开机自启动[true]"));
        if (Win::SetSelfStarting(applicationName.toStdString(), Printf(TEXT("\"%s\\%s.exe\""), 
            QDir::toNativeSeparators(applicationDirPath).toStdString(), QDir::toNativeSeparators(applicationName).toStdString()).str(), TEXT(""), true)) {
            logDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 添加成功!"));
        }
        else {
            logDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 添加失败!"));
        }
    }
}

void QtTypicalTool::Settings::logDebug(const QString& message)
{
    qtytl::logDebug(message);
}

void QtTypicalTool::Settings::help() 
{
    QMessageBox::warning(nullptr, "TypicalBackup: 帮助",
        "作者: Typical01\nGithub: https://github.com/Typical01\n\n本程序由Qt框架制作.\nQt: https://download.qt.io/archive/qt/5.12/5.12.0/single/qt-everywhere-src-5.12.0.zip");
}

void QtTypicalTool::Settings::quit()
{
    QCoreApplication::quit();
}

void QtTypicalTool::Settings::output()
{
}

void QtTypicalTool::Settings::openMainWindow()
{
    offLoadEngine();
    if (qmlApplicationEngine) {
        qmlApplicationEngine->load(QUrl(QStringLiteral("qrc:/qt/qml/typicalbackup/resource/main.qml")));

        if (qmlApplicationEngine->rootObjects().isEmpty()) {
            QMessageBox::critical(nullptr, "TypicalBackup", "加载 QML:main.qml 文件失败！\n确定后退出!");
            delete qmlApplicationEngine;
            qmlApplicationEngine = nullptr;

            QCoreApplication::quit();
        }
    }
}

void QtTypicalTool::Settings::onLoadEngine()
{
    qmlApplicationEngine = new QQmlApplicationEngine();
    qmlApplicationEngine->rootContext()->setContextProperty("settings", this);
    //qmlApplicationEngine->rootContext()->setContextProperty("backupModel", this->backupModel);
}

void QtTypicalTool::Settings::offLoadEngine()
{
    if (qmlApplicationEngine) {
        for (QObject* object : qmlApplicationEngine->rootObjects()) {
            QWindow* window = qobject_cast<QWindow*>(object);
            if (window) {
                window->close();
            }
        }

        if (!qmlApplicationEngine->rootObjects().isEmpty()) {
            qmlApplicationEngine->rootObjects().first()->deleteLater();
        }
    }
}

void QtTypicalTool::Settings::backupItemManage(Backup* bakcup)
{
    setIsClickSaveButton(false);
    logDebug(QString("Settings::backupItemManage: 备份项处理 开始!"));

    auto model = backupModel->getData();
    if (bakcup) {
        model.clear();
        model.append(bakcup);
    }

    for (auto backupItemIndex = 0; backupItemIndex < model.size(); backupItemIndex++) {
        Backup* backupItem = model[backupItemIndex];
        if (!backupItem) {
            logDebug(QString("Settings::backupItemManage: 备份项无效! 索引[%1]").arg(backupItemIndex));
            continue;
        }

        backupItem->setErrorMessage("");

        QString operateName = backupItem->getOperateName();
        QString sourceFile = backupItem->getSourceFile();
        QString destinationPath = backupItem->getDestinationPath();
        logDebug(QString("Settings::backupItemManage: 备份项[%1]").arg(operateName));

        QVector<QString> sourceFileList;
        for (auto& path : pathManage(sourceFile)) { //将字符串路径分离出来
            logDebug(QString("\t 备份项 [源文件/夹]: [%1][%2]").arg(operateName).arg(sourceFile));

            QString errorMessage = backupItem->getErrorMessage();
            if (!filePathManage(path, errorMessage)) {
                backupItem->setErrorMessage(errorMessage);
                bIsErrorMessage.store(true);
            }
            else {
                sourceFileList.append(path);
                backupItem->setErrorMessage("");
            }
        }
        if (sourceFileList.isEmpty()) {
            QString errorMessage = backupItem->getErrorMessage();
            backupItem->setErrorMessage(errorMessage + "|[源文件/夹]为空!");
        }
        backupItem->setSourceFile(sourceFile);
        backupItem->setSourceFileList(sourceFileList);

        QVector<QString> destinationPathList;
        for (auto& path : pathManage(destinationPath)) { //将字符串路径分离出来
            logDebug(QString("\t 备份项 [目的地路径]: [%1][%2]").arg(operateName).arg(destinationPath));

            QString errorMessage = backupItem->getErrorMessage();
            if (!filePathManage(path, errorMessage)) {
                backupItem->setErrorMessage(errorMessage);
                bIsErrorMessage.store(true);
            }
            else {
                destinationPathList.append(path);
                backupItem->setErrorMessage("");
            }
        }
        if (destinationPathList.isEmpty()) {
            QString errorMessage = backupItem->getErrorMessage();
            backupItem->setErrorMessage(errorMessage + "|[目的地路径]为空!");
        }
        backupItem->setDestinationPath(destinationPath);
        backupItem->setDestinationPathList(destinationPathList);
    }

    logDebug(QString("Settings::backupItemManage: 备份项处理 结束!"));
    setIsClickSaveButton(true);
}

bool QtTypicalTool::Settings::IsVaildFilePath(const QString& _FilePath)
{
    return std::filesystem::exists(std::filesystem::path(_FilePath.toStdString()));
}

QStringList QtTypicalTool::Settings::pathManage(QString& path)
{
    if (path.isEmpty()) {
        return QStringList();
    }

    // 替换所有换行符为 '|'
    path.replace("\r\n", "|"); // Windows 风格
    path.replace("\r", "|");   // Mac 风格
    path.replace("\n", "|");   // Unix/Linux 风格

    // 分割字符串并移除空字符串
    QStringList resultList = path.split("|", QString::SkipEmptyParts);

    return resultList;
}

bool QtTypicalTool::Settings::filePathManage(const QString& path, QString& errorMessage)
{
    if (!IsVaildFilePath(path)) {
        logDebug(QString("Settings::filePathManage: 路径 [%1] 无效!").arg(path));
        errorMessage.append(QString("|路径[%1] 无效!").arg(path));
        return false;
    }
    return true;
}

void QtTypicalTool::Settings::operateCopyFile(Backup* backup)
{
    // 预先计算操作总数
    const int32_t TotalFileOps = backup->getSourceFileList().size() * backup->getDestinationPathList().size();
    int32_t CompletedOps = 0;

    backup->setProgress(0);

    // 复制文件
    for (const auto& SourceFile : backup->getSourceFileList()) {
        for (const auto& DestPath : backup->getDestinationPathList()) {
            tytl::FileSystem fileSystem(SourceFile.toStdString());
            logDebug(QString("Settings::OperateCopyFile: 源文件 [%1] -> 目的地 [%2]")
                .arg(QString::fromStdString(backup->getSourceFile().toStdString()))
                .arg(QString::fromStdString(backup->getDestinationPath().toStdString())));

            // 修改权限
            if (backup->getSetPermissions()) {
                bool permResult = fileSystem.SetPermissions(
                    std::filesystem::perms::owner_read | std::filesystem::perms::owner_write,
                    std::filesystem::perm_options::replace
                );
                if (!permResult) {
                    qWarning() << QString("Settings::OperateCopyFile: 修改权限 [%1] 失败！").arg(backup->getOperateName());
                }
                else {
                    logDebug(QString("Settings::OperateCopyFile: 修改权限 [%1] 成功！").arg(backup->getOperateName()));
                }
            }

            // 执行复制操作
            if (!fileSystem.Copy(DestPath.toStdString())) {
                QString errorMsg = backup->getErrorMessage() + QString::fromStdString(fileSystem.GetErrorMessage());
                qWarning() << QString("Settings::OperateCopyFile: 错误信息 [%1]！").arg(errorMsg);
                qWarning() << QString("Settings::OperateCopyFile: 复制失败: [%1] -> [%2]")
                    .arg(QString::fromStdString(backup->getSourceFile().toStdString()))
                    .arg(QString::fromStdString(backup->getDestinationPath().toStdString()));

                // 添加错误信息
                backup->setErrorMessage(errorMsg);
                bIsErrorMessage.store(true);
            }

            if (!bIsErrorMessage.load()) {
                backup->setErrorMessage("");
            }

            logDebug(QString("Settings::OperateCopyFile: 复制成功: [%1] -> [%2]")
                .arg(QString::fromStdString(backup->getSourceFile().toStdString()))
                .arg(QString::fromStdString(backup->getDestinationPath().toStdString())));

            // 更新进度
            backup->setProgress(++CompletedOps / TotalFileOps);
        }
    }
    backup->setProgress(100);
    backup->setProgress(-1);
}

void QtTypicalTool::Settings::groupManage()
{
}

void QtTypicalTool::Settings::runBackupTask(Backup* backup)
{
    // 创建并启动后台线程
    logDebug(QString("Settings::runBackupTask: 备份开始 [%1]").arg(backup->getOperateName()));
    QThread* thread = QThread::create([=]() {
        operateCopyFile(backup);

        // 任务完成通知
        QMetaObject::invokeMethod(this, [=]() {
            logDebug(Printf(TEXT("USettingWidget::RunBackupTask: [%s]备份完成!"), backup->getOperateName().toStdString()).cstr());

            // 所有任务完成
            if (!bIsErrorMessage.load()) {
                if (bAutoStartingToQuitGame) {
                    QCoreApplication::quit();
                }
            }
            else {
                // 创建警告对话框
                QMessageBox msgBox;
                msgBox.setWindowTitle("警告");
                msgBox.setText("有错误未处理！");
                msgBox.setIcon(QMessageBox::Warning);

                // 添加“返回”按钮
                QPushButton* returnButton = msgBox.addButton("返回", QMessageBox::RejectRole);
                // 添加“确定退出”按钮
                QPushButton* quitButton = msgBox.addButton("确定退出", QMessageBox::AcceptRole);

                // 设置默认按钮（可选）
                msgBox.setDefaultButton(quitButton);

                // 显示对话框并等待用户响应
                msgBox.exec();

                // 判断用户点击的按钮
                if (msgBox.clickedButton() == quitButton) {
                    QCoreApplication::quit();
                }
            }
            }, Qt::QueuedConnection);
        });

    // 在线程完成后自动清理资源
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}