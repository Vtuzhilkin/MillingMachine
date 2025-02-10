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
        if(openedPort && operating){
            sendMessage();
        }
        if(!listCommands.empty()){
            operatingCommands();
        }
        msleep(100);
    }
}

void MillingMachine::sendMessage()
{
    bool sendMessage = false;
    // Если список не пустой то (мы можем отправить команду стоп) или (новые координаты если он откалиброван и стоит на месте)
    serialPort->flush();
    if(!messages.empty() && (messages.front().getCode() == 'S' || (statusMachine.calibrated && !statusMachine.moving))){
        sendMessage = (serialPort->write(messages.front().toQByteArray()) != -1);
    }else if(statusMachine.calibrated){
        sendMessage = (serialPort->write(Message{'P'}.toQByteArray()) != -1);
    }

    bool reciveMessage = false;
    Message response('0');
    if(sendMessage){
        if(serialPort->waitForBytesWritten(100) && serialPort->waitForReadyRead(100)){
            response = Message(serialPort->readAll());
            if(response.checkCRC()){
                messages.pop_front();
                reciveMessage = true;
            }
        }else{
            openedPort = false;
        }
    }

    if(reciveMessage){
        QVector<unsigned char> data = response.getData();
        switch (response.getCode()) {
        case 'P':
            statusMachine.xCoordinate = (float)((data[0] << 8) | data[1]) / 100.0f;
            statusMachine.yCoordinate = (float)((data[2] << 8) | data[3]) / 100.0f;
            statusMachine.zCoordinate = (float)((data[4] << 8) | data[5]) / 100.0f;
            statusMachine.velocity = (float)((data[6] << 8) | data[7]) / 100.0f;
            statusMachine.calibrated = bool(data[9]);
            statusMachine.moving = bool(data[10]);
            break;
        case 'C':
            openedPort = false;
        default:
            break;
        }
    }
}

void MillingMachine::operatingMessage(const Message& message){

}

void MillingMachine::openCOM(int numberPort)
{
    serialPort->setPortName("COM" + QString::number(numberPort));
    openedPort = serialPort->open(QIODevice::ReadWrite);
    if(openedPort){
        messages.push_front(Message{'O'});
    }
}

void MillingMachine::closeCOM()
{
    QMutexLocker locker(&m_Operating);
    listCommands.clear();
    messages.clear();
    messages.push_back(Message{'S'});
    messages.push_back(Message{'C'});
}

bool MillingMachine::getStatusCOM()
{
    return openedPort;
}

bool MillingMachine::getCalibrated()
{
    return statusMachine.calibrated;
}

bool MillingMachine::getProcessing()
{
    return statusMachine.moving;
}

void MillingMachine::startMilling(const QStringList& lCommands)
{
    QMutexLocker locker(&m_Operating);
    if(listCommands.empty() && statusMachine.calibrated && !statusMachine.moving){
        listCommands = lCommands;
        operating = true;
    }
}

void MillingMachine::stopMilling()
{
    QMutexLocker locker(&m_Operating);
    listCommands.clear();
    messages.clear();
    messages.push_front(Message{'S'});
}

void MillingMachine::continueMilling()
{
    operating = true;
}

void MillingMachine::pauseMilling()
{
    QMutexLocker locker(&m_Operating);
    messages.push_front('S');
}

void MillingMachine::changeVelocity(float velocity)
{
    QMutexLocker locker(&m_Operating);
    if(listCommands.empty() && messages.empty() && statusMachine.calibrated && !statusMachine.moving){
        QVector<unsigned char> velocity_data;
        formatedNumber(velocity, velocity_data);
        velocity_data.pop_front();
        messages.push_front({'V', static_cast<unsigned char>(velocity_data.size()), velocity_data});
    }
}


float MillingMachine::getXCoordiate(){
    return statusMachine.xCoordinate;
}

float MillingMachine::getYCoordiate(){
    return statusMachine.yCoordinate;
}

float MillingMachine::getZCoordiate(){
    return statusMachine.zCoordinate;
}

void MillingMachine::operatingCommands()
{
    std::string line;
    {
        QMutexLocker locker(&m_Operating);
        line = listCommands.front().toStdString();
        listCommands.pop_front();
    }
    GCode gcode;
    std::regex regex(R"(N(\d+)\s*G?(\d+)?\s*(X?([-\d.]*)\s*)?\s*(Y?([-\d.]*)\s*)?\s*(Z?([-\d.]*)\s*)?(I([-\d.]+))?\s*(J([-\d.]+))?\s*(K([-\d.]+))?)");
    std::smatch match;

    if (std::regex_search(line, match, regex)) {
        // Получаем номер строки
        gcode.lineNumber = std::stoi(match[1].str());

        // Получаем код функции G01 или G02
        if(!match[2].str().empty()){
            gcode.function = std::stoi(match[2].str());
        }else{
            gcode.function = previousGCode.function;
        }

        // Получаем координаты X, Y
        if (!match[3].str().empty()) {
            gcode.X = std::stof(match[4].str());
        }else{
            gcode.X = previousGCode.X;
        }

        if (!match[5].str().empty()) {
            gcode.Y = std::stof(match[6].str());
        }else{
            gcode.Y = previousGCode.Y;
        }

        // Если есть координата Z, получаем её
        if (!match[7].str().empty()) {
            gcode.Z = std::stof(match[8].str());
        }else{
            gcode.Z = previousGCode.Z;
        }
        // Если это G02, то получаем I, J, K
        if (gcode.function == 2) {
            if (!match[9].str().empty()) gcode.I = std::stof(match[10].str());
            if (!match[11].str().empty()) gcode.J = std::stof(match[12].str());
            if (!match[13].str().empty()) gcode.K = std::stof(match[14].str());
        }
    }
    addMessage(gcode, previousGCode);
    previousGCode = gcode;
}


void MillingMachine::addMessage(const GCode& gcode, const GCode& previousGCode){
    if(gcode.function == 1){
        QVector<unsigned char> result;
        formatedNumber(gcode.X, result);
        formatedNumber(gcode.Y, result);
        formatedNumber(gcode.Z, result);
        messages.push_back(Message{'N', static_cast<unsigned char>(result.size()), result});
        qDebug() << gcode.X << " " << gcode.Y << " " << gcode.Z;
    }else if(gcode.function == 2 || gcode.function == 3){
        Arc arc(Point{previousGCode.X, previousGCode.Y, previousGCode.Z},
                Point{gcode.X, gcode.Y, gcode.Z},
                Point{gcode.X + gcode.I, gcode.Y + gcode.J, gcode.Z + gcode.K}, gcode.function == 2);
        for(const Point& point: arc.getArcPoints()){
            QVector<unsigned char> coordiates_data;
            formatedNumber(point.x, coordiates_data);
            formatedNumber(point.y, coordiates_data);
            formatedNumber(point.z, coordiates_data);
            {
                QMutexLocker locker(&m_Operating);
                messages.push_back(Message{'N', static_cast<unsigned char>(coordiates_data.size()), coordiates_data});
            }
        }
    }
}

void MillingMachine::formatedNumber(float num, QVector<unsigned char>& data)
{
    int16_t num_int = num*100;
    data.push_back((uint8_t)(num_int & 0xFF));
    data.push_back((uint8_t)((num_int >> 8) & 0xFF));
}



