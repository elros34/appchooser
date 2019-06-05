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
    Q_PROPERTY(QString launchArgs READ launchArgs WRITE setLaunchArgs NOTIFY launchArgsChanged)
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

private:
    QList<ActionItem*> m_actionList;
    QStringList iconsPaths;
    QString m_launchArgs;

private:
    void appendAction(const Action &action);
    void notifyLaunching(const QString &desktop);

signals:
    void launchArgsChanged();
    void showWindow();

};

#endif // APPCHOOSER_H
