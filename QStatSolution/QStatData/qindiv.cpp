#include "qindiv.h"
namespace info {
QIndiv::QIndiv():d(new IndivData())
{

}
QIndiv::QIndiv(const DBStatIndiv &oInd,
               const QMap<IntType,QVariant> &oMap) : d(new IndivData(oInd,oMap))
{

}
QIndiv::QIndiv(const QIndiv &other):d(other.d){

}
QIndiv & QIndiv::operator=(const QIndiv &other){
    if (this != &other){
        this->d = other.d;
    }
    return (*this);
}
QIndiv::~QIndiv(){
}
IntType QIndiv::id(void) const{
    return d->id();
}
IntType QIndiv::version(void) const{
    return d->version();
}
QString  QIndiv::sigle(void) const{
    return d->sigle();
}
QString  QIndiv::name(void) const{
    return d->name();
}
QString  QIndiv::description(void) const{
  return d->description();
}
QString  QIndiv::status(void) const{
    return d->status();
}
QMap<IntType,QVariant>  QIndiv::data(void) const{
  return d->data();
}
void QIndiv::data(const QMap<IntType,QVariant> &oMap){
    d->data(oMap);
}

double QIndiv::distance(const QIndiv &other) const{
    double dRet = 0;
    int nc = 0;
    //
    QMap<IntType,QVariant> m1 = this->data();
    QMap<IntType,QVariant> m2 = other.data();
    QList<IntType> keys = m1.keys();
    Q_FOREACH(IntType key, keys)
    {
        QVariant v1 = m1[key];
        if (!v1.canConvert<double>()){
            continue;
        }
        if (m2.contains(key)){
            QVariant v2 = m2[key];
             if (v2.canConvert<double>()){
                 double d = v1.toDouble() - v2.toDouble();
                 dRet += d * d;
                 ++nc;
             }// ok
        }// contains_key
    }// key
    //
    if (nc > 1){
        dRet /= nc;
    }
    return (dRet);
}// distance
}// namespace info
