#pragma once

#include <QObject>
#include <QDebug>  
#include <QFile>
#include <QTextStream>
#include <QDateTime>


#include <TypicalTool/Tool.h>


class QtLog  : public QObject
{
	Q_OBJECT

public:
	QtLog(QObject *parent);
	~QtLog();
	
};

namespace QtTypicalTool {
    static QFile logFile("./Log/log.txt");
    static std::atomic<bool> bLogQuit = false;

    static std::mutex g_mutex; // 全局互斥锁
    static void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
    {
        std::lock_guard<std::mutex> lock(g_mutex); // 全局互斥锁

        if (!logFile.isOpen()) {
            logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        }

        QTextStream out(&logFile);
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

    static void clearLogFile()
    {
        std::lock_guard<std::mutex> lock(g_mutex); // 全局互斥锁

        bLogQuit.store(true);

        if (logFile.isOpen()) {
            logFile.flush();
            logFile.close();
        }
    }

    static void logDebug(const QString& message)
    {
        static bool initialze = false;
        if (!initialze) {
            qInstallMessageHandler(customMessageHandler);
            initialze = true;
        }

        //LogRelease(message.toStdString());
        if (!bLogQuit) qDebug() << message;
    }
    static void logDebug(const std::string& message)
    {
        static bool initialze = false;
        if (!initialze) {
            qInstallMessageHandler(customMessageHandler);
            initialze = true;
        }

        //LogRelease(message);
        if (!bLogQuit) qDebug() << message.c_str();
    }
    static void logDebug(const char* message)
    {
        static bool initialze = false;
        if (!initialze) {
            qInstallMessageHandler(customMessageHandler);
            initialze = true;
        }

        //LogRelease(message);
        if (!bLogQuit) qDebug() << message;
    }
}
namespace qtytl = QtTypicalTool;