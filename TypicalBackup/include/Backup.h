#pragma once

#include <QObject>
#include <QDebug>



class Backup  : public QObject
{
	Q_OBJECT


	Q_PROPERTY(QString m_operateName READ getOperateName WRITE setOperateName NOTIFY operateNameChanged)
	Q_PROPERTY(QString m_sourceFile READ getSourceFile WRITE setSourceFile NOTIFY sourceFileChanged)
	Q_PROPERTY(QString m_destinationPath READ getDestinationPath WRITE setDestinationPath NOTIFY destinationPathChanged)
	Q_PROPERTY(QString m_errorMessage READ getErrorMessage WRITE setErrorMessage NOTIFY errorMessageChanged)
	Q_PROPERTY(bool m_startBackup READ getStartBackup WRITE setStartBackup NOTIFY startBackupChanged)
	Q_PROPERTY(bool m_setPermissions READ getSetPermissions WRITE setSetPermissions NOTIFY setPermissionsChanged)
	Q_PROPERTY(int32_t m_progress READ getProgress WRITE setProgress NOTIFY progressChanged)
	Q_PROPERTY(QVector<QString> m_sourceFileList READ getSourceFileList WRITE setSourceFileList NOTIFY sourceFileListChanged)
	Q_PROPERTY(QVector<QString> m_destinationPathList READ getDestinationPathList WRITE setDestinationPathList NOTIFY destinationPathListChanged)

private:
	QString m_operateName; //操作名
	QString m_sourceFile; //源文件/夹
	QString m_destinationPath; //目的地路径
	QString m_errorMessage; //错误信息
	bool m_startBackup; //启动时备份
	bool m_setPermissions; //设置权限
	int32_t m_progress; //进度
	QVector<QString> m_sourceFileList;
	QVector<QString> m_destinationPathList;

public:
	Backup(QObject *parent = nullptr);
	Backup(const QString& operateName, const QString& sourceFile, const QString& destinationPath,
		const bool& startBackup, const bool& setPermissions, const int32_t& progress);
	Backup(const Backup& otherObject);
	~Backup();

	bool Backup::operator<(const Backup& other) const { 
		if (!this->m_startBackup && other.m_startBackup) {
			return true;
		}
		else if (this->m_startBackup && !other.m_startBackup) {
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
	Q_INVOKABLE QString getSourceFile() const { return m_sourceFile; }
	Q_INVOKABLE void setSourceFile(const QString& shellOperate) {
		if (m_sourceFile != shellOperate) {
			m_sourceFile = shellOperate;
			emit sourceFileChanged();
		}
	}
	Q_INVOKABLE QString getDestinationPath() const { return m_destinationPath; }
	Q_INVOKABLE void setDestinationPath(const QString& file) {
		if (m_destinationPath != file) {
			m_destinationPath = file;
			emit destinationPathChanged();
		}
	}
	Q_INVOKABLE QString getErrorMessage() const { return m_errorMessage; }
	Q_INVOKABLE void setErrorMessage(const QString& errorMessage) {
		if (m_errorMessage != errorMessage) {
			m_errorMessage = errorMessage;
			emit errorMessageChanged();
		}
	}
	Q_INVOKABLE bool getStartBackup() const { return m_startBackup; }
	Q_INVOKABLE void setStartBackup(const bool& windowShow) {
		if (m_startBackup != windowShow) {
			m_startBackup = windowShow;
			emit startBackupChanged();
		}
	}
	Q_INVOKABLE bool getSetPermissions() const { return m_setPermissions; }
	Q_INVOKABLE void setSetPermissions(const bool& menuButton) {
		if (m_setPermissions != menuButton) {
			m_setPermissions = menuButton;
			emit setPermissionsChanged();
		}
	}
	Q_INVOKABLE int32_t getProgress() const { return m_progress; }
	Q_INVOKABLE void setProgress(const int32_t& progress) {
		if (m_progress != progress) {
			m_progress = progress;
			emit progressChanged();
		}
	}
	Q_INVOKABLE QVector<QString> getSourceFileList() const { return m_sourceFileList; }
	Q_INVOKABLE void setSourceFileList(const QVector<QString>& sourceFileList) {
		if (m_sourceFileList != sourceFileList) {
			m_sourceFileList = sourceFileList;
			emit sourceFileListChanged();
		}
	}
	Q_INVOKABLE QVector<QString> getDestinationPathList() const { return m_destinationPathList; }
	Q_INVOKABLE void setDestinationPathList(const QVector<QString>& destinationPathList) {
		if (m_destinationPathList != destinationPathList) {
			m_destinationPathList = destinationPathList;
			emit destinationPathListChanged();
		}
	}

public:
	Q_INVOKABLE void output();

public slots:

signals:
	void operateNameChanged();
	void sourceFileChanged();
	void destinationPathChanged();
	void errorMessageChanged();
	void startBackupChanged();
	void setPermissionsChanged();
	void progressChanged();
	void sourceFileListChanged();
	void destinationPathListChanged();
};

Q_DECLARE_METATYPE(Backup*)