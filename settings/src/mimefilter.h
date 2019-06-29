#ifndef MIMEFILTER_H
#define MIMEFILTER_H

#include <QObject>
#include <QSortFilterProxyModel>

class MimeFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool showEmptyMimes READ showEmptyMimes WRITE setShowEmptyMimes)
public:
    MimeFilter(QObject *parent = 0);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

    bool showEmptyMimes() const;
    void setShowEmptyMimes(bool showEmptyMimes);

private:
    bool m_showEmptyMimes;

};

#endif // MIMEFILTER_H
