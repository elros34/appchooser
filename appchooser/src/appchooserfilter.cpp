#include "appchooserfilter.h"
#include <QDebug>

AppChooserFilter::AppChooserFilter(QObject *parent) :
    QSortFilterProxyModel(parent),
    m_dedicatedAppsMode(true)
{
}

bool AppChooserFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);

    static const int dedicatedRole = sourceModel()->roleNames().key("dedicated");
    static const int nameRole = sourceModel()->roleNames().key("name");
    static const int nameSimplifiedRole = sourceModel()->roleNames().key("nameSimplified");

    bool show = false;

    if (sourceIndex.isValid()) {
        show = dedicatedAppsMode() ? sourceIndex.data(dedicatedRole).toBool() : true;
        if (show) {
            QString name = sourceIndex.data(nameRole).toString();
            QString nameSimplified = sourceIndex.data(nameSimplifiedRole).toString();
            if (m_search.isEmpty()) {
                show = true;
            } else {
                show = name.contains(m_search, Qt::CaseInsensitive);
                show |= nameSimplified.contains(m_searchSimplified, Qt::CaseInsensitive);
            }
        }
    }

    return show;
}

bool AppChooserFilter::dedicatedAppsMode() const
{
    return m_dedicatedAppsMode;
}

void AppChooserFilter::setDedicatedAppsMode(bool dedicatedAppsMode)
{
    if (dedicatedAppsMode == m_dedicatedAppsMode)
        return;
    m_dedicatedAppsMode = dedicatedAppsMode;
    invalidateFilter();
}

void AppChooserFilter::search(const QString &search)
{
    m_search = search;
    m_searchSimplified = m_search;
    m_searchSimplified.remove("-");
    m_searchSimplified.remove("_");
    m_searchSimplified.remove(" ");
    invalidateFilter();
}

int AppChooserFilter::indexToSource(const int idx)
{
    QModelIndex proxyIndex = index(idx, 0);
    return mapToSource(proxyIndex).row();
}

