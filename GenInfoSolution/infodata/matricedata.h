#ifndef QMATRICEDATA_H
#define QMATRICEDATA_H
///////////////////////////
#include "distancemap.h"
///////////////////////////////
#include <QFuture>
#include <QtConcurrent>
#include <QString>
#include <QStringList>
#include <memory>
/////////////////////////////
namespace info {
///////////////////////////
/// \brief The MatriceData class
template <typename INDEXTYPE, typename FLOATTYPE, typename DISTANCETYPE>
class MatriceData
{
public:
    using index_type = INDEXTYPE;
    using float_type = FLOATTYPE;
    using distance_type = DISTANCETYPE;
    //
    using distancemap_type = DistanceMap<index_type,distance_type>;
    using matricedata_type = MatriceData<index_type,float_type,distance_type>;
private:
    int m_nrows;
    int m_ncols;
    distancemap_type *m_prowdist;
    distancemap_type *m_pcoldist;
    QString m_name;
    QStringList m_rownames;
    QStringList m_colnames;
    QVector<float_type> m_data;
private:
    MatriceData(const matricedata_type &) = delete;
    matricedata_type & operator=(const matricedata_type &) = delete;
public:
    MatriceData():m_nrows(0),m_ncols(0),m_prowdist(nullptr),m_pcoldist(nullptr){}
    template <typename X>
    MatriceData(const size_t nRows, const size_t nCols, const X &data,
                const QString &sname = QString(),
                const QStringList *pRowNames = nullptr,
                const QStringList *pColNames = nullptr):
        m_nrows(0),m_ncols(0),m_prowdist(nullptr),m_pcoldist(nullptr){
        this->setData(nRows,nCols,data,sname,pRowNames,pColNames);
    }
    virtual ~MatriceData(){
        delete m_prowdist;
        delete m_pcoldist;
    }
public:
    template <typename X>
    bool setData(const size_t nRows, const size_t nCols, const X &data,
                 const QString &sname = QString(),
                 const QStringList *pRowNames = nullptr,
                 const QStringList *pColNames = nullptr){
        this->clear();
        this->m_name = sname;
        return (this->set_data(nRows,nCols,data,pRowNames,pColNames));
    }// set_data
    template <typename X>
    QFuture<bool> setDataAsync(const size_t nRows, const size_t nCols, const X &data,
                               const QString &sname = QString(),
                               const QStringList *pRowNames = nullptr,
                               const QStringList *pColNames = nullptr){
        return QtConcurrent::run([this,nRows,nCols,data,sname,pRowNames,pColNames]()->bool{
            return this->setData(nRows,nCols,data,sname,pRowNames,pColNames);
        });
    }// setDataAsync
    void clear(void){
        m_nrows = 0;
        m_ncols = 0;
        m_name.clear();
        m_rownames.clear();
        m_colnames.clear();
        m_data.clear();
        delete m_prowdist;
        m_prowdist = nullptr;
        delete m_pcoldist;
        m_pcoldist = nullptr;
    }// clear
    bool is_valid(void) const {
       const int nc = this->m_ncols;
       const int nr = this->m_nrows;
       return ((nc > 0) && (nr > 0) && (m_rownames.size() >= nr) &&
               (m_colnames.size() >= nc) && (m_data.size() >= (nr * nc)) &&
               (m_prowdist != nullptr) && (m_pcoldist != nullptr));
    }// is_valis
    int rows(void) const {
        return (this->m_nrows);
    }
    int cols(void) const {
        return (this->m_ncols);
    }
    double get_data(const int irow, const int icol) const {
        Q_ASSERT(this->is_valid());
        Q_ASSERT(irow < this->m_nrows);
        Q_ASSERT(icol < this->m_ncols);
        return (m_data[irow *m_ncols + icol]);
    }// get_data
    QString get_row_name(const int irow) const {
         Q_ASSERT(this->is_valid());
        Q_ASSERT(irow < this->m_rownames.size());
        return (this->m_rownames.at(irow));
    }
    QString get_col_name(const int icol) const {
         Q_ASSERT(this->is_valid());
        Q_ASSERT(icol < this->m_colnames.size());
        return (this->m_colnames[icol]);
    }
    const distancemap_type *get_rows_distancesmap(void) const {
        return (this->m_prowdist);
    }
    const distancemap_type *get_cols_distancesmap(void) const {
        return (this->m_pcoldist);
    }
protected:
    bool set_names(const QStringList *pRownames = nullptr,
                   const QStringList *pColNames = nullptr){
        const int nRows = this->m_nrows;
        const int nCols = this->m_ncols;
        QStringList &rownames = this->m_rownames;
        rownames.clear();
        if ((pRownames != nullptr) && (pRownames->size() >= nRows)){
            for (int i = 0; i < nRows; ++i){
                rownames.append(pRownames->at(i));
            }// i
        } else {
            for (int i = 0; i < nRows; ++i){
                QString s = QString("ind%1").arg(i+1);
                rownames.append(s);
            }// i
        }
        QStringList &colnames = this->m_colnames;
        colnames.clear();
        if ((pColNames != nullptr) && (pColNames->size() >= nCols)){
            for (int i = 0; i < nCols; ++i){
                colnames.append(pColNames->at(i));
            }// i
        } else {
            for (int i = 0; i < nCols; ++i){
                QString s = QString("var%1").arg(i+1);
                colnames.append(s);
            }// i
        }
        return (true);
    }// set_names
    bool compute_rowdist(void){
        std::unique_ptr<distancemap_type> o(new distancemap_type());
        distancemap_type *pMap = o.get();
        if (pMap == nullptr){
            return (false);
        }
        const int nRows = this->m_nrows;
        const int nCols = this->m_ncols;
        const QVector<float_type> &data = this->m_data;
        DistanceMap<int,double> oDist;
        for (int irow1 = 0; irow1 < nRows; ++irow1){
            for (int irow2 = 0; irow2 < irow1; ++irow2){
                double s = 0;
                for (int i = 0; i < nCols; ++i){
                    double x = data[irow1 * nCols + i] - data[irow2 * nCols + i];
                    s += x * x;
                }// i
                oDist.add(irow2,irow1,s);
            }// icol2
        }// icol1
        bool bRet = oDist.recode(*pMap);
        if (bRet){
            this->m_prowdist = o.release();
        }
        return (bRet);
    }// compute_rowdist;
    bool compute_coldist(void){
        std::unique_ptr<distancemap_type> o(new distancemap_type());
        distancemap_type *pMap = o.get();
        if (pMap == nullptr){
            return (false);
        }
        const int nRows = this->m_nrows;
        const int nCols = this->m_ncols;
        const QVector<float_type> &data = this->m_data;
        DistanceMap<int,double> oDist;
        for (int icol1 = 0; icol1 < nCols; ++icol1){
            for (int icol2 = 0; icol2 < icol1; ++icol2){
                double s = 0;
                for (int i = 0; i < nRows; ++i){
                    const int pos = i * nCols;
                    double x = data[pos + icol1] - data[pos + icol2];
                    s += x * x;
                }// i
                oDist.add(icol2,icol1,s);
            }// icol2
        }// icol1

        bool bRet = oDist.recode(*pMap);
        if (bRet){
            this->m_pcoldist = o.release();
        }
        return (bRet);
    }// compute_coldist;
    template <typename X>
    bool initialize(const int nRows, const int nCols, const X &data){
        if ((nRows < 1) || (nCols < 1)){
            return (false);
        }
        QVector<float_type> oData(nCols * nRows);
        for (int icol = 0; icol < nCols; ++icol){
            double vmin = 0, vmax = 0;
            for (int irow = 0; irow < nRows; ++irow){
                double x = (double)data[irow * nCols + icol];
                if (irow == 0){
                    vmin = x;
                    vmax = x;
                } else if (x < vmin){
                    vmin = x;
                } else if (x > vmax){
                    vmax = x;
                }
            }// irow
            if (vmin >= vmax){
                return (false);
            }
            const double delta = vmax - vmin;
            for (int irow = 0; irow < nRows; ++irow){
                int pos = irow * nCols + icol;
                oData[pos] = (data[pos] - vmin) / delta;
            }// irow
        }// icol
        if (this->m_prowdist != nullptr){
            delete this->m_prowdist;
            this->m_prowdist = nullptr;
        }
        if (this->m_pcoldist != nullptr){
            delete this->m_pcoldist;
            this->m_pcoldist = nullptr;
        }
        this->m_data = oData;
        this->m_nrows = nRows;
        this->m_ncols = nCols;
        return (this->compute_rowdist() && this->compute_coldist());
    }// initialize
    template <typename X>
    bool set_data(const size_t nRows, const size_t nCols, const X &data,
                  const QStringList *pRowNames = nullptr,
                  const QStringList *pColNames = nullptr){
        bool bRet = this->initialize(nRows,nCols,data);
        bRet = bRet && this->set_names(pRowNames,pColNames);
        return (bRet);
    }// set_data
};
}// namespace info
#endif // QMATRICEDATA_H
