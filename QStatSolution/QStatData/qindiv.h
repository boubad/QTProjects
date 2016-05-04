#ifndef QINDIV_H
#define QINDIV_H
#include "qstatdata.h"
/////////////////////
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QSharedData>
#include <QSharedDataPointer>
///////////////////////
/// \brief The QIndiv class
namespace info {
//////////////////////////////////
class IndivData : public QSharedData {
public:
    IndivData(){}
    IndivData(const DBStatIndiv &oInd,const QMap<IntType,QVariant> &oMap):
    m_inddata(oInd),m_data(oMap){
    }
    IndivData(const IndivData &other):QSharedData(other),
        m_inddata(other.m_inddata),m_data(other.m_data){}
    ~IndivData(){}
public:
    IntType id(void) const {
        return (this->m_inddata.id());
    }
    IntType version(void) const {
        return (this->m_inddata.version());
    }
    QString  sigle(void) const {
        return (this->m_inddata.sigle());
    }
    QString  name(void) const {
        return (this->m_inddata.name());
    }
    QString  description(void) const {
        return (this->m_inddata.description());
    }
   QString  status(void) const {
        return (this->m_inddata.status());
    }
   QMap<IntType,QVariant>  data(void) const {
        return (this->m_data);
    }
    void data(const QMap<IntType,QVariant> &oMap){
        this->m_data = oMap;
    }
private:
    DBStatIndiv m_inddata;
    QMap<IntType,QVariant> m_data;
};// class IndivData

//////////////////////////////////
class QIndiv {
public:
    QIndiv();
    QIndiv(const DBStatIndiv &oInd,const  QMap<IntType,QVariant> &oMap);
    QIndiv(const QIndiv &other);
    QIndiv & operator=(const QIndiv &other);
    ~QIndiv();
public:
    IntType id(void) const;
    IntType version(void) const;
    QString  sigle(void) const;
    QString  name(void) const;
    QString  description(void) const;
    QString  status(void) const;
    QMap<IntType,QVariant>  data(void) const;
    void data(const QMap<IntType,QVariant> &oMap);
public:
    double distance(const QIndiv &other) const;
private:
    QSharedDataPointer<IndivData> d;
};
//////////////////////////////////////////////////
class IIndivProvider {
public:
    virtual bool is_valid(void) = 0;
    virtual bool indivs_count(int &nCount) = 0;
    virtual bool all_indivs_ids(QVector<IntType> &oIds) = 0;
    virtual bool contains_id(const IntType aId) = 0;
    virtual bool find_indiv(const IntType aIndex,QIndiv &oInd,
                            const VariableMode mode = VariableMode::modeAll) = 0;
    virtual bool find_indiv_at(const int pos,QIndiv &oInd,
                                const VariableMode mode = VariableMode::modeAll) = 0;
    virtual bool distance(const IntType aIndex1, const IntType &Index2,
                          double &dRes,
                          const VariableMode mode = VariableMode::modeNumeric) = 0;
    virtual bool distance_at(const int pos1, const int pos2,
                          double &dRes,
                          const VariableMode mode = VariableMode::modeNumeric) = 0;
    virtual bool get_variables(QList<DBStatVariable> &oVars) = 0;
    virtual bool  find_next_indiv(const DBStatDataset &oSet,
                                  const int offset,QIndiv &oInd,
                                  const VariableMode mode = VariableMode::modeAll) = 0;
    virtual bool get_dataset(DBStatDataset &oSet) = 0;
    virtual ~IIndivProvider(){}
}; // class IIndivProvider
/////////////////////////////////////////////////////
class ISerialIndivProvider  {
public:
     virtual bool is_valid(void) = 0;
    virtual bool reset(void) = 0;
    virtual bool get_variables(QList<DBStatVariable> &oVars) = 0;
    virtual bool next(QIndiv &oInd,const VariableMode mode = VariableMode::modeAll) = 0;
    virtual ~ISerialIndivProvider(){}
};// interface ISerialIndivProvider

///////////////////////////////////////////////////
}// namespace info


#endif // QINDIV_H
