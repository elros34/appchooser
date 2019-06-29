#ifndef APPCHOOSER_H
#define APPCHOOSER_H

#include <QObject>
#include <contentaction5/contentaction.h>
#include <QAbstractListModel>

using namespace ContentAction;

class ActionItem;
class AppChooser : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString launchArgs READ launchArgs NOTIFY launchArgsChanged)
    Q_PROPERTY(bool rememberChoice READ rememberChoice WRITE setRememberChoice NOTIFY rememberChoiceChanged)
    Q_PROPERTY(QString fileMimeType READ fileMimeType NOTIFY fileMimeTypeChanged)
public:
    explicit AppChooser(QObject *parent = nullptr);
    ~AppChooser();

    enum myRoles {
        NameRole = Qt::UserRole + 1,
        IconRole,
    };

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void openWith(const QString &launchArgs);
    Q_INVOKABLE void launch(int idx);
    Q_INVOKABLE void clear();

    QString launchArgs() const;
    void setLaunchArgs(const QString &launchArgs);

    bool rememberChoice() const;
    void setRememberChoice(bool rememberChoice);

    QString fileMimeType() const;
    void setFileMimeType(const QString &fileMimeType);

private:
    QList<ActionItem*> m_actionList;
    QStringList iconsPaths;
    QString m_launchArgs;
    QString m_launchArgsPretty;
    bool m_rememberChoice;
    QString m_fileMimeType;

private:
    void appendAction(const Action &action);
    void notifyLaunching(const QString &desktop);
    void checkWebcat();
    void detectIconsPaths();
    QStringList mimesForString();
    QString mimeForUrl();
    QStringList ancestorsForMime(const QString &mime);
    QStringList mimesForFile(const QString &fileName);
    void setMime(int idx);
    void appendDesktopLauncher(const QString &desktop);
    QString getIconPath(const QString &iconName);

signals:
    void launchArgsChanged();
    void showWindow();
    void rememberChoiceChanged();
    void fileMimeTypeChanged();
};

#endif // APPCHOOSER_H
