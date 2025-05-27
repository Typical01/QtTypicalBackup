#include "GroupModel.h"




GroupModel::GroupModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int GroupModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant GroupModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_data.count())
        return QVariant();

    Group* group = m_data.at(index.row());
    switch (role) {
    case GroupNameRole:
        return group->getGroupName();
    case DataRole:
        return QVariant::fromValue(group->getData());
    default:
        return QVariant();
    }
}

Q_INVOKABLE void GroupModel::forceLayout(bool bIsSort)
{
    if (bIsSort) {
        sort();
    }
    // 实现逻辑，例如发出 dataChanged 信号以刷新视图
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

void GroupModel::sort()
{
    std::sort(m_data.begin(), m_data.end());
}

void GroupModel::addGroup(Group* config)
{
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    m_data.append(config);
    sort();
    endInsertRows();
}

void GroupModel::addGroup(const QString& groupName)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    Group* group = new Group(groupName);
    m_data.append(group);
    sort();
    endInsertRows();
}

bool GroupModel::removeGroup(int index) {
    if (index < 0 || index >= m_data.count())
        return false;

    beginRemoveRows(QModelIndex(), index, index);
    delete m_data.at(index); // 如果您拥有对象的所有权
    m_data.removeAt(index);
    sort();
    endRemoveRows();

    return true;
}

QHash<int, QByteArray> GroupModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[GroupNameRole] = "groupName";
    roles[DataRole] = "data";
    return roles;
}
