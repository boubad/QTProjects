#ifndef QSTATDATA_H
#define QSTATDATA_H
#include <QString>
#include <QVariant>
#include <QDataStream>
/////////////////////////////////////////
namespace info {
//////////////////////////////////////////
typedef int IntType;
class DBStatHelper;
///////////////////////////////////////////
class StatBaseItem {
    friend class DBStatHelper;
protected:
    StatBaseItem();
    StatBaseItem(const IntType nId);
    StatBaseItem(const IntType nId, const IntType nVersion, const QString &status);
    StatBaseItem(const StatBaseItem &other);
    StatBaseItem & operator=(const StatBaseItem &other);
private:
    IntType m_id;
    IntType m_version;
    QString m_status;
protected:
    inline void id(const IntType n){
        this->m_id = n;
    }
    inline void version(const IntType n){
        this->m_version = n;
    }

public:
    virtual ~StatBaseItem();
public:
    inline bool operator==(const StatBaseItem &other) const{
        return (this->m_id == other.m_id);
    }
    inline bool operator<(const StatBaseItem &other) const{
        return (this->m_id < other.m_id);
    }
    inline IntType operator()(void) const{
        return (this->m_id);
    }
    inline IntType id(void) const {
        return (this->m_id);
    }
    inline IntType version(void) const {
        return (this->m_version);
    }
    inline const QString & status(void) const{
        return (this->m_status);
    }
    inline void status(const QString &s){
        this->m_status = s.trimmed().toUpper();
    }
    inline bool is_updateable(void) const {
        return ((this->id() != 0) && this->is_writeable());
    }
    inline bool is_removeable(void) const {
        return (this->id() != 0);
    }
public:
    virtual bool is_writeable(void) const = 0;
    virtual QDataStream & write_to(QDataStream &out) const;
    virtual QDataStream & read_from(QDataStream &in);
};// class StatBaseItem
///////////////////////////////////////////////
class StatNamedItem : public StatBaseItem {
    friend class DBStatHelper;
protected:
    StatNamedItem();
    StatNamedItem(const QString &sSigle, const QString &sName, const QString &sDesc);
    StatNamedItem(const IntType nId);
    StatNamedItem(const IntType nId, const IntType nVersion, const QString &status,
                  const QString &sSigle, const QString &sName, const QString &sDesc);
    StatNamedItem(const StatNamedItem &other);
    StatNamedItem & operator=(const StatNamedItem &other);
private:
    QString m_sigle;
    QString m_name;
    QString m_desc;
public:
    virtual ~StatNamedItem();
public:
    inline const QString & sigle(void) const {
        return (this->m_sigle);
    }
    inline void sigle(const QString &s){
        this->m_sigle = s.trimmed().toUpper();
    }
    inline const QString & name(void) const {
        return (this->m_name);
    }
    inline void name(const QString &s){
        this->m_name = s.trimmed();
    }
    inline const QString & description(void) const {
        return (this->m_desc);
    }
    inline void description(const QString &s){
        this->m_desc = s.trimmed();
    }
public:
    virtual bool is_writeable(void) const;
    virtual QDataStream & write_to(QDataStream &out) const;
    virtual QDataStream & read_from(QDataStream &in);
};
////////////////////////////////////////////////
class DBStatDataset : public StatNamedItem {
    friend class DBStatHelper;
protected:
    DBStatDataset(const IntType nId);
    DBStatDataset(const IntType nId, const IntType nVersion, const QString &status,
                  const QString &sSigle, const QString &sName, const QString &sDesc);
public:
    DBStatDataset();
    DBStatDataset(const QString &sSigle, const QString &sName = QString(),
                  const QString &sDesc = QString());
    DBStatDataset(const DBStatDataset &other);
    DBStatDataset & operator=(const DBStatDataset &other);
    virtual ~DBStatDataset();
    void swap(DBStatDataset &other);
}; // class DBStatDataset
///////////////////////////////////////////////
class DBStatDatasetChild : public StatNamedItem {
    friend class DBStatHelper;
private:
    IntType m_datasetid;
protected:
    DBStatDatasetChild();
    DBStatDatasetChild(const IntType nId);
    DBStatDatasetChild(const DBStatDataset &oSet,
                       const QString &sSigle,
                       const QString &sName,
                       const QString &sDesc);
    DBStatDatasetChild(const IntType nId, const IntType nVersion, const QString &status,
                       const QString &sSigle, const QString &sName, const QString &sDesc, const IntType nDatasetId);
    DBStatDatasetChild(const DBStatDatasetChild &other);
    DBStatDatasetChild & operator=(const DBStatDatasetChild &other);
    inline void dataset_id(const IntType n) {
        this->m_datasetid = n;
    }
public:
    virtual ~DBStatDatasetChild();
    inline IntType dataset_id(void) const{
        return (this->m_datasetid);
    }
    virtual bool is_writeable(void) const;
    virtual QDataStream & write_to(QDataStream &out) const;
    virtual QDataStream & read_from(QDataStream &in);
}; // class DBStatDatasetChild
///////////////////////////////////////////////
class DBStatVariable : public DBStatDatasetChild {
    friend class DBStatHelper;
private:
    bool m_categ;
    QString m_type;
    QString m_genre;
protected:
     DBStatVariable(const IntType nId);
     DBStatVariable(const IntType nId, const IntType nVersion, const QString &status,
                    const QString &sSigle, const QString &sName, const QString &sDesc, const IntType nDatasetId,
                    const bool bCateg, const QString &sType, const QString &sGenre);
public:
    DBStatVariable();
    DBStatVariable(const DBStatDataset &oSet,
                   const QString &sSigle = QString(),
                   const QString &sName = QString(),
                   const QString & sDesc = QString(),
                   const bool bCateg = false,
                   const QString & sType = QString("float"),
                   const QString &sGenre = QString("initial"),
                   const QString & sStatus = QString("OK"));
    DBStatVariable(const DBStatVariable &other);
    DBStatVariable & operator=(const DBStatVariable &other);
    virtual ~DBStatVariable();
public:
    inline bool is_categ(void) const {
        return (this->m_categ);
    }
    inline void is_categ(const bool b){
        this->m_categ = b;
    }
    inline const QString & vartype(void) const {
        return (this->m_type);
    }
    inline void vartype(const QString &s){
        this->m_type = s.trimmed().toLower();
    }
    inline const QString &genre(void) const {
        return (this->m_genre);
    }
    inline void genre(const QString &s){
        this->m_genre = s.trimmed().toUpper();
    }
    virtual bool is_writeable(void) const;
    void swap(DBStatVariable &other);
    virtual QDataStream & write_to(QDataStream &out) const;
    virtual QDataStream & read_from(QDataStream &in);
}; // class DBStatVariable
///////////////////////////////////////////////////
class DBStatIndiv : public DBStatDatasetChild {
    friend class DBStatHelper;
protected:
    DBStatIndiv(const IntType nId);
    DBStatIndiv(const IntType nId, const IntType nVersion, const QString &status,
                const QString &sSigle, const QString &sName, const QString &sDesc, const IntType nDatasetId);
public:
    DBStatIndiv();
    DBStatIndiv(const DBStatDataset &oSet,
                 const QString &sSigle = QString(),
                 const QString &sName = QString(),
                 const QString & sDesc = QString(),
                 const QString & sStatus = QString("OK"));
    DBStatIndiv(const DBStatIndiv &other);
    DBStatIndiv & operator=(const DBStatIndiv &other);
    virtual ~DBStatIndiv();
    void swap(DBStatIndiv &other);
}; // class DBStatIndiv
///////////////////////////////////////////////////
class DBStatValue : public StatBaseItem {
    friend class DBStatHelper;
private:
    IntType m_varid;
    IntType m_indid;
    QVariant m_val;
protected:
     DBStatValue(const IntType nId);
     DBStatValue(const IntType nId, const IntType nVersion, const QString &status,
                 const IntType nVarId, const IntType nIndId, const QVariant &val);
     inline void variable_id(const IntType n){
         this->m_varid = n;
     }
     inline void indiv_id(const IntType n){
         this->m_indid = n;
     }
public:
    DBStatValue();
    DBStatValue(const DBStatVariable &oVar, const DBStatIndiv &oInd,
                const QVariant & = QVariant());
    DBStatValue(const DBStatValue &other);
    DBStatValue & operator=(const DBStatValue &other);
    virtual ~DBStatValue();
public:
    inline IntType variable_id(void) const{
        return (this->m_varid);
    }
    inline IntType indiv_id(void) const{
        return (this->m_indid);
    }
    inline const QVariant & value(void) const{
        return (this->m_val);
    }
    inline void value(const QVariant &v){
        this->m_val = v;
    }

    virtual bool is_writeable(void) const;
    void swap(DBStatValue &other);
    virtual QDataStream & write_to(QDataStream &out) const;
    virtual QDataStream & read_from(QDataStream &in);
}; // class DBStatValue
////////////////////////////////////////////////////
}// namespace info
//////////////////////////////////////////
inline void swap(info::DBStatDataset &v1, info::DBStatDataset &v2) {
    v1.swap(v2);
}
inline void swap(info::DBStatVariable &v1, info::DBStatVariable &v2) {
    v1.swap(v2);
}
inline void swap(info::DBStatIndiv &v1, info::DBStatIndiv &v2) {
    v1.swap(v2);
}
inline void swap(info::DBStatValue &v1, info::DBStatValue &v2) {
    v1.swap(v2);
}
/////////////////////////////////////////
inline QDataStream & operator<<(QDataStream &out,const info::StatBaseItem &o){
    return o.write_to(out);
}
inline QDataStream & operator>>(QDataStream &in,info::StatBaseItem &o){
    return o.read_from(in);
}
/////////////////////////////////////////
#endif // QSTATDATA_H
