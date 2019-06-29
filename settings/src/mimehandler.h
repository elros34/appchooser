#ifndef MIMEHANDLER_H
#define MIMEHANDLER_H

#include <QObject>
#include <QAbstractListModel>

class MimeItem;
class MimeHandler : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MimeHandler(QObject *parent = nullptr);
    ~MimeHandler();

    enum myRoles {
        MimeRole = Qt::UserRole + 1,
        ActionRole,
        ActionPrettyRole,
        EmptyRole,
    };

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void resetAll();
    Q_INVOKABLE void reset(int idx);
    Q_INVOKABLE void setToAppChooser(int idx);
    Q_INVOKABLE void setVisibleToAppChooser();
    Q_INVOKABLE void setAllToAppChooser();

private:
    QList<MimeItem*> m_mimes;

public slots:
};

#endif // MIMEHANDLER_H
