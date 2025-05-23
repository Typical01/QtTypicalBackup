#include "Settings.h"


QtTypicalTool::Settings::Settings(QObject* parent)
    : QObject(parent), Application(nullptr), QmlApplicationEngine(nullptr)
    , backupModel(new ShellConfigModel(this))
{
    onLoadEngine();
}

QtTypicalTool::Settings::~Settings()
{
    qDebug() << Printf(TEXT("Settings: 程序结束!")).str().c_str();
}

void QtTypicalTool::Settings::Initialize(QApplication* _Application, const QString& _applicationName, const QString& _applicationDirPath)
{
    Application = _Application;
    applicationName = _applicationName;
    applicationDirPath = _applicationDirPath;

    tytl::Win::WindowDPI();

#ifdef _DEBUG
    Log::SetDebug(true);
#endif
    Log::SetLogFileWrite(true);
    Log::SetLogWriteLevel(LogTip);
    LogDebug.SetLogFileWriteConfig(TEXT("TypicalBackup"), Printf(TEXT("%s/Log"), applicationDirPath.toStdString()).str().c_str());

    qInstallMessageHandler(Settings::customMessageHandler); 
    qRegisterMetaType<Backup*>("Backup*");
    qRegisterMetaType<ShellConfigModel*>("ShellConfigModel*");
    qmlRegisterType<Backup>("com.example.shell", 1, 0, "Backup");
    qmlRegisterType<ShellConfigModel>("com.example.shell", 1, 0, "ShellConfigModel");
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

int32_t QtTypicalTool::Settings::GetIntId()
{
    return IntId++;
}

void QtTypicalTool::Settings::ShellOperate(QMenu* _Menu, ShellConfigModel* _shellConfigModel) {
    qDebug(TEXT("Typical_Tool::WindowsSystem::WindowShell::ShellOperate"));

    for (auto tempShell = _shellConfigModel->getData().begin(); tempShell != _shellConfigModel->getData().end(); ++tempShell) {
        // 判断类型
        //Backup* config = qvariant_cast<Backup*>(tempShell);
        Backup* config = *tempShell;
        if (!config) {
            qWarning() << "Failed to cast QVariant to Backup*";
            continue;
        }

        QString operateName = config->getOperateName();
        bool menuButton = config->getMenuButton();

        // 区分: 程序启动项/程序菜单项
        if (menuButton == false) {
            ExeRunItem.push_back(config);
            qDebug() << "操作名: [" << operateName << "]";
            qDebug() << "  注册: 程序启动项";
            config->output(); // 输出配置
        }
        else {
            int ID = GetIntId();
            ExeMenuItem.insert({ ID, config });
            qDebug() << "操作名: [" << operateName << "]";
            qDebug() << "  注册: 程序菜单项";
            // 添加菜单项
            QAction* MenuItemAction = new QAction(operateName);
            _Menu->addAction(MenuItemAction);
            connect(MenuItemAction, &QAction::triggered, [this, ID]() {
                qDebug() << "Settings::ShellOperate: 选项[" << ID << "]被触发.";
                ExeMenuItemShell(ID);
                });
            config->output(); // 输出配置
        }
    }
}

void QtTypicalTool::Settings::ExeRunItemShell() {
    qDebug() << TEXT("Settings::ExeRunItemShell.");
    //遍历执行所有: 程序启动项
    if (ExeRunItem.size() > 0) {
        for (auto tempShell = ExeRunItem.begin(); tempShell != ExeRunItem.end(); tempShell++) {
            ExecuteAnalyze((*tempShell)->getOperateName(), 
                (*tempShell)->getShellOperate(),
                (*tempShell)->getFile(),
                (*tempShell)->getArg(),
                (*tempShell)->getWindowShow());
        }
    }
    else {
        qDebug() << TEXT("ExeRunItemShell: 没有执行项!");
    }
}

void QtTypicalTool::Settings::ExeMenuItemShell(int32_t _MenuItemID) {
    qDebug() << TEXT("Settings::ExeMenuItemShell.");
    //查找并执行对应菜单ID的 Shell配置
    auto tempIndex = ExeMenuItem.find(_MenuItemID);
    if (tempIndex != ExeMenuItem.end()) {
        Backup* tempShellConfig = tempIndex->second;
        if (tempShellConfig) {
            ExecuteAnalyze(tempShellConfig->getOperateName(),
                tempShellConfig->getShellOperate(),
                tempShellConfig->getFile(),
                tempShellConfig->getArg(),
                tempShellConfig->getWindowShow());
        }
        else {
            qDebug() << Printf(TEXT("ExeMenuItemShell: Backup无效 [%s]!"), _MenuItemID).str().c_str();
        }
    }
    else {
        qDebug() << Printf(TEXT("ExeMenuItemShell: 没有找到菜单选项 [%s]!"), _MenuItemID).str().c_str();
    }
}

void QtTypicalTool::Settings::ExecuteAnalyze(QString OperateName, QString ShellOperate, QString ShellFile, QString ShellArg, bool WindowShow) {
    qDebug(TEXT("Typical_Tool::WindowsSystem::WindowShell::ExecuteAnalyze"));
    if (ShellOperate == TEXT("打开文件") || ShellOperate == TEXT("open")) {
        ShellOperate = TEXT("open");
        qDebug() <<TEXT("ExecuteAnalyze: Shell操作模式(打开文件)");
    }
    else if (ShellOperate == TEXT("管理员运行") || ShellOperate == TEXT("runas")) {
        ShellOperate = TEXT("runas");
        qDebug() <<TEXT("ExecuteAnalyze: Shell操作模式(管理员运行)");
    }
    else if (ShellOperate == TEXT("打开文件夹") || ShellOperate == TEXT("explore")) {
        ShellOperate = TEXT("explore");
        qDebug() <<TEXT("ExecuteAnalyze: Shell操作模式(打开文件夹)");
    }
    else {
        qDebug() << TEXT("ExecuteAnalyze: Shell操作模式错误(打开文件/打开文件夹/管理员运行)");
        qDebug() << Printf(TEXT("ExecuteAnalyze: 操作名: [%s]"), OperateName.toStdString()).str().c_str();
        return;
    }

    int32_t ShowWindow = 0;
    if (WindowShow) {
        ShowWindow = 5;
    }
    qDebug() << Printf(TEXT("ExecuteAnalyze: 窗口显示 [%s]"), WindowShow).str().c_str();

    ShellExecute(NULL, ShellOperate.toStdString().c_str(), ShellFile.toStdString().c_str(), ShellArg.toStdString().c_str(), NULL, ShowWindow);
}

void QtTypicalTool::Settings::loadBaseConfig()
{
    qDebug(TEXT("Settings::LoadBaseConfig: Load."));

    //先创建文件夹(否则后面的文件不能创建): \\Tools\\Config
    QString ConfigDirectory = Printf(TEXT("%s\\Config"), applicationDirPath.toStdString()).str().c_str();
    applicationConfigPath = Printf(TEXT("%s%s"), ConfigDirectory.toStdString(), TEXT("\\Config.ini")).str().c_str();
    fileSystem.SetPath(ConfigDirectory.toStdString());
    if (!fileSystem.CreateDirectorys()) {
        qDebug() << Printf(TEXT("Settings::LoadBaseConfig: 文件夹[%s]已存在!"), ConfigDirectory.toStdString()).str().c_str();
    }

    //文件不存在时，创建
    if (!jsonManage.Init(Printf(TEXT("%s"), applicationConfigPath.toStdString()).str().c_str(), true)) {
        qDebug() << Printf(TEXT("Settings::LoadBaseConfig: 开始创建Json文件[%s]!"), applicationConfigPath.toStdString()).str().c_str();
        qDebug() << Printf(TEXT("Settings::LoadBaseConfig: 配置文件初始化[%s]"), jsonManage.GetJsonFilePath()).str().c_str();

        // 基本设置
        Json::Value BaseConfig;
        BaseConfig[TEXT("注册表开机自启动")] = false;

        // ping百度配置
        Json::Value ping_baidu;
        ping_baidu[TEXT("菜单按键")] = true;
        ping_baidu[TEXT("模式")] = TEXT("打开文件");
        ping_baidu[TEXT("文件")] = TEXT("cmd.exe");
        ping_baidu[TEXT("参数")] = TEXT("/k ping -t www.baidu.com");
        ping_baidu[TEXT("显示窗口")] = true;

        // 笔记本键盘开启配置
        Json::Value BookKeyOn;
        BookKeyOn[TEXT("菜单按键")] = true;
        BookKeyOn[TEXT("模式")] = TEXT("管理员运行");
        BookKeyOn[TEXT("文件")] = TEXT("cmd.exe");
        BookKeyOn[TEXT("参数")] = TEXT("/k sc config i8042prt start= auto");
        BookKeyOn[TEXT("显示窗口")] = true;

        // 笔记本键盘关闭配置
        Json::Value BookKeyOff;
        BookKeyOff[TEXT("菜单按键")] = true;
        BookKeyOff[TEXT("模式")] = TEXT("管理员运行");
        BookKeyOff[TEXT("文件")] = TEXT("cmd.exe");
        BookKeyOff[TEXT("参数")] = TEXT("/k sc config i8042prt start= disabled");
        BookKeyOff[TEXT("显示窗口")] = true;

        rootConfig[TEXT("基本设置")] = BaseConfig;
        rootConfig[TEXT("ping-baidu")] = ping_baidu;
        rootConfig[TEXT("笔记本键盘开")] = BookKeyOn;
        rootConfig[TEXT("笔记本键盘关")] = BookKeyOff;
        jsonManage.SetJsonValue(rootConfig);
        jsonManage.WriteJsonFile();
    }
    else {
        qDebug() <<Printf(TEXT("Settings::LoadBaseConfig: 读取文件[%s]成功!"), applicationConfigPath.toStdString()).str().c_str();
        qDebug() << TEXT("Settings::LoadBaseConfig: jsonManage ToStreamString Start:");
        jsonManage.ToStreamString();
        qDebug() << TEXT("Settings::LoadBaseConfig: jsonManage ToStreamString End!");

        rootConfig = jsonManage.GetJsonValue();
        if (rootConfig.isObject() && rootConfig.isMember(TEXT("基本设置"))) {
            bIsSelfAutoStarting = rootConfig[TEXT("基本设置")][(TEXT("注册表开机自启动"))].asBool();
        }
        else {
            qDebug() << Printf(TEXT("Settings::UpdateConfig: 没有 rootConfig!")).str().c_str();
        }
        qDebug() << Printf(TEXT("Settings::UpdateConfig: 设置 开机自启动[%s]"), ToStr(bIsSelfAutoStarting)).str().c_str();
        qDebug() << Printf(TEXT("Settings::UpdateConfig: rootConfig的配置数量[%s]"), ToStr(rootConfig.size())).str().c_str();
    }
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

void QtTypicalTool::Settings::loadShellConfig(bool reLoad)
{
    // 重新加载: 只加载 菜单/启动项
    if (reLoad) {
        if (!backupModel->empty()) { //修改配置后
            qDebug() << backupModel->getData().size();
            ExeRunItem.clear();
            ExeMenuItem.clear();
        }

        return;
    }


    for (auto& ItConfig = rootConfig.begin(); ItConfig != rootConfig.end(); ++ItConfig) {
        QString OperateName = ItConfig.key().asString().c_str();
        Json::Value Value = *ItConfig;

        // 执行Shell配置（排除"基本设置"）
        if (OperateName != TEXT("基本设置")) {
            qDebug() << Printf(TEXT("Backup: [%s]"), OperateName.toStdString()).str().c_str();

            QString ShellOperate;
            QString File;
            QString Args;
            bool WindowShow;
            bool MenuButton;

            // 读取配置信息 - 使用Json::Value的安全访问方法
            if (Value.isMember("模式")) {
                ShellOperate = Value["模式"].asString().c_str();
                qDebug() << Printf(TEXT("  操作模式: [%s]"), ShellOperate.toStdString()).str().c_str();
            }
            else {
                qDebug() << Printf(TEXT("  Backup: [%s]没有 操作模式!"), OperateName.toStdString()).str().c_str();
            }

            if (Value.isMember("文件")) {
                File = Value["文件"].asString().c_str();
                qDebug() << Printf(TEXT("  文件: %s"), File.toStdString()).str().c_str();
            }
            else {
                qDebug() << Printf(TEXT("  Backup: [%s]没有 文件!"), OperateName.toStdString()).str().c_str();
            }

            if (Value.isMember("参数")) {
                Args = Value["参数"].asString().c_str();
                qDebug() << Printf(TEXT("  参数: %s"), Args.toStdString()).str().c_str();
            }
            else {
                qDebug() << Printf(TEXT("  Backup: [%s]没有 参数!"), OperateName.toStdString()).str().c_str();
            }

            if (Value.isMember("显示窗口")) {
                WindowShow = Value["显示窗口"].asBool();
                qDebug() << Printf(TEXT("  显示窗口: [%s]"), WindowShow).str().c_str();
            }
            else {
                qDebug() << Printf(TEXT("  Backup: [%s]没有 显示窗口!"), OperateName.toStdString()).str().c_str();
            }

            if (Value.isMember("菜单按键")) {
                MenuButton = Value["菜单按键"].asBool();
                qDebug() << Printf(TEXT("  菜单按键: [%s]"), MenuButton).str().c_str();
            }
            else {
                qDebug() << Printf(TEXT("  Backup: [%s]没有 菜单按键!"), OperateName.toStdString()).str().c_str();
            }

            // 创建 Backup 对象
            Backup* config = new Backup();
            config->setOperateName(OperateName);
            config->setShellOperate(ShellOperate);
            config->setFile(File);
            config->setArg(Args);
            config->setWindowShow(WindowShow);
            config->setMenuButton(MenuButton);

            // 将 Backup 对象添加到 QVariantList
            backupModel->addShellConfig(config);
            //shellConfigList.append(QVariant::fromValue(config));
            //shellConfigListSort();
        }
        else {
            qDebug() << Printf(TEXT("OtherConfig: [%s]"), OperateName.toStdString()).str().c_str();
        }
    }
}

void QtTypicalTool::Settings::loadToolsMenu()
{
    qDebug() <<TEXT("LoadToolsMenu");

    Menu->clear(); //清除现有菜单项

    //为Menu添加选项  
    /*QAction* screenResolutionAction = new QAction("修改屏幕分辨率", Menu);
    Menu->addAction(screenResolutionAction);
    Menu->addSeparator();*/

    //Shell插入位置: Menu项总数 - 4
    //ShellOperate(Menu, shellConfigList);
    ShellOperate(Menu, backupModel);

#ifdef _DEBUG
    QAction* ClickTestAction = new QAction("点击测试");
#endif
    QAction* openSettingsAction = new QAction("设置");
    QAction* helpSettingsAction = new QAction("帮助");
    QAction* quitAction = new QAction("退出");

    Menu->addSeparator(); // 添加分隔符
#ifdef _DEBUG
    Menu->addAction(ClickTestAction);
#endif
    Menu->addAction(openSettingsAction);
    Menu->addAction(helpSettingsAction);
    Menu->addSeparator(); // 添加分隔符
    Menu->addAction(quitAction);

    SystemTrayIcon->setContextMenu(Menu); // 强制更新
    SystemTrayIcon->show();

    // 连接托盘菜单动作
    QObject::connect(openSettingsAction, &QAction::triggered, [this]() {
        this->openSettingWindow();
        });
#ifdef _DEBUG
    QObject::connect(ClickTestAction, &QAction::triggered, [this]() {
        this->test();
        });
#endif

    QObject::connect(helpSettingsAction, &QAction::triggered, Application, [this]() {
        this->help();
        });
    QObject::connect(quitAction, &QAction::triggered, Application, &QApplication::quit);

    //Win::RegisterHotKeyMessage RegisterHotKey_SetScreenResolution(TEXT("Ctrl + Alt + F9"), 
    // RegisterHotKey(hWnd_Tray, Menu_ScreenResolution, MOD_CONTROL | MOD_ALT, VK_F9));
}
