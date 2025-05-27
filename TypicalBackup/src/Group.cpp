#include "Group.h"


Group::Group(QObject *parent)
	: QObject(parent)
{}

Group::Group(QString groupName)
	: m_data(), m_groupName(groupName)
{
}

Group::Group(const Group& otherObject)
{
	m_groupName = otherObject.m_groupName;
	m_data = otherObject.m_data;
}

Group::~Group()
{}

void Group::output()
{
	qDebug() << "Group::output: \n" <<
		"\tgroupName: " << m_groupName << "\n" <<
		"\tdata: \n";

	for (auto data : m_data) {
		data->output();
	}
}