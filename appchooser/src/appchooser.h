#ifndef APPCHOOSER_H
#define APPCHOOSER_H

#include <QObject>
#include <contentaction5/contentaction.h>
#include <QAbstractListModel>
#include "actionsmodel.h"

using namespace ContentAction;

class ActionItem;
class AppChooser : public ActionsModel
{
    Q_OBJECT
    Q_PROPERTY(QString launchArgs READ launchArgs NOTIFY launchArgsChanged)
    Q_PROPERTY(bool rememberChoice READ rememberChoice WRITE setRememberChoice NOTIFY rememberChoiceChanged)
    Q_PROPERTY(QString fileMimeType READ fileMimeType NOTIFY fileMimeTypeChanged)
    Q_PROPERTY(bool dedicatedAppsMode READ dedicatedAppsMode WRITE setDedicatedAppsMode NOTIFY dedicatedAppsModeChanged)
public:
    explicit AppChooser(QObject *parent = nullptr);

    Q_INVOKABLE void openWith(const QString &launchArgs);
    Q_INVOKABLE void launch(int idx);
    Q_INVOKABLE void clear();

    QString launchArgs() const;
    void setLaunchArgs(const QString &launchArgs);

    bool rememberChoice() const;
    void setRememberChoice(bool rememberChoice);

    QString fileMimeType() const;
    void setFileMimeType(const QString &fileMimeType);

    bool dedicatedAppsMode() const;
    void setDedicatedAppsMode(bool dedicatedAppsMode);

private:
    QStringList iconsPaths;
    QString m_launchArgs;
    QString m_launchArgsPretty;
    bool m_rememberChoice;
    QString m_fileMimeType;
    bool m_dedicatedAppsMode;

private:
    void appendAction(const Action &action);
    void notifyLaunching(const QString &desktop);
    void checkWebcat();
    QStringList mimesForString();
    QString mimeForUrl();
    QStringList ancestorsForMime(const QString &mime);
    QStringList mimesForFile(const QString &fileName);
    void setMime(int idx);
    void appendDesktopLauncher(const QString &desktop);
    QString actionToLaunchAction(const QString &action);
    QString launchActionToAction(QString launchAction);
    void moreApps();

signals:
    void launchArgsChanged();
    void showWindow();
    void hideWindow();
    void rememberChoiceChanged();
    void fileMimeTypeChanged();
    void dedicatedAppsModeChanged();
};

#endif // APPCHOOSER_H
