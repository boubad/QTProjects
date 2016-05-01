#include "qindivtreeelem.h"
namespace info {
///////////////////////////////////////
QIndivTree::QIndivTree(IIndivProvider *pProvider,QObject *parent):QObject(parent){
    Q_ASSERT(pProvider != nullptr);
    Q_ASSERT(pProvider ->is_valid());
    int nCount = 0;
    bool bRet = pProvider->indivs_count(nCount);
    Q_ASSERT(bRet);
    Q_ASSERT(nCount > 0);
    this->m_elems.resize(nCount);
    for (int i = 0; i < nCount; ++i){
        QIndiv oInd;
        bRet = pProvider->find_indiv_at(i,oInd,VariableMode::modeNumeric);
        Q_ASSERT(bRet);
        QIndivTreeElem *p = new QIndivTreeElem(oInd,this);
        Q_ASSERT(p != nullptr);
        this->m_elems[i] = p;
    }// i
}
QIndivTree::~QIndivTree()
{
    Q_FOREACH(QIndivTreeElem *p, this->m_elems)
    {
        delete p;
    }
    this->m_elems.clear();
}
 void QIndivTree::compute_distances(void){
     QVector<QIndivTreeElem *> &vv = this->m_elems;
     int n = vv.size();
     if (n < 2){
         return;
     }
     QVector<double> &dd = this->m_dists;
     dd.resize(n * n);
     for (int i = 0; i < n; ++i){
         dd[i * n + i] = 0;
         QIndivTreeElem *p1 = vv[i];
         Q_ASSERT(p1 != nullptr);
         for (int j = 0; j < i; ++j){
             QIndivTreeElem *p2 = vv[j];
             Q_ASSERT(p2 != nullptr);
             double d = p1->distance(p2);
             dd[i * n + j] = d;
             dd[j * n + i] = d;
         }// j
     }// i
 }// compute_distances
void QIndivTree::find_best_pair(int &i1, int &i2, double &dMin) const{
    bool bFirst = true;
    const QVector<double> &dd = this->m_dists;
    const int n = this->m_elems.size();
    for (int i = 0; i < n; ++i){
        for (int j = 0; j < i; ++j){
            const double d = dd[j * n + i];
            if (bFirst){
                i1 = j;
                i2 = i;
                dMin = d;
                bFirst = false;
            } else if (d < dMin){
                i1 = j;
                i2 = i;
                dMin = d;
            }
        }// i
    }// i
}//find_best_pair
void QIndivTree::aggreg_one_step(int &nOrder){
    QVector<QIndivTreeElem *> &vv = this->m_elems;
    int n = vv.size();
    if (n < 2){
        return;
    }
    this->compute_distances();
    int i1 = 0, i2 = 0;
    double dist = 0;
    this->find_best_pair(i1,i2,dist);
    if (i1 >= i2){
        return;
    }
    QIndivTreeElem *p1 = vv[i1];
    Q_ASSERT(p1 != nullptr);
    QIndivTreeElem *p2 = vv[i2];
    Q_ASSERT(p1 != nullptr);
    QIndivTreeElem *pn = new QIndivTreeElem(this);
    pn->m_order = ++nOrder;
    pn->m_dist = dist;
    pn->m_pleft = p1;
    pn->m_pright = p2;
    pn->update_center();
    QVector<QIndivTreeElem *> vn;
    for (int i = 0; i < n; ++i){
        if ((i != i1) && (i != i2)){
            vn.push_back(vv[i]);
        }
    }// i
    vn.push_back(pn);
    this->m_elems = vn;
}//aggreg_one_step
void QIndivTree::doWork(int nbClasses){
    Q_ASSERT(nbClasses > 0);
    int nOrder = 0;
    while (this->m_elems.size() > nbClasses){
        this->aggreg_one_step(nOrder);
    }
    this->get_indivs_ids(this->m_ids);
    QAggregResult r(this->m_ids);
    emit resultReady(r);
}//aggreg
 void QIndivTree::get_indivs_ids(QMap<int,QVector<IntType> > oIds) const{
     oIds.clear();
     const QVector<QIndivTreeElem *> &vv = this->m_elems;
     const int n = vv.size();
     for (int i = 0; i < n; ++i){
         int key = i + 1;
         QIndivTreeElem *p = vv[i];
         Q_ASSERT(p != nullptr);
         QVector<IntType> oVec;
         p->get_indivs_ids(oVec);
         oIds[key] = oVec;
     }// i
 }//get_indivs_ids
///////////////////////////////////////
QIndivTreeElem::QIndivTreeElem(QObject *parent) : QObject(parent),
  m_index(0),m_order(0),m_dist(0),m_pleft(nullptr),m_pright(nullptr)
{

}
QIndivTreeElem::QIndivTreeElem(const QIndiv &oInd,QObject *parent) : QObject(parent),
  m_index(oInd.id()),m_order(0),m_dist(0),m_pleft(nullptr),m_pright(nullptr),
  m_center(oInd.data())
{

}
 QIndivTreeElem::~QIndivTreeElem(){
    if (this->m_pleft != nullptr){
        delete this->m_pleft;
        this->m_pleft = 0;
    }
    if (this->m_pright != nullptr){
        delete this->m_pright;
        this->m_pright = nullptr;
    }
}
  void  QIndivTreeElem::update_center(void){
      QIndivTreeElem *pLeft = this->m_pleft;
      QIndivTreeElem *pRight = this->m_pright;
      if ((pLeft  == nullptr) && (pRight == nullptr)){
          return;
      }
      if ((pRight == nullptr) && (pLeft != nullptr)){
          this->m_center = pLeft->m_center;
      } else if ((pRight != nullptr) && (pLeft == nullptr)){
          this->m_center = pRight->m_center;
      } else if ((pLeft != nullptr) && (pRight != nullptr)) {
          QMap<IntType,QVariant> m1 = pLeft->m_center;
          QMap<IntType,QVariant> m2 = pRight->m_center;
          QMap<IntType, double> sommes;
          QList<IntType> keys = m1.keys();
          QMap<IntType,QVariant> oCenter;
          Q_FOREACH(IntType key, keys)
          {
              QVariant v1 = m1[key];
              if (v1.canConvert<double>()){
                 if (m2.contains(key)){
                     QVariant v2 = m2[key];
                     if (v2.canConvert<double>()){
                         double ss = (v1.toDouble() + v2.toDouble()) / 2.0;
                         oCenter[key] = QVariant(ss);
                     }// ok
                 }// m2
              }// can convert
          }// key
          this->m_center = oCenter;
      }
  }//update_center
double QIndivTreeElem::distance(QIndivTreeElem *pElem){
    Q_ASSERT(pElem != nullptr);
    double dRet = 0;
    int nc = 0;
    //
    QMap<IntType,QVariant> m1 = this->m_center;
    QMap<IntType,QVariant> m2 = pElem->m_center;
    QList<IntType> keys = m1.keys();
    Q_FOREACH(IntType key, keys)
    {
        QVariant v1 = m1[key];
        if (!v1.canConvert<double>()){
            continue;
        }
        if (m2.contains(key)){
            QVariant v2 = m2[key];
             if (v2.canConvert<double>()){
                 double d = v1.toDouble() - v2.toDouble();
                 dRet += d * d;
                 ++nc;
             }// ok
        }// contains_key
    }// key
    //
    if (nc > 1){
        dRet /= nc;
    }
    return (dRet);
}// distance
void QIndivTreeElem::get_indivs_ids(QVector<IntType> &oVec) const{
    QIndivTreeElem *pLeft = this->m_pleft;
    if (pLeft != nullptr){
        pLeft->get_indivs_ids(oVec);
    }
     QIndivTreeElem *pRight = this->m_pright;
    if ((pRight == nullptr) && (this->m_index != 0)){
        oVec.push_back(this->m_index);
    } else if (pRight != nullptr){
        pRight->get_indivs_ids(oVec);
    }
}//get_indivs_ids
 /////////////////////////////
}// namespace info

