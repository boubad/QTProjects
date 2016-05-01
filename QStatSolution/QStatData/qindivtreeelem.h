#ifndef QINDIVTREEELEM_H
#define QINDIVTREEELEM_H
#include "qindiv.h"
#include <QObject>
#include <QMap>
#include <QVariant>
#include <QVector>
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
class QIndivTree : public QObject {
    Q_OBJECT
private:
    QVector<QIndivTreeElem *> m_elems;
    QVector<double> m_dists;
    //
    void compute_distances(void);
    void find_best_pair(int &i1, int &i2, double &dMin) const;
    void aggreg_one_step(int &nOrder);
public:
    QIndivTree(IIndivProvider *pProvider,QObject *parent = 0);
    ~QIndivTree();
    void aggreg(int nbClasses);
    const  QVector<QIndivTreeElem *> & elements(void) const {
        return (this->m_elems);
    }
    void get_indivs_ids(QMap<int,QVector<IntType> > oIds) const;
    signals:
        void aggreg_started(void);
        void aggreg_terminated(void);
};// class QInvTree

///////////////////////////////////////////
}// namespace info
#endif // QINDIVTREEELEM_H
