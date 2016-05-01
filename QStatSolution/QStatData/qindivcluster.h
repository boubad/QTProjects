#ifndef QINDIVCLUSTER_H
#define QINDIVCLUSTER_H
///////////////////////////////
#include "qindiv.h"
/////////////////////////
#include <QVector>
#include <QMap>
#include <QVariant>
#include <QObject>
#include <QReadWriteLock>
//////////////////////////////////
namespace info {
///////////////////////////////////////
class QIndivCluster : public QObject
{
    Q_OBJECT
private:
    int     m_index;
    IIndivProvider *m_provider;
    QMap<IntType,QVariant> m_center;
    QVector<IntType> m_members;
private:
    QReadWriteLock m_mutex;
public:
    explicit QIndivCluster(QObject *parent = 0);
    explicit QIndivCluster(const int nIndex, IIndivProvider *pProvider
                           ,QObject *parent = 0);
    explicit QIndivCluster(const int nIndex, IIndivProvider *pProvider,
                     const QMap<IntType,QVariant> &oMap,QObject *parent = 0);
    ~QIndivCluster();
public:
    int index(void);
    void index(const int n);
    void get_center(QMap<IntType,QVariant> &oMap);
    void set_center(const QMap<IntType, QVariant> &oMap);
    void get_members(QVector<IntType> &oVec);
    void set_members(const QVector<IntType> &oVec);
    void clear_members(void);
    void add_member(const IntType aIndex);
    void update_center(const VariableMode mode = VariableMode::modeNumeric);
    double distance(const QIndiv &oInd);
    double distance(QIndivCluster *pCluster);
signals:
        void members_added(IntType aIndex);
        void center_updated(void);
public slots:
};
/////////////////////////
}// namespace info
#endif // QINDIVCLUSTER_H
