#include "appchooser.h"
#include <QDebug>
#include <mlite5/MDesktopEntry>
#include <mlite5/MGConfItem>
#include <QFileInfo>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include "actionitem.h"

AppChooser::AppChooser(QObject *parent) : QAbstractListModel(parent)
{
    MGConfItem iconSize("/desktop/sailfish/silica/icon_size_launcher");
    MGConfItem pixelRatio("/desktop/sailfish/silica/theme_pixel_ratio");

    iconsPaths << QString("/usr/share/icons/hicolor/%1x%1/apps/").arg(
                      iconSize.value().toString());
    iconsPaths << QString("/usr/share/themes/sailfish-default/meegotouch/z%1/icons/").arg(
                      QString::number(pixelRatio.value().toDouble(), 'f', 1));
}

AppChooser::~AppChooser()
{
    qDeleteAll(m_actionList);
    m_actionList.clear();
}

QVariant AppChooser::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_actionList.count())
        return QVariant();

    if (role == NameRole) {
        return m_actionList.at(index.row())->name();
    } else if (role == IconRole) {
        return m_actionList.at(index.row())->icon();
    }
    return QVariant();
}

int AppChooser::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_actionList.count();
}

QHash<int, QByteArray> AppChooser::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[IconRole] = "icon";
    return roles;
}

void AppChooser::openWith(const QString &launchArgs)
{
    setLaunchArgs(launchArgs);
    qDebug() << launchArgs;

    beginResetModel();
    if (launchArgs.startsWith("http")) {
        QList<Action> list = Action::actionsForString(launchArgs);
        foreach (const Action &action, list) {
            appendAction(action);
        }
        list = Action::actionsForScheme(launchArgs);
        foreach (const Action &action, list) {
            if (action.name() != "appchooser")
                appendAction(action);
        }

    } else if (launchArgs.startsWith("file:")) {
    } else {
    }

    if (m_actionList.count() == 1) {
        launch(0);
        clear();
        endResetModel();
    } else {
        endResetModel();
        emit showWindow();
    }
}

void AppChooser::appendAction(const Action &action)
{
    qDebug() << "origin action: " << action.name();

    ActionItem *actionItem = new ActionItem(this);
    QString actionName = action.name();
    actionItem->setOriginAction(actionName);

    if (actionName == "open-url") {
        actionName = "sailfish-browser";
    } else {
        QStringList endList;
        endList << "-openurl" << "-playvideostream" << "-openfile" << "-open-url";
        foreach (QString end, endList) {
            if (actionName.endsWith(end))
                actionName = actionName.remove(end);
        }
    }
    actionItem->setAction(actionName);
    qDebug() << "action:" << actionName;

    MDesktopEntry desktopEntry("/usr/share/applications/" + actionItem->desktop());
    QString icon = iconsPaths.first() + desktopEntry.icon() + ".png";
    if (!QFileInfo(icon).exists())
        icon = iconsPaths.at(1) + desktopEntry.icon() + ".png";
    actionItem->setIcon(icon);
    //qDebug() << icon;

    actionItem->setName(desktopEntry.name());

    m_actionList.append(actionItem);
}

void AppChooser::launch(int idx)
{
    ActionItem *actionItem = m_actionList.at(idx);
    Action launcherAction = Action::launcherAction(
                actionItem->originAction() + ".desktop",
                {m_launchArgs});
    launcherAction.trigger();

    if (actionItem->desktop() == "harbour-webcat.desktop")
        notifyLaunching(actionItem->desktop());
}

void AppChooser::notifyLaunching(const QString &desktop)
{
    static QDBusInterface *launchIf = new QDBusInterface("org.nemomobile.lipstick",
                                                         "/LauncherModel",
                                                         "org.nemomobile.lipstick.LauncherModel",
                                                         QDBusConnection::sessionBus(),
                                                         this);
    launchIf->asyncCall("notifyLaunching", desktop);
}

void AppChooser::clear()
{
    beginResetModel();
    qDeleteAll(m_actionList);
    m_actionList.clear();
    endResetModel();
}

QString AppChooser::launchArgs() const
{
    return m_launchArgs;
}

void AppChooser::setLaunchArgs(const QString &launchArgs)
{
    if (launchArgs == m_launchArgs)
        return;
    m_launchArgs = launchArgs;
    emit launchArgsChanged();
}
