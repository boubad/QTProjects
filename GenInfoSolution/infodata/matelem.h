#ifndef QMATELEM_H
#define QMATELEM_H
//////////////////////////////
#include "distancemap.h"
#include "matricedata.h"
//////////////////////////////
#include <QQueue>
#include <QPair>
#include <QFuture>
#include <QtConcurrent>
////////////////////
#include <functional>
#include <memory>
#include <atomic>
//////////////////////////////
namespace info {
/////////////////////////////////
template <typename INDEXTYPE,typename DISTANCETYPE, typename CRITERIATYPE>
class MatElem
{
public:
    using index_type = INDEXTYPE;
    using distance_type = DISTANCETYPE;
    using criteria_type = CRITERIATYPE;
    //
    using distancemap_type = DistanceMap<index_type,distance_type>;
    using ints_vector = QVector<index_type>;
    using pair_type = QPair<index_type,index_type>;
    using ints_vector_ptr = std::shared_ptr<ints_vector>;
    using matelem_result_type = QPair<criteria_type,ints_vector_ptr>;
    using matelem_callback_type = std::function<void(matelem_result_type)>;
    using matelem_type = MatElem<index_type,distance_type,criteria_type>;
    using future_type = QFuture<matelem_result_type>;
    using cancellableflag_type = std::atomic<bool>;
public:
    template <typename FLOATTYPE>
    MatElem(const MatriceData<index_type,FLOATTYPE,distance_type> *pData,
            MatDispositionType disp = MatDispositionType::dispRow,
            cancellableflag_type *pf = nullptr):
        m_pcancel(pf),m_notify(false),m_pdist(nullptr),m_crit(0),m_callback( [](matelem_result_type){}){
    Q_ASSERT(pData != nullptr);
    const distancemap_type *pMap = (disp == MatDispositionType::dispRow) ?
                pData->get_rows_distancesmap() : pData->get_cols_distancesmap();
    Q_ASSERT(pMap != nullptr);
    m_pdist = pMap;
    pMap->get_indexes(this->m_indexes);
    pMap->compute_criteria(this->m_indexes,this->m_crit);
}// MatElem

MatElem(const distancemap_type *pMap,cancellableflag_type *pf = nullptr):
    m_pcancel(pf),m_notify(false),m_pdist(pMap),m_crit(0),m_callback( [](matelem_result_type){}){
Q_ASSERT(pMap != nullptr);
pMap->get_indexes(this->m_indexes);
pMap->compute_criteria(this->m_indexes,this->m_crit);
}
virtual ~MatElem(){}
void setCallback(matelem_callback_type ff){
    this->m_callback = ff;
    this->m_notify = true;
}// setCallback
matelem_result_type getResult(void) const {
    ints_vector_ptr oPtr = std::make_shared<ints_vector>(this->m_indexes);
    return (matelem_result_type(this->m_crit,oPtr));
}// getResult
criteria_type criteria(void) const {
    return (this->m_crit);
}
const ints_vector & indexes(void) const {
    return (this->m_indexes);
}// indexes
template <typename X>
void indexes(const QVector<X> &oInds){
    ints_vector &v = this->m_indexes;
    int n = v.size();
    for (int i = 0; i < n; ++i){
        v[i] = (index_type)oInds[i];
    }
}// indexes

matelem_result_type arrange(void){
    bool done = false;
    while (!done){
        if (this->is_cancellation_requested()){
            done = true;
            break;
        }
        criteria_type bestCrit = this->m_crit;
        QQueue<pair_type> oPairs;
        if (!this->find_best_permutation(oPairs,bestCrit)){
            done = true;
            break;
        }
        if (oPairs.isEmpty()){
            done = true;
            break;
        }
        pair_type oFirst = oPairs.dequeue();
        QVector<future_type> oFutures;
        while (!oPairs.isEmpty()){
            pair_type p = oPairs.dequeue();
            index_type i = p.first;
            index_type j = p.second;
            future_type ft = QtConcurrent::run([this,i,j,bestCrit]()->matelem_result_type{
                matelem_type oMat(*this);
                oMat.m_crit = bestCrit;
                index_type t = oMat.m_indexes[i];
                oMat.m_indexes[i] = oMat.m_indexes[j];
                oMat.m_indexes[j] = t;
                return (oMat.arrange());
            });
            oFutures.push_back(ft);
        }// not empty
        index_type i1 = oFirst.first;
        index_type i2 = oFirst.second;
        index_type tx = this->m_indexes[i1];
        this->m_indexes[i1] = this->m_indexes[i2];
        this->m_indexes[i2] = tx;
        this->m_crit = bestCrit;
        this->notify();
        for (auto &pi: oFutures){
            matelem_result_type r = pi.result();
            criteria_type crit = r.first;
            if (crit < this->m_crit){
                ints_vector *pv = (r.second).get();
                if (pv != nullptr){
                    this->m_crit = crit;
                    this->m_indexes = *pv;
                    this->notify();
                }// pv
            }// exch
        }// pi
    }// not done
    return (this->getResult());
}// arrange
future_type arrangeAsync(void){
    return QtConcurrent::run([this]()->matelem_result_type{
                                 return this->arrange();
                             });
}// arrangeAsync

protected:
MatElem(const matelem_type &other):
    m_pcancel(other.m_pcancel),
    m_notify(other.m_notify),m_pdist(other.m_pdist),m_crit(other.m_crit),
    m_callback(other.m_callback),m_indexes(other.m_indexes){
}
matelem_type & operator=(const matelem_type &other){
    if (this != &other){
        this->m_pcancel = other.m_pcancel;
        this->m_notify = other.m_notify;
        this->m_pdist = other.m_pdist;
        this->m_crit = other.m_crit;
        this->m_callback =other.m_callback;
        this->m_indexes = other.m_indexes;
    }
    return (*this);
}// operator=
bool is_cancellation_requested(void){
    return ((this->m_pcancel != nullptr) && this->m_pcancel->load());
}// is_cancellation_requested

void notify(void){
    if (this->m_notify){
        matelem_result_type r = this->getResult();
        (this->m_callback)(r);
    }
}// notify
bool find_best_permutation(QQueue<pair_type> &oPairs, criteria_type &cBest)  {
    oPairs.clear();
    if (this->is_cancellation_requested()){
        return (false);
    }
    const ints_vector &oInd = this->m_indexes;
    const distancemap_type *pMap = this->m_pdist;
    criteria_type old = cBest;
    const int n = oInd.size();
    for (int i = 0; i < n; ++i){
        if (this->is_cancellation_requested()){
            oPairs.clear();
            return (false);
        }
        for (int j = 0; j < i; ++j){
            if (this->is_cancellation_requested()){
                oPairs.clear();
                return (false);
            }
            ints_vector xind(oInd);
            index_type t = xind[i];
            xind[i] = xind[j];
            xind[j] = t;
            criteria_type c = 0;
            pMap->compute_criteria(xind,c);
            if (c < old){
                old = c;
                oPairs.clear();
                oPairs.enqueue(pair_type(i,j));
            } else if (c == old){
                if (!oPairs.empty()){
                    oPairs.enqueue(pair_type(i,j));
                }
            }// equals
        }// j
    }// i
    cBest = old;
    if (this->is_cancellation_requested()){
        oPairs.clear();
        return (false);
    }
    return (!oPairs.empty());
}// find_best_permutation
private:
cancellableflag_type *m_pcancel;
bool m_notify;
const distancemap_type *m_pdist;
criteria_type m_crit;
matelem_callback_type m_callback;
ints_vector m_indexes;
};
////////////////////////////////////
}// namespace info
#endif // QMATELEM_H
