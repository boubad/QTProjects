#include "qdbindivprovider.h"
namespace info {
//////////////////////////////////
QDBIndivProvider::QDBIndivProvider(DBStatHelper *pMan,
                                   const DBStatDataset &oSet,QObject *parent) :
    QObject(parent),m_count(0),m_pman(pMan),m_oset(oSet)
{
    this->initialize();
}
QDBIndivProvider::QDBIndivProvider(DBStatHelper *pMan,
                                   const QString &sigle,QObject *parent) :
    QObject(parent),m_count(0),m_pman(pMan),m_oset(sigle)
{
    this->initialize();
}
void QDBIndivProvider::initialize(void){
    this->m_vars.clear();
    DBStatHelper *pMan = this->m_pman;
    Q_ASSERT(pMan != nullptr);
    Q_ASSERT(pMan->isValid());
    bool bRet = pMan->find_dataset(this->m_oset);
    Q_ASSERT(bRet);
    Q_ASSERT(this->m_oset.id() != 0);
    //
    int nCount = 0;
    if (!pMan->find_dataset_variables_count(this->m_oset,nCount)){
        return;
    }
    int skip = 0;
    const int taken = 100;
    QMap<IntType,DBStatVariable> &oMap = this->m_vars;
    while (skip < nCount){
        QList<DBStatVariable> v;
        if (!pMan->find_dataset_variables(this->m_oset,v,skip,taken)){
            return;
        }
        int n = v.size();
        for (int i = 0; i < n; ++i){
            DBStatVariable vb = v[i];
            IntType key = vb.id();
            Q_ASSERT(key != 0);
            oMap[key] = vb;
        }// i
        if (n < taken){
            break;
        }
        skip += n;
    }// while
    //
    nCount = 0;
    if (!pMan->find_dataset_indivs_count(this->m_oset,nCount)){
        return;
    }
    this->m_count = nCount;
    skip = 0;
    QVector<IntType> &oIds = this->m_ids;
    oIds.clear();
    while (skip < nCount){
        QList<IntType> v;
        if (!pMan->find_dataset_indivs_ids(this->m_oset,v,skip,taken)){
            return;
        }
        int n = v.size();
        for (int i = 0; i < n; ++i){
            oIds.push_back(v[i]);
        }// i
        if (n < taken){
            break;
        }
        skip += n;
    }// while
}// initialize

bool QDBIndivProvider::is_valid(void){
    return ((this->m_pman != nullptr) && (this->m_pman->isValid()) &&
            (this->m_oset.id() != 0));
}

bool QDBIndivProvider::indivs_count(int &nCount){
    Q_ASSERT(this->is_valid());
    nCount = this->m_count;
    return (true);
}

bool QDBIndivProvider::all_indivs_ids(QVector<IntType> &oIds){
    Q_ASSERT(this->is_valid());
    oIds = this->m_ids;
    return (true);
}
bool QDBIndivProvider::find_indiv_at(const int pos,QIndiv &oInd,
                                        const VariableMode mode /*= VariableMode::modeAll*/){
    Q_ASSERT(this->is_valid());
    if ((pos < 0) || (pos >= this->m_ids.size())){
        return (false);
    }
    return this->find_indiv(this->m_ids[pos],oInd,mode);
}//find_indiv_at
 bool QDBIndivProvider::contains_id(const IntType aIndex){
    Q_ASSERT(this->is_valid());
     Q_FOREACH(IntType xIndex, this->m_ids)
     {
         if (aIndex == xIndex){
             return (true);
         }
     }
     return (false);
}//contains_ids

bool QDBIndivProvider::find_indiv(const IntType aIndex, QIndiv &oInd,
                                  const VariableMode mode /*= VariableMode::modeAll*/){
    Q_ASSERT(this->is_valid());
    DBStatHelper *pMan = this->m_pman;
    Q_ASSERT(pMan != nullptr);
    Q_ASSERT(pMan->isValid());
    QList<DBStatValue> oVals;
    DBStatIndiv xInd(aIndex);
    if (!pMan->find_indiv_values(xInd,oVals)){
        return (false);
    }
    QMap<IntType,QVariant> oMap;
    //
    Q_FOREACH(const DBStatValue &vv, oVals)
    {
        QVariant vx = vv.value();
        if (vx.isValid()){
            IntType key = vv.variable_id();
            if (this->m_vars.contains(key)){
                const DBStatVariable  &vz = this->m_vars[key];
                switch (mode){
                case VariableMode::modeNumeric:
                    if (vz.is_numeric()){
                        oMap[key] = vx;
                    }
                    break;
                case VariableMode::modeNominal:
                    if (vz.is_categ()){
                        oMap[key] = vx;
                    }
                    break;
                case VariableMode::modeAll:
                    oMap[key] = vx;
                    break;
                default:
                    break;
                }// mode
            }// found var
        }// vx
    }// vv
    oInd =  QIndiv(xInd,oMap);
    return (true);
}// find_indiv
bool QDBIndivProvider::distance(const IntType aIndex1, const IntType &aIndex2,
                      double &dRes){
    Q_ASSERT(this->is_valid());
    QIndiv oInd1, oInd2;
    dRes = 0;
    if (!this->find_indiv(aIndex1,oInd1,VariableMode::modeNumeric)){
        return (false);
    }
    if (!this->find_indiv(aIndex2,oInd2,VariableMode::modeNumeric)){
        return (false);
    }
    dRes = oInd1.distance(oInd2);
    return (true);
}

bool QDBIndivProvider::distance_at(const int pos1, const int pos2,
                      double &dRes){
    Q_ASSERT(this->is_valid());
    QIndiv oInd1, oInd2;
    dRes = 0;
    if (!this->find_indiv_at(pos1,oInd1,VariableMode::modeNumeric)){
        return (false);
    }
    if (!this->find_indiv_at(pos2,oInd2,VariableMode::modeNumeric)){
        return (false);
    }
    dRes = oInd1.distance(oInd2);
    return (true);
}

////////////////////////////////////
}// namespace info
