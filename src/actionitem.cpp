#include "actionitem.h"

ActionItem::ActionItem(QObject *parent) : QObject(parent)
{

}

QString ActionItem::name() const
{
    return m_name;
}

void ActionItem::setName(const QString &name)
{
    m_name = name;
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

QString ActionItem::originAction() const
{
    return m_originAction;
}

void ActionItem::setOriginAction(const QString &originAction)
{
    m_originAction = originAction;
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
