#ifndef APPCHOOSERFILTER_H
#define APPCHOOSERFILTER_H

#include <QObject>
#include <QSortFilterProxyModel>

class AppChooserFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool dedicatedAppsMode READ dedicatedAppsMode WRITE setDedicatedAppsMode)
public:
    AppChooserFilter(QObject *parent = 0);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

    bool dedicatedAppsMode() const;
    void setDedicatedAppsMode(bool dedicatedAppsMode);

    Q_INVOKABLE void search(const QString &search);
    Q_INVOKABLE int indexToSource(const int idx);

private:
    bool m_dedicatedAppsMode;
    QString m_search;
};

#endif // APPCHOOSERFILTER_H
