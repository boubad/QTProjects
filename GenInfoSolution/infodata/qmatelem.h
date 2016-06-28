#ifndef QMATELEM_H
#define QMATELEM_H
//////////////////////////////
#include "qdistancemap.h"
//////////////////////////////
#include <QPair>
#include <QThread>
////////////////////
#include <memory>
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
    using result_type = std::shared_ptr<int>;
public:
    explicit QMatElem(const QDistanceMap *pMap, QObject *parent = 0);
    virtual ~QMatElem();
signals:
    void start_arrange(void);
    void newcriteria(double c, result_type res );
    void end_arrange(void);
public slots:
    void arrange(void);
    void cancel(void);
public:
    double criteria(void) const {
        return (this->m_lastcrit);
    }
    const ints_vector & indexes(void) const {
        return (this->m_indexes);
    }
protected:
    result_type get_result(void);
    bool find_best_permutation(pair_type &oPair, double &bestCrit) const;
private:
    std::atomic<bool> m_cancel;
    const QDistanceMap *m_pMap;
    double m_lastcrit;
    ints_vector m_indexes;
};
////////////////////////////////////
class QMatElemControl : public QObject {
    Q_OBJECT
    QThread workerThread;
public:
    using result_type = std::shared_ptr<int>;
public:
    explicit QMatElemControl(const QDistanceMap *pMap, QObject *parent = 0);
    virtual ~QMatElemControl();
signals:
    void arrange(void);
    void current(double c, result_type res);
    void started(void);
    void finished(void);
public slots:
    void start_arrange(void);
    void newcriteria(double c, result_type res );
    void end_arrange(void);
};// class QMatElemControl

///////////////////////
}// namespace info
#endif // QMATELEM_H
