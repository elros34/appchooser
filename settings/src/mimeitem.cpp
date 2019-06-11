#include "mimeitem.h"

MimeItem::MimeItem(QObject *parent) :
    QObject(parent),
    m_cleared(false)
{

}

QString MimeItem::mime() const
{
    return m_mime;
}

void MimeItem::setMime(const QString &name)
{
    m_mime = name;
}

QString MimeItem::action() const
{
    return m_action;
}

void MimeItem::setAction(const QString &action)
{
    m_action = action;
    if (action.isEmpty()) {
        m_cleared = true;
        m_actionPretty.clear();
    } else
        m_cleared = false;
}

QString MimeItem::actionPretty() const
{
    return m_actionPretty;
}

void MimeItem::setActionPretty(const QString &actionPretty)
{
    m_actionPretty = actionPretty;
}

bool MimeItem::cleared() const
{
    return m_cleared;
}

void MimeItem::setCleared(bool cleared)
{
    m_cleared = cleared;
    if (cleared) {
        m_action.clear();
        m_actionPretty.clear();
    }
}
