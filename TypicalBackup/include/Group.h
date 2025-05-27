#pragma once

#include <QObject>
#include <QDebug>
#include <QVector>

#include "Backup.h"



class Group : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString m_groupName READ getGroupName WRITE setGroupName NOTIFY groupNameChanged)
	Q_PROPERTY(QVector<Backup*> m_data READ getData WRITE setData NOTIFY dataChanged)

private:
	QString m_groupName;
	QVector<Backup*> m_data;

public:
	Group(QObject *parent = nullptr);
	Group(QString groupName);
	Group(const Group& otherObject);
	~Group();

	bool Group::operator<(const Group& other) const { 
		return this->m_groupName < other.m_groupName;
	}

public:
	Q_INVOKABLE QVector<Backup*> getData() const { return m_data; }
	Q_INVOKABLE void setData(QVector<Backup*> data) {
		if (m_data != data) {
			m_data = data;
			emit dataChanged();
		}
	}
	Q_INVOKABLE QString getGroupName() const { return m_groupName; }
	Q_INVOKABLE void setGroupName(QString groupName) {
		if (m_groupName != groupName) {
			m_groupName = groupName;
			emit groupNameChanged();
		}
	}

public:
	Q_INVOKABLE void output();

public slots:

signals:
	void dataChanged();
	void groupNameChanged();
};

Q_DECLARE_METATYPE(Group*)