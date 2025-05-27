#include "Backup.h"


Backup::Backup(QObject *parent)
	: QObject(parent)
{}

Backup::Backup(const QString & operateName, const QString & sourceFile, const QString & destinationPath,
	const bool& startBackup, const bool& setPermissions, const int32_t& progress)
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
	qDebug() << "Backup::output: \n" <<
		"\t operateName		 :" << m_operateName << "\n" <<
		"\t sourceFile 		 :" << m_sourceFile << "\n" <<
		"\t destinationPath  :" << m_destinationPath << "\n" <<
		"\t startBackup 	 :" << m_startBackup << "\n" <<
		"\t setPermissions   :" << m_setPermissions << "\n";
}