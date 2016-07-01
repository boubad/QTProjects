#ifndef INDIV_H
#define INDIV_H
////////////////////////////////////
#include <QString>
#include <memory>
/////////////////////////////////////
namespace info {
///////////////////////////////
template <typename U, typename DATATYPE>
class Indiv {
public:
    using string_type = QString;
    using index_type = U;
    using data_type = DATATYPE;
    //
    using IndivType = Indiv<index_type,data_type>;
protected:
    index_type m_index;
    int m_ncols;
    string_type m_sigle;
    std::unique_ptr<data_type> m_center;
    //
    Indiv():m_index(0),m_ncols(0){}
private:
    Indiv(const IndivType &) = delete;
    IndivType & operator=(const IndivType &) = delete;
public:
    template <typename X>
    Indiv(index_type aIndex,int nCols, const X &pxdata,const string_type &sSigle = string_type()):
        m_index(aIndex),m_ncols(nCols),m_sigle(sSigle){
        Q_ASSERT(nCols > 0);
        this->m_center.reset(new data_type[nCols]);
        data_type *p = this->m_center.get();
        Q_ASSERT(p != nullptr);
        for (int i = 0; i < nCols; ++i){
            p[i] = (data_type)pxdata[i];
        }// i
    }// Indiv
    virtual ~Indiv(){
    }
    index_type id(void) const {
        return (this->m_index);
    }
    int size(void) const {
        assert(this->m_ncols > 0);
        return (this->m_ncols);
    }
    string_type sigle(void) const {
        return (this->m_sigle);
    }
    const data_type *center(void) const {
        Q_ASSERT(this->m_center.get() != nullptr);
        return (this->m_center.get());
    }
    template <typename F>
    void distance(const IndivType &other, F &res) const {
        const int n = this->size();
        Q_ASSERT(n > 0);
        Q_ASSERT(n == other.size());
        const data_type *p1 = this->center();
        const data_type *p2 = other.center();
        res = 0;
        for (int i = 0; i < n; ++i){
            F x = (F)(p1[i] - p2[i]);
            res = (F)(res + x * x);
        }// i
    }// distance
};// class Indiv<U,DATATYPE>
//////////////////////////////////
}// namespace info
////////////////////////////////////
#endif // INDIV_H

