#ifndef APPCHOOSER_H
#define APPCHOOSER_H

#include <QObject>
#include <contentaction5/contentaction.h>
#include <QAbstractListModel>
#include "actionsmodel.h"

using namespace ContentAction;

class ActionItem;
class MGConfItem;
class AppChooser : public ActionsModel
{
    Q_OBJECT
    Q_PROPERTY(QString launchArgs READ launchArgs NOTIFY launchArgsChanged)
    Q_PROPERTY(bool rememberChoice READ rememberChoice WRITE setRememberChoice NOTIFY rememberChoiceChanged)
    Q_PROPERTY(bool dedicatedAppsMode READ dedicatedAppsMode WRITE setDedicatedAppsMode NOTIFY dedicatedAppsModeChanged)
    Q_PROPERTY(QString currentMimeType READ currentMimeType NOTIFY currentMimeTypeChanged)
public:
    explicit AppChooser(QObject *parent = nullptr);

    Q_INVOKABLE void openWith(const QString &launchArgs);
    Q_INVOKABLE void launch(int idx);
    Q_INVOKABLE void clear();

    QString launchArgs() const;
    void setLaunchArgs(const QString &launchArgs);

    bool rememberChoice() const;
    void setRememberChoice(bool rememberChoice);

    bool dedicatedAppsMode() const;
    void setDedicatedAppsMode(bool dedicatedAppsMode);

    QString currentMimeType() const;
    void setCurrentMimeType(const QString &mimeType);

private:
    QStringList iconsPaths;
    QString m_launchArgs;
    QString m_launchArgsPretty;
    bool m_rememberChoice;
    QString m_fileMimeType;
    QString m_currentMimeType;
    bool m_dedicatedAppsMode;
    MGConfItem *httpHandlerConf;

private:
    void appendAction(const Action &action);
    void notifyLaunching(const QString &desktop);
    void checkMimeinfoCache();
    QStringList mimesForString();
    QString mimeForUrl();
    QStringList ancestorsForMime(const QString &mime);
    QStringList mimesForFile(const QString &fileName);
    void saveMime(int idx);
    void appendDesktopLauncher(const QString &desktop);
    QString actionToLaunchAction(const QString &action);
    QString launchActionToAction(QString launchAction);
    void moreApps();
    bool launchApp(const QString &name);

signals:
    void launchArgsChanged();
    void showWindow();
    void hideWindow();
    void rememberChoiceChanged();
    void fileMimeTypeChanged();
    void dedicatedAppsModeChanged();
    void currentMimeTypeChanged();
};

#endif // APPCHOOSER_H
