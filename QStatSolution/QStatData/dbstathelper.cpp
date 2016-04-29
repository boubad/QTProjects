#include "dbstathelper.h"
/////////////////////////////////////
#include <QSqlError>
#include <QList>
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
/////////////////////////////////////
static const char *SQL_FIND_ALL_DATASETS =
        "SELECT datasetid,optlock,sigle,nom,description,status"
        " FROM dbdataset ORDER BY sigle";
static const char *SQL_FIND_DATASET_BY_ID =
        "SELECT datasetid,optlock,sigle,nom,description,status"
        " FROM dbdataset WHERE datasetid = :id";
static const char *SQL_FIND_DATASET_BY_SIGLE =
        "SELECT datasetid,optlock,sigle,nom,description,status"
        " FROM dbdataset WHERE UPPER(LTRIM(RTRIM(sigle))) = :sigle";
static const char *SQL_INSERT_DATASET =
        "INSERT INTO dbdataset (sigle,nom,description,status)"
        " VALUES (:sigle,:name,:desc,:status)";
static const char *SQL_UPDATE_DATASET =
        "UPDATE dbdataset SET optlock = optlock + 1,"
        " sigle = :sigle, nom = :name, description = :desc, status = :status WHERE datasetid = :id";
static const char *SQL_REMOVE_DATASET =
        "DELETE FROM dbdataset WHERE datasetid = ?1";
////////////////////////////////////
/// \brief DBStatHelper::find_dataset
/// \param cur
/// \return
bool DBStatHelper::maintains_dataset(DBStatDataset &cur){
    if (!cur.is_writeable()){
        return (false);
    }
    bool bInTrans = this->m_base.transaction();
    DBStatDataset xSet(cur);
    this->find_dataset(xSet);
    IntType nId = xSet.id();
    QString sigle = cur.sigle();
}// maintains_datase
bool DBStatHelper::find_dataset(DBStatDataset &cur){
    IntType nId = cur.id();
    if (nId != 0){
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_FIND_DATASET_BY_ID)){
            return (false);
        }
        q.bindValue(":id",nId);
        if (!q.exec()){
            return (false);
        }
        if (q.next()){
            this->read_dataset(q,cur);
            return (true);
        }
    }
    QString sigle = cur.sigle();
    if ((!sigle.isEmpty()) && (!sigle.isNull())) {
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_FIND_DATASET_BY_SIGLE)){
            return (false);
        }
        q.bindValue(":sigle",sigle);
        if (!q.exec()){
            return (false);
        }
        if (q.next()){
            this->read_dataset(q,cur);
            return (true);
        }
    }
    return (false);
}// find_dataset
bool  DBStatHelper::find_all_datasets(QList<DBStatDataset> &oList){
    oList.clear();
    QSqlQuery q(SQL_FIND_ALL_DATASETS,this->m_base);
    while (q.next()){
        DBStatDataset cur;
        this->read_dataset(q,cur);
        oList.append(cur);
    }
    return (true);
}//find_all_datasets
////////////////////////////////////
void DBStatHelper::read_dataset(QSqlQuery &q, DBStatDataset &cur){
    IntType nId = q.value(0).toInt();
    IntType nVersion = q.value(1).toInt();
    QString sSigle = q.value(2).toString();
    QString sName = q.value(3).toString();
    QString sDesc = q.value(4).toString();
    QString status = q.value(5).toString();
    cur = DBStatDataset(nId,nVersion,status,sSigle,sName,sDesc);
}

void DBStatHelper::read_variable(QSqlQuery &q, DBStatVariable &cur){
    IntType nId = q.value(0).toInt();
    IntType nVersion = q.value(1).toInt();
    IntType nDatasetId = q.value(2).toInt();
    QString sSigle = q.value(3).toString();
    QString sType = q.value(4).toString();
    int nCateg = q.value(5).toInt();
    QString sName = q.value(6).toString();
    QString sDesc = q.value(7).toString();
    QString sGenre = q.value(8).toString();
    QString status = q.value(9).toString();
    bool bCateg = (nCateg != 0)? true : false;
    cur = DBStatVariable(nId,nVersion,status,sSigle,sName,sDesc,nDatasetId,
                         bCateg,sType,sGenre);
}

void DBStatHelper::read_indiv(QSqlQuery &q, DBStatIndiv &cur){
    IntType nId = q.value(0).toInt();
    IntType nVersion = q.value(1).toInt();
    IntType nDatasetId = q.value(2).toInt();
    QString sSigle = q.value(3).toString();
    QString sName = q.value(4).toString();
    QString sDesc = q.value(5).toString();
    QString status = q.value(6).toString();
    cur = DBStatIndiv(nId,nVersion,status,sSigle,sName,sDesc,nDatasetId);
}

void DBStatHelper::read_value(QSqlQuery &q, DBStatValue &cur){
    IntType nId = q.value(0).toInt();
    IntType nVersion = q.value(1).toInt();
    IntType nVarId = q.value(2).toInt();
    IntType nIndId = q.value(3).toInt();
    QVariant v = q.value(4);
    QString status = q.value(5).toString();
    cur = DBStatValue(nId,nVersion,status,nVarId,nIndId,v);
}
////////////////////////////////
DBStatHelper::DBStatHelper(const QString &baseType /*= DEFAULT_DATABASE_TYPE*/,
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
