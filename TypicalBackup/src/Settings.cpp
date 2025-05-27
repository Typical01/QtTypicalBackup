#include "Settings.h"


QtTypicalTool::Settings::Settings(QObject* parent)
    : QObject(parent), application(nullptr), qmlApplicationEngine(nullptr)
    , backupModel(new BackupModel(this))
{
    onLoadEngine();
}

QtTypicalTool::Settings::~Settings()
{
    qDebug() << Printf(TEXT("Settings: 程序结束!")).cstr();
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

    qInstallMessageHandler(Settings::customMessageHandler); 
    qRegisterMetaType<Backup*>("Backup*");
    qRegisterMetaType<BackupModel*>("BackupModel*");
    qRegisterMetaType<Group*>("Group*");
    qRegisterMetaType<GroupModel*>("GroupModel*");
    qmlRegisterType<Backup>("com.example.backup", 1, 0, "Backup");
    qmlRegisterType<BackupModel>("com.example.backup", 1, 0, "BackupModel");
    qmlRegisterType<Group>("com.example.group", 1, 0, "Group");
    qmlRegisterType<GroupModel>("com.example.group", 1, 0, "GroupModel");
    qmlRegisterType<Settings>("com.example.settings", 1, 0, "Settings");
}

void QtTypicalTool::Settings::customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    static QFile file("./Log/log.txt");
    if (!file.isOpen()) {
        file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    }

    QTextStream out(&file);
    QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logLevel;

    switch (type) {
    case QtDebugMsg:
        logLevel = "DEBUG";
        break;
    case QtInfoMsg:
        logLevel = "INFO";
        break;
    case QtWarningMsg:
        logLevel = "WARNING";
        break;
    case QtCriticalMsg:
        logLevel = "CRITICAL";
        break;
    case QtFatalMsg:
        logLevel = "FATAL";
        break;
    }

    out << QString("[%1] %2: %3 (%4:%5, %6)\n")
        .arg(timeStamp)
        .arg(logLevel)
        .arg(msg)
        .arg(context.file)
        .arg(context.line)
        .arg(context.function);
    out.flush();

    if (type == QtFatalMsg) {
        abort();
    }
}

void QtTypicalTool::Settings::loadData()
{
    qDebug(TEXT("Settings::loadData: Load."));

    //先创建文件夹(否则后面的文件不能创建): \\Tools\\Config
    QString ConfigDirectory = Printf(TEXT("%s\\Config"), applicationDirPath.toStdString()).cstr();
    applicationConfigPath = Printf(TEXT("%s%s"), ConfigDirectory.toStdString(), TEXT("\\Config.ini")).cstr();
    fileSystem.SetPath(ConfigDirectory.toStdString());
    if (!fileSystem.CreateDirectorys()) {
        qDebug() << Printf(TEXT("Settings::loadData: 文件夹[%s]已存在!"), ConfigDirectory.toStdString()).cstr();
    }

    //文件不存在时，创建
    if (!jsonManage.Init(Printf(TEXT("%s"), applicationConfigPath.toStdString()).cstr(), true)) {
        qDebug() << Printf(TEXT("Settings::loadData: 开始创建Json文件[%s]!"), applicationConfigPath.toStdString()).cstr();
        qDebug() << Printf(TEXT("Settings::loadData: 配置文件初始化[%s]"), jsonManage.GetJsonFilePath()).cstr();

        // 基本设置
        Json::Value BaseConfig;
        BaseConfig[TEXT("注册表开机自启动")] = false;

        rootConfig[TEXT("基本设置")] = BaseConfig;
        jsonManage.SetJsonValue(rootConfig);
        jsonManage.WriteJsonFile();
    }
    else {
        qDebug() <<Printf(TEXT("Settings::loadData: 读取文件[%s]成功!"), applicationConfigPath.toStdString()).cstr();
        qDebug() << TEXT("Settings::loadData: jsonManage ToStreamString Start:");
        jsonManage.ToStreamString();
        qDebug() << TEXT("Settings::loadData: jsonManage ToStreamString End!");

        rootConfig = jsonManage.GetJsonValue();
        if (rootConfig.isObject() && rootConfig.isMember(TEXT("基本设置"))) {
            bIsSelfAutoStarting = rootConfig[TEXT("基本设置")][(TEXT("注册表开机自启动"))].asBool();
        }
        else {
            qDebug() << Printf(TEXT("Settings::UpdateConfig: 没有 rootConfig!")).cstr();
        }
        qDebug() << Printf(TEXT("Settings::UpdateConfig: 设置 开机自启动[%s]"), ToStr(bIsSelfAutoStarting)).cstr();
        qDebug() << Printf(TEXT("Settings::UpdateConfig: rootConfig的配置数量[%s]"), ToStr(rootConfig.size())).cstr();
    }

    //loadData
    if (rootConfig.isMember("备份项")) {
        Json::Value backupItems = rootConfig["备份项"];
        if (backupItems.isObject()) {
            for (Json::Value::const_iterator it = backupItems.begin(); it != backupItems.end(); ++it) {
                std::string operateName = it.key().asString();
                const Json::Value& item = *it;
                qDebug() << QString("Settings::loadData: 备份项[%1]").arg(operateName.c_str());

                QString sourceFile = item.get("源文件/夹", "").asString().c_str();
                qDebug() << QString("\t 源文件/夹 [%1]").arg(sourceFile);
                QString destinationPath = item.get("目标路径", "").asString().c_str();
                qDebug() << QString("\t 目标路径  [%1]").arg(destinationPath);
                bool startBackup = item.get("开始备份", false).asBool();
                qDebug() << QString("\t 开始备份  [%1]").arg(startBackup);
                bool setPermissions = item.get("设置权限", false).asBool();
                qDebug() << QString("\t 设置权限  [%1]").arg(setPermissions);
                int progress = item.get("进度", 0).asInt();
                qDebug() << QString("\t 进度      [%1]").arg(progress);

                QVector<QString> sourceFileList;
                const Json::Value& jsonSourceFileList = item["源文件列表"];
                if (jsonSourceFileList.isArray()) {
                    for (const auto& val : jsonSourceFileList) {
                        sourceFileList.append(QString::fromStdString(val.asString()));
                        qDebug() << QString("\t 源文件列表[%1]").arg(val.asString().c_str());
                    }
                }

                QVector<QString> destinationPathList;
                const Json::Value& jsonDestinationPathList = item["目标路径列表"];
                if (jsonDestinationPathList.isArray()) {
                    for (const auto& val : jsonDestinationPathList) {
                        destinationPathList.append(QString::fromStdString(val.asString()));
                        qDebug() << QString("\t 目标路径列表[%1]").arg(val.asString().c_str());
                    }
                }

                // 创建 Backup 对象并设置属性
                Backup* backup = new Backup();
                backup->setOperateName(QString::fromStdString(operateName));
                backup->setSourceFile(sourceFile);
                backup->setDestinationPath(destinationPath);
                backup->setStartBackup(startBackup);
                backup->setSetPermissions(setPermissions);
                backup->setProgress(progress);
                backup->setSourceFileList(sourceFileList);
                backup->setDestinationPathList(destinationPathList);

                // 将 Backup 对象添加到模型中
                backupModel->addBackup(backup);
            }
        }
    }
}

void QtTypicalTool::Settings::saveData()
{
    qDebug() << "saveData: QmlList[listViewShellConfig] Item Sun: " << backupModel->rowCount();
    rootConfig.clear();

    Json::Value jsonBase;
    jsonBase["注册表开机自启动"] = bIsSelfAutoStarting;
    rootConfig[TEXT("基本设置")] = jsonBase;

    Json::Value jsonBackupItem;
    for (int index = 0; index < backupModel->rowCount(); ++index) {
        QModelIndex modelIndex = backupModel->index(index, 0);
        QString operateName = backupModel->data(modelIndex, BackupModel::OperateNameRole).toString();
        QString sourceFile = backupModel->data(modelIndex, BackupModel::SourceFileRole).toString();
        QString destinationPath = backupModel->data(modelIndex, BackupModel::DestinationPathRole).toString();
        QString startBackup = backupModel->data(modelIndex, BackupModel::StartBackupRole).toString();
        bool setPermissions = backupModel->data(modelIndex, BackupModel::SetPermissionsRole).toBool();
        int32_t progress = backupModel->data(modelIndex, BackupModel::ProgressRole).toInt();
        QVector<QVariant> sourceFileList = backupModel->data(modelIndex, BackupModel::SourceFileListRole).toList().toVector();
        QVector<QVariant> destinationPathList = backupModel->data(modelIndex, BackupModel::DestinationPathListRole).toList().toVector();

        Json::Value json;
        json["源文件/夹"] = sourceFile.toStdString();
        qDebug() << "saveData: sourceFile: " << sourceFile;
        json["目标路径"] = destinationPath.toStdString();
        qDebug() << "saveData: destinationPath: " << destinationPath;
        json["开始备份"] = startBackup.toStdString();
        qDebug() << "saveData: startBackup: " << startBackup;
        json["设置权限"] = setPermissions;
        qDebug() << "saveData: setPermissions: " << setPermissions;
        json["进度"] = progress;
        qDebug() << "saveData: progress: " << progress;

        // 构建源文件列表的 JSON 数组
        Json::Value jsonSourceFileList(Json::arrayValue);
        for (const QVariant& item : sourceFileList) {
            jsonSourceFileList.append(item.toString().toStdString());
            qDebug() << "saveData: sourceFileList: " << sourceFileList;
        }
        json["源文件列表"] = jsonSourceFileList;

        // 构建目标路径列表的 JSON 数组
        Json::Value jsonDestinationPathList(Json::arrayValue);
        for (const QVariant& item : destinationPathList) {
            jsonDestinationPathList.append(item.toString().toStdString());
            qDebug() << "saveData: destinationPathList: " << destinationPathList;
        }
        json["目标路径列表"] = jsonDestinationPathList;

        jsonBackupItem[operateName.toStdString()] = json;
        qDebug() << "saveData: operateName: " << operateName;
    }
    rootConfig["备份项"] = jsonBackupItem;

    jsonManage.SetJsonValue(rootConfig);
    jsonManage.ToStreamString();
    jsonManage.WriteJsonFile();
    qDebug() << "saveData: 完成!";
}

void QtTypicalTool::Settings::updateConfig()
{
    qDebug(TEXT("Typical_Tool::WindowsSystem::WindowShell::updateConfig"));

    if (jsonManage.GetJsonValue().empty()) { //修改配置后, 重新加载
        qDebug() << TEXT("Settings::UpdateConfig: jsonManage为空!");
        return;
    }
    //获取更新后的 rootConfig
    qDebug() << TEXT("Settings::UpdateConfig: jsonManage ToStreamString Start:");
    jsonManage.ToStreamString();
    qDebug() << TEXT("Settings::UpdateConfig: jsonManage ToStreamString End!");

    //更新 开机自启动
    if (bIsSelfAutoStarting != true) {
        qDebug(TEXT("Settings::UpdateConfig: 注册开机自启动[false]"), LogTip);

        if (Win::SetSelfStarting(applicationName.toStdString(), Printf(TEXT("\"%s\\%s.exe\""), 
            QDir::toNativeSeparators(applicationDirPath).toStdString(), QDir::toNativeSeparators(applicationName).toStdString()).str(), TEXT(""), false)) {
            qDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 删除成功!"), LogTip);
        }
        else {
            qDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 删除失败!"), LogErr);
        }
    }
    else {
        qDebug(TEXT("Settings::UpdateConfig: 注册开机自启动[true]"), LogTip);
        if (Win::SetSelfStarting(applicationName.toStdString(), Printf(TEXT("\"%s\\%s.exe\""), 
            QDir::toNativeSeparators(applicationDirPath).toStdString(), QDir::toNativeSeparators(applicationName).toStdString()).str(), TEXT(""), true)) {
            qDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 添加成功!"), LogTip);
        }
        else {
            qDebug(TEXT("Settings::UpdateConfig: 注册开机自启动 添加失败!"), LogErr);
        }
    }
}

void QtTypicalTool::Settings::logDebug(const QString& message)
{
    qDebug() << message;
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

void QtTypicalTool::Settings::backupItemManage()
{
    setIsClickSaveButton(true);
    qDebug() << QString("Settings::backupItemManage: 备份项处理 开始!");

    for (auto backup : backupModel->getData()) {
        QString operateName = backup->getOperateName();
        QString sourceFile = backup->getSourceFile();
        QString destinationPath = backup->getDestinationPath();
        qDebug() << QString("Settings::backupItemManage: 备份项[%1]").arg(operateName);

        QVector<QString> sourceFileList;
        for (auto& path : pathManage(sourceFile)) { //将字符串路径分离出来
            qDebug() << QString("\t 备份项 [源文件/夹]: [%1][%2]").arg(operateName).arg(sourceFile);
            
            QString errorMessage = backup->getErrorMessage();
            if (!filePathManage(path, errorMessage)) {
                backup->setErrorMessage(errorMessage);
                bIsErrorMessage.store(true);
            }
            else {
                sourceFileList.append(path);
                backup->setErrorMessage("");
            }
        }
        backup->setSourceFile(sourceFile);
        backup->setSourceFileList(sourceFileList);

        QVector<QString> destinationPathList;
        for (auto& path : pathManage(destinationPath)) { //将字符串路径分离出来
            qDebug() << QString("\t 备份项 [目的地路径]: [%1][%2]").arg(operateName).arg(destinationPath);

            QString errorMessage = backup->getErrorMessage();
            if (!filePathManage(path, errorMessage)) {
                backup->setErrorMessage(errorMessage);
                bIsErrorMessage.store(true);
            }
            else {
                destinationPathList.append(path);
                backup->setErrorMessage("");
            }
        }
        backup->setDestinationPath(destinationPath);
        backup->setDestinationPathList(destinationPathList);
    }

    setIsClickSaveButton(false);
}

bool QtTypicalTool::Settings::IsVaildFilePath(const QString& _FilePath)
{
    return std::filesystem::exists(std::filesystem::path(_FilePath.toStdString()));
}

QStringList QtTypicalTool::Settings::pathManage(QString& path)
{
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
        qDebug() << QString("Settings::filePathManage: 路径 [%1] 无效!").arg(path);
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

    // 复制文件
    for (const auto& SourceFile : backup->getSourceFileList()) {
        for (const auto& DestPath : backup->getDestinationPathList()) {
            tytl::FileSystem fileSystem(SourceFile.toStdString());
            qDebug() << QString("Settings::OperateCopyFile: 源文件 [%1] -> 目的地 [%2]")
                .arg(QString::fromStdString(backup->getSourceFile().toStdString()))
                .arg(QString::fromStdString(backup->getDestinationPath().toStdString()));

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
                    qDebug() << QString("Settings::OperateCopyFile: 修改权限 [%1] 成功！").arg(backup->getOperateName());
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

            qDebug() << QString("Settings::OperateCopyFile: 复制成功: [%1] -> [%2]")
                .arg(QString::fromStdString(backup->getSourceFile().toStdString()))
                .arg(QString::fromStdString(backup->getDestinationPath().toStdString()));

            // 更新进度
            backup->setProgress(static_cast<int32_t>(++CompletedOps) / TotalFileOps * 0.5f);
        }
    }
}

void QtTypicalTool::Settings::groupManage()
{
}

void QtTypicalTool::Settings::runBackupTask(Backup* backup)
{
    // 创建并启动后台线程
    QThread* thread = QThread::create([=]() {
        operateCopyFile(backup);

        // 任务完成通知
        QMetaObject::invokeMethod(this, [=]() {
            qDebug() << Printf(TEXT("USettingWidget::RunBackupTask: [%s]备份完成!"), backup->getOperateName().toStdString()).cstr();

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