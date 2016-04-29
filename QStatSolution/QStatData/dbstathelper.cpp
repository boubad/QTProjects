#include "dbstathelper.h"
/////////////////////////////////////
#include <QSqlQuery>
#include <QSqlError>
///////////////////////////////////
namespace info {
///////////////////////////
const QString DBStatHelper::DEFAULT_DATABASE_TYPE("QSQLITE");
const QString DBStatHelper::DEFAULT_CONNECTION_NAME("InfoConnectName");
const QString DBStatHelper::DEFAULT_DATABASE_NAME("info_sets.sqlite");
/////////////////////////////////////
static const char *SQL_CREATE_DATASET =
        "CREATE TABLE IF NOT EXISTS dbdataset("
        " datasetid INTEGER PRIMARY KEY AUTOINCREMENT,"
        " optlock INTEGER NOT NULL DEFAULT 1,"
        " sigle TEXT NOT NULL UNIQUE,"
        " nom TEXT DEFAULT NULL,"
        " description TEXT DEFAULT NULL,"
        " status TEXT DEFAULT NULL"
        " )";
static const char *SQL_CREATE_VARIABLE =
        "CREATE TABLE IF NOT EXISTS dbvariable("
        " variableid INTEGER PRIMARY KEY AUTOINCREMENT,"
        " optlock INTEGER NOT NULL DEFAULT 1,"
        " datasetid INTEGER NOT NULL,"
        " sigle TEXT NOT NULL,"
        " vartype TEXT NOT NULL,"
        " categvar INTEGER NOT NULL DEFAULT 0,"
        " nom TEXT DEFAULT NULL,"
        " description TEXT DEFAULT NULL,"
        " genre TEXT DEFAULT NULL,"
        " status TEXT DEFAULT NULL,"
        " CONSTRAINT uc_variable UNIQUE (datasetid, sigle),"
        " CONSTRAINT fk_variable_dataset FOREIGN KEY (datasetid) REFERENCES dbdataset (datasetid) ON DELETE CASCADE"
        " )";
static const char *SQL_CREATE_INDIV =
        "CREATE TABLE IF NOT EXISTS dbindiv("
        " individ INTEGER PRIMARY KEY AUTOINCREMENT,"
        " optlock INTEGER NOT NULL DEFAULT 1,"
        " datasetid INTEGER NOT NULL,"
        " sigle TEXT NOT NULL,"
        " nom TEXT DEFAULT NULL,"
        " description TEXT DEFAULT NULL,"
        " status TEXT DEFAULT NULL,"
        " CONSTRAINT uc_indiv UNIQUE (datasetid, sigle),"
        " CONSTRAINT fk_indiv_dataset FOREIGN KEY (datasetid) REFERENCES dbdataset (datasetid) ON DELETE CASCADE"
        " )";
static const char *SQL_CREATE_VALUE =
        "CREATE TABLE IF NOT EXISTS dbvalue("
        " valueid INTEGER PRIMARY KEY AUTOINCREMENT,"
        " optlock INTEGER NOT NULL DEFAULT 1,"
        " variableid INTEGER NOT NULL,"
        " individ INTEGER NOT NULL,"
        " stringval TEXT NULL,"
        " status TEXT DEFAULT NULL,"
        " CONSTRAINT uc_value UNIQUE (variableid, individ),"
        " CONSTRAINT fk_value_variable FOREIGN KEY (variableid) REFERENCES dbvariable (variableid) ON DELETE CASCADE,"
        " CONSTRAINT fk_value_indiv FOREIGN KEY (individ) REFERENCES dbindiv (individ) ON DELETE CASCADE"
        " )";
static const char *CREATE_SQL[] = {
        SQL_CREATE_DATASET,
        SQL_CREATE_VARIABLE,
        SQL_CREATE_INDIV,
        SQL_CREATE_VALUE,
        nullptr };
////////////////////////////////
DBStatHelper:: DBStatHelper(const QString &baseType /*= DEFAULT_DATABASE_TYPE*/,
                            const QString &sDatabaseName /* = DEFAULT_DATABASE_NAME*/,
                            const QString &ConnectionName  /*= DEFAULT_CONNECTION_NAME*/,
                            QObject *parent /*= 0*/) : QObject(parent)
{
    this->m_base = QSqlDatabase::addDatabase(baseType,ConnectionName);
    Q_ASSERT(this->m_base.isValid());
    this->m_base.setDatabaseName(sDatabaseName);
    this->m_base.open();
    Q_ASSERT(this->m_base.isOpen());
    this->check_schema();
}
DBStatHelper::DBStatHelper(QSqlDatabase &oBase,QObject *parent /* = 0/*/):QObject(parent),
    m_base(oBase){
    Q_ASSERT(this->m_base.isOpen());
     this->check_schema();
}
void DBStatHelper::check_schema(void){
    QString sDriverName = this->m_base.driverName();
    if (sDriverName != DEFAULT_DATABASE_TYPE ){
        return;
    }
    int i = 0;
    while (CREATE_SQL[i] != nullptr){
        const char *pszSQL = CREATE_SQL[i];
        QString sql(pszSQL);
        QSqlQuery q = this->m_base.exec(sql);
        QSqlError err = q.lastError();
        Q_ASSERT(err.type() == QSqlError::NoError);
        i++;
    }// i
}// check_schema

///////////////////////////////////////
}// namespace info
