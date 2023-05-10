#ifndef DEVICES_SCAN_H
#define DEVICES_SCAN_H

#include <QMainWindow>
#include <QObject>
#include <QtNetwork/QtNetwork>
#include <QModbusTcpClient>
#include <QModbusRtuSerialClient>
#include <QUrl>
#include <QTest>
#include <QTimer>
#include <QLabel>
#include <QSpinBox>
#include <QVector>
#include <QTableWidgetItem>
#include <QThreadPool>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QCheckBox>
#include <QProcess>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QProgressBar>
#include "spinboxdelegate.h"
#include "rtu_port_search.h"
//#include "tcp_port_search.h"
#include "tcp_scan_thread.h"


namespace Ui {
class devices_scan;
}

class devices_scan : public QMainWindow
{
    Q_OBJECT

public:
    explicit devices_scan(QWidget *parent = nullptr);
    ~devices_scan();
bool serial_tcp;
void subnets();
void tree_creation(QList<QString> sub);
void ip_port_tree_creation(QStringList list);
void init_COM_cb();
bool rtu_scan_COM(QString com);
QList<QString> subnet_list;
QList<QHostAddress> ipAddressesList;
QStringList list_of_ips;
QStringList list_of_port;
QStringList list_of_rtu;
QStringList list_of_checked_ips;
QStringList list_of_checked_ports;
QVector<QModbusTcpClient *> mod_vec;
QVector <TCP_port_search*> tcp_class_vec;
int range=0;
qint64 prog=0;
int checked_start=0;
int checked_end=0;
int steps_amount;
bool subnet_scan_in_process=false;
bool ip_port_scan_in_process=false;
bool rtu_port_scan_in_process=false;
bool end_scanone_ip=0;
bool scan_cycle_break=0;
void scan_port_button_slot();
private slots:
void destr(QObject *f);
void for_progress_bar_rtu(int i);
void add_port_tcp(QString port);
void add_port_rtu(QString port);
void end_scan_one_ip();
void on_scan_sub_but_clicked();
void on_scan_port_but_clicked();
void for_progress_bar();
void on_clear_sub_but_clicked();
void on_clear_pores_but_clicked();
void on_add_tcp_but_clicked();
void on_Scan_RTU_but_clicked();
void on_clear_rtu_but_clicked();
void on_add_rtu_but_clicked();
void on_send_but_clicked();
void on_refr_but_clicked();
void on_rtu_mod_addr_sb2_valueChanged(int arg1);
void on_rtu_mod_addr_sb1_valueChanged(int arg1);

signals:
   void send_list_rtu(QStringList list_of_rtu);
   void send_list_tcp(QStringList list_of_port);
   void stop();
private:
    Ui::devices_scan *ui;
    QTableWidgetItem *item1;
    QTableWidgetItem *item2;
    QProcess *pingProcess;
    QThreadPool *pool;
    QModbusRtuSerialClient *modbusRTUDevice;
    QThread *thr;
    QStandardItem *st_item;
    QStandardItemModel *st_model;
    QStandardItemModel *ip_port_model;
    SpinBoxDelegate *sb;
    SpinBoxDelegate *ip_port_delegate;
    QProgressBar *progress;
    RTU_port_search *RTU_class;
    tcp_scan_thread *tcp_thread;
    QLabel *label;
};

#endif // DEVICES_SCAN_H
