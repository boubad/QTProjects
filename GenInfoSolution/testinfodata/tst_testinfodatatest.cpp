#include <QString>
#include <QtTest>
//////////////////////////////////
#include "infotestdata.h"
//////////////////////////////
#include <QDebug>
#include <qmatricedata.h>
#include <qmatelem.h>
////////////////////////
#include  <atomic>
///////////////////
using namespace info;
//////////
class TestinfodataTest : public QObject
{
    Q_OBJECT
public:
    using result_type = std::shared_ptr<int>;
private:
    int  nRows;
    int nCols;
    std::atomic<bool> m_ended;
    QString name;
    QStringList rowNames;
    QStringList colNames;
    std::unique_ptr<int> gdata;
public:
    TestinfodataTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testMatDataSetData();
    void testArrangeMatElem();
private:
    void receive_results(double c);
    void receive_start(void);
    void receive_end(void);
};

TestinfodataTest::TestinfodataTest():nRows(0),nCols(0),m_ended(false)
{
}

void TestinfodataTest::initTestCase()
{
    std::string sname;
    std::vector<std::string> rn, cn;
    std::vector<int> xdata;
    size_t r = 0, c = 0;
    //
    m_ended.store(false);
    name.clear();
    nRows = 0;
    nCols = 0;
    rowNames.clear();
    colNames.clear();
    gdata.reset();

    //
    InfoTestData::get_mortal_data(sname,r,c,xdata,rn,cn);
    nRows = (int)r;
    nCols = (int)c;
    QVERIFY(nRows > 0);
    QVERIFY(nCols > 0);
    QVERIFY(xdata.size() >= (size_t)(nCols * nRows));
    QVERIFY(rn.size() >= (size_t)nRows);
    QVERIFY(cn.size() >= (size_t)nCols);
    //
    gdata.reset(new int[nCols * nRows]);
    int *pData = gdata.get();
    QVERIFY(pData != nullptr);
    name = QString::fromStdString(sname);
    for (int i = 0; i < nRows; ++i){
        rowNames.append(QString::fromStdString(rn[i]));
        for (int j = 0; j < nCols; ++j){
            int pos = i * nCols + j;
            pData[pos] = xdata[pos];
        }// i
    }// i
    for (int i = 0; i < nCols; ++i){
        colNames.append(QString::fromStdString(cn[i]));
    }// i
}

void TestinfodataTest::cleanupTestCase()
{
    name.clear();
    nRows = 0;
    nCols = 0;
    rowNames.clear();
    colNames.clear();
    gdata.reset();
}
 void TestinfodataTest::testMatDataSetData(){
     QMatriceData *pData = new QMatriceData(this);
     QVERIFY(pData != nullptr);
     const int *pxdata = this->gdata.get();
     QVERIFY(pxdata != nullptr);
     bool bRet = pData->set_data(nRows,nCols,pxdata,&rowNames,&colNames);
     QVERIFY(bRet);
     delete pData;
 }// testMatDataSetData
  void TestinfodataTest::testArrangeMatElem(){
      QMatriceData *pData = new QMatriceData(this);
      QVERIFY(pData != nullptr);
      const int *pxdata = this->gdata.get();
      QVERIFY(pxdata != nullptr);
      bool bRet = pData->set_data(nRows,nCols,pxdata,&rowNames,&colNames);
      QVERIFY(bRet);
      const QDistanceMap *pMap = pData->get_rows_distancesmap();
      QVERIFY(pMap != nullptr);
      QMatElem *pElem = new QMatElem(pMap,this);
      QVERIFY(pElem != nullptr);
      connect(pElem,&QMatElem::start_arrange,this,&TestinfodataTest::receive_start);
      connect(pElem,&QMatElem::end_arrange,this,&TestinfodataTest::receive_end);
      connect(pElem,&QMatElem::newcriteria,this,&TestinfodataTest::receive_results);
      pElem->arrange();
      delete pElem;
      delete pData;
  }

 void TestinfodataTest::receive_results(double c){
    QString sRes = QString("CRIT: %1").arg(c);
    qDebug() << sRes;
 }

 void TestinfodataTest::receive_start(void){
     this->m_ended.store(false);
      qDebug() << QString("RECEIVED START...!!!....");
 }

 void TestinfodataTest::receive_end(void){
    this->m_ended.store(true);
     qDebug() << QString("RECEIVED FINISHED....");
 }
 ////////////////////////////////////////
QTEST_GUILESS_MAIN(TestinfodataTest)
//QTEST_APPLESS_MAIN(TestinfodataTest)
////////////////////////////////////////
#include "tst_testinfodatatest.moc"
