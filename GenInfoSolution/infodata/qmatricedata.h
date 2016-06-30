#ifndef QMATRICEDATA_H
#define QMATRICEDATA_H
///////////////////////////
#include "qdistancemap.h"
///////////////////////////////
#include <QFuture>
#include <QtConcurrent>
#include <QString>
#include <QStringList>
#include <memory>
/////////////////////////////
namespace info {
///////////////////////////
class QMatriceData : public QObject
{
    Q_OBJECT
public:
public:
    explicit QMatriceData(QObject *parent = 0);
    virtual ~QMatriceData();
signals:

public slots:
public:
    template <typename X>
    bool set_data(const size_t nRows, const size_t nCols, const X &data,
                  const QStringList *pRowNames = nullptr,
                  const QStringList *pColNames = nullptr){
        bool bRet = this->initialize(nRows,nCols,data);
        bRet = bRet && this->compute_rowdist();
        bRet = bRet && this->compute_coldist();
        bRet = bRet && this->set_names(pRowNames,pColNames);
        return (bRet);
    }// set_data
    template <typename X>
    QFuture<bool> setDataAsync(const size_t nRows, const size_t nCols, const X &data,
                               const QStringList *pRowNames = nullptr,
                               const QStringList *pColNames = nullptr){
        return QtConcurrent::run([this,nRows,nCols,data,pRowNames,pColNames]()->bool{
            return this->set_data(nRows,nCols,data,pRowNames,pColNames);
        });
    }// setDataAsync

    int rows(void) const {
        return (this->m_nrows);
    }
    int cols(void) const {
        return (this->m_ncols);
    }
    double get_data(const int irow, const int icol) const {
        Q_ASSERT(irow < this->m_nrows);
        Q_ASSERT(icol < this->m_ncols);
        const double *pData = this->m_data.get();
        Q_ASSERT(pData != nullptr);
        return (pData[irow * this->m_ncols + icol]);
    }// get_data
    QString get_row_name(const int irow) const {
        Q_ASSERT(irow < this->m_rownames.size());
        return (this->m_rownames.at(irow));
    }
    QString get_col_name(const int icol) const {
        Q_ASSERT(icol < this->m_colnames.size());
        return (this->m_colnames[icol]);
    }
    const QDistanceMap *get_rows_distancesmap(void) const {
        return (this->m_prowdist);
    }
    const QDistanceMap *get_cols_distancesmap(void) const {
        return (this->m_pcoldist);
    }
protected:
    bool compute_rowdist(void);
    bool compute_coldist(void);
    bool set_names(const QStringList *pRownames = nullptr,
                   const QStringList *pColNames = nullptr);
    template <typename X>
    bool initialize(const int nRows, const int nCols, const X &data){
        if ((nRows < 1) || (nCols < 1)){
            return (false);
        }
        std::unique_ptr<double> oData(new double[nCols * nRows]);
        double *pData = oData.get();
        if (pData == nullptr){
            return (false);
        }
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
                pData[pos] = (data[pos] - vmin) / delta;
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
        this->m_data.reset(oData.release());
        this->m_nrows = nRows;
        this->m_ncols = nCols;
        return (true);
    }// initialize
private:
    int m_nrows;
    int m_ncols;
    QDistanceMap *m_prowdist;
    QDistanceMap *m_pcoldist;
    std::unique_ptr<double> m_data;
    QStringList m_rownames;
    QStringList m_colnames;
};
}// namespace info
#endif // QMATRICEDATA_H
