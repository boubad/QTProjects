#include "qserialindivprovideractor.h"
#include <QMutexLocker>
//////////////////////////////
namespace info {
///////////////////////////////////////////////////
InfoStatData::InfoStatData(IntType aIndex) :_index(aIndex), _vmin(0), _vmax(0), _vmean(0), _vstd(0), _count(0),
    _somme1(0), _somme2(0) {

}
InfoStatData::InfoStatData(const InfoStatData &other) : _index(other._index), _vmin(other._vmin), _vmax(other._vmax),
    _vmean(other._vmean), _vstd(other._vstd), _count(other._count),
    _somme1(other._somme1), _somme2(other._somme2) {

}
InfoStatData & InfoStatData::operator=(const InfoStatData &other) {
    if (this != &other) {
        this->_index = other._index;
        this->_vmin = other._vmin;
        this->_vmax = other._vmax;
        this->_vmean = other._vmean;
        this->_vstd = other._vstd;
        this->_count = other._count;
        this->_somme1 = other._somme1;
        this->_somme2 = other._somme2;
    }
    return (*this);
}
InfoStatData::~InfoStatData() {

}
bool InfoStatData::operator==(const InfoStatData &other) const {
    return (this->_index == other._index);
}
bool InfoStatData::operator<(const InfoStatData &other) const {
    return (this->_index < other._index);
}
void InfoStatData::add(const double dval) {
    if (this->_count < 1) {
        this->_vmin = dval;
        this->_vmax = dval;
    }
    else {
        if (dval < this->_vmin) {
            this->_vmin = dval;
        }
        if (dval > this->_vmax) {
            this->_vmax = dval;
        }
    }
    this->_count = this->_count + 1;
    this->_somme1 += dval;
    this->_somme2 += dval * dval;
}
void InfoStatData::compute(void) {
    size_t nc = this->_count;
    if (nc > 0) {
        const double s1 = this->_somme1 / nc;
        this->_vmean = s1;
        double s2 = ((this->_somme2) / nc) - (s1 * s1);
        Q_ASSERT(s2 >= 0);
        this->_vstd = std::sqrt(s2);
    }// nc
}
void InfoStatData::get(IntType &index, double &vmin, double &vmax, double &vmean, double &vstd) const {
    index = this->_index;
    vmin = this->_vmin;
    vmax = this->_vmax;
    vmean = this->_vmean;
    vstd = this->_vstd;
}
/////////////////////////////////////////////////
QSerialIndivConsumerActor::QSerialIndivConsumerActor(IIndivProvider *pProvider,
                                                     QSemaphore *pFree,
                                                     QSemaphore *pUsed,
                                                     QQueue<QIndivPtr> *pResult,QObject *parent) : QThread(parent),m_provider(pProvider),
    m_freeSemaphore(pFree),m_usedSemaphore(pUsed),m_pindiv(pResult)
{
    Q_ASSERT(this->m_provider != nullptr);
    Q_ASSERT(this->m_provider->is_valid());
    Q_ASSERT(this->m_freeSemaphore != nullptr);
    Q_ASSERT(this->m_usedSemaphore != nullptr);
    Q_ASSERT(this->m_pindiv != nullptr);
}
void QSerialIndivConsumerActor::process_indiv(const QIndivPtr &oIndPtr){
    const QIndiv *pInd = oIndPtr.get();
    if (pInd == nullptr){
        return;
    }
    QMap<IntType,InfoStatData> &oStats = this->m_stats;
   const QMap<IntType,QVariant> oMap = pInd->data();
   QList<IntType> keys = oMap.keys();
   Q_FOREACH(IntType key, keys)
   {
       QVariant v = oMap[key];
       if (v.isValid()){
           if (v.canConvert<double>()){
               double dval = v.toDouble();
               if (!oStats.contains(key)){
                   oStats[key] = InfoStatData(key);
               }
               (oStats[key]).add(dval);
           }// canConvert
       }// valid
   }
   emit indiv_acquired(oIndPtr);
}// process_indiv

void QSerialIndivConsumerActor::run(){
    IIndivProvider *pProvider = this->m_provider;
    Q_ASSERT(pProvider != nullptr);
    Q_ASSERT(pProvider->is_valid());
    QSemaphore *pFree = this->m_freeSemaphore;
    Q_ASSERT(pFree != nullptr);
    QSemaphore *pUsed = this->m_usedSemaphore;
    Q_ASSERT(pUsed != nullptr);
    QQueue<QIndivPtr> *pResult = this->m_pindiv;
    Q_ASSERT(pResult != nullptr);
    //
    this->m_stats.clear();
    int nTotal = 0;
    bool bRet = pProvider->indivs_count(nTotal);
    Q_ASSERT(bRet);
    for (int i = 0; i < nTotal; ++i){
        pUsed->acquire();
        if (!pResult->isEmpty()){
            QIndivPtr oPtr = pResult->dequeue();
            QIndiv *pInd = oPtr.get();
            if ((pInd != nullptr) && (pInd->id() != 0)){
                this->process_indiv(oPtr);
            }
        }// not empty
        pFree->release();
    } // i
}// run
////////////////////////////////////////////////
QSerialIndivProviderActor::QSerialIndivProviderActor(IIndivProvider *pProvider,
                                                     QSemaphore *pFree,
                                                     QSemaphore *pUsed,
                                                     QQueue<QIndivPtr> *pResult,QObject *parent) : QThread(parent),m_provider(pProvider),
    m_freeSemaphore(pFree),m_usedSemaphore(pUsed),m_pindiv(pResult)
{
    Q_ASSERT(this->m_provider != nullptr);
    Q_ASSERT(this->m_provider->is_valid());
    Q_ASSERT(this->m_freeSemaphore != nullptr);
    Q_ASSERT(this->m_usedSemaphore != nullptr);
    Q_ASSERT(this->m_pindiv != nullptr);
}
void QSerialIndivProviderActor::run(){
    IIndivProvider *pProvider = this->m_provider;
    Q_ASSERT(pProvider != nullptr);
    Q_ASSERT(pProvider->is_valid());
    QSemaphore *pFree = this->m_freeSemaphore;
    Q_ASSERT(pFree != nullptr);
    QSemaphore *pUsed = this->m_usedSemaphore;
    Q_ASSERT(pUsed != nullptr);
    QQueue<QIndivPtr> *pResult = this->m_pindiv;
    Q_ASSERT(pResult != nullptr);
    //
    int nTotal = 0;
    bool bRet = pProvider->indivs_count(nTotal);
    Q_ASSERT(bRet);
    DBStatDataset oSet;
    bRet = pProvider->get_dataset(oSet);
    Q_ASSERT(bRet);
    for (int i = 0; i < nTotal; ++i){
        pFree->acquire();
        QIndiv oInd;
        QIndivPtr oPtr;
        bRet = pProvider->find_next_indiv(oSet,i,oInd);
        if (bRet){
            oPtr = std::make_shared<QIndiv>(oInd);
        }
        pResult->enqueue(oPtr);
        pUsed->release();
    } // i
}// run
/////////////////////////////////
}// namespace info
