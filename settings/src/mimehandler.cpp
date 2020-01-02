#include "mimehandler.h"
#include "mimeitem.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <contentaction5/contentaction.h>
#include <QMimeDatabase>
#include <QtXml/QDomComment>
#include <mlite5/MGConfItem>
#include <mlite5/MDesktopEntry>

using namespace ContentAction;

MimeHandler::MimeHandler(QObject *parent) : QAbstractListModel(parent)
{
    httpHandlerConf = new MGConfItem("/apps/appchooser/deafaultHttpHandler", this);
    fixMimeAppsList();
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

    QStringList mimes;

    QDir dir("/usr/share/contentaction/");
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setNameFilters({"*.xml"});
    QStringList entryList = dir.entryList();
    for (const QString &xml : entryList) {
        if (xml == "tracker1.xml" || xml == "highlight1.xml") // examples
            continue;
        mimes << mimesForXml(dir.path() + "/" + xml);
    }
    appendMimes("x-maemo-highlight/", mimes);

    mimes = QStringList({"http", "https", "mailto", "sms", "rtsp", "tel", "geo"});
    appendMimes("x-scheme-handler/", mimes);

    mimes.clear();
    QFile typeFile("/usr/share/mime/types");
    if (typeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!typeFile.atEnd()) {
            mimes << typeFile.readLine().simplified();
        }
    }
    appendMimes("", mimes);

    endResetModel();
}

void MimeHandler::resetAll()
{
    for (MimeItem *mimeItem : m_mimes) {
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
    if (!mime.contains("x-scheme-handler/http"))
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
    for (MimeItem *mimeItem : m_mimes) {
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
    for (MimeItem *mimeItem : m_mimes) {
        if (mimeItem->action() != "appchooser") {
            setMimeDefault(mimeItem->mime(), "appchooser");
            mimeItem->setAction("appchooser");
            mimeItem->setActionPretty("Ask");
        }
    }
    emit dataChanged(index(0, 0), index(m_mimes.length() - 1, 0), {ActionRole, ActionPrettyRole, EmptyRole});
}

QStringList MimeHandler::mimesForXml(const QString &xml)
{
    QStringList mimes;
    QFile file(xml);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return mimes;
    QDomDocument doc;
    doc.setContent(file.readAll());
    file.close();
    QDomNodeList highlights = doc.documentElement().elementsByTagName("highlight");
    for (int i = 0; i < highlights.length(); ++i) {
        QString name = highlights.at(i).toElement().attribute("name");
        if (!name.isEmpty())
            mimes << name;
    }
    return mimes;
}

void MimeHandler::appendMimes(const QString &prefix, const QStringList &mimes)
{
    for (const QString &mime : mimes) {
        MimeItem *mimeItem = new MimeItem(this);
        mimeItem->setMime(prefix + mime);

        //special case for https so regexp based mime types can be use
        if (mime == "http" || mime == "https") {
            QString httpHandler = httpHandlerConf->value().toString();
            if (!httpHandler.isEmpty() && httpHandler != "appchooser") {
                MDesktopEntry desktop("/usr/share/applications/" + httpHandler + ".desktop");
                if (desktop.isValid()) {
                    mimeItem->setAction(httpHandler);
                    mimeItem->setActionPretty(desktop.name());
                }
            }
            if (mimeItem->action().isEmpty()) {
                mimeItem->setAction("appchooser");
                mimeItem->setActionPretty("Ask");
            }
        } else {
            Action action = defaultActionForMime(mimeItem->mime());
            if (action.isValid()) {
                mimeItem->setAction(action.name());
                if (action.name() == "appchooser")
                    mimeItem->setActionPretty("Ask");
                else
                    mimeItem->setActionPretty(action.localizedName());
            }
        }
        m_mimes.append(mimeItem);
    }
}

//Remove empty mimes in mimeapps.list because they breaks defaultActionForMime()
void MimeHandler::fixMimeAppsList()
{
    QByteArray data;
    QFile file("/home/nemo/.local/share/applications/mimeapps.list");
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            if (!line.startsWith("=")) {
               data.append(line);
            }
        }
        file.resize(0);
        file.write(data);
    }
}

void MimeHandler::resetMimeDefault(const QString &mimeType)
{
    if (mimeType.contains("x-scheme-handler/http")) {
        ContentAction::setMimeDefault(mimeType, "appchooser");
        httpHandlerConf->unset();
    } else {
        ContentAction::resetMimeDefault(mimeType);
    }
}

void MimeHandler::setMimeDefault(const QString &mimeType, const QString &app)
{
    if (mimeType.contains("x-scheme-handler/http")) {
        ContentAction::setMimeDefault(mimeType, "appchooser");
        httpHandlerConf->set(app);
    } else {
        ContentAction::setMimeDefault(mimeType, app);
    }
}
