#ifndef QDISTANCEMAP_H
#define QDISTANCEMAP_H
////////////////////////////
#include <QMap>
#include <QSet>
#include <QVector>
//////////////////////////////////
namespace info {
////////////////////////////////////////
/// \brief The QDistanceMap class
template <typename INDEXTYPE,typename DATATYPE>
class DistanceMap
{
public:
    using index_type = INDEXTYPE;
    using data_type = DATATYPE;
    //
    using first_map_type = QMap<index_type,data_type>;
    using map_type = QMap<index_type,first_map_type>;
    using set_type = QSet<index_type>;
public:
    DistanceMap(){}
    virtual ~DistanceMap(){}
    void clear(void){
        this->m_map.clear();
        this->m_set.clear();
    }// clear
    template <typename X>
    void get_indexes(QVector<X> &oInds) const{
        oInds.clear();
        const set_type &oSet = this->m_set;
        for (auto &ival : oSet){
            oInds.push_back(ival);
        }
    }// get_indexes
    template <typename XU, typename XA>
    void add(const XU i, const XU j, const XA val){
        index_type ii = (index_type)i, jj = (index_type)j;
        if (ii > jj){
            index_type t = ii;
            ii = jj;
            jj = t;
        }
        data_type dd = (data_type)val;
        map_type &oMap = this->m_map;
        if (!oMap.contains(ii)){
            first_map_type m;
            m[jj] = dd;
            oMap[ii] = m;
        } else {
            first_map_type &m = oMap[ii];
            m[jj] = dd;
        }
        set_type &oSet = this->m_set;
        oSet.insert(ii);
        oSet.insert(jj);
    }// add
    template <typename XU, typename XA>
    bool get(const XU i, const XU j, XA &val) const{
        index_type ii = (index_type)i, jj = (index_type)j;
        if (ii > jj){
            index_type t = ii;
            ii = jj;
            jj = t;
        }
        const map_type &oMap = this->m_map;
        if (oMap.contains(ii)){
            const first_map_type &m = oMap[ii];
            if (m.contains(jj)){
                data_type vval = m[jj];
                val = (XA)vval;
                return (true);
            }
        }
        return (false);
    }// get
    template <typename XU, typename XA>
    void compute_criteria(const QVector<XU> &oInds, XA &res) const{
        XA dRet = 0.0;
        const int n = oInds.size();
        for (int i = 1; i < n; ++ i){
            const index_type ii = (index_type)oInds[i-1];
            const index_type jj = (index_type)oInds[i];
            XA val;
            if (this->get(ii,jj,val)){
                dRet = (XA)(dRet + val);
            }
        }// i
        res = dRet;
    }// criteria
    template <typename XU,typename XA>
    bool recode(DistanceMap<XU,XA> &oResMap, const XA zMax = 1000, const XA zMin = 0) const{
        bool bRet = false;
        if (zMax <= zMin){
            return (bRet);
        }
        double vmin = 0, vmax = 0;
        bool bFirst = true;
        const map_type &oMap = this->m_map;
        for (auto it = oMap.begin(); it != oMap.end(); ++it){
            const first_map_type &m = it.value();
            for (auto jt = m.begin(); jt != m.end(); ++jt){
                double x = (double)jt.value();
                if (bFirst){
                    vmin = x;
                    vmax = x;
                    bFirst = false;
                } else if (x < vmin){
                    vmin = x;
                } else if (x > vmax){
                    vmax = x;
                }
            }// jt
        }// it
        if (vmin >= vmax){
            return (bRet);
        }
        const double delta = ((double)zMax -(double)zMin) /(vmax - vmin);
        oResMap.clear();
        for (auto it = oMap.begin(); it != oMap.end(); ++it){
            index_type i = it.key();
             XU ii = (XU)i;
            const first_map_type &m = it.value();
            for (auto jt = m.begin(); jt != m.end(); ++jt){
                index_type j = jt.key();
                data_type d =jt.value();
                double x = ((double)d - vmin) * delta + zMin;
                XU jj = (XU)j;
                XA v = (XA)x;
                oResMap.add(ii,jj,v);
            }// jt
        }// it
        return (true);
    }// recode
private:
    map_type m_map;
    set_type m_set;
};
//////////////////////////////////////
}// namespace info
#endif // QDISTANCEMAP_H
