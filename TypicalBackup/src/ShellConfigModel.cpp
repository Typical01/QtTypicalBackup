#include "ShellConfigModel.h"




ShellConfigModel::ShellConfigModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellConfigModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant ShellConfigModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_data.count())
        return QVariant();

    ShellConfig* config = m_data.at(index.row());
    switch (role) {
    case OperateNameRole:
        return config->getOperateName();
    case ShellOperateRole:
        return config->getShellOperate();
    case FileRole:
        return config->getFile();
    case ArgRole:
        return config->getArg();
    case WindowShowRole:
        return config->getWindowShow();
    case MenuButtonRole:
        return config->getMenuButton();
    default:
        return QVariant();
    }
}

Q_INVOKABLE void ShellConfigModel::forceLayout(bool bIsSort)
{
    if (bIsSort) {
        sort();
    }
    // 实现逻辑，例如发出 dataChanged 信号以刷新视图
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

void ShellConfigModel::sort()
{
    std::sort(m_data.begin(), m_data.end());
    std::partition(m_data.begin(), m_data.end(), [](ShellConfig* shellConfig) {
        return !shellConfig->getMenuButton();
        }
    );
}

void ShellConfigModel::addShellConfig(ShellConfig* config)
{
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    m_data.append(config);
    std::sort(m_data.begin(), m_data.end());
    std::partition(m_data.begin(), m_data.end(), [](ShellConfig* shellConfig) {
        return !shellConfig->getMenuButton();
        }
    );
    endInsertRows();
}

bool ShellConfigModel::removeShellConfig(int index) {
    if (index < 0 || index >= m_data.count())
        return false;

    beginRemoveRows(QModelIndex(), index, index);
    delete m_data.at(index); // 如果您拥有对象的所有权
    m_data.removeAt(index);
    std::sort(m_data.begin(), m_data.end());
    std::partition(m_data.begin(), m_data.end(), [](ShellConfig* shellConfig) {
        return !shellConfig->getMenuButton();
        }
    );
    endRemoveRows();

    return true;
}

QHash<int, QByteArray> ShellConfigModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[OperateNameRole] = "operateName";
    roles[ShellOperateRole] = "shellOperate";
    roles[FileRole] = "file";
    roles[ArgRole] = "arg";
    roles[WindowShowRole] = "windowShow";
    roles[MenuButtonRole] = "menuButton";
    return roles;
}
