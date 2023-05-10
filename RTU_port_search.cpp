#include "rtu_port_search.h"

RTU_port_search::RTU_port_search(QString comPort, int t_out,int parity, int baud, int data_bits,int stop_bits, int st_MB_addr,int en_MB_addr, QObject *parent) :
    QObject(parent),
    _comPort(comPort),
    _time_out(t_out),
    _parity(parity),
    _baud(baud),
    _data_bits(data_bits),
    _stop_bits(stop_bits),
    _st_MB_addr(st_MB_addr),
    _en_MB_addr(en_MB_addr)
{
}

void RTU_port_search::connect_rtu()
{
if(!modbusRTUDevice){
    modbusRTUDevice = new QModbusRtuSerialClient;
    modbusRTUDevice->setObjectName(_comPort+":"+QString::number(_parity)
                                           +":"+QString::number(_baud)
                                           +":"+QString::number(_data_bits)
                                           +":"+QString::number(_stop_bits));

    if (modbusRTUDevice->state() != QModbusDevice::ConnectedState){
        modbusRTUDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
            _comPort);
        modbusRTUDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
            _parity);
        modbusRTUDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
            _baud);
        modbusRTUDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
            _data_bits);
        modbusRTUDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
            _stop_bits);
        modbusRTUDevice->setTimeout(500);
        modbusRTUDevice->setNumberOfRetries(0);
        modbusRTUDevice->connectDevice();
        QTest::qWait(2000);
        if(modbusRTUDevice->state()==QModbusDevice::ConnectedState){
        }else{
            qDebug()<<modbusRTUDevice->error();
            modbusRTUDevice->disconnectDevice();

        }
    }
}
}

void RTU_port_search::request(QString dev,int reg_addr,int mod_bus_addr)
{
        QModbusDataUnit readUnit(QModbusDataUnit::DiscreteInputs,reg_addr,1);
        QModbusReply *lastRequest = modbusRTUDevice->sendReadRequest(readUnit,mod_bus_addr);
        connect(lastRequest,SIGNAL(finished()), this, SLOT(responcestatus()));
        lastRequest->setObjectName(dev+";"+QString::number(mod_bus_addr));
        if(!lastRequest->isFinished()) ;
        else lastRequest->deleteLater();
        QTest::qWait(100);

        QModbusDataUnit readUnit2(QModbusDataUnit::Coils,reg_addr, 1);
        QModbusReply *lastRequest2 = modbusRTUDevice->sendReadRequest(readUnit2,mod_bus_addr);
        connect(lastRequest2,SIGNAL(finished()), this, SLOT(responcecoil()));
        lastRequest2->setObjectName(dev+";"+QString::number(mod_bus_addr));
        if(!lastRequest2->isFinished());
        else lastRequest2->deleteLater();

        QTest::qWait(100);
        QModbusDataUnit readUnit3(QModbusDataUnit::InputRegisters,reg_addr, 1);
        QModbusReply *lastRequest3 =modbusRTUDevice->sendReadRequest(readUnit3,mod_bus_addr);
        connect(lastRequest3,SIGNAL(finished()), this, SLOT(responceinput()));
        lastRequest3->setObjectName(dev+";"+QString::number(mod_bus_addr));
        if(!lastRequest3->isFinished()) ;
        else lastRequest3->deleteLater();

        QTest::qWait(100);
        QModbusDataUnit readUnit4(QModbusDataUnit::HoldingRegisters,reg_addr, 1);
        QModbusReply *lastRequest4 = modbusRTUDevice->sendReadRequest(readUnit4,mod_bus_addr);
        lastRequest4->setObjectName(dev+";"+QString::number(mod_bus_addr));
        connect(lastRequest4,SIGNAL(finished()), this, SLOT(responcehold()));
        if(!lastRequest4->isFinished())    ;
        else lastRequest4->deleteLater();
}

void RTU_port_search::responcecoil()
{

    QModbusReply *lastRequest = qobject_cast <QModbusReply *> (sender());
    QString name=lastRequest->objectName();
    if (!lastRequest) return;
     if(lastRequest->error() == QModbusDevice::NoError)
     {
        if(added.contains(name))return;
        QModbusDataUnit unit = lastRequest->result();
        emit send_port_RTU(name);
        added.append(name);
        found = false;
        lastRequest->deleteLater();
     }
     lastRequest->deleteLater();
}

void RTU_port_search::responcehold()
{
    QModbusReply *lastRequest = qobject_cast <QModbusReply *> (sender());
    QString name=lastRequest->objectName();
     if (!lastRequest) return;
    if(lastRequest->error() == QModbusDevice::NoError)
    {
        if(added.contains(name))return;
        QModbusDataUnit unit = lastRequest->result();
        emit send_port_RTU(name);
        found = false;
        lastRequest->deleteLater();
    }
    lastRequest->deleteLater();
}

void RTU_port_search::responcestatus()
{
    QModbusReply *lastRequest = qobject_cast <QModbusReply *> (sender());
    if (!lastRequest) return;
    QString name=lastRequest->objectName();
    if(lastRequest->error() == QModbusDevice::NoError)
    {
        if(added.contains(name))return;
        QModbusDataUnit unit = lastRequest->result();
        emit send_port_RTU(name);
        found = false;
        lastRequest->deleteLater();
    }
    lastRequest->deleteLater();
}

void RTU_port_search::responceinput()
{
    QModbusReply *lastRequest = qobject_cast <QModbusReply *> (sender());
    if (!lastRequest) return;
    QString name=lastRequest->objectName();
    if(lastRequest->error() == QModbusDevice::NoError)
    {
        if(added.contains(name))return;
        QModbusDataUnit unit = lastRequest->result();
        emit send_port_RTU(name);
        found = false;
        lastRequest->deleteLater();
    }
    lastRequest->deleteLater();
}

void RTU_port_search::run()
{
    all_steps=((_en_MB_addr-_st_MB_addr)+1)*65535;
    qDebug()<<all_steps;
    added.clear();
    found = true;
    connect_rtu();
    QString name = modbusRTUDevice->objectName();
    QTest::qWait(1000);
    for(int k=_st_MB_addr;k<_en_MB_addr+1;k++){
            for(int j=0;j<65536;j++){
                if (stop_bit){
                   step=all_steps;
                   emit for_progress_bar(step);
                   goto enddd;
                }
                QApplication::processEvents();
                QTest::qWait(50);
                QApplication::processEvents();
                if(found){
                    request(name,j,k);
                    QTest::qWait(500);
                    step++;
                    emit for_progress_bar(step);
                }else{
                    step=step +(65535-j);
                    emit for_progress_bar(step);
                    QTest::qWait(500);
                    found=true;
                    j=65536;
                    found=true;
                }
            }
    }
enddd:
 modbusRTUDevice->disconnectDevice();
 QTest::qWait(5000);
}


