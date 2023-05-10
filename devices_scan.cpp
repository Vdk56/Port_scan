#include "devices_scan.h"
#include "ui_devices_scan.h"

devices_scan::devices_scan(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::devices_scan)
{
    ui->setupUi(this);

    subnets();
    init_COM_cb();
    QThreadPool::globalInstance()->setMaxThreadCount(600);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setColumnWidth(0,158);
    ui->tableWidget->setColumnWidth(1,101);
    ui->tableWidget->verticalHeader()->hide();

    ui->clear_rtu_but->setDisabled(1);
    ui->add_rtu_but->setDisabled(1);

    progress = new QProgressBar;
    progress->setValue(0);
    progress->setMaximumHeight(15);
    progress->setMaximumWidth(200);
    label = new QLabel;
    statusBar()->addWidget(progress);
    statusBar()->addWidget(label);
    progress->hide();

    ui->add_tcp_but->setDisabled(1);
    ui->scan_port_but->setDisabled(1);
    ui->clear_pores_but->setDisabled(1);
    ui->send_but->setDisabled(1);
}

devices_scan::~devices_scan()
{
    delete ui;
}

void devices_scan::subnets()// Отображает все подсети
{
    //ipAddressesList.at(i) != QHostAddress::LocalHost&&
    QString tempAddress;
    ipAddressesList = QNetworkInterface::allAddresses();
    int count = ipAddressesList.count();
    for (int i=0; i<count; i++)
    {
        if (ipAddressesList.at(i).toIPv4Address())
        {
            tempAddress = ipAddressesList.at(i).toString();
            QStringList tempMask = tempAddress.split(".");
            tempMask.removeLast();
            tempAddress = tempMask.join(".");
            if (!subnet_list.contains(tempAddress))
            {
                subnet_list.append(tempAddress);

            }
        }
    }
    tree_creation(subnet_list);
}

void devices_scan::tree_creation(QList<QString> sub)
{
    st_model = new QStandardItemModel(sub.count(),3);
    sb = new SpinBoxDelegate(false);
    ui->treeView->setItemDelegate(sb);
    ui->treeView->setModel(st_model);
    for (int i = 0; i < sub.count(); ++i) {
        QModelIndex mod_in1;
        QModelIndex mod_in2;

        mod_in1 = st_model->index(i,1);
        mod_in2 = st_model->index(i,2);

        st_model->setData(mod_in1,0,Qt::EditRole);
        st_model->setData(mod_in2,255,Qt::EditRole);

        st_item=new QStandardItem(sub[i]);
        st_item->setEditable(0);
        st_item->setCheckable(1);
        st_item->setCheckState(Qt::Unchecked);
        st_model->setHeaderData(0,Qt::Horizontal,"Подсети",Qt::DisplayRole);
        st_model->setHeaderData(1,Qt::Horizontal,"От",Qt::DisplayRole);
        st_model->setHeaderData(2,Qt::Horizontal,"До",Qt::DisplayRole);
        st_model->setItem(i,0,st_item);

    }
    ui->treeView->header()->setStretchLastSection(0);
    ui->treeView->setColumnWidth(0,155);
    ui->treeView->setColumnWidth(1,45);
    ui->treeView->setColumnWidth(2,45);

}

void devices_scan::ip_port_tree_creation(QStringList list)
{
    ip_port_model = new QStandardItemModel(list.count(),3);
    ip_port_delegate = new SpinBoxDelegate(true);
    ui->treeView_2->setItemDelegate(ip_port_delegate);
    ui->treeView_2->setModel(ip_port_model);
    for (int i = 0; i < list.count(); ++i) {
        QModelIndex mod_in1;
        QModelIndex mod_in2;

        mod_in1 = ip_port_model->index(i,1);
        mod_in2 = ip_port_model->index(i,2);

        ip_port_model->setData(mod_in1,0,Qt::EditRole);
        ip_port_model->setData(mod_in2,65535,Qt::EditRole);

        st_item=new QStandardItem(list[i]);
        st_item->setEditable(0);
        st_item->setCheckable(1);
        st_item->setCheckState(Qt::Unchecked);
        ip_port_model->setHeaderData(0,Qt::Horizontal,"IP Адреса",Qt::DisplayRole);
        ip_port_model->setHeaderData(1,Qt::Horizontal,"От",Qt::DisplayRole);
        ip_port_model->setHeaderData(2,Qt::Horizontal,"До",Qt::DisplayRole);
        ip_port_model->setItem(i,0,st_item);
    }
    ui->treeView_2->header()->setStretchLastSection(0);
    ui->treeView_2->setColumnWidth(0,155);
    ui->treeView_2->setColumnWidth(1,50);
    ui->treeView_2->setColumnWidth(2,50);
}

void devices_scan::init_COM_cb()
{
    QString com_p;
    for (int i = 0; i < 12; ++i) {
        com_p = "COM"+QString::number(i+1);
        if(rtu_scan_COM(com_p)){
            ui->COM_cb->addItem(com_p);
        }
    }
}

bool devices_scan::rtu_scan_COM(QString com)
{
    modbusRTUDevice = new QModbusRtuSerialClient;
    if (modbusRTUDevice->state() != QModbusDevice::ConnectedState){
        modbusRTUDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
            com);
        modbusRTUDevice->connectDevice();
        QTest::qWait(150);
        if(modbusRTUDevice->state()==QModbusDevice::ConnectedState){

                modbusRTUDevice->disconnectDevice();
                modbusRTUDevice->deleteLater();

            return true;
        }else{
            modbusRTUDevice->deleteLater();
            return false;
        }
    }
    return true;
}

void devices_scan::destr(QObject *f)
{
    qDebug()<<"Удалён";
}

void devices_scan::add_port_tcp(QString port)// Это слот, в который прилетает ip и порт из класса tcp_port_search в случае если соединение установлено.
//Если пусто добавляет объект в топ левел, далее проверка есть ли уже в топ левеле такой ip.
//Если нет - добавляет, а если есть до добавляет child к существующему
{
    QStringList ip_port = port.split(":");
    int position = 0;
    if(!list_of_port.contains(port)){
        for (int var = 0; var < ip_port_model->rowCount(); ++var) {
            if(ip_port[0]==ip_port_model->index(var,0).data().toString()){
               position=var;
            }
        }
        st_item=new QStandardItem(ip_port[1]);
        st_item->setEditable(0);
        st_item->setCheckable(1);
        st_item->setCheckState(Qt::Unchecked);
        ip_port_model->item(position,0)->setChild(ip_port_model->item(position,0)->rowCount(),0,st_item);
        list_of_port.append(port);
        return;
    position=0;
    }
}

void devices_scan::add_port_rtu(QString port)//Слот аналогичный слоту выше, только для последовательного порта
{
    QStringList port_par = port.split(";");

    if(ui->tableWidget->rowCount()==0){
        list_of_rtu.clear();
        item1 = new QTableWidgetItem(port_par[0]);
        item1->setCheckState(Qt::Unchecked);
        item2 = new QTableWidgetItem(port_par[1]);
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0,0,item1);
        ui->tableWidget->setItem(0,1,item2);
        list_of_rtu.append(port);
    }
    if(!list_of_rtu.contains(port)){
        item1 = new QTableWidgetItem(port_par[0]);
        item1->setCheckState(Qt::Unchecked);
        item2 = new QTableWidgetItem(port_par[1]);
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,item1);
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,item2);
       list_of_rtu.append(port);
    }
}

void devices_scan::end_scan_one_ip()
{
    end_scanone_ip=true;
    checked_end++;

    if(checked_end>=checked_start){
        scan_cycle_break=true;
        ip_port_scan_in_process = false;
    }
}

void devices_scan::on_scan_sub_but_clicked()//Слот запускается от кнопки и запускает пинг всех адресов выбраных подсетей
//Также сдесь блокируются кнопки, которые не стоит нажимать во время выполнения этого слота. Так же, исходя из количества выбраных подсетей,
//устанавливается число повторений цикла, которое ещё используется для работы progress bar
{
    QString t_add;
    range=0;
    QApplication::processEvents();
    if(subnet_scan_in_process){
        ui->scan_sub_but->setText("Сканировать подсети");
        subnet_scan_in_process=false;
    }
    else{
        ui->scan_sub_but->setText("Стоп");
       subnet_scan_in_process=true;
    }
    ui->add_tcp_but->setDisabled(1);
    ui->scan_port_but->setDisabled(1);
    ui->clear_pores_but->setDisabled(1);
    ui->send_but->setDisabled(1);
    progress->setValue(0);
    steps_amount=0;
    progress->show();
    for (int i = 0; i < st_model->rowCount(); ++i) {
        if(st_model->item(i,0)->checkState()){
            int j = st_model->index(i,2).data().toInt()-st_model->index(i,1).data().toInt();
            steps_amount = steps_amount + j+1;
        }
    }
    progress->setRange(0,steps_amount);

    for (int i = 0; i < st_model->rowCount(); ++i) {
        if(st_model->item(i,0)->checkState()){
            for (int j = st_model->index(i,1).data().toInt(); j < st_model->index(i,2).data().toInt()+1; ++j) {
                QApplication::processEvents();
                if(subnet_scan_in_process){

                        t_add = st_model->index(i,0).data().toString()+"."+QString::number(j);
                        int exitCode = pingProcess->execute("ping", QStringList() << "-n" << "1" <<"-w"<<"5"<<t_add);
                        if (exitCode==0){
                            if(!list_of_ips.contains(t_add)){

                                st_item=new QStandardItem(t_add);
                                st_item->setEditable(0);
                                st_item->setCheckable(1);
                                st_item->setCheckState(Qt::Unchecked);
                                st_model->item(i,0)->setChild(st_model->item(i,0)->rowCount(),0,st_item);

                                list_of_ips.append(t_add);
                            }
                        }
                    range++;
                    progress->setValue(range);
                }else{
                    if(list_of_ips.count()>0){
                         ui->send_but->setDisabled(0);
                        ui->send_but->setEnabled(1);
                    }
                    return;
                }
            }
        }
    }
    if(list_of_ips.count()>0){
        ui->send_but->setDisabled(0);
        ui->send_but->setEnabled(1);
    }
    ui->send_but->setDisabled(0);
    ui->send_but->setEnabled(1);
    ui->scan_sub_but->setText("Сканировать подсети");
    subnet_scan_in_process=false;
}

void devices_scan::on_scan_port_but_clicked()//В этом слоте происходит скан портов выбраных ip
{
    QString t_port;
    QApplication::processEvents();
    if(ip_port_scan_in_process){
        ui->scan_port_but->setText("Сканировать порты");
        ui->clear_pores_but->setEnabled(1);
        if(list_of_port.count()>0)ui->add_tcp_but->setEnabled(1);
        ip_port_scan_in_process=false;
        emit stop();
    }
    else{
        ui->scan_port_but->setText("Стоп");
       ip_port_scan_in_process=true;
    }
    range=0;
    checked_end=0;
    checked_start=0;
    progress->setValue(0);
    steps_amount=0;
    int start;
    int stop;
    end_scanone_ip=false;
    scan_cycle_break=false;
    ui->add_tcp_but->setDisabled(1);
    ui->clear_pores_but->setDisabled(1);
    checked_start=0;
    for (int i = 0; i < ip_port_model->rowCount(); ++i) {
        if(ip_port_model->item(i,0)->checkState()){
            int j = ip_port_model->index(i,2).data().toInt()-ip_port_model->index(i,1).data().toInt();
            steps_amount = steps_amount + j+1;
            checked_start++;
        }
    }
    progress->setRange(0,steps_amount);
    for (int var = 0; var <ip_port_model->rowCount(); ++var) {
        if(ip_port_model->item(var,0)->checkState()){
                QApplication::processEvents();
                if(ip_port_scan_in_process){
                    start = ip_port_model->index(var,1).data().toInt();
                    stop = ip_port_model->index(var,2).data().toInt();
                    t_port = ip_port_model->index(var,0).data().toString();
                    thr = new QThread();
                    tcp_thread = new tcp_scan_thread(t_port,ui->t_out_tcp->value(),start,stop);
                    connect(tcp_thread,SIGNAL(send_port(QString)),this,SLOT(add_port_tcp(QString)));
                    connect(this,SIGNAL(stop()),tcp_thread,SLOT(stop()));
                    connect(thr, SIGNAL(finished()),tcp_thread,SLOT(deleteLater()));
                    connect(thr, &QThread::finished, thr,&QThread::deleteLater);
                    connect(thr, SIGNAL(started()), tcp_thread,SLOT(run()));
                    connect(tcp_thread,SIGNAL(for_status_bar()),this,SLOT(for_progress_bar()));
                    connect(tcp_thread,SIGNAL(end()),this,SLOT(end_scan_one_ip()));
                    tcp_thread->moveToThread(thr);
                    thr->start();
                    while (!end_scanone_ip&ip_port_scan_in_process) {
                        QTest::qWait(100);
                    }
                    thr->terminate();
                    end_scanone_ip=false;
                    if(scan_cycle_break) break;
                }else{
                    thr->terminate();
                    end_scanone_ip=false;
                     scan_cycle_break=false;
                     break;
                }
        }
    }
    scan_cycle_break=false;
    ui->scan_port_but->setText("Сканировать порты");
    ui->clear_pores_but->setEnabled(1);
    ip_port_scan_in_process=false;
}

void devices_scan::for_progress_bar()
{
    range++;
    progress->setValue(range);
    if(range>(progress->maximum()-1)){
        ui->add_tcp_but->setEnabled(1);
        ui->clear_pores_but->setEnabled(1);
    }
}

void devices_scan::on_clear_sub_but_clicked()
{
    st_model->clear();
    st_model->deleteLater();
    list_of_ips.clear();
    subnets();
}

void devices_scan::on_clear_pores_but_clicked()
{
    ip_port_model->clear();
    ip_port_model->deleteLater();
    ui->clear_pores_but->setDisabled(1);
    list_of_checked_ips.clear();
}

void devices_scan::on_add_tcp_but_clicked()
{
    for (int i = 0; i < ip_port_model->rowCount(); ++i) {
        if(ip_port_model->item(i,0)->checkState()){
            for (int j = 0; j < ip_port_model->item(i,0)->rowCount(); ++j) {
                if(ip_port_model->item(i,0)->child(j,0)->checkState()){
                    list_of_checked_ports<<ip_port_model->item(i,0)->child(j,0)->text();
                }
            }
        }
    }
    emit send_list_tcp(list_of_checked_ports);
    list_of_port.clear();
    list_of_ips.clear();
    list_of_checked_ports.clear();
    this->close();
}

void devices_scan::on_Scan_RTU_but_clicked()
{
    if(rtu_port_scan_in_process){
        ui->Scan_RTU_but->setText("Сканировать");
        if(list_of_rtu.count()>0)ui->clear_rtu_but->setEnabled(1);
        if(list_of_rtu.count()>0)ui->add_rtu_but->setEnabled(1);
        rtu_port_scan_in_process=false;
        RTU_class->stop_bit=1;
        list_of_rtu.clear();
        return;
    }
    else{
        ui->Scan_RTU_but->setText("Стоп");
        rtu_port_scan_in_process=true;
        ui->clear_rtu_but->setDisabled(1);
        ui->add_rtu_but->setDisabled(1);
    }
if(rtu_port_scan_in_process){
    prog=0;
    int parity;
    int data_bits = ui->Data_bits_cb->currentText().toInt();
    int stop_bits = ui->Stop_bits_cb->currentText().toInt();
    int first_addr = ui->rtu_mod_addr_sb1->value();
    int last_addr = ui->rtu_mod_addr_sb2->value();
    prog = ((last_addr-first_addr)+1)*65535;
    progress->setValue(0);
    thr = new QThread;
    qDebug()<<1;
    switch (ui->Parity_cb->currentIndex()) {
    case 0:
        parity = 0;
        break;
    case 1:
        parity = 3;
        break;
    case 2:
        parity = 2;
        break;
    }
    progress->setRange(0,prog);
    progress->show();
    RTU_class = new RTU_port_search(ui->COM_cb->currentText(),ui->timeout_line->text().toInt(),
                                                    parity,ui->Baud_cb->currentText().toInt(),
                                                     data_bits,stop_bits,first_addr,last_addr);
    connect(RTU_class,SIGNAL(for_progress_bar(int)),this,SLOT(for_progress_bar_rtu(int)));
    connect(RTU_class,SIGNAL(send_port_RTU(QString)),this,SLOT(add_port_rtu(QString)));
    connect(thr, SIGNAL(finished()),RTU_class,SLOT(deleteLater()));
    connect(thr, &QThread::finished, thr,&QThread::deleteLater);
    connect(thr, SIGNAL(started()), RTU_class,SLOT(run()));
    RTU_class->moveToThread(thr);
    thr->start();

}
}

void devices_scan::for_progress_bar_rtu(int i)
{
    label->setText(QString::number(i)+" из "+QString::number(prog));
    progress->setValue(i);
}


void devices_scan::on_clear_rtu_but_clicked()
{
    list_of_rtu.clear();
    for(int i=0;i<ui->tableWidget->rowCount();++i){
        ui->tableWidget->removeRow(0);
    }
    ui->clear_rtu_but->setDisabled(1);
    ui->add_rtu_but->setDisabled(1);
}

void devices_scan::on_add_rtu_but_clicked()
{
    list_of_rtu.clear();
    for (int var = 0; var < ui->tableWidget->rowCount(); ++var) {
        if(ui->tableWidget->item(var,0)->checkState()){
            QString temp = "";
            for (int i = 0; i < 5; ++i) {
               temp.append(ui->tableWidget->item(var,i)->text());
               if(i<4)temp.append(":");
            }
            list_of_rtu.append(temp);
            temp.~QString();
        }
    }
    emit send_list_rtu(list_of_rtu);
    list_of_rtu.clear();
    ui->tableWidget->clear();
    this->close();
}

void devices_scan::on_send_but_clicked()
{
    for (int i = 0; i < st_model->rowCount(); ++i) {
        if(st_model->item(i,0)->checkState()){
            for (int j = 0; j < st_model->item(i,0)->rowCount(); ++j) {
                if(st_model->item(i,0)->child(j,0)->checkState()){
                    list_of_checked_ips<<st_model->item(i,0)->child(j,0)->text();
                }
            }
        }
    }
    ui->scan_port_but->setEnabled(1);
    ui->clear_pores_but->setEnabled(1);
    ip_port_tree_creation(list_of_checked_ips);
}


void devices_scan::on_refr_but_clicked()
{
    init_COM_cb();
}


void devices_scan::on_rtu_mod_addr_sb2_valueChanged(int arg1)
{
    if(arg1<ui->rtu_mod_addr_sb1->value()) ui->rtu_mod_addr_sb2->setValue(ui->rtu_mod_addr_sb1->value());
}


void devices_scan::on_rtu_mod_addr_sb1_valueChanged(int arg1)
{
    if(arg1>ui->rtu_mod_addr_sb2->value()) ui->rtu_mod_addr_sb1->setValue(ui->rtu_mod_addr_sb2->value());
}

