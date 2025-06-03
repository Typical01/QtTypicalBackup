#include "Backup.h"


Backup::Backup(QObject* parent)
	: QObject(parent), m_sourceFileError(false), m_destinationPathError(false), m_errorMessageList(), m_sourceFileList(), m_destinationPathList()
{
}

Backup::Backup(const QString& operateName, const QString& sourceFile, const QString& destinationPath,
	bool startBackup, bool setPermissions, float progress)
	: m_sourceFileError(false), m_destinationPathError(false), m_errorMessageList(), m_sourceFileList(), m_destinationPathList()
{
	m_operateName = operateName;
	m_sourceFile = sourceFile;
	m_destinationPath = destinationPath;
	m_startBackup = startBackup;
	m_setPermissions = setPermissions;
	m_progress = progress;
}

Backup::Backup(const Backup& otherObject)
{
	m_operateName = otherObject.m_operateName;
	m_sourceFile = otherObject.m_sourceFile;
	m_destinationPath = otherObject.m_destinationPath;
	m_sourceFileError = otherObject.m_sourceFileError;
	m_destinationPathError = otherObject.m_destinationPathError;
	m_startBackup = otherObject.m_startBackup;
	m_setPermissions = otherObject.m_setPermissions;
	m_progress = otherObject.m_progress;
	m_errorMessageList = otherObject.m_errorMessageList;
	m_sourceFileList = otherObject.m_sourceFileList;
	m_destinationPathList = otherObject.m_destinationPathList;
}

Backup::~Backup()
{}

void Backup::output()
{
	logDebug(QString("Backup::output: "));
	logDebug(QString("    operateName: [%1]").arg(m_operateName));
	logDebug(QString("    sourceFile: [%1]").arg(m_sourceFile));
	logDebug(QString("    destinationPath: [%1]").arg(m_destinationPath));
	logDebug(QString("    sourceFileError: [%1]").arg(m_sourceFileError));
	logDebug(QString("    destinationPathError: [%1]").arg(m_destinationPathError));
	logDebug(QString("    startBackup: [%1]").arg(m_startBackup));
	logDebug(QString("    setPermissions: [%1]").arg(m_setPermissions));

	logDebug("\t errorMessageList: 开始!\n");
	for (auto& fileList : m_errorMessageList) {
		logDebug(QString("    [%1]").arg(fileList));
	}
	logDebug("\t errorMessageList: 结束!\n");

	logDebug("\t sourceFileList: 开始!\n");
	for (auto& fileList : m_sourceFileList) {
		logDebug(QString("    [%1]").arg(fileList));
	}
	logDebug("\t sourceFileList: 结束!\n");

	logDebug("\t destinationPathList: 开始!\n");
	for (auto& pathList : m_destinationPathList) {
		logDebug(QString("    [%1]").arg(pathList));
	}
	logDebug("\t destinationPathList: 结束!\n");
}
