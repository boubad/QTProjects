#ifndef TREEELEM_H
#define TREEELEM_H
//////////////////////
#include "indiv.h"
#include "matricedata.h"
////////////////////////////////
#include <QMap>
#include <QVector>
#include <memory>
#include <algorithm>
///////////////////////////////
namespace info {
//////////////////////////////////////
template<typename INDEXTYPE,typename DATATYPE>
class TreeItem  : public Indiv<INDEXTYPE,DATATYPE> {
public:
    using index_type  = INDEXTYPE;
    using data_type = DATATYPE;
    using IndivType = Indiv<index_type,data_type>;
    using string_type = typename IndivType::string_type;
    //
    using TreeItemType = TreeItem<index_type,data_type>;
    using PTreeItemType = TreeItemType *;
    using treeitem_ptr = std::shared_ptr<TreeItemType>;
    using ints_sizet_map = QMap<index_type,int>;
    using ints_vector = QVector<index_type>;
private:
    treeitem_ptr m_left;
    treeitem_ptr m_right;
    //
    TreeItem(const TreeItemType &) = delete;
    TreeItemType & operator=(const TreeItemType &) = delete;
public:
    template <typename X>
    TreeItem(index_type aIndex,int nCols, const X &pxdata,
             const string_type &sSigle = string_type()) :
        IndivType(aIndex,nCols,pxdata,sSigle) {
        Q_ASSERT(aIndex != 0);
    }
    TreeItem(treeitem_ptr left, treeitem_ptr right) :
        m_left(left),m_right(right) {
        PTreeItemType pLeft = left.get();
        Q_ASSERT(pLeft != nullptr);
        PTreeItemType pRight = right.get();
        Q_ASSERT(pRight != nullptr);
        const int nCols = pLeft->size();
        Q_ASSERT(nCols > 0);
        Q_ASSERT(pRight->size() == nCols);
        this->m_ncols = nCols;
        this->m_center.reset(new data_type[nCols]);
        data_type *p = this->m_center.get();
        Q_ASSERT(p != nullptr);
        const data_type *p1 = pLeft->center();
        const data_type *p2 = pRight->center();
        for (size_t i = 0; i < nCols; ++i){
            p[i] = (data_type)((p1[i] + p2[i]) / 2.0);
        }// i
    } // TreeItem
    virtual ~TreeItem() {
    }
public:
    bool is_leaf(void) const {
        return ((this->m_left.get() == nullptr) || (this->m_right.get() == nullptr));
    }
    const TreeItemType *left(void) const {
        return (this->m_left.get());
    }
    const TreeItemType *right(void) const {
        return (this->m_right.get());
    }
    void get_map(ints_sizet_map &oMap, const int val) const {
        if (this->is_leaf()) {
            oMap[this->id()] = val;
            return;
        }
        this->m_left->get_map(oMap, val);
        this->m_right->get_map(oMap, val);
    } // get_map
    void get_ids(ints_vector &oVec) const {
        if (this->is_leaf()) {
            oVec.push_back(this->id());
            return;
        }
        this->m_left->get_ids(oVec);
        this->m_right->get_ids(oVec);
    } // get_map
};
// class TreeItem<INDEXTYPE,DATATYPE>
//////////////////////////////////////////
template <typename INDEXTYPE>
class TreeResult {
public:
  using index_type  = INDEXTYPE;
  using ints_sizet_map = QMap<index_type,int>;
  using ints_vector = QVector<index_type>;
  using TreeResultType = TreeResult<index_type>;
  using TreeResultTypePtr = std::shared_ptr<TreeResultType>;
private:
  ints_vector m_ids;
  ints_sizet_map m_map;
public:
  TreeResult(){}
  TreeResult(const TreeResultType &other):m_ids(other.m_ids),m_map(other.m_map){}
  TreeResultType & operator=(const TreeResultType &other){
    if (this != &other){
      this->m_ids = other.m_ids;
      this->m_map = other.m_map;
    }
    return (*this);
  }
  virtual ~TreeResult(){}
public:
  void clear(void){
    this->m_ids.clear();
    this->m_map.clear();
  }
  const ints_vector & ids(void) const {
    return (this->m_ids);
  }
  ints_vector & ids(void) {
    return (this->m_ids);
  }
  const ints_sizet_map & map(void) const {
    return (this->m_map);
  }
  ints_sizet_map & map(void) {
    return (this->m_map);
  }
};// class TreeResult<INDEXTYPE>
/////////////////////////////////////////
template<typename INDEXTYPE,typename DATATYPE>
class MatTree {
public:
    using index_type  = INDEXTYPE;
    using data_type = DATATYPE;
    //
    using IndivType = Indiv<index_type,data_type>;
    using string_type = typename IndivType::string_type;
    using TreeItemType = TreeItem<index_type,data_type>;
    using PTreeItemType = TreeItemType *;
    using ints_sizet_map = typename TreeItemType::ints_sizet_map;
    using ints_vector = typename TreeItemType::ints_vector;
    using treeitem_ptr = typename TreeItemType::treeitem_ptr;
    using treeitems_vector = QVector<treeitem_ptr>;
    //
    using MatTreeType = MatTree<INDEXTYPE,DATATYPE>;
    using pair_type = std::pair<int, int>;
    using cancellableflag_type = std::atomic<bool>;
    using TreeResultType = TreeResult<index_type>;
    using TreeResultTypePtr = std::shared_ptr<TreeResultType>;
private:
    cancellableflag_type *m_pcancel;
    treeitems_vector m_items;
    TreeResultType m_result;
    //
    MatTree(const MatTreeType &) = delete;
    MatTreeType & operator=(const MatTreeType &) = delete;
public:
    MatTree(cancellableflag_type *pf = nullptr):m_pcancel(pf){}
    template <typename XU,typename XF, typename XDIST>
    MatTree(const MatriceData<XU,XF,XDIST> *pData,
            MatDispositionType disp = MatDispositionType::dispRow,
            cancellableflag_type *pf = nullptr):m_pcancel(pf){
        this->init(pData,disp);
    }// MatTree
    virtual ~MatTree(){
        this->clear();
    }
public:
    void clear(void){
        this->m_items.clear();
        this->m_result.clear();
    }// clear
    template <typename XU,typename XF, typename XDIST>
    void init(const MatriceData<XU,XF,XDIST> *pData,
              MatDispositionType disp = MatDispositionType::dispRow){
        Q_ASSERT(pData != nullptr);
        Q_ASSERT(pData->is_valid());
        const int nRows = pData->rows();
        const int nCols = pData->cols();
        this->clear();
        treeitems_vector &v = this->m_items;
        if (disp == MatDispositionType::dispRow){
            for (int i = 0; i < nRows; ++i){
                index_type aIndex = (index_type)(i + 1);
                string_type sigle = pData->row_name(i);
                QVector<data_type> data(nCols);
                for (int j = 0; j < nCols; ++j){
                    data[j] = (data_type)pData->get_value(i,j);
                }// j
                treeitem_ptr oPtr = std::make_shared<TreeItemType>(aIndex,nCols,data,sigle);
                Q_ASSERT(oPtr.get() != nullptr);
                v.push_back(oPtr);
            }// i
        } else if (disp == MatDispositionType::dispCol){
            for (int i = 0; i < nCols; ++i){
                index_type aIndex = (index_type)(i + 1);
                string_type sigle = pData->col_name(i);
                QVector<data_type> data(nRows);
                for (int j = 0; j < nRows; ++j){
                    data[j] = (data_type)pData->get_value(j,i);
                }// j
                treeitem_ptr oPtr = std::make_shared<TreeItemType>(aIndex,nCols,data,sigle);
                Q_ASSERT(oPtr.get() != nullptr);
                v.push_back(oPtr);
            }// i
        }
    }// init
    template <typename XF>
    TreeResultTypePtr aggreg(XF r, int nClasses = 5){
        TreeResultTypePtr oRet;
        bool done = false;
        XF res = r;
        while (!done){
            if (!this->is_cancellation_requested()){
                return (oRet);
            }
            if (this->m_items.size() <= nClasses){
                done = true;
                break;
            }
            if (!this->aggreg_one_step(res,nClasses)){
                if (!this->is_cancellation_requested()){
                    return (oRet);
                }
                done = true;
                break;
            }
        }// done
        this->get_ids(this->m_result.ids());
        oRet = std::make_shared<TreeResultType>(this->m_result);
        return (oRet);
    }// aggreg
    template <typename XF>
    QFuture<TreeResultTypePtr> aggregAsync(XF res, int nClasses = 5){
        return QtConcurrent::run([this,res,nClasses]()->TreeResultTypePtr{
            return (this->aggreg(res,nClasses));
        });
    }// aggregAsync

protected:
    void get_ids(ints_vector &oVec)  {
            oVec.clear();
            const treeitems_vector &v = this->m_items;
            const int n = v.size();
            for (int i = 0; i < n; ++i){
              int val = i + 1 ;
              PTreeItemType p = (v[i]).get();
              Q_ASSERT(p != nullptr);
              p->get_ids(oVec);
            }// i
        } // get_ids
    void get_map(ints_sizet_map &oMap){
        oMap.clear();
        const treeitems_vector &v = this->m_items;
        const int n = v.size();
        for (int i = 0; i < n; ++i){
            int val = i + 1 ;
            PTreeItemType p = (v[i]).get();
            Q_ASSERT(p != nullptr);
            p->get_map(oMap, val);
        }// i
    } // get_map
    bool is_cancellation_requested(void){
        return ((this->m_pcancel != nullptr) && this->m_pcancel->load());
    }// is_cancellation_requested

    template <typename XF>
    bool find_best_aggreg(pair_type &oPair, XF &crit)  {
        const treeitems_vector &v = this->m_items;
        oPair.first = 0;
        oPair.second = 0;
        const int n = v.size();
        for (int i = 0; i < n; ++i){
            if (this->is_cancellation_requested()){
                return (false);
            }
            PTreeItemType p1 = (v[i]).get();
            Q_ASSERT(p1 != nullptr);
            for (int j = 0; j < i; ++j){
                if (this->is_cancellation_requested()){
                    return (false);
                }
                PTreeItemType p2 = (v[j]).get();
                Q_ASSERT(p2 != nullptr);
                XF s = 0;
                p1->distance(*p2, s);
                if (oPair.first == oPair.second){
                    oPair.first = j;
                    oPair.second = i;
                    crit = s;
                } else if (s < crit){
                    oPair.first = j;
                    oPair.second = i;
                    crit = s;
                }
            }// j
        }// i
        if (this->is_cancellation_requested()){
            return (false);
        }
        return (oPair.first != oPair.second);
    }//
    template <typename XF>
    bool aggreg_one_step(XF &crit, int nClasses){
        treeitems_vector &v = this->m_items;
        size_t n = v.size();
        if (n < 2){
            return (false);
        }
        if (n == nClasses){
            this->get_map(this->m_result.map());
        }
        pair_type oPair;
        if (!this->find_best_aggreg(oPair)){
            return (false);
        }
        int i1 = oPair.first;
        int i2 = oPair.second;
        Q_ASSERT(i1 != i2);
        Q_ASSERT(i1 < n);
        Q_ASSERT(i2 < n);
        treeitem_ptr left = v[i1];
        PTreeItemType pLeft = left.get();
        Q_ASSERT(pLeft != nullptr);
        treeitem_ptr right = v[i2];
        PTreeItemType pRight = right.get();
        Q_ASSERT(pRight != nullptr);
        auto it1 = std::find_if(v.begin(),v.end(),[pLeft](treeitem_ptr o)->bool{
                return (o.get() == pLeft);
    });
        Q_ASSERT(it1 != v.end());
        v.erase(it1);
        auto it2 = std::find_if(v.begin(),v.end(),[pRight](treeitem_ptr o)->bool{
                return (o.get() == pRight);
    });
        Q_ASSERT(it2 != v.end());
        v.erase(it2);
        treeitem_ptr oPtr = std::make_shared<TreeItemType>(left,right);
        Q_ASSERT(oPtr.get() != nullptr);
        v.push_back(oPtr);
        return (true);
    }// aggreg_one_step
};// class MatTree
/////////////////////////////////////////
}// namespace info
//////////////////
#endif // TREEELEM_H

