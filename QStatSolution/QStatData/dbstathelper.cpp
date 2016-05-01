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
static const char *SQL_FIND_ALL_DATASETS_IDS =
        "SELECT datasetid"
        " FROM dbdataset ORDER BY datasetid"
        " LIMIT :taken OFFSET :skip";
static const char *SQL_FIND_ALL_DATASETS =
        "SELECT datasetid,optlock,sigle,nom,description,status"
        " FROM dbdataset ORDER BY sigle"
        " LIMIT :taken OFFSET :skip";
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
        "DELETE FROM dbdataset WHERE datasetid = :id";
static const char *SQL_FIND_DATASETS_COUNT =
        "SELECT COUNT(*) FROM dbdataset";
//
static const char *SQL_FIND_DATASET_VARIABLES =
        "SELECT variableid, optlock, datasetid , sigle, vartype, categvar, nom, description, genre, status"
        " FROM dbvariable WHERE datasetid = :datasetid"
        " ORDER BY categvar DESC, sigle ASC"
        " LIMIT :taken OFFSET :skip";
static const char *SQL_FIND_DATASET_VARIABLES_IDS =
        "SELECT variableid FROM dbvariable WHERE datasetid = :datasetid"
        " ORDER BY variableid"
        " LIMIT :taken OFFSET :skip";
static const char *SQL_VARIABLE_BY_ID =
        "SELECT variableid,optlock,datasetid,sigle,vartype,categvar,nom,description,genre,status"
        " FROM dbvariable WHERE variableid = :id";
static const char *SQL_VARIABLE_BY_DATASET_AND_SIGLE =
        "SELECT variableid,optlock,datasetid,sigle,vartype,categvar,nom,description,genre,status"
        " FROM dbvariable WHERE datasetid = :datasetid AND UPPER(LTRIM(RTRIM(sigle))) = :sigle";
static const char *SQL_INSERT_VARIABLE =
        "INSERT INTO dbvariable (datasetid,sigle,vartype,categvar,nom,description,genre,status)"
        " VALUES (:datasetid,:sigle,:vartype,:categ,:name,:desc,:genre,:status)";
static const char *SQL_UPDATE_VARIABLE =
        "UPDATE dbvariable SET optlock = optlock + 1,"
        " sigle = :sigle, vartype = :vartype, categvar = :categ, nom = :name, description = :desc, genre = :genre, status = :status WHERE variableid = :id";
static const char *SQL_REMOVE_VARIABLE =
        "DELETE FROM dbvariable WHERE variableid = :id";
static const char *SQL_FIND_DATASET_VARIABLES_COUNT =
        "SELECT COUNT(*) FROM dbvariable WHERE datasetid = :datasetid";
//
static const char *SQL_FIND_DATASET_INDIVS_COUNT =
        "SELECT COUNT(*) FROM dbindiv"
        " WHERE datasetid = :datasetid";
static const char *SQL_GET_DATASET_INDIV_IDS =
        "SELECT individ FROM dbindiv"
        " WHERE datasetid = :datasetid ORDER BY individ ASC"
        " LIMIT :taken OFFSET :skip";
static const char *SQL_FIND_DATASET_INDIVS =
        "SELECT individ,optlock,datasetid,sigle,nom,description,status"
        " FROM dbindiv WHERE datasetid = :datasetid ORDER BY sigle"
        " LIMIT :taken OFFSET :skip";
static const char *SQL_INDIV_BY_ID =
        "SELECT individ,optlock,datasetid,sigle,nom,description,status"
        " FROM dbindiv WHERE individ = :id";
static const char *SQL_INDIV_BY_DATASET_AND_SIGLE =
        "SELECT individ,optlock,datasetid,sigle,nom,description,status"
        " FROM dbindiv WHERE datasetid = :datasetid AND UPPER(LTRIM(RTRIM(sigle))) = :sigle";
static const char *SQL_INSERT_INDIV =
        "INSERT INTO dbindiv (datasetid,sigle,nom,description,status)"
        " VALUES(:datasetid,:sigle,:name,:desc,:status)";
static const char *SQL_UPDATE_INDIV =
        "UPDATE dbindiv SET optlock = OPTLOCK + 1,"
        " sigle = :sigle, nom = :name, description = :desc, status = :status WHERE individ = :id";
static const char *SQL_REMOVE_INDIV =
        "REMOVE FROM dbindiv WHERE individ = :id";
//
static const char *SQL_VALUE_BY_ID =
        "SELECT valueid,optlock,variableid,individ,stringval,status"
        " FROM dbvalue WHERE valueid = :id";
static const char *SQL_VALUES_BY_VARIABLE_INDIV =
        "SELECT valueid,optlock,variableid,individ,stringval,status"
        " FROM dbvalue WHERE variableid = :varid AND individ = :indid";
static const char *SQL_INSERT_VALUE =
        "INSERT INTO dbvalue (variableid,individ,stringval,status)"
        " VALUES(:varid,:indid,:stringval,:status)";
static const char *SQL_UPDATE_VALUE =
        "UPDATE dbvalue SET optlock = optlock + 1,"
        " stringval = :stringval, status = :status WHERE valueid = :id ";
static const char *SQL_REMOVE_VALUE =
        "DELETE from dbvalue WHERE valueid = :id";
static const char *SQL_FIND_DATASET_VALUES_COUNT = "SELECT COUNT(*)"
                                                   " FROM dbvalue a, dbvariable b"
                                                   " WHERE a.variableid = b.variableid AND b.datasetid = :datasetid";
static const char *SQL_FIND_DATASET_VALUES =
        "SELECT a.valueid,a.optlock,a.variableid,a.individ,a.stringval,a.status"
        " FROM dbvalue a, dbvariable b"
        " WHERE a.variableid = b.variableid AND b.datasetid = :datasetid"
        " ORDER BY a.variableid ASC, a.individ ASC"
        " LIMIT :taken OFFSET :skip";
static const char *SQL_VALUES_BY_VARIABLEID =
        "SELECT valueid,optlock,variableid,individ,stringval,status"
        " FROM dbvalue WHERE variableid = :varid"
        " LIMIT :taken OFFSET :skip";
static const char *SQL_VALUES_BY_INDIVID =
        "SELECT valueid,optlock,variableid,individ,stringval,status"
        " FROM dbvalue WHERE individ = :indid"
        " LIMIT :taken OFFSET :skip";
static const char *SQL_VARIABLE_VALUES_DISTINCT =
        "SELECT DISTINCT stringval FROM dbvalue WHERE variableid = :varid"
        " LIMIT :taken OFFSET :skip";
static const char *SQL_DELETE_VARIABLE_VALUES =
        "DELETE FROM dbvalue where variableid = :varid";
static const char *SQL_DELETE_INDIV_VALUES =
        "DELETE FROM dbvalue where individ = :indid";
static const char *SQL_FIND_DATASET_VARIABLES_TYPES =
        "SELECT variableid,vartype FROM dbvariable WHERE datasetid = :datasetid";
///////////////////////////////////////
bool DBStatHelper::find_dataset_variables_types(const DBStatDataset &oSet,QMap<IntType,QString> &oMap){
    DBStatDataset xSet(oSet);
    oMap.clear();
    if (!this->find_dataset(xSet)){
        return (false);
    }
    IntType nDatasetId = xSet.id();
    Q_ASSERT(nDatasetId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_FIND_DATASET_VARIABLES_TYPES)){
        return (false);
    }
    q.bindValue(":datasetid",nDatasetId);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        IntType key = q.value(0).toInt();
        QString val = q.value(1).toString().trimmed().toLower();
        oMap[key] = val;
    }
    return (true);
}//find_dataset_variables_types

bool DBStatHelper::remove_indiv(const DBStatIndiv &oInd, bool bCommit /*= true*/){
    IntType nIndId = oInd.id();
    if (nIndId == 0){
        return (false);
    }
    bool bInTrans = false;
    if (bCommit){
        bInTrans = this->m_base.transaction();
    }
    {
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_DELETE_INDIV_VALUES)){
            if (bInTrans && bCommit){
                this->m_base.rollback();
            }
            return (false);
        }
        q.bindValue(":indid",nIndId);
        if (!q.exec()){
            if (bInTrans && bCommit){
                this->m_base.rollback();
            }
            return (false);
        }
    }
    {
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_REMOVE_INDIV)){
            if (bInTrans && bCommit){
                this->m_base.rollback();
            }
            return (false);
        }
        q.bindValue(":id",nIndId);
        if (!q.exec()){
            if (bInTrans && bCommit){
                this->m_base.rollback();
            }
            return (false);
        }
    }
    if (bInTrans){
        if (!this->m_base.commit()){
            return (false);
        }
    }
    return (true);
}//remove_indiv
bool DBStatHelper::remove_variable(const DBStatVariable &oVar, bool bCommit /*= true*/){
    IntType nVarId = oVar.id();
    if (nVarId == 0){
        return (false);
    }
    bool bInTrans = false;
    if (bCommit){
        bInTrans = this->m_base.transaction();
    }
    {
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_DELETE_VARIABLE_VALUES)){
            if (bInTrans && bCommit){
                this->m_base.rollback();
            }
            return (false);
        }
        q.bindValue(":varid",nVarId);
        if (!q.exec()){
            if (bInTrans && bCommit){
                this->m_base.rollback();
            }
            return (false);
        }
    }
    {
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_REMOVE_VARIABLE)){
            if (bInTrans && bCommit){
                this->m_base.rollback();
            }
            return (false);
        }
        q.bindValue(":id",nVarId);
        if (!q.exec()){
            if (bInTrans && bCommit){
                this->m_base.rollback();
            }
            return (false);
        }
    }
    if (bInTrans){
        if (!this->m_base.commit()){
            return (false);
        }
    }
    return (true);
}//remove_variable

//////////////////// VALUES /////////////////////
bool  DBStatHelper::find_dataset_values_count(const DBStatDataset &oSet, int &nCount){
    DBStatDataset xSet(oSet);
    nCount = 0;
    if (!this->find_dataset(xSet)){
        return (false);
    }
    IntType nDatasetId = xSet.id();
    Q_ASSERT(nDatasetId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_FIND_DATASET_VALUES_COUNT)){
        return (false);
    }
    q.bindValue(":datasetid",nDatasetId);
    if (!q.exec()){
        return (false);
    }
    if (q.next()){
        nCount = q.value(0).toInt();
    }
    return (true);
}//find_dataset_values_count

bool  DBStatHelper::find_dataset_values(const DBStatDataset &oSet,
                                        QList<DBStatValue> &oList,
                                        int skip /*= 0*/, int count /*= 100*/){
    oList.clear();
    if (skip < 0){
        skip = 0;
    }
    if (count < 1){
        count = 100;
    }
    DBStatDataset xSet(oSet);
    if (!this->find_dataset(xSet)){
        return (false);
    }
    IntType nDatasetId = xSet.id();
    Q_ASSERT(nDatasetId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_FIND_DATASET_VALUES)){
        return (false);
    }
    q.bindValue(":datasetid",nDatasetId);
    q.bindValue(":taken",count);
    q.bindValue(":skip",skip);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        DBStatValue cur;
        this->read_value(q,cur);
        oList.append(cur);
    }
    return (true);
}//find_dataset_values

bool  DBStatHelper::find_variable_values(DBStatVariable &oVar,QList<DBStatValue> &oList,
                                         int skip /*= 0*/, int count /*= 100*/){
    oList.clear();
    if (skip < 0){
        skip = 0;
    }
    if (count < 1){
        count = 100;
    }
    if (!this->find_variable(oVar)){
        return (false);
    }
    IntType nVarId = oVar.id();
    Q_ASSERT(nVarId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_VALUES_BY_VARIABLEID)){
        return (false);
    }
    q.bindValue(":varid",nVarId);
    q.bindValue(":taken",count);
    q.bindValue(":skip",skip);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        DBStatValue cur;
        this->read_value(q,cur);
        oList.append(cur);
    }
    return (true);
}

bool  DBStatHelper::find_variable_distinct_values(DBStatVariable &oVar,
                                                  QList<DBStatValue> &oList,
                                                  int skip /*= 0*/, int count /*= 100*/){
    oList.clear();
    if (skip < 0){
        skip = 0;
    }
    if (count < 1){
        count = 100;
    }
    if (!this->find_variable(oVar)){
        return (false);
    }
    IntType nVarId = oVar.id();
    Q_ASSERT(nVarId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_VARIABLE_VALUES_DISTINCT)){
        return (false);
    }
    q.bindValue(":varid",nVarId);
    q.bindValue(":taken",count);
    q.bindValue(":skip",skip);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        DBStatValue cur;
        this->read_value(q,cur);
        oList.append(cur);
    }
    return (true);
}//find_variable_distinct_values

bool  DBStatHelper::find_indiv_values(DBStatIndiv &oInd, QList<DBStatValue> &oList,
                                      int skip /*= 0*/, int count /*= 100*/){
    oList.clear();
    if (skip < 0){
        skip = 0;
    }
    if (count < 1){
        count = 100;
    }
    if (!this->find_indiv(oInd)){
        return (false);
    }
    IntType nIndId = oInd.id();
    Q_ASSERT(nIndId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_VALUES_BY_INDIVID)){
        return (false);
    }
    q.bindValue(":indid",nIndId);
    q.bindValue(":taken",count);
    q.bindValue(":skip",skip);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        DBStatValue cur;
        this->read_value(q,cur);
        oList.append(cur);
    }
    return (true);
}//find_indiv_values

///
bool DBStatHelper::maintains_values(const QList<DBStatValue> &oVals,
                                    bool bCommit /*= true*/,
                                    bool bRemove /*= false*/){
    bool bInTrans = false;
    if (bCommit){
        bInTrans = this->m_base.transaction();
    }
    //
    QSqlQuery qInsert(this->m_base);
    QSqlQuery qUpdate(this->m_base);
    QSqlQuery qRemove(this->m_base);
    //
    if (bRemove){
        if (!qRemove.prepare(SQL_REMOVE_VALUE)){
            if (bCommit && bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
    } else {
        bool bRet = qInsert.prepare(SQL_INSERT_VALUE) &&
                qUpdate.prepare(SQL_UPDATE_VALUE);
        if (!bRet){
            if (bCommit && bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
    }
    //
    Q_FOREACH(const DBStatValue &oVal,oVals)
    {
        bool mustRemove = bRemove;
        DBStatValue xVal(oVal);
        this->find_value(xVal);
        IntType nId = xVal.id();
        QString sval = oVal.value().toString();
        if (nId != 0){
            if (sval.isNull() || sval.isEmpty()){
                mustRemove = true;
            }
        }
        if (mustRemove){
            if (nId != 0){
                qRemove.bindValue(":id",nId);
                if (!qRemove.exec()){
                    if (bCommit && bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            }// nId
        } else if (oVal.is_writeable()){
            QString status = oVal.status();
            if (nId != 0){
                qUpdate.bindValue(":stringval",sval);
                qUpdate.bindValue(":status",status);
                qUpdate.bindValue(":id",nId);
                if (!qUpdate.exec()){
                    if (bCommit && bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            } else {
                IntType nVarId = oVal.variable_id();
                IntType nIndId = oVal.indiv_id();
                qInsert.bindValue(":varid",nVarId);
                qInsert.bindValue(":indid",nIndId);
                qInsert.bindValue(":stringval",sval);
                qInsert.bindValue(":status",status);
                if (!qInsert.exec()){
                    if (bCommit && bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            }
        }
    }// oInd
    //
    if (bCommit && bInTrans){
        if (!this->m_base.commit()){
            return (false);
        }
    }
    return (true);
}//maintains_values

bool DBStatHelper::find_value(DBStatValue &cur){
    IntType nId = cur.id();
    if (nId != 0){
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_VALUE_BY_ID)){
            return (false);
        }
        q.bindValue(":id",nId);
        if (!q.exec()){
            return (false);
        }
        if (q.next()){
            this->read_value(q,cur);
            return (true);
        }
    }
    IntType nVarId = cur.variable_id();
    IntType nIndId = cur.indiv_id();
    if ((nVarId != 0) && (nIndId != 0)) {
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_VALUES_BY_VARIABLE_INDIV)){
            return (false);
        }
        q.bindValue(":varid", nVarId);
        q.bindValue(":indid",nIndId);
        if (!q.exec()){
            return (false);
        }
        if (q.next()){
            this->read_value(q,cur);
            return (true);
        }
    }
    return (false);
}//find_value
/////////////////////////////// INDIVS ///////////////
bool DBStatHelper::find_dataset_indivs_count(const DBStatDataset &oSet,int &nCount){
    DBStatDataset xSet(oSet);
    nCount = 0;
    if (!this->find_dataset(xSet)){
        return (false);
    }
    IntType nDatasetId = xSet.id();
    Q_ASSERT(nDatasetId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_FIND_DATASET_INDIVS_COUNT)){
        return (false);
    }
    q.bindValue(":datasetid",nDatasetId);
    if (!q.exec()){
        return (false);
    }
    if (q.next()){
        nCount = q.value(0).toInt();
    }
    return (true);
}//find_dataset_indivs_count

bool DBStatHelper::maintains_indivs(const QList<DBStatIndiv> &oInds,
                                    bool bCommit /*= true*/,
                                    bool bRemove /*= false*/){
    bool bInTrans = false;
    if (bCommit){
        bInTrans = this->m_base.transaction();
    }
    //
    QSqlQuery qInsert(this->m_base);
    QSqlQuery qUpdate(this->m_base);
    QSqlQuery qRemove(this->m_base);
    //
    if (bRemove){
        if (!qRemove.prepare(SQL_REMOVE_INDIV)){
            if (bCommit && bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
    } else {
        bool bRet = qInsert.prepare(SQL_INSERT_INDIV) &&
                qUpdate.prepare(SQL_UPDATE_INDIV);
        if (!bRet){
            if (bCommit && bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
    }
    //
    Q_FOREACH(const DBStatIndiv &oInd,oInds)
    {
        DBStatIndiv xInd(oInd);
        this->find_indiv(xInd);
        IntType nId = xInd.id();
        if (bRemove){
            if (nId != 0){
                qRemove.bindValue(":id",nId);
                if (!qRemove.exec()){
                    if (bCommit && bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            }// nId
        } else if (oInd.is_writeable()){
            QString sigle = oInd.sigle();
            QString name = oInd.name();
            QString desc = oInd.description();
            QString status = oInd.status();
            IntType nDatasetId = oInd.dataset_id();
            if (nId != 0){
                qUpdate.bindValue(":sigle",sigle);
                qUpdate.bindValue(":name",name);
                qUpdate.bindValue(":desc",desc);
                qUpdate.bindValue(":status",status);
                qUpdate.bindValue(":id",nId);
                if (!qUpdate.exec()){
                    if (bCommit && bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            } else {
                qInsert.bindValue(":datasetid",nDatasetId);
                qInsert.bindValue(":sigle",sigle);
                qInsert.bindValue(":name",name);
                qInsert.bindValue(":desc",desc);
                qInsert.bindValue(":status",status);
                if (!qInsert.exec()){
                    if (bCommit && bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            }
        }
    }// oInd
    //
    if (bCommit && bInTrans){
        if (!this->m_base.commit()){
            return (false);
        }
    }
    return (true);
}//maintains_indivs

bool DBStatHelper::find_dataset_indivs_ids(const DBStatDataset &oSet, QList<IntType> &oList,
                                           int skip /*=0*/,int count /*=100*/){
    oList.clear();
    if (skip < 0){
        skip = 0;
    }
    if (count < 1){
        count = 100;
    }
    DBStatDataset xSet(oSet);
    if (!this->find_dataset(xSet)){
        return (false);
    }
    IntType nDatasetId = xSet.id();
    Q_ASSERT(nDatasetId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_GET_DATASET_INDIV_IDS)){
        return (false);
    }
    q.bindValue(":datasetid",nDatasetId);
    q.bindValue(":taken",count);
    q.bindValue(":skip",skip);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        IntType nId = q.value(0).toInt();
        oList.append(nId);
    }
    return (true);
}//find_dataset_indivs_ids
bool DBStatHelper::find_dataset_indivs(const DBStatDataset &oSet,
                                       QList<DBStatIndiv> &oList,
                                       int skip /*=0*/,int count /*=100*/){
    oList.clear();
    if (skip < 0){
        skip = 0;
    }
    if (count < 1){
        count = 100;
    }
    DBStatDataset xSet(oSet);
    if (!this->find_dataset(xSet)){
        return (false);
    }
    IntType nDatasetId = xSet.id();
    Q_ASSERT(nDatasetId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_FIND_DATASET_INDIVS)){
        return (false);
    }
    q.bindValue(":datasetid",nDatasetId);
    q.bindValue(":taken",count);
    q.bindValue(":skip",skip);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        DBStatIndiv cur;
        this->read_indiv(q,cur);
        oList.append(cur);
    }
    return (true);
}//find_dataset_indivs
////////////////////////////////////
bool DBStatHelper::find_indiv(DBStatIndiv &cur){
    IntType nId = cur.id();
    if (nId != 0){
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_INDIV_BY_ID)){
            return (false);
        }
        q.bindValue(":id",nId);
        if (!q.exec()){
            return (false);
        }
        if (q.next()){
            this->read_indiv(q,cur);
            return (true);
        }
    }
    QString sigle = cur.sigle();
    IntType nDatasetId = cur.dataset_id();
    if ((!sigle.isEmpty()) && (!sigle.isNull()) && (nDatasetId != 0)) {
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_INDIV_BY_DATASET_AND_SIGLE)){
            return (false);
        }
        q.bindValue(":datasetid", nDatasetId);
        q.bindValue(":sigle",sigle);
        if (!q.exec()){
            return (false);
        }
        if (q.next()){
            this->read_indiv(q,cur);
            return (true);
        }
    }
    return (false);
}//find_indiv
////////////////////////////// VARIABLES /////
bool DBStatHelper::find_dataset_variables_count(const DBStatDataset &oSet,int &nCount){
    DBStatDataset xSet(oSet);
    nCount = 0;
    if (!this->find_dataset(xSet)){
        return (false);
    }
    IntType nDatasetId = xSet.id();
    Q_ASSERT(nDatasetId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_FIND_DATASET_VARIABLES_COUNT)){
        return (false);
    }
    q.bindValue(":datasetid",nDatasetId);
    if (!q.exec()){
        return (false);
    }
    if (q.next()){
        nCount = q.value(0).toInt();
    }
    return (true);
}//find_dataset_variables_count

bool DBStatHelper::maintains_variables(const QList<DBStatVariable> &oVars,
                                       bool bCommit /*= true*/,
                                       bool bRemove /*= false*/){
    bool bInTrans = false;
    if (bCommit){
        bInTrans = this->m_base.transaction();
    }
    //
    QSqlQuery qInsert(this->m_base);
    QSqlQuery qUpdate(this->m_base);
    QSqlQuery qRemove(this->m_base);
    //
    if (bRemove){
        if (!qRemove.prepare(SQL_REMOVE_VARIABLE)){
            if (bCommit && bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
    } else {
        bool bRet = qInsert.prepare(SQL_INSERT_VARIABLE) &&
                qUpdate.prepare(SQL_UPDATE_VARIABLE);
        if (!bRet){
            if (bCommit && bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
    }
    //
    Q_FOREACH(const DBStatVariable &oVar,oVars)
    {
        DBStatVariable xVar(oVar);
        this->find_variable(xVar);
        IntType nId = xVar.id();
        if (bRemove){
            if (nId != 0){
                qRemove.bindValue(":id",nId);
                if (!qRemove.exec()){
                    if (bCommit && bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            }// nId
        } else if (oVar.is_writeable()){
            QString sigle = oVar.sigle();
            QString name = oVar.name();
            QString desc = oVar.description();
            QString status = oVar.status();
            IntType nCateg = (oVar.is_categ()) ? 1 : 0;
            QString vartype = oVar.vartype();
            QString genre = oVar.genre();
            IntType nDatasetId = oVar.dataset_id();
            if (nId != 0){
                qUpdate.bindValue(":sigle",sigle);
                qUpdate.bindValue(":vartype",vartype);
                qUpdate.bindValue(":categ",nCateg);
                qUpdate.bindValue(":name",name);
                qUpdate.bindValue(":desc",desc);
                qUpdate.bindValue(":genre",genre);
                qUpdate.bindValue(":status",status);
                qUpdate.bindValue(":id",nId);
                if (!qUpdate.exec()){
                    if (bCommit && bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            } else {
                qInsert.bindValue(":datasetid",nDatasetId);
                qInsert.bindValue(":sigle",sigle);
                qInsert.bindValue(":vartype",vartype);
                qInsert.bindValue(":categ",nCateg);
                qInsert.bindValue(":name",name);
                qInsert.bindValue(":desc",desc);
                qInsert.bindValue(":genre",genre);
                qInsert.bindValue(":status",status);
                if (!qInsert.exec()){
                    if (bCommit && bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            }
        }
    }// oVar
    //
    if (bCommit && bInTrans){
        if (!this->m_base.commit()){
            return (false);
        }
    }
    return (true);
}//maintains_variables

bool DBStatHelper::find_dataset_variables_ids(const DBStatDataset &oSet, QList<IntType> &oList,
                                              int skip /*=0*/,int count /*=100*/){
    oList.clear();
    if (skip < 0){
        skip = 0;
    }
    if (count < 1){
        count = 100;
    }
    DBStatDataset xSet(oSet);
    if (!this->find_dataset(xSet)){
        return (false);
    }
    IntType nDatasetId = xSet.id();
    Q_ASSERT(nDatasetId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_FIND_DATASET_VARIABLES_IDS)){
        return (false);
    }
    q.bindValue(":datasetid",nDatasetId);
    q.bindValue(":taken",count);
    q.bindValue(":skip",skip);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        IntType nId = q.value(0).toInt();
        oList.append(nId);
    }
    return (true);
}//find_dataset_variables_ids

bool DBStatHelper::find_dataset_variables(const DBStatDataset &oSet, QList<DBStatVariable> &oList,
                                          int skip /*=0*/,int count /*=100*/){
    oList.clear();
    if (skip < 0){
        skip = 0;
    }
    if (count < 1){
        count = 100;
    }
    DBStatDataset xSet(oSet);
    if (!this->find_dataset(xSet)){
        return (false);
    }
    IntType nDatasetId = xSet.id();
    Q_ASSERT(nDatasetId != 0);
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_FIND_DATASET_VARIABLES)){
        return (false);
    }
    q.bindValue(":datasetid",nDatasetId);
    q.bindValue(":taken",count);
    q.bindValue(":skip",skip);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        DBStatVariable cur;
        this->read_variable(q,cur);
        oList.append(cur);
    }
    return (true);
}//find_dataset_variables

////////////////////////////////////
bool DBStatHelper::find_variable(DBStatVariable &cur){
    IntType nId = cur.id();
    if (nId != 0){
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_VARIABLE_BY_ID)){
            return (false);
        }
        q.bindValue(":id",nId);
        if (!q.exec()){
            return (false);
        }
        if (q.next()){
            this->read_variable(q,cur);
            return (true);
        }
    }
    QString sigle = cur.sigle();
    IntType nDatasetId = cur.dataset_id();
    if ((!sigle.isEmpty()) && (!sigle.isNull()) && (nDatasetId != 0)) {
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_VARIABLE_BY_DATASET_AND_SIGLE)){
            return (false);
        }
        q.bindValue(":datasetid", nDatasetId);
        q.bindValue(":sigle",sigle);
        if (!q.exec()){
            return (false);
        }
        if (q.next()){
            this->read_variable(q,cur);
            return (true);
        }
    }
    return (false);
}//find_variable

/////////////////////////////////////////////////////
bool DBStatHelper::remove_dataset(const DBStatDataset &cur,
                                  bool bCommit /*= true*/){
    DBStatDataset xSet(cur);
    if (!this->find_dataset(xSet)){
        return (false);
    }
    bool bInTrans = false;
    if (bCommit){
        bInTrans = this->m_base.transaction();
    }
    IntType nId = xSet.id();
    Q_ASSERT(nId != 0);
    {
        int nCount = 0;
        if (!this->find_dataset_indivs_count(xSet,nCount)){
            if (bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
        if (nCount > 0){
            QList<IntType> oIds;
            if (!this->find_dataset_indivs_ids(xSet,oIds,0,nCount)){
                if (bInTrans){
                    this->m_base.rollback();
                }
                return (false);
            }
            Q_FOREACH(IntType nIndId,oIds)
            {
                DBStatIndiv xInd(nIndId);
                if (!this->remove_indiv(xInd,false)){
                    if (bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            }
        }// nCount
    }// indivs
    {
        int nCount = 0;
        if (!this->find_dataset_variables_count(xSet,nCount)){
            if (bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
        if (nCount > 0){
            QList<IntType> oIds;
            if (!this->find_dataset_variables_ids(xSet,oIds,0,nCount)){
                if (bInTrans){
                    this->m_base.rollback();
                }
                return (false);
            }
            Q_FOREACH(IntType nVarId,oIds)
            {
                DBStatVariable xInd(nVarId);
                if (!this->remove_variable(xInd,false)){
                    if (bInTrans){
                        this->m_base.rollback();
                    }
                    return (false);
                }
            }
        }// nCount
    }// variables
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_REMOVE_DATASET)){
        if (bInTrans){
            this->m_base.rollback();
        }
        return (false);
    }
    q.bindValue(":id",nId);
    if (!q.exec()){
        if (bInTrans){
            this->m_base.rollback();
        }
        return (false);
    }
    if (bInTrans){
        if (!this->m_base.commit()){
            return (false);
        }
    }
    return (true);
}// remove_dataset

bool DBStatHelper::maintains_dataset(DBStatDataset &cur){
    if (!cur.is_writeable()){
        return (false);
    }
    bool bInTrans = this->m_base.transaction();
    DBStatDataset xSet(cur);
    this->find_dataset(xSet);
    IntType nId = xSet.id();
    QString sigle = cur.sigle();
    QString name = cur.name();
    QString desc = cur.description();
    QString status = cur.status();
    if (nId != 0){
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_UPDATE_DATASET)){
            if (bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
        q.bindValue(":sigle",sigle);
        q.bindValue(":name",name);
        q.bindValue(":desc",desc);
        q.bindValue(":status",status);
        q.bindValue(":id",nId);
        if (!q.exec()){
            if (bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
    } else {
        QSqlQuery q(this->m_base);
        if (!q.prepare(SQL_INSERT_DATASET)){
            if (bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
        q.bindValue(":sigle",sigle);
        q.bindValue(":name",name);
        q.bindValue(":desc",desc);
        q.bindValue(":status",status);
        if (!q.exec()){
            if (bInTrans){
                this->m_base.rollback();
            }
            return (false);
        }
    }
    if (bInTrans){
        this->m_base.commit();
    }
    return (this->find_dataset(cur));
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
bool  DBStatHelper::find_all_datasets(QList<DBStatDataset> &oList,
                                      int skip /* = 0*/,
                                      int count /* = 100 */){
    oList.clear();
    if (skip < 0){
        skip = 0;
    }
    if (count < 1){
        count = 100;
    }
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_FIND_ALL_DATASETS)){
        return (false);
    }
    q.bindValue(":taken",count);
    q.bindValue(":skip",skip);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        DBStatDataset cur;
        this->read_dataset(q,cur);
        oList.append(cur);
    }
    return (true);
}//find_all_datasets
bool  DBStatHelper::find_all_datasets_ids(QList<IntType> &oList,
                                          int skip /* = 0*/,
                                          int count /* = 100 */){
    oList.clear();
    if (skip < 0){
        skip = 0;
    }
    if (count < 1){
        count = 100;
    }
    QSqlQuery q(this->m_base);
    if (!q.prepare(SQL_FIND_ALL_DATASETS_IDS)){
        return (false);
    }
    q.bindValue(":taken",count);
    q.bindValue(":skip",skip);
    if (!q.exec()){
        return (false);
    }
    while (q.next()){
        IntType n = q.value(0).toInt();
        oList.append(n);
    }
    return (true);
}//find_all_datasets_ids
bool DBStatHelper::find_all_datasets_count(int &nCount){
    nCount = 0;
    QSqlQuery q(SQL_FIND_DATASETS_COUNT,this->m_base);
    if (q.next()){
        nCount = q.value(0).toInt();
    } else {
        return (false);
    }
    return (true);
}//find_dataset_variables_count
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
DBStatHelper::DBStatHelper(QObject *parent /*= 0*/,
                           const QString &sDatabaseName /* = DEFAULT_DATABASE_NAME*/,
                           const QString &baseType /*= DEFAULT_DATABASE_TYPE*/,
                           const QString &ConnectionName  /*= DEFAULT_CONNECTION_NAME*/
                           ) : QObject(parent)
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
