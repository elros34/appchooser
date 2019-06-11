#ifndef MIMEITEM_H
#define MIMEITEM_H

#include <QObject>

class MimeItem : public QObject
{
    Q_OBJECT
public:
    explicit MimeItem(QObject *parent = nullptr);

    QString mime() const;
    void setMime(const QString &mime);

    QString action() const;
    void setAction(const QString &action);

    QString actionPretty() const;
    void setActionPretty(const QString &actionPretty);

    bool cleared() const;
    void setCleared(bool cleared);

private:
    QString m_mime;
    QString m_action;
    QString m_actionPretty;
    bool m_cleared;

signals:

public slots:
};

#endif // MIMEITEM_H
