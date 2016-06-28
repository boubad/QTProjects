#include "qworkindivprovider.h"
#include "qdbindivprovider.h"
//////////////////////////////////
namespace info {
////////////////////////////////////////////
QWorkIndivProvider::QWorkIndivProvider(IIndivProvider *pProvider,
                                           QObject *parent) : QObject(parent),m_mustDelete(false),m_provider(pProvider),m_current(0),m_nmax(0)
{
    IIndivProvider *p = this->m_provider;
    Q_ASSERT(p != nullptr);
    Q_ASSERT(p->is_valid());
    bool bRet = p->indivs_count(this->m_nmax);
    Q_ASSERT(bRet);
    bRet = p->get_dataset(this->m_oset);
    Q_ASSERT(bRet);
    QList<DBStatVariable> oList;
    bRet = p->get_variables(oList);
    QMap<IntType,DBStatVariable> m_allvars;
}
QWorkIndivProvider::QWorkIndivProvider(DBStatHelper *pMan,
                                           const DBStatDataset &oSet,QObject *parent):
    QObject(parent),m_mustDelete(true),m_provider(nullptr),m_current(0),m_nmax(0){
    this->m_provider = new QDBIndivProvider(pMan,oSet,this);
    IIndivProvider *p = this->m_provider;
    Q_ASSERT(p != nullptr);
    Q_ASSERT(p->is_valid());
    bool bRet = p->indivs_count(this->m_nmax);
    Q_ASSERT(bRet);
    bRet = p->get_dataset(this->m_oset);
    Q_ASSERT(bRet);
}

QWorkIndivProvider::QWorkIndivProvider(DBStatHelper *pMan,
                                           const QString &sigleDataset,
                                           QObject *parent):
    QObject(parent),m_mustDelete(true),m_provider(nullptr),m_current(0),m_nmax(0){
    this->m_provider = new QDBIndivProvider(pMan,sigleDataset,this);
    IIndivProvider *p = this->m_provider;
    Q_ASSERT(p != nullptr);
    Q_ASSERT(p->is_valid());
    bool bRet = p->indivs_count(this->m_nmax);
    Q_ASSERT(bRet);
    bRet = p->get_dataset(this->m_oset);
    Q_ASSERT(bRet);
}

QWorkIndivProvider::~QWorkIndivProvider(){
    if (this->m_mustDelete && (this->m_provider != nullptr)){
        delete this->m_provider;
        this->m_provider = nullptr;
    } // delete
}

bool QWorkIndivProvider::is_valid(void){
    return ((this->m_provider != nullptr) && this->m_provider->is_valid());
}

bool QWorkIndivProvider::reset(void){
    Q_ASSERT(this->is_valid());
    QMutexLocker olOck(&_mutex);
    this->m_current = 0;
    return (true);
}

bool QWorkIndivProvider::get_variables(QList<DBStatVariable> &oVars){
    Q_ASSERT(this->is_valid());
    return (this->m_provider->get_variables(oVars));
}

bool QWorkIndivProvider::next(QIndiv &oInd,
                                const VariableMode mode /*= VariableMode::modeAll*/){
    Q_ASSERT(this->is_valid());
    QMutexLocker olOck(&_mutex);
    int n = this->m_current;
    bool bRet = false;
    if (n < this->m_nmax){
        bRet  = this->m_provider->find_next_indiv(this->m_oset,n,oInd, mode);
        this->m_current = n + 1;
        if (!bRet){
            oInd = QIndiv();
        }
    } else {
        oInd = QIndiv();
    }
    return (bRet);
}// next
bool QWorkIndivProvider::indivs_count(int &nCount){
    Q_ASSERT(this->is_valid());
    return this->m_provider->indivs_count(nCount);
}

bool QWorkIndivProvider::all_indivs_ids(QVector<IntType> &oIds){
    Q_ASSERT(this->is_valid());
    return this->m_provider->all_indivs_ids(oIds);
}

bool QWorkIndivProvider::contains_id(const IntType aId){
    Q_ASSERT(this->is_valid());
    return this->m_provider->contains_id(aId);
}

bool QWorkIndivProvider::find_indiv(const IntType aIndex,QIndiv &oInd,
                                      const VariableMode mode /*= VariableMode::modeAll*/){
    Q_ASSERT(this->is_valid());
    return this->m_provider->find_indiv(aIndex,oInd,mode);
}

bool QWorkIndivProvider::find_indiv_at(const int pos,QIndiv &oInd,
                                         const VariableMode mode /*= VariableMode::modeAll*/){
    Q_ASSERT(this->is_valid());
    return this->m_provider->find_indiv_at(pos,oInd,mode);
}

bool QWorkIndivProvider::distance(const IntType aIndex1, const IntType &aIndex2,
                                    double &dRes,const VariableMode mode /*= VariableMode::modeNumeric*/){
    Q_ASSERT(this->is_valid());
    return this->m_provider->distance(aIndex1,aIndex2,dRes,mode);
}

bool QWorkIndivProvider::distance_at(const int pos1, const int pos2,
                                       double &dRes,const VariableMode mode /*= VariableMode::modeNumeric*/){
    Q_ASSERT(this->is_valid());
    return this->m_provider->distance(pos1,pos2,dRes,mode);
}

bool  QWorkIndivProvider::find_next_indiv(const DBStatDataset &oSet,
                                            const int offset,QIndiv &oInd,
                                            const VariableMode mode /*= VariableMode::modeAll*/){
    Q_ASSERT(this->is_valid());
    return this->m_provider->find_next_indiv(oSet,offset,oInd,mode);
}

bool QWorkIndivProvider::get_dataset(DBStatDataset &oSet){
    Q_ASSERT(this->is_valid());
    return this->m_provider->get_dataset(oSet);
}

/////////////////////////////////////////////////////
//////////////////////////////////////////////////////
}// namespace info
