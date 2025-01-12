#include "MillingMachine.h"

MillingMachine* MillingMachine::millingMachine = nullptr;

MillingMachine *MillingMachine::create(QQmlEngine *, QJSEngine *)
{
    return new MillingMachine();
}

MillingMachine *MillingMachine::instance()
{
    if(millingMachine == nullptr){
        millingMachine = new MillingMachine();
    }
    return millingMachine;
}

MillingMachine::MillingMachine(QObject *parent)
    : QObject{parent}
{
    //serialPort = new QSerialPort(this);
    //serialPort->setBaudRate(QSerialPort::Baud9600);
    //serialPort->setDataBits(QSerialPort::Data8);
    //serialPort->setParity(QSerialPort::NoParity);
    //serialPort->setStopBits(QSerialPort::OneStop);
    //serialPort->setFlowControl(QSerialPort::NoFlowControl);
}

void MillingMachine::connect(int numberPort)
{
    //serialPort->setPortName("COM" + QString::number(numberPort));
    openedPort = serialPort->open(QIODevice::ReadWrite);
    qDebug() << openedPort << " Here";
}



