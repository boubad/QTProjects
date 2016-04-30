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
    void testCase1();
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

void DTestStatDataTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_MAIN(DTestStatDataTest)

#include "tst_dteststatdatatest.moc"
