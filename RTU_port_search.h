#ifndef RTU_PORT_SEARCH_H
#define RTU_PORT_SEARCH_H
#include <QObject>
#include <QModbusRtuSerialClient>
#include <QModbusClient>
#include <QRunnable>
#include <QTest>
#include <QtSerialBus/qtserialbusglobal.h>
#include <QSerialPort>
//
class RTU_port_search : public QObject, public QRunnable
{
    Q_OBJECT
    public:
        explicit RTU_port_search(QString comPort, int t_out,int parity, int baud, int data_bits, int stop_bits, int st_MB_addr,int en_MB_addr, QObject *parent = 0);

    void connect_rtu();
    void request(QString dev,int reg_addr,int mod_bus_addr);
    QString _comPort;
    QStringList added;
    int _time_out;
    int _parity;
    int _baud;
    int _data_bits;
    int _stop_bits;
    int _st_MB_addr;
    int _en_MB_addr;
    qint64 all_steps;
    qint64 step=0;
    bool found = true;
    bool stop_bit = false;
    public slots:
    void run();
    void responcecoil();
    void responcehold();
    void responcestatus();
    void responceinput();
    signals:
        void send_port_RTU(QString com_port);
        void for_progress_bar(int step);
    private:


        QModbusRtuSerialClient *modbusRTUDevice;

};


#endif // RTU_PORT_SEARCH_H
