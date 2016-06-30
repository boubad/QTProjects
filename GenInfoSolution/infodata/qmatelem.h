#ifndef QMATELEM_H
#define QMATELEM_H
//////////////////////////////
#include "qdistancemap.h"
//////////////////////////////
#include <QFuture>
#include <QPair>
////////////////////
#include <atomic>
//////////////////////////////
namespace info {
/////////////////////////////////

class QMatElem : public QObject
{
    Q_OBJECT
public:
    using ints_vector = QVector<int>;
    using pair_type = QPair<int,int>;
public:
    explicit QMatElem(const QDistanceMap *pMap, QObject *parent = 0);
    virtual ~QMatElem();
signals:
    void start_arrange(void);
    void newcriteria(double c);
    void end_arrange(void);
public slots:
    void arrange(void);
    QFuture<bool> arrangeAsync(void);
    void cancel(void);
public:
    double criteria(void) const {
        return (this->m_lastcrit);
    }
    const ints_vector & indexes(void) const {
        return (this->m_indexes);
    }
protected:
    bool find_best_permutation(pair_type &oPair, double &bestCrit) const;
private:
    std::atomic<bool> m_cancel;
    const QDistanceMap *m_pMap;
    double m_lastcrit;
    ints_vector m_indexes;
};
////////////////////////////////////
}// namespace info
#endif // QMATELEM_H
