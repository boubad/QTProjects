#include <QString>
#include <QtTest>
#include <QCoreApplication>
//////////////////////////////////////
#include <dbstathelper.h>
///
///
using namespace info;
/////////////////////////////////
class DTestStatDataTest : public QObject
{
    Q_OBJECT

public:
    DTestStatDataTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
};

DTestStatDataTest::DTestStatDataTest()
{
}

void DTestStatDataTest::initTestCase()
{
    DBStatHelper oHelper;
}

void DTestStatDataTest::cleanupTestCase()
{
}

void DTestStatDataTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_MAIN(DTestStatDataTest)

#include "tst_dteststatdatatest.moc"
