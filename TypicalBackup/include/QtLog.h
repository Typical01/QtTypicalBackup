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
    static void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
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

    static void logDebug(const QString& message)
    {
        static bool initialze = false;
        if (!initialze) {
            qInstallMessageHandler(customMessageHandler);
            initialze = true;
        }

        LogDebug(message.toStdString());
        qDebug() << message;
    }
    static void logDebug(const std::string& message)
    {
        static bool initialze = false;
        if (!initialze) {
            qInstallMessageHandler(customMessageHandler);
            initialze = true;
        }

        LogDebug(message);
        qDebug() << message.c_str();
    }
    static void logDebug(const char* message)
    {
        static bool initialze = false;
        if (!initialze) {
            qInstallMessageHandler(customMessageHandler);
            initialze = true;
        }

        LogDebug(message);
        qDebug() << message;
    }
}
namespace qtytl = QtTypicalTool;