#include "mimefilter.h"
#include <QDebug>
#include <QTimer>

MimeFilter::MimeFilter(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool MimeFilter::showEmptyMimes() const
{
    return m_showEmptyMimes;
}

void MimeFilter::setShowEmptyMimes(bool showEmptyMimes)
{
    m_showEmptyMimes = showEmptyMimes;
    invalidateFilter();
}

int MimeFilter::indexToSource(const int idx)
{
    QModelIndex proxyIndex = index(idx, 0);
    return mapToSource(proxyIndex).row();
}

bool MimeFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);

    static const int mimeRole = sourceModel()->roleNames().key("mime");
    static const int emptyRole = sourceModel()->roleNames().key("empty");

    bool show = false;

    if (sourceIndex.isValid()) {
        show = showEmptyMimes() ? true : !sourceIndex.data(emptyRole).toBool();
        if (show) {
            QString mime = sourceIndex.data(mimeRole).toString();
            show = filterRegExp().isEmpty() ? true : mime.contains(filterRegExp());
        }
    }

    return show;
}
