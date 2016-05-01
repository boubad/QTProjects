#include "qstatdata.h"

namespace info {
//////////////////////////////////////////////
DBStatValue::DBStatValue(const DBStatVariable &oVar, const DBStatIndiv &oInd,
                         const QVariant &v) :
    m_varid(oVar.id()), m_indid(oInd.id()), m_val(v) {
}// DBStatValue
DBStatValue::DBStatValue(const IntType nId) : StatBaseItem(nId) {}
DBStatValue::DBStatValue(const IntType nId, const IntType nVersion,
                         const QString &status,
                         const IntType nVarId, const IntType nIndId, const QVariant &val) : StatBaseItem(nId, nVersion, status),
    m_varid(nVarId), m_indid(nIndId), m_val(val) {}

DBStatValue::DBStatValue() {
}
DBStatValue::DBStatValue(const DBStatValue &other) : StatBaseItem(other), m_varid(other.m_varid),
    m_indid(other.m_indid), m_val(other.m_val) {

}
DBStatValue & DBStatValue::operator=(const DBStatValue &other) {
    if (this != &other) {
        StatBaseItem::operator=(other);
        this->m_varid = other.m_varid;
        this->m_indid = other.m_indid;
        this->m_val = other.m_val;
    }
    return (*this);
}
DBStatValue::~DBStatValue() {}

bool DBStatValue::is_writeable(void) const {
    return ((this->m_varid != 0) && (this->m_indid != 0));
}
void DBStatValue::swap(DBStatValue &other) {
    DBStatValue t(*this);
    *this = other;
    other = t;
}// swap
QDataStream & DBStatValue::write_to(QDataStream &out) const{
    StatBaseItem::write_to(out);
    out << this->m_varid;
    out << this->m_indid;
    out << this->m_val;
    return out;
}
QDataStream & DBStatValue::read_from(QDataStream &in){
    StatBaseItem::read_from(in);
    in >> this->m_varid;
    in >> this->m_indid;
    in >> this->m_val;
    return in;
}
/////////////////////////////////////////////////
DBStatIndiv::DBStatIndiv(const IntType n) : DBStatDatasetChild(n) {}

DBStatIndiv::DBStatIndiv(const IntType nId, const IntType nVersion,
                         const QString &status,
                         const QString &sSigle,
                         const QString &sName,
                         const QString &sDesc, const IntType nDatasetId) :
    DBStatDatasetChild(nId, nVersion, status, sSigle, sName, sDesc, nDatasetId) {
}

DBStatIndiv::DBStatIndiv() {}
DBStatIndiv::DBStatIndiv(const DBStatDataset &oSet,
                         const QString &sSigle /*= QString()*/,
                         const QString &sName /*= QString()*/,
                         const QString & sDesc /*= QString()*/,
                         const QString & sStatus /*= QString("OK")*/):
    DBStatDatasetChild(oSet,sSigle,sName,sDesc){
    this->status(sStatus);
}
DBStatIndiv::DBStatIndiv(const DBStatIndiv &other) : DBStatDatasetChild(other) {}
DBStatIndiv & DBStatIndiv::operator=(const DBStatIndiv &other) {
    if (this != &other) {
        DBStatDatasetChild::operator=(other);
    }
    return (*this);
}
DBStatIndiv::~DBStatIndiv() {

}
void DBStatIndiv::swap(DBStatIndiv &other) {
    DBStatIndiv t(*this);
    *this = other;
    other = t;
}// swap
//////////////////////////////////////////////
DBStatVariable::DBStatVariable() : m_categ(false) {
    this->vartype(QString("float"));
    this->genre(QString("initial"));
}
DBStatVariable::DBStatVariable(const IntType nId) : DBStatDatasetChild(nId), m_categ(false) {
    this->vartype(QString("float"));
    this->genre(QString("initial"));
}
DBStatVariable::DBStatVariable(const IntType nId, const IntType nVersion, const QString &status,
                               const QString &sSigle, const QString &sName, const QString &sDesc,
                               const IntType nDatasetId,
                               const bool bCateg, const QString &sType, const QString &sGenre) :
    DBStatDatasetChild(nId, nVersion, status, sSigle, sName, sDesc, nDatasetId), m_categ(bCateg) {
    this->vartype(sType);
    this->genre(sGenre);
}

DBStatVariable::DBStatVariable(const DBStatDataset &oSet,
                               const QString &sSigle /*= QString()*/,
                               const QString &sName /*= QString()*/,
                               const QString & sDesc /*= QString()*/,
                               const bool bCateg /*= false*/,
                               const QString & sType /*= QString("float")*/,
                               const QString &sGenre /*= QString("initial")*/,
                               const QString & sStatus /*= QString("OK")*/):
    DBStatDatasetChild(oSet,sSigle,sName,sDesc),m_categ(bCateg){
    this->status(sStatus);
    this->vartype(sType);
    this->genre(sGenre);
}

DBStatVariable::DBStatVariable(const DBStatVariable &other) : DBStatDatasetChild(other),
    m_categ(other.m_categ), m_type(other.m_type), m_genre(other.m_genre) {}
DBStatVariable & DBStatVariable::operator=(const DBStatVariable &other) {
    if (this != &other) {
        DBStatDatasetChild::operator=(other);
        this->m_categ = other.m_categ;
        this->m_type = other.m_type;
        this->m_genre = other.m_genre;
    }
    return (*this);
}
DBStatVariable::~DBStatVariable() {

}
bool DBStatVariable::is_writeable(void) const {
    const QString s = this->vartype();
    return ((!s.isNull()) && (!s.isEmpty()) && DBStatDatasetChild::is_writeable());
}
bool DBStatVariable::is_numeric(void) const{
    if (this->is_categ()){
        return (false);
    }
    QString s = this->vartype();
    return ((s == "double") || (s == "float") || (s =="real") ||
            (s == "int") || (s == "integer") || (s=="long"));
}
void DBStatVariable::swap(DBStatVariable &other) {
    DBStatVariable t(*this);
    *this = other;
    other = t;
}// swap
QDataStream & DBStatVariable::write_to(QDataStream &out) const{
    DBStatDatasetChild::write_to(out);
    out << this->m_categ;
    out << this->m_type;
    out << this->m_genre;
    return out;
}
QDataStream & DBStatVariable::read_from(QDataStream &in){
    DBStatDatasetChild::read_from(in);
    in >> this->m_categ;
    in >> this->m_type;
    in >> this->m_genre;
    return in;
}
///////////////////////////////////////////////
DBStatDatasetChild::DBStatDatasetChild() :m_datasetid(0) {
}
DBStatDatasetChild::DBStatDatasetChild(const DBStatDataset &oSet,
                                       const QString &sSigle,
                                       const QString &sName,
                                       const QString &sDesc):StatNamedItem(sSigle,sName,sDesc),
    m_datasetid(oSet.id()){

}

DBStatDatasetChild::DBStatDatasetChild(const IntType nId) : StatNamedItem(nId), m_datasetid(0) {
}
DBStatDatasetChild::DBStatDatasetChild(const IntType nId, const IntType nVersion, const QString &status,
                                       const QString &sSigle, const QString &sName, const QString &sDesc, const IntType nDatasetId) :
    StatNamedItem(nId, nVersion, status, sSigle, sName, sDesc), m_datasetid(nDatasetId) {}

DBStatDatasetChild::DBStatDatasetChild(const DBStatDatasetChild &other) : StatNamedItem(other),
    m_datasetid(other.m_datasetid) {}
DBStatDatasetChild & DBStatDatasetChild::operator=(const DBStatDatasetChild &other) {
    if (this != &other) {
        StatNamedItem::operator=(other);
        this->m_datasetid = other.m_datasetid;
    }
    return (*this);
}
DBStatDatasetChild::~DBStatDatasetChild() {}
bool DBStatDatasetChild::is_writeable(void) const {
    return ((this->m_datasetid != 0) && StatNamedItem::is_writeable());
}
QDataStream & DBStatDatasetChild::write_to(QDataStream &out) const{
    StatNamedItem::write_to(out);
    out << this->m_datasetid;
    return out;
}
QDataStream & DBStatDatasetChild::read_from(QDataStream &in){
    StatNamedItem::read_from(in);
    in >> this->m_datasetid;
    return in;
}
/////////////////////////////////////////////
DBStatDataset::DBStatDataset() {
}
DBStatDataset::DBStatDataset(const QString &sigle,
                             const QString &sName,
                             const QString &sDesc):StatNamedItem(sigle,sName,sDesc) {
}
DBStatDataset::DBStatDataset(const IntType nId) :StatNamedItem(nId) {
}
DBStatDataset::DBStatDataset(const IntType nId, const IntType nVersion, const QString &status,
                             const QString &sSigle, const QString &sName, const QString &sDesc) :
    StatNamedItem(nId, nVersion, status, sSigle, sName, sDesc) {}
DBStatDataset::DBStatDataset(const DBStatDataset &other) : StatNamedItem(other) {}
DBStatDataset & DBStatDataset::operator=(const DBStatDataset &other) {
    if (this != &other) {
        StatNamedItem::operator=(other);
    }
    return (*this);
}
DBStatDataset::~DBStatDataset() {
}
void DBStatDataset::swap(DBStatDataset &other) {
    DBStatDataset t(*this);
    *this = other;
    other = t;
}// swap
//////////////////////////////////////////////
StatNamedItem::StatNamedItem() {

}
StatNamedItem::StatNamedItem(const IntType nId) :StatBaseItem(nId) {

}
StatNamedItem::StatNamedItem(const QString &sSigle, const QString &sName, const QString &sDesc){
    this->sigle(sSigle);
    this->name(sName);
    this->description(sDesc);
}

StatNamedItem::StatNamedItem(const IntType nId, const IntType nVersion,
                             const QString &status,
                             const QString  &sSigle,
                             const QString &sName, const QString &sDesc) :
    StatBaseItem(nId, nVersion, status) {
    this->sigle(sSigle);
    this->name(sName);
    this->description(sDesc);
}
StatNamedItem::StatNamedItem(const StatNamedItem &other) :StatBaseItem(other),
    m_sigle(other.m_sigle), m_name(other.m_name), m_desc(other.m_desc) {

}
StatNamedItem & StatNamedItem::operator=(const StatNamedItem &other) {
    if (this != &other) {
        StatBaseItem::operator=(other);
        this->m_sigle = other.m_sigle;
        this->m_name = other.m_name;
        this->m_desc = other.m_desc;
    }
    return (*this);
}
StatNamedItem::~StatNamedItem() {

}
bool StatNamedItem::is_writeable(void) const{
    const QString &s = this->sigle();
    return ((!s.isEmpty()) && (!s.isNull()));
}
QDataStream & StatNamedItem::write_to(QDataStream &out) const{
    StatBaseItem::write_to(out);
    out << this->m_sigle;
    out << this->m_name;
    out << this->m_desc;
    return out;
}
QDataStream & StatNamedItem::read_from(QDataStream &in){
    StatBaseItem::read_from(in);
    in >> this->m_sigle;
    in >> this->m_name;
    in >> this->m_desc;
    return in;
}
///////////////////////////////////////////////
StatBaseItem::StatBaseItem() :m_id(0), m_version(0) {
}
StatBaseItem::StatBaseItem(const IntType nId) : m_id(nId), m_version(0) {
}
StatBaseItem::StatBaseItem(const IntType nId, const IntType nVersion, const QString &sStatus) : m_id(nId), m_version(nVersion) {
    this->status(sStatus);
}
StatBaseItem::StatBaseItem(const StatBaseItem &other) : m_id(other.m_id), m_version(other.m_version), m_status(other.m_status) {
}
StatBaseItem & StatBaseItem::operator=(const StatBaseItem &other) {
    if (this != &other) {
        this->m_id = other.m_id;
        this->m_version = other.m_version;
        this->m_status = other.m_status;
    }
    return (*this);
}
StatBaseItem::~StatBaseItem() {

}
QDataStream & StatBaseItem::write_to(QDataStream &out) const{
    out << this->m_id;
    out << this->m_version;
    out << this->m_status;
    return out;
}
QDataStream & StatBaseItem::read_from(QDataStream &in){
    in >> this->m_id;
    in >> this->m_version;
    in >> this->m_status;
    return in;
}

//////////////////////////////////////////////
}// namespace info
