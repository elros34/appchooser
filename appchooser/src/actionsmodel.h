#ifndef ACTIONSMODEL_H
#define ACTIONSMODEL_H

#include <QObject>
#include <QAbstractListModel>

class ActionItem;
class ActionsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy WRITE setBusy NOTIFY busyChanged)
public:
    ActionsModel(QObject *parent = nullptr);
    ~ActionsModel();

    enum myRoles {
        NameRole = Qt::UserRole + 1,
        NameSimplifiedRole,
        IconRole,
        DedicatedRole,
    };

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;

    void detectIconsPaths();
    QString getIconPath(const QString &iconName);

    bool busy() const;
    void setBusy(bool busy);

protected:
    QList<ActionItem*> m_actionList;
    bool dedicatedAppsAdded;

protected:

private:
    QStringList iconsPaths;
    bool m_busy;

signals:
    void busyChanged();
};

#endif // ACTIONSMODEL_H
