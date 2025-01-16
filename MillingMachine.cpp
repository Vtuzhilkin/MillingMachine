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
    : QThread{parent}
{
    serialPort = new QSerialPort(this);
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(&timerUpdate, &QTimer::timeout, this, &MillingMachine::updateLeds);
    timerUpdate.setInterval(100);
    timerUpdate.start();

    mThread = true;
    start();
}

MillingMachine::~MillingMachine()
{
    serialPort->close();
    mThread = false;
    msleep(1000);
}

void MillingMachine::run()
{
    while(mThread){
        if(openedPort){

        }
        msleep(100);
    }
}

void MillingMachine::openCOM(int numberPort)
{
    serialPort->setPortName("COM" + QString::number(numberPort));
    openedPort = !serialPort->open(QIODevice::ReadWrite);
    messages.push_back(Message{'O'});
    emit updateLeds();
}

void MillingMachine::closeCOM()
{
    openedPort = false;
    serialPort->close();
}

bool MillingMachine::getStatusCOM()
{
    return openedPort;
}



