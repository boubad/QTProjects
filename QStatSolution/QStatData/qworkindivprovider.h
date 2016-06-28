#ifndef QWORKINDIVPROVIDER_H
#define QWORKINDIVPROVIDER_H
///////////////////////////////////
#include "qindiv.h"
//////////////////////////////////////
#include <QObject>
#include <QMutex>
#include <QSet>
////////////////////////////////////////
namespace info {
//////////////////////////
class DBStatHelper;
////////////////////////////////
class QWorkIndivProvider :  public QObject, public ISerialIndivProvider,
        public IIndivProvider
{
    Q_OBJECT
private:
    bool     m_mustDelete;
    IIndivProvider *m_provider;
    int m_current;
    int m_nmax;
    QMap<IntType,DBStatVariable> m_allvars;
    QSet<IntType> m_filter;
    DBStatDataset m_oset;
    QMutex   _mutex;
public:
    explicit QWorkIndivProvider(IIndivProvider *pProvider,QObject *parent = 0);
    explicit QWorkIndivProvider(DBStatHelper *pMan,
                              const DBStatDataset &oSet,
                              QObject *parent = 0);
    explicit QWorkIndivProvider(DBStatHelper *pMan,
                              const QString &sigleDataset,
                              QObject *parent = 0);
    virtual ~QWorkIndivProvider();
public:
    virtual bool is_valid(void);
    virtual bool indivs_count(int &nCount);
    virtual bool all_indivs_ids(QVector<IntType> &oIds);
    virtual bool contains_id(const IntType aId);
    virtual bool find_indiv(const IntType aIndex,QIndiv &oInd,
                            const VariableMode mode = VariableMode::modeAll);
    virtual bool find_indiv_at(const int pos,QIndiv &oInd,
                                const VariableMode mode = VariableMode::modeAll);
    virtual bool distance(const IntType aIndex1, const IntType &Index2,
                          double &dRes,
                          const VariableMode mode = VariableMode::modeNumeric);
    virtual bool distance_at(const int pos1, const int pos2,
                          double &dRes,
                             const VariableMode mode = VariableMode::modeNumeric);
    virtual bool get_variables(QList<DBStatVariable> &oVars);
    virtual bool  find_next_indiv(const DBStatDataset &oSet,
                                  const int offset,QIndiv &oInd,
                                  const VariableMode mode = VariableMode::modeAll);
    virtual bool get_dataset(DBStatDataset &oSet);
    //
    virtual bool reset(void);
    virtual bool next(QIndiv &oInd,const VariableMode mode = VariableMode::modeAll);
signals:

public slots:
};
}// namespace info
#endif // QWORKINDIVPROVIDER_H
