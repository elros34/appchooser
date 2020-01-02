#include "appchooser.h"
#include <QDebug>
#include <mlite5/MDesktopEntry>
#include <mlite5/MGConfItem>
#include <QFileInfo>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include "actionitem.h"
#include <contentaction5/contentinfo.h>
#include <QProcess>
#include <QMimeDatabase>
#include <QDir>

const QStringList fileActionPostfixList = {"-openfile", "-open-file", "-import"};
const QStringList schemeActionPostfixList = {"-openurl", "-open-url", "-url", "-playvideostream"};

AppChooser::AppChooser(QObject *parent) :
    ActionsModel(parent),
    m_rememberChoice(false),
    m_dedicatedAppsMode(true)
{
    httpHandlerConf = new MGConfItem("/apps/appchooser/deafaultHttpHandler", this);
    detectIconsPaths();
    checkMimeinfoCache();
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
            emit showWindow();
            return;
        }
        setCurrentMimeType(mimes.first());
        qDebug() << "mimes: " << mimes;

        for (const QString &mime : mimes) {
            list << actionsForMime(mime);
        }
    } else if (launchArgs.startsWith("http")) {
        QStringList regexpMimes = mimesForString();
        qDebug() << "regexpMimes: " << regexpMimes;
        if (regexpMimes.length() == 1) {
            QString mime = regexpMimes.first();
            Action defaultAction = defaultActionForMime(mime);
            if (defaultAction.isValid() && launchApp(defaultAction.name())) {
                endResetModel();
                return;
            }

            setCurrentMimeType(mime);
        } else if (regexpMimes.length() > 1) {// more than 1 regexp based mime type
            setCurrentMimeType("");
        } else {
            setCurrentMimeType(mimeForUrl());
            // check for default browser
            if (currentMimeType().contains("x-scheme-handler/http")) {
                QString httpHandler = httpHandlerConf->value().toString();
                if (!httpHandler.isEmpty() && launchApp(httpHandler)) {
                    endResetModel();
                    return;
                }
            }
        }

        list << Action::actionsForScheme(launchArgs);
        list << Action::actionsForString(launchArgs);
    } else {
        list << Action::actionsForScheme(launchArgs);
    }
    for (const Action &action : list)
        appendAction(action);

    if (currentMimeType() == "application/x-desktop")
        appendDesktopLauncher(launchArgs.section("/", -1));

    endResetModel();
    emit showWindow();
}

void AppChooser::appendAction(const Action &action)
{
    if (action.name() == "appchooser" || !action.isValid())
        return;

    for (const ActionItem *item : m_actionList) {
        if (action.name() == item->launchAction())
            return;
    }

    ActionItem *actionItem = new ActionItem(this);
    QString actionName = action.name();
    actionItem->setLaunchAction(actionName);
    actionItem->setAction(launchActionToAction(actionName));

    MDesktopEntry desktopEntry("/usr/share/applications/" + actionItem->desktop());
    actionItem->setIcon(getIconPath(desktopEntry.icon()));
    actionItem->setName(desktopEntry.name());
    actionItem->setDedicated(true);

    m_actionList.append(actionItem);
}

void AppChooser::appendDesktopLauncher(const QString &desktop)
{
    Action action = Action::launcherAction(desktop, {});
    if (!action.isValid())
        return;
    ActionItem *actionItem = new ActionItem(this);
    actionItem->setLaunchAction(action.name());
    actionItem->setAction(action.name());
    actionItem->setIcon(getIconPath(action.icon()));
    actionItem->setName("Launch '" + action.localizedName() + "'");
    m_actionList.append(actionItem);
}

void AppChooser::launch(int idx)
{
    if (idx < 0 || idx >= m_actionList.length())
        return;
    ActionItem *actionItem = m_actionList.at(idx);
    if (actionItem->launchAction().isEmpty())
        actionItem->setLaunchAction(actionToLaunchAction(actionItem->action()));
    qDebug() << actionItem->launchAction();
    Action launcherAction = Action::launcherAction(
                actionItem->launchAction() + ".desktop",
                {m_launchArgs});
    if (!launcherAction.isValid()) {
        emit hideWindow();
        return;
    }
    launcherAction.trigger();

    emit hideWindow();

    notifyLaunching(actionItem->desktop());

    if (m_rememberChoice)
        saveMime(idx);
}

bool AppChooser::launchApp(const QString &name)
{
    Action launcherAction = Action::launcherAction(name + ".desktop", {m_launchArgs});
    if (!launcherAction.isValid() || name == "appchooser") {
        return false;
    }
    launcherAction.trigger();
    notifyLaunching(launchActionToAction(name) + ".desktop");

    return true;
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
    setCurrentMimeType("");
    setDedicatedAppsMode(true);
    setRememberChoice(false);
    dedicatedAppsAdded = false;
}

void AppChooser::saveMime(int idx)
{
    ActionItem *item = m_actionList.at(idx);
    if (!item->name().startsWith("Launch '") && !currentMimeType().isEmpty()) {
        if (currentMimeType().contains("x-scheme-handler/http")) {
            httpHandlerConf->set(item->launchAction());
        } else {
            setMimeDefault(currentMimeType(), item->launchAction());
        }
    }
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

QString AppChooser::currentMimeType() const
{
    return m_currentMimeType;
}

void AppChooser::setCurrentMimeType(const QString &mimeType)
{
    if (m_currentMimeType == mimeType)
        return;
    m_currentMimeType = mimeType;
    emit currentMimeTypeChanged();
}

bool AppChooser::dedicatedAppsMode() const
{
    return m_dedicatedAppsMode;
}

void AppChooser::setDedicatedAppsMode(bool dedicatedAppsMode)
{
    if (dedicatedAppsMode == m_dedicatedAppsMode)
        return;
    m_dedicatedAppsMode = dedicatedAppsMode;
    if (!dedicatedAppsMode) {
        if (!dedicatedAppsAdded)
            moreApps();
        else
            setBusy(false);
    }
    emit dedicatedAppsModeChanged();
}

void AppChooser::checkMimeinfoCache()
{
    if (QFileInfo("/home/nemo/.local/share/applications/mimeinfo.cache").exists()) {
        qDebug() << "harbour compatible webcat or/and microtube detected. It will cause issues for other \"x-scheme-handler/https\" handlers."
                 << "Remove /home/nemo/.local/share/applications/mimeinfo.cache and move /home/nemo/.local/share/applications/*.desktop to /usr/share/applications/."
                 << "Next run update-desktop-database as root if you want to use other browsers.";
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
    return mimes.simplified().split(" ", QString::SkipEmptyParts);
}

QString AppChooser::mimeForUrl()
{
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForUrl(m_launchArgs);
    if (mimeType.name() == "application/octet-stream") // unknown
        return "x-scheme-handler/" + QUrl(m_launchArgs).scheme();
    else
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
    QMimeDatabase db;
    QMimeType mimeType;
    QUrl fileUrl = QUrl(fileName);
    ContentInfo contentInfo = ContentInfo::forFile(fileUrl);
    if (contentInfo.mimeType() == "application/x-trash")
        mimeType = db.mimeTypeForFile(fileUrl.path(), QMimeDatabase::MatchContent);
    else
        mimeType = db.mimeTypeForName(contentInfo.mimeType());
    QStringList mimes = {mimeType.name()};
    mimes << mimeType.allAncestors();
    return mimes;
}

QString AppChooser::actionToLaunchAction(const QString &action)
{
    QString launchAction = action;
    if (action == "sailfish-browser") {
        launchAction = "open-url";
    } else {
        QStringList postfixList;
        if (m_launchArgs.startsWith("file:/"))
            postfixList << fileActionPostfixList;
        else
            postfixList << schemeActionPostfixList;

        for (const QString &postfix : postfixList) {
            if (QFileInfo("/usr/share/applications/" + action + postfix + ".desktop").exists())
                launchAction = action + postfix;
        }
    }

    return launchAction;
}

QString AppChooser::launchActionToAction(QString launchAction)
{
    QString action = launchAction;
    if (launchAction == "open-url") {
        action = "sailfish-browser";
    } else {
        QStringList postfixList;
        postfixList << fileActionPostfixList << schemeActionPostfixList;
        for (const QString &postfix : postfixList) {
            if (launchAction.endsWith(postfix))
                launchAction = launchAction.remove(postfix);
        }
        if (QFileInfo("/usr/share/applications/" + launchAction + ".desktop").exists())
            action = launchAction;
    }

    return action;
}

void AppChooser::moreApps()
{
    setBusy(true);

    QDir dir("/usr/share/applications/");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setNameFilters({"*.desktop"});
    QStringList entryList = dir.entryList();

    beginResetModel();
    for (const QString &entry : entryList) {
        bool dedicated = false;
        for (ActionItem *i : m_actionList) {
            if (i->desktop() == entry) {
                dedicated = true;
                break;
            }
        }
        if (dedicated)
            continue;

        MDesktopEntry desktopEntry(dir.path() + "/" + entry);
        if (desktopEntry.noDisplay())
            continue;
        ActionItem *actionItem = new ActionItem(this);
        actionItem->setName(desktopEntry.name());
        actionItem->setDesktop(entry);
        actionItem->setAction(entry.section(".", 0, 0));
        actionItem->setIcon(getIconPath(desktopEntry.icon()));
        actionItem->setDedicated(false);
        m_actionList.append(actionItem);
    }

    qSort(m_actionList.begin(), m_actionList.end(),
          [] (const ActionItem *a, const ActionItem *b) { return a->name() > b->name(); });
    endResetModel();

    dedicatedAppsAdded = true;
    setBusy(false);
}
