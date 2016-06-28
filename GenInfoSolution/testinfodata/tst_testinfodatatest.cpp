#include <QString>
#include <QtTest>
//////////////////////////////////
#include "infotestdata.h"
//////////////////////////////
#include <qmatricedata.h>
///////////////////
using namespace info;
//////////
class TestinfodataTest : public QObject
{
    Q_OBJECT
private:
    int  nRows;
    int nCols;
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
};

TestinfodataTest::TestinfodataTest()
{
}

void TestinfodataTest::initTestCase()
{
    std::string sname;
    std::vector<std::string> rn, cn;
    std::vector<int> xdata;
    size_t r = 0, c = 0;
    //
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
 }// testMatDataSetData


QTEST_APPLESS_MAIN(TestinfodataTest)

#include "tst_testinfodatatest.moc"
