#ifndef TCP_SCAN_THREAD_H
#define TCP_SCAN_THREAD_H
#include <QObject>
#include <QModbusTcpClient>
#include <QRunnable>
#include <QThreadPool>
#include <QTest>
#include "tcp_port_search.h"


class tcp_scan_thread : public QObject, public QRunnable
{
    Q_OBJECT
    public:
        explicit tcp_scan_thread(QString ipAddress,int timeout, int first_port, int last_port,  QObject *parent = 0);
    public slots:
    void run();
    void add_port_tcp(QString port);
    void for_progress_bar();
    void stop();
    signals:
        void send_port(QString port);
        void for_status_bar();
        void mod_dev(QModbusTcpClient *mod);
        void end();
        void stop_port();
    private:
        QString _ipAddress;
        int _timeout;
        int _first_port;
        bool runing = true;
        int _last_port;
        QModbusTcpClient *modbusTCPDevice;
        TCP_port_search *tcp;
        int p=0;
        void portScanTCP(QString ip, int port);
};

#endif // TCP_SCAN_THREAD_H
