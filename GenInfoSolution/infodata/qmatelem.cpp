#include "qmatelem.h"
/////////////////////////////
namespace info {
////////////////////////////////
QMatElemControl::QMatElemControl(const QDistanceMap *pMap,QObject *parent):QObject(parent){
    Q_ASSERT(pMap != nullptr);
    QMatElem *worker = new QMatElem(pMap,this);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this,  &QMatElemControl::arrange, worker, &QMatElem::arrange);
    connect(worker, &QMatElem::start_arrange, this, &QMatElemControl::start_arrange);
    connect(worker, &QMatElem::newcriteria, this, &QMatElemControl::newcriteria);
    connect(worker, &QMatElem::end_arrange, this,&QMatElemControl::end_arrange);
    workerThread.start();
}
QMatElemControl::~QMatElemControl() {
    workerThread.quit();
    workerThread.wait();
}
void QMatElemControl::start_arrange(void){
    emit started();
}

void QMatElemControl::newcriteria(double c, result_type ind ){
    emit current(c,ind);
}

void QMatElemControl::end_arrange(void){
    emit finished();
}
////////////////////////
QMatElem::QMatElem(const QDistanceMap *pMap,QObject *parent) : QObject(parent),
    m_cancel(false),m_pMap(pMap),m_lastcrit(0)
{
    Q_ASSERT(this->m_pMap != nullptr);
    pMap->get_indexes(this->m_indexes);
    this->m_lastcrit = pMap->criteria(this->m_indexes);
}
QMatElem::~QMatElem(){

}
void QMatElem::cancel(void){
    this->m_cancel.store(true);
}
QMatElem::result_type  QMatElem::get_result(void){
    ints_vector &oInd = this->m_indexes;
    const int n = oInd.size();
    std::shared_ptr<int> ox;
    ox.reset(new int[n]);
    int *pi = ox.get();
    if (pi != nullptr){
        for (int i = 0; i < n; ++i){
            pi[i] = oInd[i];
        }
        emit newcriteria(this->m_lastcrit,ox);
    }// pi
    return (ox);
}

void QMatElem::arrange(void){
    emit start_arrange();
    bool done = false;
    this->m_cancel.store(false);
    double crit = this->m_lastcrit;
    pair_type oPair;
    ints_vector &oInd = this->m_indexes;
    while (!done){
        if (this->m_cancel.load()){
            done = true;
            break;
        }
        if (!this->find_best_permutation(oPair,crit)){
            done = true;
            break;
        }
        int i = oPair.first;
        int j = oPair.second;
        if (i == j){
            done = true;
            break;
        }
        int t = oInd[i];
        oInd[i] = oInd[j];
        oInd[j] = t;
        this->m_lastcrit = crit;
        result_type ox = this->get_result();
        emit newcriteria(crit,ox);
    }//
    emit end_arrange();
}// arrange
bool QMatElem::find_best_permutation(pair_type &oPair, double &bestCrit) const{
    oPair.first = 0;
    oPair.second = 0;
    const ints_vector &oInd = this->m_indexes;
    const QDistanceMap *pMap = this->m_pMap;
    const int n = oInd.size();
    for (int i = 0; i < n; ++i){
        if (this->m_cancel.load()){
            return (false);
        }
        for (int j = 0; j < i; ++j){
            if (this->m_cancel.load()){
                return (false);
            }
            ints_vector xind(oInd);
            int t = xind[i];
            xind[i] = xind[j];
            xind[j] = t;
            double c = pMap->criteria(xind);
            if (c < bestCrit){
                bestCrit = c;
                oPair.first = j;
                oPair.second = i;
            }
        }// j
    }// i
    return (oPair.first != oPair.second);
}// find_best_permutation
////////////////////////////
} // namespace info
