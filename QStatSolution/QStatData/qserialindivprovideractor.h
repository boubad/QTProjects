#ifndef QSERIALINDIVPROVIDERACTOR_H
#define QSERIALINDIVPROVIDERACTOR_H
//////////////////////////////////////
#include "qindiv.h"
///////////////////////////////////////
#include <QObject>
#include <QThread>
#include <QSemaphore>
#include <QQueue>
//////////////////////////////
#include <memory>
///////////////////////////
namespace info {
///////////////////////////////
typedef std::shared_ptr<QIndiv> QIndivPtr;
///////////////////////////
class InfoStatData {
    private:
        IntType _index;
        double _vmin;
        double _vmax;
        double _vmean;
        double _vstd;
    private:
        size_t _count;
        size_t _somme1;
        size_t _somme2;
    public:
        InfoStatData(IntType aIndex = 0);
        InfoStatData(const InfoStatData &other);
        InfoStatData & operator=(const InfoStatData &other);
        ~InfoStatData();
    public:
        bool operator==(const InfoStatData &other) const;
        bool operator<(const InfoStatData &other) const;
        IntType operator()(void) const {
            return (this->_index);
        }
    public:
        void add(const double dval);
        InfoStatData & operator+=(const double dval){
            this->add(dval);
            return (*this);
        }

        void compute(void);
    public:
        IntType index(void) const {
            return (this->_index);
        }
        double average(void) const {
            return (this->_vmean);
        }
        double standard_deviation(void) const {
            return (this->_vstd);
        }
        double minimum(void) const {
            return (this->_vmin);
        }
        double maximum(void) const {
            return (this->_vmax);
        }
        size_t count(void) const {
            return (this->_count);
        }
        void get(IntType &index, double &vmin, double &vmax, double &vmean, double &vstd) const;
    };// class InfoStatData
//////////////////////////////////
class QSerialIndivProviderActor : public QThread
{
    Q_OBJECT
private:
    IIndivProvider *m_provider;
    QSemaphore  *m_freeSemaphore;
    QSemaphore  *m_usedSemaphore;
    QQueue<QIndivPtr> *m_pindiv;
public:
    explicit QSerialIndivProviderActor(IIndivProvider *pProvider,
                                       QSemaphore *pFree,
                                       QSemaphore *pUsed,
                                       QQueue<QIndivPtr> *pResult,
                                       QObject *parent = 0);
protected:
    virtual void run();
signals:

public slots:
};
/////////////////////////////////////////
class QSerialIndivConsumerActor : public QThread {
    Q_OBJECT
private:
    IIndivProvider *m_provider;
    QSemaphore  *m_freeSemaphore;
    QSemaphore  *m_usedSemaphore;
    QQueue<QIndivPtr> *m_pindiv;
    QMap<IntType, InfoStatData> m_stats;
public:
    explicit QSerialIndivConsumerActor(IIndivProvider *pProvider,
                                       QSemaphore *pFree,
                                       QSemaphore *pUsed,
                                       QQueue<QIndivPtr> *pResult,
                                       QObject *parent = 0);
protected:
    virtual void run();
    virtual void process_indiv(const QIndivPtr &oIndPtr);
public:
     QMap<IntType, InfoStatData> get_statistics(void) const {
         return (this->m_stats);
     }
signals:
    void indiv_acquired(QIndivPtr oInd);
public slots:
};// classQSerialIndivConsumerActor

//////////////////////////////////////////
}// namespace info
#endif // QSERIALINDIVPROVIDERACTOR_H
