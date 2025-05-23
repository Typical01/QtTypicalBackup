#include <QGuiApplication>

#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QApplication>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QMenu>
#include <QUrl>

#include <QQuickView>

#include "Settings.h"



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
    SettingsInstance->Initialize(app, QCoreApplication::applicationName(), 
        QCoreApplication::applicationDirPath()); //保存当前程序目录

#ifdef _DEBUG
    SettingsInstance->openSettingWindow();
#endif

    return app->exec();
}
