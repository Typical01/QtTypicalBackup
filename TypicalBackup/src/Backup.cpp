#include "Backup.h"


Backup::Backup(QObject *parent)
	: QObject(parent)
{}

Backup::Backup(const QString & _operateName, const QString & _shellOperate, const QString & _file, 
	const QString & _arg, const bool& _windowShow, const bool& _menuButton)
{
	m_operateName = _operateName;
	m_shellOperate = _shellOperate;
	m_file = _file;
	m_arg = _arg;
	m_windowShow = _windowShow;
	m_menuButton = _menuButton;
}

Backup::Backup(const Backup& otherObject)
{
	m_operateName = otherObject.m_operateName;
	m_shellOperate = otherObject.m_shellOperate;
	m_file = otherObject.m_file;
	m_arg = otherObject.m_arg;
	m_windowShow = otherObject.m_windowShow;
	m_menuButton = otherObject.m_menuButton;
}

Backup::~Backup()
{}

void Backup::output()
{
	qDebug() << "Backup::output: \n" <<
		"\toperateName: " << m_operateName << "\n" <<
		"\tshellOperate: " << m_shellOperate << "\n" <<
		"\tfile		  : " << m_file << "\n" <<
		"\targ		  : " << m_arg << "\n" <<
		"\twindowShow  : " << m_windowShow << "\n" <<
		"\tmenuButton  : " << m_menuButton << "\n";
}