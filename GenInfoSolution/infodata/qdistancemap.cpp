#include "qdistancemap.h"
namespace info {
QDistanceMap::QDistanceMap(QObject *parent) : QObject(parent)
{

}
QDistanceMap::~QDistanceMap(){

}
void QDistanceMap::add(const int i, const int j, const double val){
    int ii = i, jj = j;
    if (ii > jj){
        int t = ii;
        ii = jj;
        jj = t;
    }
    map_type &oMap = this->m_map;
    if (!oMap.contains(ii)){
        first_map_type m;
        m[jj] = val;
        oMap[ii] = m;
    } else {
        first_map_type &m = oMap[ii];
        m[jj] = val;
    }
    set_type &oSet = this->m_set;
    oSet.insert(ii);
    oSet.insert(jj);
}// add
bool QDistanceMap::get(const int i, const int j, double &val) const {
    int ii = i, jj = j;
    if (ii > jj){
        int t = ii;
        ii = jj;
        jj = t;
    }
    const map_type &oMap = this->m_map;
    if (oMap.contains(ii)){
        const first_map_type &m = oMap[ii];
        if (m.contains(jj)){
            val = m[jj];
            return (true);
        }
    }
    return (false);
}// get
void QDistanceMap::get_indexes(QVector<int> &oInds) const {
    oInds.clear();
    const set_type &oSet = this->m_set;
    for (auto &ival : oSet){
        oInds.push_back(ival);
    }
}// get_indexes
double QDistanceMap::criteria(const ints_vector &oInds) const {
    double dRet = 0.0;
    int nc = 0;
    const int n = oInds.size();
    for (int i = 1; i < n; ++ i){
        const int ii = oInds[i-1];
        const int jj = oInds[i];
        double val;
        if (this->get(ii,jj,val)){
            dRet += val;
            ++nc;
        }
    }// i
    if (nc > 1){
        dRet /= nc;
    }
    return (dRet);
}// criteria
void QDistanceMap::clear(void){
    this->m_map.clear();
    this->m_set.clear();
}// clear

////////////////////////////////
}// namespace info
