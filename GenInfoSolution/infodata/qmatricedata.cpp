#include "qmatricedata.h"
namespace info {
QMatriceData::QMatriceData(QObject *parent) : QObject(parent), m_nrows(0),m_ncols(0),
    m_prowdist(nullptr),m_pcoldist(nullptr)
{

}
QMatriceData::~QMatriceData()
{
    delete m_prowdist;
    m_prowdist = nullptr;
    delete m_pcoldist;
    m_pcoldist = nullptr;
}
bool QMatriceData::compute_coldist(void){
    if (this->m_pcoldist != nullptr){
        return (true);
    }
    const int nRows = this->m_nrows;
    const int nCols = this->m_ncols;
    const double *pData = this->m_data.get();
    if ((nRows < 1) || (nCols < 1) || (pData == nullptr)){
        return (false);
    }
    QDistanceMap *pMap = new QDistanceMap();
    if (pMap == nullptr){
        return (false);
    }
    for (int icol1 = 0; icol1 < nCols; ++icol1){
        for (int icol2 = 0; icol2 < icol1; ++icol2){
            double s = 0;
            for (int i = 0; i < nRows; ++i){
                const int pos = i * nCols;
                double x = pData[pos + icol1] - pData[pos + icol2];
                s += x * x;
            }// i
            s /= nRows;
            pMap->add(icol2,icol1,s);
        }// icol2
    }// icol1
    this->m_pcoldist = pMap;
    return (true);
}// compute_coldist
bool QMatriceData::compute_rowdist(void){
    if (this->m_prowdist != nullptr){
        return (true);
    }
    const int nRows = this->m_nrows;
    const int nCols = this->m_ncols;
    const double *pData = this->m_data.get();
    if ((nRows < 1) || (nCols < 1) || (pData == nullptr)){
        return (false);
    }
    QDistanceMap *pMap = new QDistanceMap();
    if (pMap == nullptr){
        return (false);
    }
    for (int irow1 = 0; irow1 < nRows; ++irow1){
        const double *p1 = pData + irow1 * nCols;
        for (int irow2 = 0; irow2 < irow1; ++irow2){
            const double *p2 = pData + irow2 * nCols;
            double s = 0;
            for (int i = 0; i < nCols; ++i){
                double x = p1[i] - p2[i];
                s += x * x;
            }// i
            s /= nCols;
            pMap->add(irow2,irow1,s);
        }// irow2
    }// irow1
    this->m_prowdist = pMap;
    return (true);
}// compute_rowdist

bool QMatriceData::set_names(const QStringList *pRownames /*= nullptr*/,
               const QStringList *pColnames/* = nullptr */){
    const int nRows = this->m_nrows;
    const int nCols = this->m_ncols;
    if ((nRows < 1) || (nCols < 1)){
        return (false);
    }
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
    if ((pColnames != nullptr) && (pColnames->size() >= nCols)){
        for (int i = 0; i < nCols; ++i){
            colnames.append(pColnames->at(i));
        }// i
    } else {
        for (int i = 0; i < nCols; ++i){
            QString s = QString("var%1").arg(i+1);
            colnames.append(s);
        }// i
    }
    return (true);
}// set_names

////////////////////////////////////////////
}// namespace info
