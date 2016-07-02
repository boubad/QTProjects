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
    template <typename X>
    static QString display_array(const X &v){
        QString sRes("[ ");
        for (auto it = v.begin(); it != v.end(); ++it){
            if (it != v.begin()){
                sRes += QString(", ");
            }
            sRes += QString("%1").arg(*it);
        }// it
        return sRes;
        sRes += QString(" ]");
    }// display_array
    template <typename X,typename F>
    static QString display_map(const QMap<X,F>  &oMap){
        QString sRes("[ ");
        for (auto it = oMap.begin(); it != oMap.end(); ++it){
            if (it != oMap.begin()){
                sRes += QString(", ");
            }
            X v1 = it.key();
            F v2 = it.value();
            sRes += QString("(%1").arg(v1);
            sRes += QString(",%1)").arg(v2);
        }// it
        sRes += QString(" ]");
        return sRes;
    }// display_array
    static void process_matelem_result(matelem_result_type r){
        QString sRes = QString("CRIT: %1 ").arg(r.first);
        ints_vector_ptr oind = r.second;
        ints_vector *pv = oind.get();
        if (pv != nullptr){
            QString ss = display_array(*pv);
            sRes += ss;
        }// pv
        qDebug() << sRes;
    }//  process_matelem_result
    static void process_tree_result(TreeResultTypePtr r){
        QString sRes;
        TreeResultType *p = r.get();
        if (p != nullptr){
            QString s1 = display_array(p->ids());
            QString s2 = display_map(p->map());
            sRes += QString("%1\n%2").arg(s1,s2);
        }// p
        qDebug() << sRes;
    }//process_tree_result

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
    void testAggreg();
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
void TestinfodataTest::testAggreg(){
    matricedata_type oData;
    QFuture<bool> f = oData.setDataAsync(nRows,nCols,gdata.get(),name,&rowNames,&colNames);
    bool bRet = f.result();
    QVERIFY(bRet);
    QVERIFY(oData.is_valid());
    double s = 0;
    int nbClasses = 5;
    mattree_type oTree(&oData);
    TreeResultTypePtr r = oTree.aggreg(s,nbClasses);
    TreeResultType *pr = r.get();
    QVERIFY(pr != nullptr);
    TestinfodataTest::process_tree_result(r);
    matelem_type oMat(&oData);
    /*
    oMat.setCallback([](matelem_result_type r){
        TestinfodataTest::process_matelem_result(r);
   });
   */
    oMat.indexes(pr->ids());
    matelem_result_type rm= oMat.arrange();
    TestinfodataTest::process_matelem_result(rm);
}// testAggreg

////////////////////////////////////////
QTEST_MAIN(TestinfodataTest)
//QTEST_GUILESS_MAIN(TestinfodataTest)
//QTEST_APPLESS_MAIN(TestinfodataTest)
////////////////////////////////////////
#include "tst_testinfodatatest.moc"
