#include "mimehandler.h"
#include "mimeitem.h"
#include <QFile>
#include <QDebug>
#include <contentaction5/contentaction.h>
#include <QMimeDatabase>

using namespace ContentAction;

MimeHandler::MimeHandler(QObject *parent) : QAbstractListModel(parent)
{
    refresh();
}

MimeHandler::~MimeHandler()
{
    qDeleteAll(m_mimes);
    m_mimes.clear();
}

QVariant MimeHandler::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_mimes.count())
        return QVariant();

    if (role == MimeRole) {
        return m_mimes.at(index.row())->mime();
    } else if (role == ActionRole) {
        return m_mimes.at(index.row())->action();
    } else if (role == ActionPrettyRole) {
        return m_mimes.at(index.row())->actionPretty();
    } else if (role == EmptyRole) {
        return m_mimes.at(index.row())->action().isEmpty();
    }
    return QVariant();
}

int MimeHandler::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_mimes.count();
}

QHash<int, QByteArray> MimeHandler::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MimeRole] = "mime";
    roles[ActionRole] = "action";
    roles[ActionPrettyRole] = "actionPretty";
    roles[EmptyRole] = "empty";
    return roles;
}

//resetMimeDefault does not update libcontentaction internal cache
void MimeHandler::refresh()
{
    beginResetModel();
    qDeleteAll(m_mimes);
    m_mimes.clear();

    QStringList schemeHandlers({"mailto", "http", "https", "sms", "rtsp", "tel", "geo"});
    foreach (const QString &scheme, schemeHandlers) {
        MimeItem *mimeItem = new MimeItem(this);
        mimeItem->setMime("x-scheme-handler/" + scheme);
        Action action = defaultActionForMime(mimeItem->mime());
        if (action.isValid()) {
            mimeItem->setAction(action.name());
            if (action.name() == "appchooser")
                mimeItem->setActionPretty("Ask");
            else
                mimeItem->setActionPretty(action.localizedName());
        }
        m_mimes.append(mimeItem);
    }

    QFile file("/usr/share/mime/types");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString mime = file.readLine().simplified();
            Action action = defaultActionForMime(mime);
            MimeItem *mimeItem = new MimeItem(this);
            mimeItem->setMime(mime);
            if (action.isValid()) {
                mimeItem->setAction(action.name());
                if (action.name() == "appchooser")
                    mimeItem->setActionPretty("Ask");
                else
                    mimeItem->setActionPretty(action.localizedName());
            }
            m_mimes.append(mimeItem);
        }
    }

    endResetModel();
}

void MimeHandler::resetAll()
{
    foreach (MimeItem *mimeItem, m_mimes) {
        if (!mimeItem->cleared() && !mimeItem->action().isEmpty()) {
            resetMimeDefault(mimeItem->mime());
            mimeItem->setCleared(true);
        }
    }
    emit dataChanged(index(0, 0), index(m_mimes.length() - 1, 0), {ActionRole, ActionPrettyRole});
}

void MimeHandler::reset(int idx)
{
    MimeItem *mimeItem = m_mimes.at(idx);
    QString mime = mimeItem->mime();
    resetMimeDefault(mime);
    mimeItem->setCleared(true);
    emit dataChanged(index(idx, 0), index(idx, 0), {ActionRole, ActionPrettyRole});

    Action action = defaultActionForMime(mime);
    qDebug() << "after reset name:" << action.name();
}

void MimeHandler::setToAppChooser(int idx)
{
    MimeItem *mimeItem = m_mimes.at(idx);
    QString mime = mimeItem->mime();
    setMimeDefault(mime, "appchooser");
    mimeItem->setAction("appchooser");
    mimeItem->setActionPretty("Ask");

    emit dataChanged(index(idx, 0), index(idx, 0), {ActionRole, ActionPrettyRole});
}

void MimeHandler::setVisibleToAppChooser()
{
    foreach (MimeItem *mimeItem, m_mimes) {
        if (!mimeItem->action().isEmpty()) {
            setMimeDefault(mimeItem->mime(), "appchooser");
            mimeItem->setAction("appchooser");
            mimeItem->setActionPretty("Ask");
        }
    }
    emit dataChanged(index(0, 0), index(m_mimes.length() - 1, 0), {ActionRole, ActionPrettyRole});
}

void MimeHandler::setAllToAppChooser()
{
    foreach (MimeItem *mimeItem, m_mimes) {
        if (mimeItem->action() != "appchooser") {
            setMimeDefault(mimeItem->mime(), "appchooser");
            mimeItem->setAction("appchooser");
            mimeItem->setActionPretty("Ask");
        }
    }
    emit dataChanged(index(0, 0), index(m_mimes.length() - 1, 0), {ActionRole, ActionPrettyRole, EmptyRole});
}
