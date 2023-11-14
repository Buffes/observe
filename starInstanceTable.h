#ifndef STARINSTANCETABLE_H
#define STARINSTANCETABLE_H

#include <QAbstractListModel>
#include <QQuick3DInstancing>
#include <QQmlEngine>
#include "datastructures.h"

struct StarEntry {
    float id;
    double right_ascension;
    double declination;
    char spectral_type[2];
    int16_t magnitude;
    float proper_motion_ra;
    float proper_motion_decl;
    float scale;
};

/*
class StarModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
public:
    StarModel();

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    enum CustomRoles{
        XRole=Qt::UserRole+1,
        YRole,
        ZRole,
    };

    void loadStarCatalog(QString path);

private:
    QList<StarEntry> m_stars; // Star catalog data
    QList<dVector3D> m_cartesian_coords;
};
*/


class StarInstanceTable : public QQuick3DInstancing {
    Q_OBJECT
    QML_ELEMENT

public:
    StarInstanceTable();

    QByteArray getInstanceBuffer(int *instanceCount);
    void loadStarCatalog(QString path);

private:
    QList<StarEntry> m_stars; // Star catalog data
    QList<dVector3D> m_cartesian_coords;
    QByteArray m_instanceData;
    bool m_dirty;
    size_t m_instanceCount;
};

#endif // STARINSTANCETABLE_H
