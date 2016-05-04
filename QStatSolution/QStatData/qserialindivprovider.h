#ifndef QSERIALINDIVPROVIDER_H
#define QSERIALINDIVPROVIDER_H

#include "qindiv.h"
///////////////////////////////////////
#include <QObject>
#include <QMutex>
////////////////////////////////
namespace info {
class DBStatHelper;
////////////////////////////////////////
class QSerialIndivProvider : public QObject, public ISerialIndivProvider,
        public IIndivProvider
{
    Q_OBJECT
private:
    bool m_mustDelete;
    IIndivProvider *m_provider;
    int m_current;
    int m_nmax;
    DBStatDataset m_oset;
    QMutex   _mutex;
public:
    explicit QSerialIndivProvider(IIndivProvider *pProvider,QObject *parent = 0);
    explicit QSerialIndivProvider(DBStatHelper *pMan,
                              const DBStatDataset &oSet,
                              QObject *parent = 0);
    explicit QSerialIndivProvider(DBStatHelper *pMan,
                              const QString &sigleDataset,
                              QObject *parent = 0);
    virtual ~QSerialIndivProvider();
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
#endif // QSERIALINDIVPROVIDER_H
