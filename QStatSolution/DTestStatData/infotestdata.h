#ifndef INFOTESTDATA_H
#define INFOTESTDATA_H
///////////////////////////////////
#include <QString>
#include <QVector>
//////////////////////////////////////
namespace info {
    ////////////////////////////////
    class InfoTestData
    {
    public:
        static void get_test_indiv_data(QString &name, int &nRows, int &nCols);
        static void get_mortal_name(QString &name);
        static void get_conso_name(QString &name);
        static void get_default_type_genre_status(QString &type, QString &genre, QString &status);
        static void get_database_filename(QString &filename);
        static void get_conso_data(QString &name, int &nRows, int &nCols,
            QVector<int> &data, QVector<QString> &indNames, QVector<QString> &varNames);
        static void get_mortal_data(QString &name, int &nRows, int &nCols,
            QVector<int> &data, QVector<QString> &indNames, QVector<QString> &varNames);
        static void get_data(const QString &name, int &nRows, int &nCols,
            QVector<int> &data, QVector<QString> &indNames, QVector<QString> &varNames);
        static void get_data_names(QVector<QString> &onames);
    };// classInfoTestData
      ////////////////////////////////////////
 }// namespace info
#endif // INFOTESTDATA_H
