#pragma once

#include <QAbstractListModel>
#include <QVector>
#include "ShellConfig.h"

class ShellConfigModel : public QAbstractListModel
{
    Q_OBJECT


public:
    enum ShellConfigRoles {
        OperateNameRole = Qt::UserRole + 1,
        ShellOperateRole,
        FileRole,
        ArgRole,
        WindowShowRole,
        MenuButtonRole
    };

    explicit ShellConfigModel(QObject* parent = nullptr);

    
    bool ShellConfigModel::operator!=(const ShellConfigModel& other) const {
        return this->m_data != other.m_data;
    }

    void clear() {
        m_data.clear();
    }
    bool empty() {
        return m_data.empty();
    }
    void setData(const ShellConfigModel& shellConfigModel) {
        m_data = shellConfigModel.m_data;
    }
    const QVector<ShellConfig*>& getData() {
        return m_data;
    }
    Q_INVOKABLE ShellConfig* get(int index) const {
        if (index >= 0 && index < m_data.size()) {
            return m_data.at(index);
        }
        return nullptr;
    }
    Q_INVOKABLE void remove(int index) {
        if (index >= 0 && index < m_data.size()) {
            beginRemoveRows(QModelIndex(), index, index);
            m_data.removeAt(index);
            endRemoveRows();
        }
    }

public:

    // 必须实现的函数
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int getRowCount() const { return rowCount(); }
    Q_INVOKABLE void forceLayout(bool bIsSort = false);
    Q_INVOKABLE void sort();

    // 可选：添加数据的函数
    Q_INVOKABLE void addShellConfig(ShellConfig* config);
    Q_INVOKABLE void addShellConfig(const QString& operateName,
        const QString& shellOperate,
        const QString& file,
        const QString& arg,
        bool windowShow,
        bool menuButton) 
    {
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
        ShellConfig* config = new ShellConfig(operateName, shellOperate, file, arg, windowShow, menuButton);
        m_data.append(config);
        endInsertRows();
    }
    Q_INVOKABLE bool removeShellConfig(int index);

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<ShellConfig*> m_data;
};
Q_DECLARE_METATYPE(ShellConfigModel*)