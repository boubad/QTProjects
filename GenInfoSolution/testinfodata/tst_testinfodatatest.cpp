#include <QString>
#include <QtTest>
//////////////////////////////////
#include "infotestdata.h"
//////////////////////////////
#include <QDebug>
#include <matricedata.h>
#include <matelem.h>
#include <treeelem.h>
////////////////////////
#include  "global_defs.h"
///////////////////
using namespace info;
//////////
class TestinfodataTest : public QObject
{
    Q_OBJECT
public:
    using matricedata_type = MatriceData<INDEXTYPE,FLOATTYPE,DISTANCETYPE>;
    using distancemap_type = typename matricedata_type::distancemap_type;
    using  matelem_type = MatElem<INDEXTYPE,DISTANCETYPE,CRITERIATYPE>;
    using ints_vector_ptr = typename matelem_type::ints_vector_ptr;
    using ints_vector = typename matelem_type::ints_vector;
    using matelem_result_type = typename matelem_type::matelem_result_type;
    using criteria_type = typename matelem_type::criteria_type;
    using index_type = typename matelem_type::criteria_type;
    //
    using mattree_type = MatTree<INDEXTYPE,FLOATTYPE>;
    using TreeResultType = mattree_type::TreeResultType;
    using TreeResultTypePtr = mattree_type::TreeResultTypePtr;
    using ints_sizet_map = mattree_type::ints_sizet_map;
public:
    static void process_matelem_result(matelem_result_type r){
        QString sRes = QString("CRIT: %1").arg(r.first);
        ints_vector_ptr oind = r.second;
        ints_vector *pv = oind.get();
        if (pv != nullptr){
            const int n = pv->size();
            sRes += QString(" [ ");
            for (int i = 0; i < n; ++i){
                if (i != 0){
                    sRes += QString(", ");
                }
                sRes += QString("%1").arg((*pv)[i]);
            }// i
            sRes += QString(" ]");
        }// pv
        qDebug() << sRes;
    }//  process_matelem_result

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
    void testMatDataSetDataAsync();
    void testArrangeMatElem();
    void testArrangeMatElemAsync();

};

TestinfodataTest::TestinfodataTest():nRows(0),nCols(0)
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
    //
    matricedata_type oData;
    bool bRet = oData.setData(nRows,nCols,gdata.get(),name,&rowNames,&colNames);
    QVERIFY(bRet);
    QVERIFY(oData.is_valid());
}// testMatDataSetData
void TestinfodataTest::testMatDataSetDataAsync(){
    //
    matricedata_type oData;
    QFuture<bool> f = oData.setDataAsync(nRows,nCols,gdata.get(),name,&rowNames,&colNames);
    bool bRet = f.result();
    QVERIFY(bRet);
    QVERIFY(oData.is_valid());
}// testMatDataSetData
void TestinfodataTest::testArrangeMatElem(){
    //
    matricedata_type oData;
    QFuture<bool> f = oData.setDataAsync(nRows,nCols,gdata.get(),name,&rowNames,&colNames);
    bool bRet = f.result();
    QVERIFY(bRet);
    QVERIFY(oData.is_valid());
    const distancemap_type *pMap = oData.get_rows_distancesmap();
    QVERIFY(pMap != nullptr);
    matelem_type oMat(pMap);
  //  oMat.setCallback([](matelem_result_type r){
   //     TestinfodataTest::process_matelem_result(r);
   // });
    matelem_result_type r = oMat.arrange();
    TestinfodataTest::process_matelem_result(r);
}
void TestinfodataTest::testArrangeMatElemAsync(){
    matricedata_type oData;
    QFuture<bool> f = oData.setDataAsync(nRows,nCols,gdata.get(),name,&rowNames,&colNames);
    bool bRet = f.result();
    QVERIFY(bRet);
    QVERIFY(oData.is_valid());
    const distancemap_type *pMap = oData.get_rows_distancesmap();
    QVERIFY(pMap != nullptr);
    matelem_type oMat(pMap);
    QFuture<matelem_result_type> fr = oMat.arrangeAsync();
    matelem_result_type r = fr.result();
    TestinfodataTest::process_matelem_result(r);
}
////////////////////////////////////////
QTEST_MAIN(TestinfodataTest)
//QTEST_GUILESS_MAIN(TestinfodataTest)
//QTEST_APPLESS_MAIN(TestinfodataTest)
////////////////////////////////////////
#include "tst_testinfodatatest.moc"
