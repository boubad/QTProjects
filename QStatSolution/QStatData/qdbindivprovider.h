#ifndef QDBINDIVPROVIDER_H
#define QDBINDIVPROVIDER_H

#include <QObject>
#include "qindiv.h"
#include "dbstathelper.h"
//////////////////////
namespace info {
class QDBIndivProvider : public QObject, public IIndivProvider
{
    Q_OBJECT
private:
    int     m_count;
    DBStatHelper *m_pman;
    DBStatDataset m_oset;
    QMap<IntType,DBStatVariable> m_vars;
    QVector<IntType> m_ids;
    //
    void initialize(void);
public:
    explicit QDBIndivProvider(DBStatHelper *pMan,
                              const DBStatDataset &oSet,
                              QObject *parent = 0);
    explicit QDBIndivProvider(DBStatHelper *pMan,
                              const QString &sigleDataset,
                              QObject *parent = 0);
    virtual ~QDBIndivProvider(){}
public:
    virtual bool is_valid(void);
    virtual bool get_variables(QList<DBStatVariable> &oVars);
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
    virtual bool  find_next_indiv(const DBStatDataset &oSet,
                                  const int offset,QIndiv &oInd,
                                  const VariableMode mode = VariableMode::modeAll);
    virtual bool get_dataset(DBStatDataset &oSet);
public slots:
    // overrides

};
}// namespace info
#endif // QDBINDIVPROVIDER_H
