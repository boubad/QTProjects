#include "qarrangemanager.h"
///////////////////////////
namespace info {
QArrangeManager::QArrangeManager(QObject *parent) : QObject(parent),
    m_cancel(false),m_colbusy(false),m_rowbusy(false),m_rowdone(false),m_coldone(false)
{

}
QArrangeManager::~QArrangeManager()
{

}
QFuture<bool> QArrangeManager::setData(const QString &name,int nRows, int nCols,
                                       const QVector<data_type> &pxdata,
                                       const QStringList *pRowNames /* = nullptr*/,
                                       const QStringList *pColNames /*= nullptr*/){
    return QtConcurrent::run([this,name,nRows,nCols,pxdata,pRowNames,pColNames]()->bool{
        if (this->m_colbusy.load() || this->m_rowbusy.load()){
            return (false);
        }
        this->reset_cancel();
        this->notify_started();
        matricedata_type *pData = this->m_data.get();
        if (pData == nullptr){
            this->m_data.reset(new matricedata_type());
            if ((pData = this->m_data.get()) == nullptr){
                this->signal_abort();
                this->notify_finished();
                return (false);
            }
        }
        if (!pData->setData(nRows,nCols,pxdata,name,pRowNames,pColNames)){
            this->signal_abort();
            this->notify_finished();
            return (false);
        }
        this->signal_data();
        QFuture<bool> b1 = QtConcurrent::run([this]()->bool{
                                                 return (this->process_cols());
                                             });
        QFuture<bool> b2 = QtConcurrent::run([this]()->bool{
                                                 return (this->process_rows());
                                             });
        bool bRet = b1.result() && b2.result();
        return (bRet);

    });
}// setData
void QArrangeManager::signal_abort(void){
    emit aborted();
}

void QArrangeManager::signal_data(void){
    emit data_available();
}// signal_data

void QArrangeManager::notify_finished(void){
    this->m_coldone.store(true);
    this->m_rowdone.store(true);
    emit finished();
}
void QArrangeManager::notify_started(void){
    this->m_coldone.store(false);
    this->m_rowdone.store(false);
    emit started();
}
void QArrangeManager::notify_cols(matelemresult_type r){
    emit cols_changed(r);
}

void QArrangeManager::notify_rows(matelemresult_type r){
    emit rows_changed(r);
}

bool QArrangeManager::process_cols(void){
    this->m_colbusy.store(true);
    matricedata_type *pData = this->m_data.get();
    this->m_coltree.reset(new tree_type(pData,MatDispositionType::dispCol,&(this->m_cancel)));
    tree_type *pColTree = this->m_coltree.get();
    if (pColTree == nullptr){
        this->m_colbusy.store(false);
        this->m_coldone.store(true);
        this->signal_abort();
        return (false);
    }
    this->m_colmat.reset(new matelem_type(pData,MatDispositionType::dispCol,&(this->m_cancel)));
    matelem_type *pMat = this->m_colmat.get();
    if (pMat == nullptr){
        this->m_colbusy.store(false);
        this->m_coldone.store(true);
        this->signal_abort();
        return (false);
    }
    pMat->setCallback([this](matelemresult_type r){
        this->notify_cols(r);
    });
    double s = 0;
    int nclasses = 1;
    treeresult_type_ptr r = pColTree->aggreg(s,nclasses);
    treeresult_type *t = r.get();
    if (t == nullptr){
        this->m_colbusy.store(false);
        this->m_coldone.store(true);
        this->signal_abort();
        return (false);
    }
    emit cols_started();
    pMat->indexes(t->ids());
    this->m_colres = pMat->arrange();
    this->m_colbusy.store(false);
    this->m_coldone.store(true);
    emit cols_ended();
    if (this->m_rowdone.load()){
        emit finished();
    }
    return (true);
}// process_cols
bool QArrangeManager::process_rows(void){
    this->m_rowbusy.store(true);
    matricedata_type *pData = this->m_data.get();
    this->m_rowtree.reset(new tree_type(pData,MatDispositionType::dispRow,&(this->m_cancel)));
    tree_type *pRowTree = this->m_rowtree.get();
    if (pRowTree == nullptr){
        this->m_rowbusy.store(false);
        this->m_rowdone.store(true);
        this->signal_abort();
        return (false);
    }
    this->m_rowmat.reset(new matelem_type(pData,MatDispositionType::dispRow,&(this->m_cancel)));
    matelem_type *pMat = this->m_rowmat.get();
    if (pMat == nullptr){
        this->m_rowbusy.store(false);
        this->m_rowdone.store(true);
        this->signal_abort();
        return (false);
    }
    pMat->setCallback([this](matelemresult_type r){
        this->notify_rows(r);
    });
    double s = 0;
    int nclasses = 1;
    treeresult_type_ptr r = pRowTree->aggreg(s,nclasses);
    treeresult_type *t = r.get();
    if (t == nullptr){
        this->m_rowbusy.store(false);
        this->m_rowdone.store(true);
        this->signal_abort();
        return (false);
    }
    emit rows_started();
    pMat->indexes(t->ids());
    this->m_rowres = pMat->arrange();
    this->m_rowbusy.store(false);
    emit rows_ended();
    this->m_rowdone.store(true);
    if (this->m_coldone.load()){
        emit finished();
    }
    return (true);
}// process_rows
void QArrangeManager::cancel(void){
    this->m_cancel.store(true);
}

void QArrangeManager::reset_cancel(void){
    this->m_cancel.store(false);
}

bool QArrangeManager::is_cancelled(void){
    return (this->m_cancel.load());
}


/////////////////////////////
}// namespace info
