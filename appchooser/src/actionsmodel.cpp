#include "actionsmodel.h"
#include "actionitem.h"
#include <mlite5/MGConfItem>
#include <QFileInfo>
#include <QDebug>

ActionsModel::ActionsModel(QObject *parent) :
    QAbstractListModel(parent),
    dedicatedAppsAdded(false),
    m_busy(false)
{
}

ActionsModel::~ActionsModel()
{
    qDeleteAll(m_actionList);
    m_actionList.clear();
}

QVariant ActionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_actionList.count())
        return QVariant();

    if (role == NameRole) {
        return m_actionList.at(index.row())->name();
    } else if (role == IconRole) {
        return m_actionList.at(index.row())->icon();
    } else if (role == DedicatedRole) {
        return m_actionList.at(index.row())->dedicated();
    }
    return QVariant();
}

int ActionsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_actionList.count();
}

QHash<int, QByteArray> ActionsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[IconRole] = "icon";
    roles[DedicatedRole] = "dedicated";
    return roles;
}

void ActionsModel::detectIconsPaths()
{
    MGConfItem iconSize("/desktop/sailfish/silica/icon_size_launcher");
    iconsPaths << QString("/usr/share/icons/hicolor/%1x%1/apps/").arg(iconSize.value().toString());

    MGConfItem pixelRatio("/desktop/sailfish/silica/theme_pixel_ratio");
    QString ratio = pixelRatio.value().toString();
    if (ratio == "1" || ratio == "2")
        ratio += ".0";
    iconsPaths << QString("/usr/share/themes/sailfish-default/meegotouch/z%1/icons/").arg(ratio);
    iconsPaths << "/usr/share/themes/glacier/meegotouch/icons/";
}

QString ActionsModel::getIconPath(const QString &iconName)
{
    foreach (const QString &path, iconsPaths) {
        QString iconPath = path + iconName + ".png";
        if (QFileInfo(iconPath).exists())
            return iconPath;
    }
    return QString();
}

bool ActionsModel::busy() const
{
    return m_busy;
}

void ActionsModel::setBusy(bool busy)
{
    if (busy == m_busy)
        return;
    m_busy = busy;
    emit busyChanged();
}
