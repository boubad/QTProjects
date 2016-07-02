#ifndef QARRANGEMANAGER_H
#define QARRANGEMANAGER_H
///////////////////////////////
#include "treeelem.h"
#include "matelem.h"
////////////////////////////////
#include <QObject>
////////////////////////////////
namespace info {
class QArrangeManager : public QObject
{
    Q_OBJECT
public:
    using index_type = int;
    using data_type = float;
    using float_type = float;
    using distance_type = short;
    using criteria_type = long;
    using cancellableflag_type = std::atomic<bool>;
    //
    using matricedata_type = MatriceData<index_type,float_type,distance_type>;
    //
    using tree_type = MatTree<index_type,data_type>;
    using treeresult_type = typename tree_type::TreeResultType;
    using treeresult_type_ptr = typename tree_type::TreeResultTypePtr;
    //
    using matelem_type = MatElem<index_type,distance_type,criteria_type>;
    using matelemresult_type = typename matelem_type::matelem_result_type;
private:
    bool process_cols(void);
    bool process_rows(void);
    void notify_cols(matelemresult_type r);
    void notify_rows(matelemresult_type r);
    void notify_finished(void);
    void notify_started(void);
    void signal_data(void);
    void signal_abort(void);
public:
    explicit QArrangeManager(QObject *parent = 0);
    virtual ~QArrangeManager();
    QFuture<bool> setData(const QString &name,int nRows, int nCols,
                          const QVector<data_type> &pxdata,
                          const QStringList *pRowNames = nullptr,
                          const QStringList *pColNames = nullptr);
signals:
    void started(void);
    void rows_changed(matelemresult_type r);
    void cols_changed(matelemresult_type r);
    void cols_ended(void);
    void rows_ended(void);
    void cols_started(void);
    void rows_started(void);
    void finished(void);
    void data_available(void);
    void aborted(void);
public slots:
    void cancel(void);
    void reset_cancel(void);
    bool is_cancelled(void);
public:
    const matricedata_type *matrice_data(void) const{
        return (this->m_data.get());
    }
    const matelemresult_type & cols_result(void) const {
        return (this->m_colres);
    }
    const matelemresult_type & rows_result(void) const {
        return (this->m_rowres);
    }
private:
    cancellableflag_type m_cancel;
    cancellableflag_type m_colbusy;
    cancellableflag_type m_rowbusy;
    cancellableflag_type m_rowdone;
    cancellableflag_type m_coldone;
    //
    std::unique_ptr<matricedata_type> m_data;
    std::unique_ptr<matelem_type> m_colmat;
    std::unique_ptr<matelem_type> m_rowmat;
    std::unique_ptr<tree_type> m_coltree;
    std::unique_ptr<tree_type> m_rowtree;
    matelemresult_type m_colres;
    matelemresult_type m_rowres;
};
///////////////////////////
}// namespace info
#endif // QARRANGEMANAGER_H
