#include <QIcon>
#include <QUrl>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTextCodec>

#include "Settings.h"



bool GetCommandLineArgument(QCommandLineParser& parser, const QString& param, QString* paramValue = nullptr)
{
    // 移除参数前缀 "-" 或 "--"
    QString paramName = param;
    if (param.startsWith("--")) {
        paramName = param.mid(2);
    }
    else if (param.startsWith("-")) {
        paramName = param.mid(1);
    }

    // 检查参数是否存在
    if (parser.isSet(paramName)) {
        if (paramValue) {
            *paramValue = parser.value(paramName);
        }
        return true;
    }

    return false;
}

class SingleInstanceGuard {
public:
    explicit SingleInstanceGuard(const QString& key)
        : m_shared(key)
    {
        // 尝试创建1字节的共享内存
        if (!m_shared.create(1)) {
            if (m_shared.error() == QSharedMemory::AlreadyExists) {
                m_isRunning = true;
                return;
            }
            // 处理其他错误
            qCritical() << "Shared memory error:" << m_shared.errorString();
        }
        m_isRunning = false;
    }

    ~SingleInstanceGuard() {
        if (!m_isRunning) {
            m_shared.detach(); // 正常退出时释放
        }
    }

    bool isRunning() const { return m_isRunning; }

private:
    QSharedMemory m_shared;
    bool m_isRunning = true;
};


int main(int argc, char* argv[])
{
#if defined(Q_OS_WIN) && QT_VERSION_CHECK(5, 6, 0) <= QT_VERSION && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication* app = new QApplication(argc, argv);
    app->setApplicationName("TypicalBackup");  // 设置应用程序名称
    app->setQuitOnLastWindowClosed(false);

    QIcon Icon(":/qt/qml/typicalbackup/resource/Icon/TypicalBackup.ico");
    app->setWindowIcon(Icon);

    //单例运行
    SingleInstanceGuard guard("TypicalBackup_SINGLE_INSTANCE");
    if (guard.isRunning()) {
        QMessageBox::warning(nullptr, "TypicalBackup", "警告: 程序已在运行中!😵‍💫");
        return 1;
    }

    QtTypicalTool::Settings* SettingsInstance = new QtTypicalTool::Settings();

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption("AutoQuit", "Enable auto-start mode.")); // 定义 --Auto
    parser.process(app->arguments());
    if (GetCommandLineArgument(parser, "AutoQuit")) {
        logDebug("CommandLine: 已启用[开机自启动]");
        SettingsInstance->bAutoStartingToQuitGame = true;
    }

    SettingsInstance->Initialize(app, QCoreApplication::applicationName(), 
        QCoreApplication::applicationDirPath()); //保存当前程序目录

    SettingsInstance->loadData();
    SettingsInstance->runBackupItemManage(nullptr, true);
    SettingsInstance->output();
    SettingsInstance->openMainWindow();
    //SettingsInstance->autoStartBackupItem();

    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [SettingsInstance]() {

        if (!SettingsInstance) return;

        // 通知线程停止
        //if (SettingsInstance->threadBackupItemManage) SettingsInstance->threadBackupItemManage->deleteLater();
        //if (SettingsInstance->threadBackupTask) SettingsInstance->threadBackupTask->deleteLater();
        // 等待线程完成
        //thread->wait();
        });

    return app->exec();
}
