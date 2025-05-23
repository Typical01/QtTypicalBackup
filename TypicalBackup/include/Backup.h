#pragma once

#include <QObject>
#include <QDebug>



class Backup  : public QObject
{
	Q_OBJECT


	Q_PROPERTY(QString m_operateName READ getOperateName WRITE setOperateName NOTIFY operateNameChanged)
	Q_PROPERTY(QString m_shellOperate READ getShellOperate WRITE setShellOperate NOTIFY shellOperateChanged)
	Q_PROPERTY(QString m_file READ getFile WRITE setFile NOTIFY fileChanged)
	Q_PROPERTY(QString m_arg READ getArg WRITE setArg NOTIFY argChanged)
	Q_PROPERTY(bool m_windowShow READ getWindowShow WRITE setWindowShow NOTIFY windowShowChanged)
	Q_PROPERTY(bool m_menuButton READ getMenuButton WRITE setMenuButton NOTIFY menuButtonChanged)

private:
	QString m_operateName; //操作名
	QString m_shellOperate; //Shell操作
	QString m_file; //文件
	QString m_arg; //参数
	bool m_windowShow; //窗口显示
	bool m_menuButton; //菜单按键

public:
	Backup(QObject *parent = nullptr);
	Backup(const QString& m_operateName, const QString& m_shellOperate, const QString& m_file, 
		const QString& m_arg, const bool& m_windowShow, const bool& m_menuButton);
	Backup(const Backup& otherObject);
	~Backup();

	bool Backup::operator<(const Backup& other) const { 
		if (!this->m_menuButton && other.m_menuButton) {
			return true;
		}
		else if (this->m_menuButton && !other.m_menuButton) {
			return false;
		}
		else {
			return this->m_operateName < other.m_operateName;
		}
	}

public:
	Q_INVOKABLE QString getOperateName() const { return m_operateName; }
	Q_INVOKABLE void setOperateName(const QString& operateName) {
		if (m_operateName != operateName) {
			m_operateName = operateName;
			emit operateNameChanged();
		}
	}
	Q_INVOKABLE QString getShellOperate() const { return m_shellOperate; }
	Q_INVOKABLE void setShellOperate(const QString& shellOperate) {
		if (m_shellOperate != shellOperate) {
			m_shellOperate = shellOperate;
			emit shellOperateChanged();
		}
	}
	Q_INVOKABLE QString getFile() const { return m_file; }
	Q_INVOKABLE void setFile(const QString& file) {
		if (m_file != file) {
			m_file = file;
			emit fileChanged();
		}
	}
	Q_INVOKABLE QString getArg() const { return m_arg; }
	Q_INVOKABLE void setArg(const QString& arg) {
		if (m_arg != arg) {
			m_arg = arg;
			emit argChanged();
		}
	}
	Q_INVOKABLE bool getWindowShow() const { return m_windowShow; }
	Q_INVOKABLE void setWindowShow(const bool& windowShow) {
		if (m_windowShow != windowShow) {
			m_windowShow = windowShow;
			emit windowShowChanged();
		}
	}
	Q_INVOKABLE bool getMenuButton() const { return m_menuButton; }
	Q_INVOKABLE void setMenuButton(const bool& menuButton) {
		if (m_menuButton != menuButton) {
			m_menuButton = menuButton;
			emit menuButtonChanged();
		}
	}

public:
	Q_INVOKABLE void output();

public slots:

signals:
	void operateNameChanged();
	void shellOperateChanged();
	void fileChanged();
	void argChanged();
	void windowShowChanged();
	void menuButtonChanged();
};

Q_DECLARE_METATYPE(Backup*)