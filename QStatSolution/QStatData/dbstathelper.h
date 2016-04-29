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
    //
    static const QString DEFAULT_DATABASE_TYPE;
    static const QString DEFAULT_CONNECTION_NAME;
    static const QString DEFAULT_DATABASE_NAME;
    //
    void read_dataset(QSqlQuery &q, DBStatDataset &cur);
    void read_variable(QSqlQuery &q, DBStatVariable &cur);
    void read_indiv(QSqlQuery &q, DBStatIndiv &cur);
    void read_value(QSqlQuery &q, DBStatValue &cur);
public:
    explicit DBStatHelper(const QString &baseType = DEFAULT_DATABASE_TYPE,
                          const QString &sDatabaseName = DEFAULT_DATABASE_NAME,
                          const QString &ConnectionName  = DEFAULT_CONNECTION_NAME,
                          QObject *parent = 0);
    explicit DBStatHelper(QSqlDatabase &oBase,QObject *parent = 0);

signals:

public slots:
    bool find_all_datasets(QList<DBStatDataset> &oList);
    bool find_dataset(DBStatDataset &cur);
    bool maintains_dataset(DBStatDataset &cur);
    bool remove_dataset(const DBStatDataset &cur);
};
}// namespace info

#endif // DBSTATHELPER_H
