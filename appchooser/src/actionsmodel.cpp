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

    ActionItem *item = m_actionList.at(index.row());

    switch (role) {
    case NameRole:
        return item->name();
    case NameSimplifiedRole:
        return item->nameSimplified();
    case IconRole:
        return item->icon();
    case DedicatedRole:
        return item->dedicated();
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
    roles[NameSimplifiedRole] = "nameSimplified";
    roles[IconRole] = "icon";
    roles[DedicatedRole] = "dedicated";
    return roles;
}

void ActionsModel::detectIconsPaths()
{
    // no longer exist in 3.4.0.24 or it was set by a patch?
    MGConfItem iconSize("/desktop/sailfish/silica/icon_size_launcher");
    iconsPaths << QString("/usr/share/icons/hicolor/%1x%1/apps/").arg(iconSize.value(86).toString());

    MGConfItem pixelRatio("/desktop/sailfish/silica/theme_pixel_ratio");
    QString ratio = pixelRatio.value().toString();
    if (ratio == "1" || ratio == "2")
        ratio += ".0";
    iconsPaths << QString("/usr/share/themes/sailfish-default/meegotouch/z%1/icons/").arg(ratio);
    iconsPaths << "/usr/share/themes/glacier/meegotouch/icons/";
}

QString ActionsModel::getIconPath(const QString &iconName)
{
    if (iconName.startsWith("data:image/png"))
        return iconName;
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
