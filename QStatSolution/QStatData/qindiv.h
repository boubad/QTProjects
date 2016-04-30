#ifndef QINDIV_H
#define QINDIV_H
#include "qstatdata.h"
/////////////////////
#include <QObject>
#include <QMap>
#include <QVariant>
///////////////////////
/// \brief The QIndiv class
namespace info {
class QIndiv : public QObject
{
    Q_OBJECT
public:
    explicit QIndiv(QObject *parent = 0);
    explicit QIndiv(const DBStatIndiv &oInd, QObject *parent = 0);

signals:

public slots:
    inline IntType id(void) const {
        return (this->m_inddata.id());
    }
    inline IntType version(void) const {
        return (this->m_inddata.version());
    }
    inline const QString & sigle(void) const {
        return (this->m_inddata.sigle());
    }
    inline const QString & name(void) const {
        return (this->m_inddata.name());
    }
    inline const QString & description(void) const {
        return (this->m_inddata.description());
    }
    inline const QString & status(void) const {
        return (this->m_inddata.status());
    }
    inline const QMap<IntType,QVariant> & data(void) const {
        return (this->m_data);
    }
    inline void data(const QMap<IntType,QVariant> &oMap){
        this->m_data = oMap;
    }

private:
    DBStatIndiv m_inddata;
    QMap<IntType,QVariant> m_data;
};
}// namespace info


#endif // QINDIV_H
