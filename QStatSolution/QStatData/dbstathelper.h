#ifndef DBSTATHELPER_H
#define DBSTATHELPER_H

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QList>
////////////////////////////////
#include "qstatdata.h"
//
namespace info {
class DBStatHelper : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase m_base;
    //
    void check_schema(void);
    void read_dataset(QSqlQuery &q, DBStatDataset &cur);
    void read_variable(QSqlQuery &q, DBStatVariable &cur);
    void read_indiv(QSqlQuery &q, DBStatIndiv &cur);
    void read_value(QSqlQuery &q, DBStatValue &cur);
public:
    static const QString DEFAULT_DATABASE_TYPE;
    static const QString DEFAULT_CONNECTION_NAME;
    static const QString DEFAULT_DATABASE_NAME;
public:
    explicit DBStatHelper(QObject *parent = 0,
                          const QString &sDatabaseName = DEFAULT_DATABASE_NAME,
                          const QString &baseType = DEFAULT_DATABASE_TYPE,
                          const QString &ConnectionName  = DEFAULT_CONNECTION_NAME
                          );
    explicit DBStatHelper(QSqlDatabase &oBase,QObject *parent = 0);

signals:

public slots:
    bool isValid(void){
        return (this->m_base.isValid()) && (this->m_base.isOpen());
    }

    bool find_all_datasets_count(int &nCount);
    bool find_all_datasets(QList<DBStatDataset> &oList, int skip = 0,int count = 100);
    bool find_all_datasets_ids(QList<IntType> &oList, int skip = 0,int count = 100);
    bool find_dataset(DBStatDataset &cur);
    bool maintains_dataset(DBStatDataset &cur);
    bool remove_dataset(const DBStatDataset &cur);
    //
    bool find_dataset_variables_count(const DBStatDataset &oSet, int &nCount);
    bool find_dataset_variables_ids(const DBStatDataset &oSet, QList<IntType> &oList,
                                int skip = 0, int count = 100);
    bool find_dataset_variables(const DBStatDataset &oSet,
                                QList<DBStatVariable> &oList,
                                int skip = 0, int count = 100);
    bool find_variable(DBStatVariable &cur);
    bool maintains_variables(const QList<DBStatVariable> &oVars,bool bCommit = true,
                             bool bRemove = false);
    //
    bool find_dataset_indivs_count(const DBStatDataset &oSet, int &nCount);
    bool find_dataset_indivs_ids(const DBStatDataset &oSet, QList<IntType> &oList,
                                int skip = 0, int count = 100);
    bool find_dataset_indivs(const DBStatDataset &oSet,
                                QList<DBStatIndiv> &oList,
                                int skip = 0, int count = 100);
    bool find_indiv(DBStatIndiv &cur);
    bool maintains_indivs(const QList<DBStatIndiv> &oInds,bool bCommit = true,
                             bool bRemove = false);
    //
    bool find_value(DBStatValue &cur);
    bool maintains_values(const QList<DBStatValue> &oVals,bool bCommit = true,
                          bool bRemove = false);
    bool find_dataset_values_count(const DBStatDataset &oSet, int &nCount);
    bool find_dataset_values(const DBStatDataset &oSet, QList<DBStatValue> &oList,
                             int skip = 0, int count = 100);
    bool find_variable_values(DBStatVariable &oVar,QList<DBStatValue> &oList,
                             int skip = 0, int count = 100);
    bool find_variable_distinct_values(DBStatVariable &oVar,QList<DBStatValue> &oList,
                             int skip = 0, int count = 100);
    bool find_indiv_values(DBStatIndiv &oInd, QList<DBStatValue> &oList,
                             int skip = 0, int count = 100);


};
}// namespace info

#endif // DBSTATHELPER_H
