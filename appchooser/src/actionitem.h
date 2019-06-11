#ifndef ACTIONITEM_H
#define ACTIONITEM_H

#include <QObject>

class ActionItem : public QObject
{
    Q_OBJECT
public:
    explicit ActionItem(QObject *parent = nullptr);


    QString name() const;
    void setName(const QString &name);

    QString icon() const;
    void setIcon(const QString &icon);

    QString desktop() const;
    void setDesktop(const QString &desktop);

    QString originAction() const;
    void setOriginAction(const QString &originAction);

    QString action() const;
    void setAction(const QString &action);

private:
    QString m_name;
    QString m_icon;
    QString m_desktop;
    QString m_originAction;
    QString m_action;

signals:

public slots:
};

#endif // ACTIONITEM_H
