#ifndef STARINSTANCETABLE_H
#define STARINSTANCETABLE_H

#include <QAbstractListModel>
#include <QQuick3DInstancing>
#include <QQmlEngine>
#include "datamanager.h"


class StarInstanceTable : public QQuick3DInstancing {
    Q_OBJECT
    QML_ELEMENT

public:
    StarInstanceTable();

    QByteArray getInstanceBuffer(int *instanceCount);

private:
    DataManager *data_manager;
    QByteArray m_instanceData;
    bool m_dirty;
    size_t m_instanceCount;
};

#endif // STARINSTANCETABLE_H
