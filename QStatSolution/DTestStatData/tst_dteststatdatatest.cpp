#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QMap>
/////////////////////////////////////
#include <dbstathelper.h>
#include "infotestdata.h"
///
///
using namespace info;
/////////////////////////////////
class DTestStatDataTest : public QObject
{
    Q_OBJECT
private:
    DBStatHelper *m_pHelper;
    //
    static void import_dataset(DBStatHelper *pMan,
                               QString &name, int nRows, int nCols,
                               QVector<int> &data,
                               QVector<QString> &indNames,
                               QVector<QString> &varNames);
public:
    DTestStatDataTest(QObject *pParent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCaseDataSet();
    void testCaseVariables();
    void testCaseIndivs();
    void testCaseValues();
};
void DTestStatDataTest::import_dataset(DBStatHelper *pMan,
                                       QString &name, int nRows, int nCols,
                                       QVector<int> &data,
                                       QVector<QString> &indNames,
                                       QVector<QString> &varNames){
    QVERIFY(pMan != nullptr);
    QVERIFY(pMan->isValid());
    DBStatDataset oSet(name);
    bool bRet = pMan->find_dataset(oSet);
    if (!bRet){
        bRet = pMan->maintains_dataset(oSet);
        QVERIFY(bRet);
    }
    QVERIFY(oSet.id() != 0);
    QList<DBStatVariable> oVars;
    Q_FOREACH(const QString &sigle,varNames)
    {
        DBStatVariable oVar(oSet,sigle);
        if (!pMan->find_variable(oVar)){
            oVars.push_back(oVar);
        }
    }// sigle
    if (!oVars.empty()){
        bRet = pMan->maintains_variables(oVars);
        QVERIFY(bRet);
    }
    QList<DBStatIndiv> oInds;
    Q_FOREACH(const QString &sigle,indNames)
    {
        DBStatIndiv oInd(oSet,sigle);
        if (!pMan->find_indiv(oInd)){
            oInds.push_back(oInd);
        }
    }// sigle
    if (!oInds.empty()){
        bRet = pMan->maintains_indivs(oInds);
        QVERIFY(bRet);
    }
    oInds.clear();
    oVars.clear();
    bRet = pMan->find_dataset_indivs(oSet,oInds);
    bRet = bRet && pMan->find_dataset_variables(oSet,oVars);
    QVERIFY(bRet);
    QMap<QString,const DBStatVariable *> oVarsMap;
    QMap<QString, const DBStatIndiv *> oIndsMap;
    Q_FOREACH(const DBStatVariable &v, oVars)
    {
        QString key = v.sigle();
        oVarsMap[key] = &v;
    }
    Q_FOREACH(const DBStatIndiv &v, oInds)
    {
        QString key = v.sigle();
        oIndsMap[key] = &v;
    }
    QList<DBStatValue> oVals;
    for (int i = 0; i < nRows; ++i){
        QString s = indNames[i];
        QString sind = s.trimmed().toUpper();
        Q_ASSERT(oIndsMap.contains(sind));
        const DBStatIndiv *pInd = oIndsMap[sind];
        QVERIFY(pInd != nullptr);
        for (int j = 0; j < nCols; ++j){
            QString s = varNames[j];
            QString svar = s.trimmed().toUpper();
            Q_ASSERT(oVarsMap.contains(svar));
            const DBStatVariable *pVar = oVarsMap[svar];
            QVERIFY(pVar != nullptr);
            QVariant v = data[i * nCols + j];
            DBStatValue vv(*pVar, *pInd,v);
            if (!pMan->find_value(vv)){
                oVals.push_back(vv);
            }
        }// j
    }// i
    if (!oVals.empty()){
        bRet = pMan->maintains_values(oVals);
        QVERIFY(bRet);
    }
}//import_dataset

DTestStatDataTest::DTestStatDataTest(QObject *pParent):QObject(pParent),m_pHelper(nullptr)
{

}

void DTestStatDataTest::initTestCase()
{
    QString dbFilename;
    InfoTestData::get_database_filename(dbFilename);
    QVERIFY(!dbFilename.isNull());
    QVERIFY(!dbFilename.isEmpty());
    DBStatHelper *pHelper =  new DBStatHelper(this,dbFilename);
    QVERIFY(pHelper != nullptr);
    QVERIFY(pHelper->isValid());
    this->m_pHelper = pHelper;
    //
    QString name;
    int nRows = 0, nCols = 0;
    QVector<int> data;
    QVector<QString> indNames, varNames;
    InfoTestData::get_mortal_data(name,nRows,nCols,data,indNames,varNames);
    import_dataset(pHelper,name,nRows,nCols,data,indNames,varNames);
    InfoTestData::get_conso_data(name,nRows,nCols,data,indNames,varNames);
    import_dataset(pHelper,name,nRows,nCols,data,indNames,varNames);
}//initTestCase

void DTestStatDataTest::cleanupTestCase()
{
    delete this->m_pHelper;
    this->m_pHelper = nullptr;
}

void DTestStatDataTest::testCaseDataSet()
{
    DBStatHelper *pMan = this->m_pHelper;
    QVERIFY(pMan != nullptr);
    int nCount = 0;
    bool bRet = pMan->find_all_datasets_count(nCount);
    QVERIFY2(bRet,"find_all_datasets_count");
    QVERIFY2(nCount > 1,"Bad datasets count");
    //
    QList<IntType> oIds;
    bRet = pMan->find_all_datasets_ids(oIds);
    QVERIFY2(bRet,"find_all_datasets_ids");
    QVERIFY2(oIds.size()==nCount,"datasets count mismatch");
    //
    QList<DBStatDataset> oSets;
    bRet = pMan->find_all_datasets(oSets);
    QVERIFY2(bRet,"find_all_datasets");
    QVERIFY2(oSets.size()==nCount,"datasets count mismatch");
}
void DTestStatDataTest::testCaseVariables(){
    DBStatHelper *pMan = this->m_pHelper;
    QVERIFY(pMan != nullptr);
    //
    QString name;
    InfoTestData::get_mortal_name(name);
    DBStatDataset oSet(name);
    bool bRet = pMan->find_dataset(oSet);
    QVERIFY(bRet);
    QVERIFY(oSet.id() != 0);
    //
    QMap<IntType,QString> oMap;
    bRet = pMan->find_dataset_variables_types(oSet,oMap);
    QVERIFY2(bRet,"find_dataset_variables_types");
    QVERIFY2(!oMap.empty(), "Variables types count error");
    //
    int nCount = 0;
    bRet = pMan->find_dataset_variables_count(oSet,nCount);
    QVERIFY2(bRet,"find_dataset_variables_count");
    QVERIFY2(nCount > 0, "Variables count error");
    //
    QList<IntType> oIds;
    bRet = pMan->find_dataset_variables_ids(oSet,oIds,0,nCount);
    QVERIFY2(bRet,"find_dataset_variables_ids");
    QVERIFY2(oIds.size() == nCount, "Variables count error");
    //
    QList<DBStatVariable> oVars;
    bRet = pMan->find_dataset_variables(oSet,oVars,0,nCount);
    QVERIFY2(bRet,"find_dataset_variables");
    QVERIFY2(oVars.size() == nCount, "Variables count error");
}//testCaseVariables
void DTestStatDataTest::testCaseIndivs(){
    DBStatHelper *pMan = this->m_pHelper;
    QVERIFY(pMan != nullptr);
    //
    QString name;
    InfoTestData::get_mortal_name(name);
    DBStatDataset oSet(name);
    bool bRet = pMan->find_dataset(oSet);
    QVERIFY(bRet);
    QVERIFY(oSet.id() != 0);
    int nCount = 0;
    bRet = pMan->find_dataset_indivs_count(oSet,nCount);
    QVERIFY2(bRet,"find_dataset_indivs_count");
    QVERIFY2(nCount > 0, "Indivs count error");
    //
    QList<IntType> oIds;
    bRet = pMan->find_dataset_indivs_ids(oSet,oIds,0,nCount);
    QVERIFY2(bRet,"find_dataset_indivs_ids");
    QVERIFY2(oIds.size() == nCount, "Indivs count error");
    //
    QList<DBStatIndiv> oInds;
    bRet = pMan->find_dataset_indivs(oSet,oInds,0,nCount);
    QVERIFY2(bRet,"find_dataset_indivs");
    QVERIFY2(oInds.size() == nCount, "Variables count error");
}//testCaseIndivs
void DTestStatDataTest::testCaseValues(){
    DBStatHelper *pMan = this->m_pHelper;
    QVERIFY(pMan != nullptr);
    //
    QString name;
    InfoTestData::get_mortal_name(name);
    DBStatDataset oSet(name);
    bool bRet = pMan->find_dataset(oSet);
    QVERIFY(bRet);
    QVERIFY(oSet.id() != 0);
    int nCount = 0;
    bRet = pMan->find_dataset_values_count(oSet,nCount);
    QVERIFY2(bRet,"find_dataset_values_count");
    QVERIFY2(nCount > 0, "values count error");
    //
    QList<DBStatValue> oVals;
    bRet = pMan->find_dataset_values(oSet,oVals,0,nCount);
    QVERIFY2(bRet,"find_dataset_values");
    QVERIFY2(oVals.size() == nCount, "Values count error");
    //
    QList<DBStatVariable> oVars;
    bRet = pMan->find_dataset_variables(oSet,oVars);
    QVERIFY(bRet);
    int nVars = oVars.size();
    QVERIFY(nVars > 0);
    Q_FOREACH(const DBStatVariable &oVar,oVars)
    {
        DBStatVariable xVar(oVar);
        QList<DBStatValue> xVals;
        bRet = pMan->find_variable_values(xVar,xVals);
        QVERIFY(bRet);
        QVERIFY(xVals.size() > 0);
        //
        QList<DBStatValue> yVals;
        bRet = pMan->find_variable_distinct_values(xVar,yVals);
        QVERIFY(bRet);
        QVERIFY(yVals.size() > 0);
    }// oVar
    //
    QList<DBStatIndiv> oInds;
    bRet = pMan->find_dataset_indivs(oSet,oInds);
    QVERIFY(bRet);
    int nRows = oInds.size();
    QVERIFY(nRows > 0);
    Q_FOREACH(const DBStatIndiv &oInd,oInds)
    {
        DBStatIndiv xInd(oInd);
        QList<DBStatValue> xVals;
        bRet = pMan->find_indiv_values(xInd,xVals);
        QVERIFY(bRet);
        QVERIFY(xVals.size() > 0);
    }// oVar
}//testCaseValues
//
QTEST_MAIN(DTestStatDataTest)

#include "tst_dteststatdatatest.moc"
