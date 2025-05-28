#pragma once

#include <QAbstractListModel>
#include <QVector>
#include "Backup.h"

#include <QtLog.h>

using namespace qtytl;

#include <TypicalTool/Tool.h>

class BackupModel : public QAbstractListModel
{
    Q_OBJECT


public:
    enum BackupRoles {
        OperateNameRole = Qt::UserRole + 1,
        SourceFileRole,
        DestinationPathRole,
        ErrorMessageRole,
        StartBackupRole,
        SetPermissionsRole,
        ProgressRole
        /*SourceFileListRole,
        DestinationPathListRole*/
    };

    explicit BackupModel(QObject* parent = nullptr);

    
    bool BackupModel::operator!=(const BackupModel& other) const {
        return this->m_data != other.m_data;
    }

    void clear() {
        m_data.clear();
    }
    bool empty() {
        return m_data.empty();
    }
    void setData(const BackupModel& shellConfigModel) {
        m_data = shellConfigModel.m_data;
    }
    QVector<Backup*> getData() {
        return m_data;
    }
    Q_INVOKABLE Backup* get(int index) const {
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
    Q_INVOKABLE void addBackup(Backup* config);
    Q_INVOKABLE void addBackup(const QString& operateName, const QString& sourceFile, const QString& destinationPath,
        bool startBackup, bool setPermissions, float progress);
    Q_INVOKABLE bool removeBackup(int index);

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<Backup*> m_data;
};
Q_DECLARE_METATYPE(BackupModel*)