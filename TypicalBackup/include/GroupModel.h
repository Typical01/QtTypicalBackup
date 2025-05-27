#pragma once

#include <QAbstractListModel>
#include <QVector>
#include "Group.h"

class GroupModel : public QAbstractListModel
{
    Q_OBJECT


public:
    enum GroupRoles {
        GroupNameRole = Qt::UserRole + 1,
        DataRole
    };

    explicit GroupModel(QObject* parent = nullptr);

    bool GroupModel::operator!=(const GroupModel& other) const {
        return this->m_data != other.m_data;
    }

    void clear() {
        m_data.clear();
    }
    bool empty() {
        return m_data.empty();
    }
    void setData(const GroupModel& shellConfigModel) {
        m_data = shellConfigModel.m_data;
    }
    const QVector<Group*>& getData() {
        return m_data;
    }
    Q_INVOKABLE Group* get(int index) const {
        if (index >= 0 && index < m_data.size()) {
            return m_data.at(index);
        }
        return nullptr;
    }

public:

    // 必须实现的函数
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int getRowCount() const { return rowCount(); }
    Q_INVOKABLE void forceLayout(bool bIsSort = false);
    Q_INVOKABLE void sort();

    // 可选：添加数据的函数
    Q_INVOKABLE void addGroup(Group* config);
    Q_INVOKABLE void addGroup(const QString& groupName);
    Q_INVOKABLE bool removeGroup(int index);

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<Group*> m_data;
};
Q_DECLARE_METATYPE(GroupModel*)