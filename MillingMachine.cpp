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
    connect(&timerUpdate, &QTimer::timeout, this, &MillingMachine::updateLeds);
    connect(&timerUpdate, &QTimer::timeout, this, &MillingMachine::updateCoordiantes);
    timerUpdate.setInterval(30);
    timerUpdate.start();

    mThread = true;
    start();
}

MillingMachine::~MillingMachine()
{
    mThread = false;
    msleep(1000);
}

void MillingMachine::run()
{
    QSerialPort* serialPort = new QSerialPort(this);
    serialPort->setBaudRate(QSerialPort::Baud38400);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    while(mThread){
        if(connectDivece){
            if(openedPort){
                sendMessage(serialPort);
            }else{
                connectDevice(serialPort);
            }
            if(!listCommands.empty()){
                operatingCommands();
            }
        }
        msleep(10);
    }

    serialPort->close();
}

void MillingMachine::sendMessage(QSerialPort* serialPort)
{
    QMutexLocker locker(&m_Operating);
    bool sendMessage = false;
    int nTry = 3;
    // Если список не пустой то (мы можем отправить команду стоп) или (новые координаты если он откалиброван и стоит на месте)
    serialPort->flush();
    bool shouldSend = !messages.empty() && (messages.front().getCode() == 'S' ||
                                            messages.front().getCode() == 'C' ||
                                            messages.front().getCode() == 'Z' ||
                                            (statusMachine.calibrated && statusMachine.sendNextCoordinate));
    Message command('0');
    if (shouldSend) {
        command = messages.front();
    }else{
        command = Message('P');
    }

    while (!sendMessage && nTry) {
        sendMessage = (serialPort->write(command.toQByteArray().data(), 20) != -1);
        serialPort->waitForBytesWritten(1000);
        nTry--;
    }

    bool reciveMessage = false;
    Message response('0');
    if(sendMessage){
        if(serialPort->waitForReadyRead(1000)){
            response = Message(serialPort->readAll());
            if(response.checkCRC()){
                if(command.getCode() != 'P'){
                    messages.pop_front();
                }
                reciveMessage = true;
            }
        }else{
            serialPort->close();
            openedPort = false;
        }
    }

    if(reciveMessage){
        QVector<uint8_t> data = response.getData();
        switch (response.getCode()) {
        case 'P':
            statusMachine.xCoordinate = (data[2] & 0b10000000) ? -1 : 1;
            statusMachine.xCoordinate *= (float)(((data[2] & 0b01111111) << 16) | (data[1] << 8) | data[0])/ 100.0f;

            statusMachine.yCoordinate = (data[5] & 0b10000000) ? -1 : 1;
            statusMachine.yCoordinate *= (float)(((data[5] & 0b01111111) << 16) | (data[4] << 8) | data[3]) / 100.0f;

            statusMachine.zCoordinate = (data[8] & 0b10000000) ? -1 : 1;
            statusMachine.zCoordinate *= (float)(((data[8] & 0b01111111) << 16) | (data[7] << 8) | data[6]) / 100.0f;

            statusMachine.velocity = (float)((data[10] << 8) | data[9]) / 100.0f;
            statusMachine.calibrated = data[11] && 1;
            statusMachine.moving = (data[11] >> 1) && 1;
            statusMachine.sendNextCoordinate = (data[11] >> 2) && 1;
            break;
        case 'C':
            serialPort->close();
            openedPort = false;
            connectDivece = false;
            break;
        default:
            break;
        }
    }
}

void MillingMachine::connectDevice(QSerialPort* serialPort)
{
    serialPort->setPortName("COM" + QString::number(numberPort));
    serialPort->close();
    if(serialPort->open(QIODevice::ReadWrite)){
        serialPort->flush();
        serialPort->write(Message{'O'}.toQByteArray().data(), 20);
        serialPort->waitForBytesWritten(1000);
        if(serialPort->waitForReadyRead(1000)){
            Message response = Message(serialPort->readAll());
            if(response.checkCRC() && response.getCode() == 'O'){
                openedPort = true;
            }
        }
    }
}

void MillingMachine::openCOM(int numPort)
{
    QMutexLocker locker(&m_Connect);
    connectDivece = true;
    numberPort = numPort;
}

void MillingMachine::closeCOM()
{
    listCommands.clear();
    messages.clear();
    messages.push_front(Message{'C'});
    messages.push_front(Message{'S'});
}

bool MillingMachine::getStatusCOM()
{
    return openedPort;
}

bool MillingMachine::getCalibrated()
{
    return statusMachine.calibrated && openedPort;
}

bool MillingMachine::getProcessing()
{
    return statusMachine.moving;
}

void MillingMachine::startMilling(const QStringList& lCommands)
{
    QMutexLocker locker(&m_Operating);
    if(listCommands.empty() && !statusMachine.moving){
        if(statusMachine.calibrated){
            listCommands = lCommands;
            operating = true;
        }else{
            messages.push_front(Message{'Z'});
        }
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
        QVector<uint8_t> velocity_data;
        formatedNumber(velocity, velocity_data);
        velocity_data.pop_front();
        messages.push_front({'V', static_cast<uint8_t>(velocity_data.size()), velocity_data});
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
        QVector<uint8_t> result;
        formatedNumber(gcode.X, result);
        formatedNumber(gcode.Y, result);
        formatedNumber(gcode.Z, result);
        QMutexLocker locker(&m_Operating);
        messages.push_back(Message{'N', static_cast<uint8_t>(result.size()), result});
        //qDebug() << gcode.X << " " << gcode.Y << " " << gcode.Z;
    }else if(gcode.function == 2 || gcode.function == 3){
        Arc arc(Point{previousGCode.X, previousGCode.Y, previousGCode.Z},
                Point{gcode.X, gcode.Y, gcode.Z},
                Point{gcode.X + gcode.I, gcode.Y + gcode.J, gcode.Z + gcode.K}, gcode.function == 2);
        for(const Point& point: arc.getArcPoints()){
            QVector<uint8_t> coordiates_data;
            formatedNumber(point.x, coordiates_data);
            formatedNumber(point.y, coordiates_data);
            formatedNumber(point.z, coordiates_data);
            {
                QMutexLocker locker(&m_Operating);
                messages.push_back(Message{'N', static_cast<uint8_t>(coordiates_data.size()), coordiates_data});
            }
        }
    }
}

void MillingMachine::formatedNumber(float num, QVector<uint8_t>& data)
{
    uint32_t num_int = num*100;
    data.push_back((uint8_t)(num_int & 0xFF));
    data.push_back((uint8_t)((num_int >> 8) & 0xFF));
    if(num < 0){
        data.push_back(((uint8_t)((num_int >> 16) & 0xFF)) | 0b10000000);
    }else{
        data.push_back((uint8_t)((num_int >> 16) & 0xFF));
    }
}



