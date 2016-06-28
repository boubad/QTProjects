#ifndef QDISTANCEMAP_H
#define QDISTANCEMAP_H
////////////////////////////
#include <QObject>
#include <QMap>
#include <QSet>
#include <QVector>
//////////////////////////////////
namespace info {
////////////////////////////////////////
class QDistanceMap : public QObject
{
    Q_OBJECT
public:
    using first_map_type = QMap<int,double>;
    using map_type = QMap<int,first_map_type>;
    using set_type = QSet<int>;
    using ints_vector = QVector<int>;
public:
    explicit QDistanceMap(QObject *parent = 0);
    virtual ~QDistanceMap();
signals:

public slots:
    void clear(void);
    void get_indexes(QVector<int> &oInds) const;
    void add(const int i, const int j, const double val);
    bool get(const int i, const int j, double &val) const;
    double criteria(const ints_vector &oInds) const;
private:
    map_type m_map;
    set_type m_set;
};
//////////////////////////////////////
}// namespace info
#endif // QDISTANCEMAP_H
