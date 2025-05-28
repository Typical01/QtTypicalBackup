#include "Backup.h"


Backup::Backup(QObject* parent)
	: QObject(parent), m_errorMessage(""), m_sourceFileList(), m_destinationPathList()
{
}

Backup::Backup(const QString& operateName, const QString& sourceFile, const QString& destinationPath,
	bool startBackup, bool setPermissions, float progress)
	: m_errorMessage(""), m_sourceFileList(), m_destinationPathList()
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
	m_errorMessage = otherObject.m_errorMessage;
	m_startBackup = otherObject.m_startBackup;
	m_setPermissions = otherObject.m_setPermissions;
	m_progress = otherObject.m_progress;
	m_sourceFileList = otherObject.m_sourceFileList;
	m_destinationPathList = otherObject.m_destinationPathList;
}

Backup::~Backup()
{}

void Backup::output()
{
	logDebug(QString("Backup::output: \n\t operateName		 : [%1]\n\t sourceFile 		 : [%2]\
\t destinationPath  : [%3]\t startBackup 	 : [%4]\t setPermissions   : [%5]"
).arg(m_operateName).arg(m_sourceFile).arg(m_destinationPath).arg(m_errorMessage).arg(m_startBackup).arg(m_setPermissions));

	logDebug("\t sourceFileList: 开始!\n");
	for (auto& fileList : m_sourceFileList) {
		logDebug(QString("\t\t [%1]").arg(fileList));
	}
	logDebug("\t sourceFileList: 结束!\n");

	logDebug("\t destinationPathList: 开始!\n");
	for (auto& pathList : m_destinationPathList) {
		logDebug(QString("\t\t [%1]").arg(pathList));
	}
	logDebug("\t destinationPathList: 结束!\n");
}
