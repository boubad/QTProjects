#ifndef QINDIVTREEELEM_H
#define QINDIVTREEELEM_H
#include "qindiv.h"
#include <QObject>
#include <QMap>
#include <QVariant>
#include <QVector>
#include <QSharedData>
#include <QSharedDataPointer>
/////////////////////////////
namespace info {
class QIndivTree;
///////////////////////////
class QIndivTreeElem : public QObject
{
    friend class QIndivTree;
    Q_OBJECT
private:
    IntType m_index;
    int m_order;
    double m_dist;
    QIndivTreeElem *m_pleft;
    QIndivTreeElem *m_pright;
    QMap<IntType,QVariant> m_center;
private:
    void update_center(void);
    double distance(QIndivTreeElem *pElem);
public:
    explicit QIndivTreeElem(QObject *parent = 0);
    explicit QIndivTreeElem(const QIndiv &oInd, QObject *parent = 0);
    virtual ~QIndivTreeElem();
signals:
public:
    bool isLeaf(void) const {
        return ((this->m_pleft == nullptr) && (this->m_pright == nullptr));
    }
    QIndivTreeElem *left(void) const {
        return (this->m_pleft);
    }
    QIndivTreeElem *right(void) const {
        return (this->m_pright);
    }
    IntType index(void) const {
        return (this->m_index);
    }
    int order(void) const {
        return (this->m_order);
    }
    double criteria(void) const {
        return (this->m_dist);
    }
    QMap<IntType,QVariant> center(void) const {
        return (this->m_center);
    }
    void get_indivs_ids(QVector<IntType> &oVec) const;
public slots:

};
///////////////////////////////////////////
class QAggregResultData : public QSharedData {
public:
     QAggregResultData(){}
     QAggregResultData(const QAggregResultData &other):QSharedData(other),m_data(other.m_data){}

    ~QAggregResultData(){}
     const  QMap<int,QVector<IntType> > &data(void) const {
         return this->m_data;
     }
     void data( const QMap<int,QVector<IntType> > &d){
         this->m_data = d;
     }
private:
     QMap<int,QVector<IntType> > m_data;
};
class QAggregResult {
public:
    QAggregResult(){
        d = new QAggregResultData();
    }
    QAggregResult(const  QMap<int,QVector<IntType> > v ){
        d = new QAggregResultData();
        d->data(v);
    }
    QAggregResult(const QAggregResult &other):d(other.d){}
    QAggregResult & operator=(const QAggregResult &other){
        if (this != &other){
            this->d = other.d;
        }
        return (*this);
    }
    ~QAggregResult(){}
    const  QMap<int,QVector<IntType> > &data(void) const {
        return this->d->data();
    }
    void data( const QMap<int,QVector<IntType> > &dx){
        this->d->data(dx);
    }
private:
    QSharedDataPointer<QAggregResultData> d;
};

///////////////////////////////////////////
class QIndivTree : public QObject {
    Q_OBJECT
private:
    QVector<QIndivTreeElem *> m_elems;
    QVector<double> m_dists;
    QMap<int,QVector<IntType> > m_ids;
    //
    void compute_distances(void);
    void find_best_pair(int &i1, int &i2, double &dMin) const;
    void aggreg_one_step(int &nOrder);
     void get_indivs_ids(QMap<int,QVector<IntType> > oIds) const;
public:
    QIndivTree(IIndivProvider *pProvider,QObject *parent = 0);
    ~QIndivTree();

    const  QVector<QIndivTreeElem *> & elements(void) const {
        return (this->m_elems);
    }
public slots:
    void doWork(int nbClasses);
signals:
    void resultReady(QAggregResult oIds);
};// class QInvTree

///////////////////////////////////////////
}// namespace info
#endif // QINDIVTREEELEM_H
