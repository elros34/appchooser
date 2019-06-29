#include "appchooser.h"
#include <QDebug>
#include <mlite5/MDesktopEntry>
#include <mlite5/MGConfItem>
#include <QFileInfo>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include "actionitem.h"
#include <contentaction5/contentinfo.h>
#include <QElapsedTimer>
#include <QProcess>
#include <QMimeDatabase>

AppChooser::AppChooser(QObject *parent) :
    QAbstractListModel(parent),
    m_rememberChoice(false)
{
    detectIconsPaths();
    checkWebcat();
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
    QList<Action> list;
    if (launchArgs.startsWith("file:/")) {
        QStringList mimes = mimesForFile(launchArgs);
        if (mimes.isEmpty()) {
            endResetModel();
            return;
        }
        setFileMimeType(mimes.first());
        qDebug() << "mimes: " << mimes;

        foreach (const QString &mime, mimes) {
            QList<Action> actions = actionsForMime(mime);
            list << actions;
        }
    } else {
        list << Action::actionsForString(launchArgs);
        list << Action::actionsForScheme(launchArgs);
    }
    foreach (const Action &action, list)
        appendAction(action);

    if (fileMimeType() == "application/x-desktop")
        appendDesktopLauncher(launchArgs.section("/", -1));

    if (m_actionList.count() > 1) {
        endResetModel();
        emit showWindow();
    } else if (m_actionList.count() == 1) {
        launch(0);
        clear();
        endResetModel();
    } else {
        // TODO add "More" button
        qDebug() << "no handler for: " << launchArgs;
    }
}

void AppChooser::appendAction(const Action &action)
{
    if (action.name() == "appchooser" || !action.isValid())
        return;

    foreach (const ActionItem *item, m_actionList) {
        if (action.name() == item->originAction())
            return;
    }

    qDebug() << "origin action: " << action.name();

    ActionItem *actionItem = new ActionItem(this);
    QString actionName = action.name();
    actionItem->setOriginAction(actionName);

    if (actionName == "open-url") {
        actionName = "sailfish-browser";
    } else {
        QStringList endList;
        endList << "-openurl" << "-playvideostream" << "-openfile" << "-open-url" << "-import";
        foreach (QString end, endList) {
            if (actionName.endsWith(end))
                actionName = actionName.remove(end);
        }
    }
    if (!QFileInfo("/usr/share/applications/" + actionName + ".desktop").exists())
        actionName = actionItem->originAction();
    actionItem->setAction(actionName);
    qDebug() << "action:" << actionName;

    MDesktopEntry desktopEntry("/usr/share/applications/" + actionItem->desktop());
    actionItem->setIcon(getIconPath(desktopEntry.icon()));
    actionItem->setName(desktopEntry.name());

    m_actionList.append(actionItem);
}

void AppChooser::appendDesktopLauncher(const QString &desktop)
{
    Action action = Action::launcherAction(desktop, {});
    ActionItem *actionItem = new ActionItem(this);
    actionItem->setOriginAction(action.name());
    actionItem->setAction(action.name());
    actionItem->setIcon(getIconPath(action.icon()));
    actionItem->setName("Launch '" + action.localizedName() + "'");
    m_actionList.append(actionItem);
}

void AppChooser::launch(int idx)
{
    ActionItem *actionItem = m_actionList.at(idx);
    Action launcherAction = Action::launcherAction(
                actionItem->originAction() + ".desktop",
                {m_launchArgs});
    launcherAction.trigger();

    notifyLaunching(actionItem->desktop());

    if (m_rememberChoice)
        setMime(idx);
}

void AppChooser::notifyLaunching(const QString &desktop)
{
    // notify for broken (probably most of non jolla) mime handlers
    if (desktop.startsWith("jolla-") || desktop.startsWith("sailfish-browser"))
        return;
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
    setFileMimeType("");
}

void AppChooser::setMime(int idx)
{
    ActionItem *item = m_actionList.at(idx);
    if (!item->name().startsWith("Launch '"))
        setMimeDefault(fileMimeType(), item->originAction());
}

QString AppChooser::launchArgs() const
{
    return m_launchArgsPretty;
}

void AppChooser::setLaunchArgs(const QString &launchArgs)
{
    if (launchArgs == m_launchArgs)
        return;
    m_launchArgs = m_launchArgsPretty = launchArgs;

    QUrl url(m_launchArgsPretty);
    m_launchArgsPretty = url.toString();
    if (m_launchArgsPretty.startsWith("file:///"))
        m_launchArgsPretty.remove("file://");

    emit launchArgsChanged();
}

bool AppChooser::rememberChoice() const
{
    return m_rememberChoice;
}

void AppChooser::setRememberChoice(bool rememberChoice)
{
    if (rememberChoice == m_rememberChoice)
        return;
    m_rememberChoice = rememberChoice;
    emit rememberChoiceChanged();
}

QString AppChooser::fileMimeType() const
{
    return m_fileMimeType;
}

void AppChooser::setFileMimeType(const QString &fileMimeType)
{
    if (fileMimeType == m_fileMimeType)
        return;
    m_fileMimeType = fileMimeType;
    emit fileMimeTypeChanged();
}

void AppChooser::detectIconsPaths()
{
    MGConfItem iconSize("/desktop/sailfish/silica/icon_size_launcher");
    iconsPaths << QString("/usr/share/icons/hicolor/%1x%1/apps/").arg(iconSize.value().toString());

    MGConfItem pixelRatio("/desktop/sailfish/silica/theme_pixel_ratio");
    QString ratio = pixelRatio.value().toString();
    if (ratio == "1" || ratio == "2")
        ratio += ".0";
    iconsPaths << QString("/usr/share/themes/sailfish-default/meegotouch/z%1/icons/").arg(ratio);

}

QString AppChooser::getIconPath(const QString &iconName)
{
    QString iconPath = iconsPaths.first() + iconName + ".png";
    if (!QFileInfo(iconPath).exists())
        iconPath = iconsPaths.at(1) + iconName + ".png";
    return iconPath;
}

void AppChooser::checkWebcat()
{
    if (QFileInfo("/home/nemo/.local/share/applications/mimeinfo.cache").exists() &&
        QFileInfo("/home/nemo/.local/share/applications/harbour-webcat-open-url.desktop").exists()) {
        qDebug() << "harbour compatible webcat detected. It will cause issues for other \"x-scheme-handler/https\" handlers."
                 << "Remove /home/nemo/.local/share/applications/mimeinfo.cache and move /home/nemo/.local/share/applications/harbour-webcat-open-url.desktop to /usr/share/applications/"
                 << "if you want to use other browsers.";
    }
}

QStringList AppChooser::mimesForString()
{
    QProcess proc;
    proc.start("lca-tool --string --printmimes " + m_launchArgs);
    if (!proc.waitForStarted(2000))
        return QStringList();
    if (!proc.waitForFinished(2000))
        return QStringList();
    QString mimes = proc.readAll();
    mimes.remove("x-maemo-highlight/http-url");
    return mimes.simplified().split(" ");
}

QString AppChooser::mimeForUrl()
{
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForUrl(m_launchArgs);
    return mimeType.name();
}

QStringList AppChooser::ancestorsForMime(const QString &mime)
{
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForName(mime);
    QStringList ancestors = mimeType.allAncestors();
    return ancestors;
}

QStringList AppChooser::mimesForFile(const QString &fileName)
{
    ContentInfo contentInfo = ContentInfo::forFile(fileName);
    QStringList ancestors = ancestorsForMime(contentInfo.mimeType());
    ancestors.prepend(contentInfo.mimeType());
    return ancestors;
}
