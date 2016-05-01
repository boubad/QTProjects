#include "qindivcluster.h"
namespace info {
////////////////////////////////////
QIndivCluster::QIndivCluster(QObject *parent) : QObject(parent),
    m_index(0),m_provider(nullptr)
{

}
QIndivCluster::QIndivCluster(const int nIndex,IIndivProvider *pProvider,
                             QObject *parent):
    QObject(parent),m_index(nIndex),m_provider(pProvider){
    Q_ASSERT(this->m_provider != nullptr);
    Q_ASSERT(this->m_provider->is_valid());
}
QIndivCluster::QIndivCluster(const int nIndex,IIndivProvider *pProvider,
                             const QMap<IntType,QVariant> &oMap,QObject *parent):
    QObject(parent),m_index(nIndex),m_provider(pProvider),m_center(oMap){
    Q_ASSERT(this->m_provider != nullptr);
    Q_ASSERT(this->m_provider->is_valid());
}
QIndivCluster::~QIndivCluster()
{

}
int QIndivCluster::index(void) {
    QReadLocker oLock(&(this->m_mutex));
    return this->m_index;
}
void QIndivCluster::index(const int n){
    QWriteLocker oLock(&(this->m_mutex));
    this->m_index = n;
}
void QIndivCluster::get_center(QMap<IntType,QVariant> &oMap) {
    QReadLocker oLock(&(this->m_mutex));
    oMap = this->m_center;
}
void QIndivCluster::set_center(const QMap<IntType, QVariant> &oMap){
    QWriteLocker oLock(&(this->m_mutex));
    this->m_center = oMap;
}// set_center
void QIndivCluster::get_members(QVector<IntType> &oVec){
    QReadLocker oLock(&(this->m_mutex));
    oVec = this->m_members;
}
void QIndivCluster::set_members(const QVector<IntType> &oVec){
    QWriteLocker oLock(&(this->m_mutex));
    this->m_members = oVec;
}
void QIndivCluster::clear_members(void){
    QWriteLocker oLock(&(this->m_mutex));
    this->m_members.clear();
}
void QIndivCluster::add_member(const IntType aIndex){
    if (aIndex == 0){
        return;
    }
    IIndivProvider *pProvider = this->m_provider;
    if (pProvider == nullptr){
        return;
    }
    if (!pProvider->contains_id(aIndex)){
        return;
    }
    bool bFound = false;
    {
        QWriteLocker oLock(&(this->m_mutex));
        Q_FOREACH(IntType xIndex, this->m_members)
        {
            if (xIndex == aIndex){
                bFound = true;
                break;
            }
        }
        if (!bFound){
            this->m_members.push_back(aIndex);
        }
    }
    if (bFound){
        emit members_added(aIndex);
    }
}// add_member
void QIndivCluster::update_center(const VariableMode mode /*= VariableMode::modeNumeric*/){
    {
        QWriteLocker oLock(&(this->m_mutex));
        IIndivProvider *pProvider = this->m_provider;
        QVector<IntType> members = this->m_members;
        const int np = members.size();
        if ((np < 1) || (pProvider == nullptr)){
            return;
        }
        if (!pProvider->is_valid()){
            return;
        }
        QMap<IntType,int> counts;
        QMap<IntType, double> sommes;
        Q_FOREACH(IntType aIndex, members)
        {
            QIndiv oInd;
            if (pProvider->find_indiv(aIndex,oInd,mode)){
                QMap<IntType,QVariant> oMap = oInd.data();
                QList<IntType> keys = oMap.keys();
                Q_FOREACH(IntType key, keys)
                {
                    QVariant v = oMap[key];
                    if (v.canConvert<double>()){
                        int nx = 0;
                        double ss = 0;
                        double dd = v.toDouble();
                        if (counts.contains(key)){
                            nx = counts[key];
                            ss = sommes[key];
                        }
                        ++nx;
                        ss += dd;
                        counts[key] = nx;
                        sommes[key] = ss;
                    }// can convert
                }// key
            }// got indiv
        }// aIndex
        QMap<IntType,QVariant> oCenter;
        QList<IntType> keys = counts.keys();
        Q_FOREACH(IntType key, keys)
        {
            int nx = counts[key];
            double ss = sommes[key];
            if (nx > 1){
                ss /= nx;
            }
            oCenter[key] = QVariant(ss);
        }//keys
        this->m_center = oCenter;
    }
    emit center_updated();
}// update_center
double QIndivCluster::distance(const QIndiv &oInd) {
    double dRet = 0;
    int nc = 0;
    //
    QMap<IntType,QVariant> m1;
    this->get_center(m1);
    QMap<IntType,QVariant> m2 = oInd.data();
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
double QIndivCluster::distance(QIndivCluster *pCluster) {
    Q_ASSERT(pCluster != nullptr);
    double dRet = 0;
    int nc = 0;
    //
    QMap<IntType,QVariant> m1;
    this->get_center(m1);
    QMap<IntType,QVariant> m2;
    pCluster->get_center(m2);
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
//////////////////////////////////
}// namespace info
