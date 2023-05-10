#include "tcp_scan_thread.h"
tcp_scan_thread::tcp_scan_thread(QString ipAddress,int timeout,int first_port, int last_port, QObject *parent) :
    QObject(parent),
    _ipAddress(ipAddress),
    _timeout(timeout),
    _first_port(first_port),
    _last_port(last_port)
{
}

void tcp_scan_thread::add_port_tcp(QString port)
{
    emit send_port(port);
}

void tcp_scan_thread::for_progress_bar()
{
    ++p;
    emit for_status_bar();
    if(p>(_last_port-_first_port)){    emit end();}
}

void tcp_scan_thread::stop()
{
    emit stop_port();
    runing=false;
}

void tcp_scan_thread::run()
{
    QThreadPool *pool = new QThreadPool();
    pool->setMaxThreadCount(600);
    int start_wile=_first_port;
    int end_while;
    while (runing) {
        QApplication::processEvents();
        if ((_last_port-start_wile)<256){
            tcp = new TCP_port_search(_ipAddress,start_wile,start_wile+(_last_port-start_wile),_timeout);
            connect(tcp,SIGNAL(send_port(QString)),this,SLOT(add_port_tcp(QString)));
            connect(tcp,SIGNAL(for_status_bar()),this,SLOT(for_progress_bar()));
            connect(this, SIGNAL(stop_port()),tcp,SLOT(stop_this()));
            pool->start(tcp);
            QTest::qWait(100);
            break;
        }
        end_while=start_wile+255;
        tcp = new TCP_port_search(_ipAddress,start_wile,end_while,_timeout);
        connect(tcp,SIGNAL(send_port(QString)),this,SLOT(add_port_tcp(QString)));
        connect(tcp,SIGNAL(for_status_bar()),this,SLOT(for_progress_bar()));
        connect(this, SIGNAL(stop_port()),tcp,SLOT(stop_this()));
        pool->start(tcp);
        QTest::qWait(100);
        start_wile=end_while+1;
    }

}
