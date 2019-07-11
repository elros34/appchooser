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

    bool show = false;

    if (sourceIndex.isValid()) {
        show = dedicatedAppsMode() ? sourceIndex.data(dedicatedRole).toBool() : true;
        if (show) {
            QString name = sourceIndex.data(nameRole).toString();
            show = m_search.isEmpty() ? true : name.contains(m_search, Qt::CaseInsensitive);
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
    invalidateFilter();
}

int AppChooserFilter::indexToSource(const int idx)
{
    QModelIndex proxyIndex = index(idx, 0);
    return mapToSource(proxyIndex).row();
}

