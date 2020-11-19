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

    QString launchAction() const;
    void setLaunchAction(const QString &launchAction);

    QString action() const;
    void setAction(const QString &action);

    bool dedicated() const;
    void setDedicated(bool dedicated);

    QString nameSimplified() const;
    void setNameSimplified(const QString &nameSimplified);

private:
    QString m_name;
    QString m_nameSimplified;
    QString m_icon;
    QString m_desktop;
    QString m_launchAction;
    QString m_action;
    bool m_dedicated;

signals:

public slots:
};

#endif // ACTIONITEM_H
