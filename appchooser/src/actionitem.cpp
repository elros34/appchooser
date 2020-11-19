#include "actionitem.h"

ActionItem::ActionItem(QObject *parent) :
    QObject(parent),
    m_dedicated(true)
{

}

QString ActionItem::name() const
{
    return m_name;
}

void ActionItem::setName(const QString &name)
{
    m_name = name;
    QString sn = name;
    sn.remove("-");
    sn.remove("_");
    sn.remove(" ");
    sn.replace("µ", "u");
    sn = sn.toLower();
    setNameSimplified(sn);
}

QString ActionItem::icon() const
{
    return m_icon;
}

void ActionItem::setIcon(const QString &icon)
{
    m_icon = icon;
}

QString ActionItem::desktop() const
{
    return m_desktop;
}

void ActionItem::setDesktop(const QString &desktop)
{
    m_desktop = desktop;
}

QString ActionItem::launchAction() const
{
    return m_launchAction;
}

void ActionItem::setLaunchAction(const QString &launchAction)
{
    m_launchAction = launchAction;
}

QString ActionItem::action() const
{
    return m_action;
}

void ActionItem::setAction(const QString &action)
{
    m_action = action;
    setDesktop(action + ".desktop");
}

bool ActionItem::dedicated() const
{
    return m_dedicated;
}

void ActionItem::setDedicated(bool dedicated)
{
    m_dedicated = dedicated;
}

QString ActionItem::nameSimplified() const
{
    return m_nameSimplified;
}

void ActionItem::setNameSimplified(const QString &nameSimplified)
{
    m_nameSimplified = nameSimplified;
}
