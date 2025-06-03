#include "BackupModel.h"




BackupModel::BackupModel(QObject* parent)
    : QAbstractListModel(parent), m_data()
{
}

int BackupModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant BackupModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_data.count())
        return QVariant();

    Backup* backup = m_data.at(index.row());
    switch (role) {
    case OperateNameRole:
        return backup->getOperateName();
    case SourceFileRole:
        return backup->getSourceFile();
    case DestinationPathRole:
        return backup->getDestinationPath();
    case SourceFileErrorRole:
        return backup->getSourceFileError();
    case DestinationPathErrorRole:
        return backup->getDestinationPathError();
    case StartBackupRole:
        return backup->getStartBackup();
    case SetPermissionsRole:
        return backup->getSetPermissions();
    case ProgressRole:
        return backup->getProgress();
    case ErrorMessageListRole:
        return QVariant::fromValue(backup->getErrorMessageList());
    /*case SourceFileListRole:
        return QVariant::fromValue(backup->getSourceFileList());
    case DestinationPathListRole:
        return QVariant::fromValue(backup->getDestinationPathList());*/
    default:
        return QVariant();
    }
}

Q_INVOKABLE void BackupModel::forceLayout(bool bIsSort)
{
    if (bIsSort) {
        sort();
    }
    // 实现逻辑，例如发出 dataChanged 信号以刷新视图
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

void BackupModel::sort()
{
    std::sort(m_data.begin(), m_data.end());
    std::partition(m_data.begin(), m_data.end(), [](Backup* backup) {
        return backup->getStartBackup();
        }
    );
}

void BackupModel::addBackup(Backup* backup)
{
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    m_data.append(backup);
    sort();
    endInsertRows();
}

void BackupModel::addBackup(const QString& operateName, const QString& sourceFile, 
    const QString& destinationPath, bool startBackup, 
    bool setPermissions, float progress)
{
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    Backup* backup = new Backup(operateName, sourceFile, destinationPath, startBackup, setPermissions, progress);
    m_data.append(backup);
    sort();
    endInsertRows();
}

bool BackupModel::removeBackup(int index) {
    if (index < 0 || index >= m_data.count())
        return false;

    beginRemoveRows(QModelIndex(), index, index);
    delete m_data.at(index); // 如果您拥有对象的所有权
    m_data.removeAt(index);
    sort();
    endRemoveRows();

    return true;
}

QHash<int, QByteArray> BackupModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[OperateNameRole] = "m_operateName";
    roles[SourceFileRole] = "m_sourceFile";
    roles[DestinationPathRole] = "m_destinationPath";
    roles[SourceFileErrorRole] = "m_sourceFileError";
    roles[DestinationPathErrorRole] = "m_destinationPathError";
    roles[StartBackupRole] = "m_startBackup";
    roles[SetPermissionsRole] = "m_setPermissions";
    roles[ProgressRole] = "m_progress";
    roles[ErrorMessageListRole] = "m_errorMessageList";
    /*roles[SourceFileListRole] = "m_sourceFileList";
    roles[DestinationPathListRole] = "m_destinationPathList";*/
    return roles;
}
