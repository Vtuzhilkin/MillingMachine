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
    bool shouldSend = !messages.empty() && (messages.front().getCode() != 'N' ||
                                            (messages.front().getCode() == 'N' && 
                                                statusMachine.calibrated && statusMachine.sendNextCoordinate));
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
            statusMachine.calibrated = data[11] & 1;
            statusMachine.moving = (data[11] >> 1) & 1;
            statusMachine.sendNextCoordinate = (data[11] >> 2) & 1;
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
    return statusMachine.moving && openedPort;
}

void MillingMachine::startMilling(const QStringList& lCommands)
{
    QMutexLocker locker(&m_Operating);
    if(listCommands.empty() && !statusMachine.moving){
        if(statusMachine.calibrated){
            listCommands = lCommands;
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
    QMutexLocker locker(&m_Operating);
    messages.push_front('K');
}

void MillingMachine::pauseMilling()
{
    QMutexLocker locker(&m_Operating);
    messages.push_front('B');
}

void MillingMachine::changeVelocity(float velocity)
{
    QMutexLocker locker(&m_Operating);
    if(statusMachine.calibrated && !statusMachine.moving && velocity > 0.01 && velocity < 10){
        QVector<uint8_t> velocity_data;
        formatedNumber(velocity, velocity_data);
        velocity_data.pop_back();
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
        line = listCommands.front().toUtf8();
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
    uint32_t num_int = abs(num*100);
    data.push_back((uint8_t)(num_int & 0xFF));
    data.push_back((uint8_t)((num_int >> 8) & 0xFF));
    if(num < 0){
        data.push_back(((uint8_t)((num_int >> 16) & 0xFF)) | 0b10000000);
    }else{
        data.push_back((uint8_t)((num_int >> 16) & 0xFF));
    }
}

N1 G01 X45.0 Y85.0 Z30.0
N2 G01 X45.0 Y130.0 Z30.0
N3 G01 X55.0 Y130.0 Z30.0
N4 G01 X55.0 Y85.0 Z30.0
N5 G01 X65.0 Y85.0 Z30.0
N6 G01 X65.0 Y130.0 Z30.0
N7 G01 X75.0 Y130.0 Z30.0
N8 G01 X75.0 Y85.0 Z30.0
N9 G01 X85.0 Y85.0 Z30.0
N10 G01 X85.0 Y130.0 Z30.0
N11 G01 X95.0 Y130.0 Z30.0
N12 G01 X95.0 Y85.0 Z30.0
N13 G01 X105.0 Y85.0 Z30.0
N14 G01 X105.0 Y130.0 Z30.0
N15 G01 X115.0 Y130.0 Z30.0
N16 G01 X115.0 Y85.0 Z30.0
N17 G01 X120.0 Y85.0 Z30.0
N18 G01 X120.0 Y130.0 Z30.0
N19 G01 X45.0 Y85.0 Z30.0
N20 G01 X45.0 Y85.0 Z29.0
N21 G01 X45.0 Y130.0 Z29.0
N22 G01 X55.0 Y130.0 Z29.0
N23 G01 X55.0 Y85.0 Z29.0
N24 G01 X65.0 Y85.0 Z29.0
N25 G01 X65.0 Y130.0 Z29.0
N26 G01 X75.0 Y130.0 Z29.0
N27 G01 X75.0 Y85.0 Z29.0
N28 G01 X85.0 Y85.0 Z29.0
N29 G01 X85.0 Y130.0 Z29.0
N30 G01 X95.0 Y130.0 Z29.0
N31 G01 X95.0 Y85.0 Z29.0
N32 G01 X105.0 Y85.0 Z29.0
N33 G01 X105.0 Y130.0 Z29.0
N34 G01 X115.0 Y130.0 Z29.0
N35 G01 X115.0 Y85.0 Z29.0
N36 G01 X120.0 Y85.0 Z29.0
N37 G01 X120.0 Y130.0 Z29.0
N38 G01 X45.0 Y85.0 Z29.0
N39 G01 X45.0 Y85.0 Z28.0
N40 G01 X45.0 Y130.0 Z28.0
N41 G01 X55.0 Y130.0 Z28.0
N42 G01 X55.0 Y85.0 Z28.0
N43 G01 X65.0 Y85.0 Z28.0
N44 G01 X65.0 Y130.0 Z28.0
N45 G01 X75.0 Y130.0 Z28.0
N46 G01 X75.0 Y85.0 Z28.0
N47 G01 X85.0 Y85.0 Z28.0
N48 G01 X85.0 Y130.0 Z28.0
N49 G01 X95.0 Y130.0 Z28.0
N50 G01 X95.0 Y85.0 Z28.0
N51 G01 X105.0 Y85.0 Z28.0
N52 G01 X105.0 Y130.0 Z28.0
N53 G01 X115.0 Y130.0 Z28.0
N54 G01 X115.0 Y85.0 Z28.0
N55 G01 X120.0 Y85.0 Z28.0
N56 G01 X120.0 Y130.0 Z28.0
N57 G01 X45.0 Y85.0 Z28.0
N58 G01 X45.0 Y85.0 Z27.0
N59 G01 X45.0 Y130.0 Z27.0
N60 G01 X55.0 Y130.0 Z27.0
N61 G01 X55.0 Y85.0 Z27.0
N62 G01 X65.0 Y85.0 Z27.0
N63 G01 X65.0 Y130.0 Z27.0
N64 G01 X75.0 Y130.0 Z27.0
N65 G01 X75.0 Y85.0 Z27.0
N66 G01 X85.0 Y85.0 Z27.0
N67 G01 X85.0 Y130.0 Z27.0
N68 G01 X95.0 Y130.0 Z27.0
N69 G01 X95.0 Y85.0 Z27.0
N70 G01 X105.0 Y85.0 Z27.0
N71 G01 X105.0 Y130.0 Z27.0
N72 G01 X115.0 Y130.0 Z27.0
N73 G01 X115.0 Y85.0 Z27.0
N74 G01 X120.0 Y85.0 Z27.0
N75 G01 X120.0 Y130.0 Z27.0
N76 G01 X45.0 Y85.0 Z27.0
N77 G01 X45.0 Y85.0 Z26.0
N78 G01 X45.0 Y130.0 Z26.0
N79 G01 X55.0 Y130.0 Z26.0
N80 G01 X55.0 Y85.0 Z26.0
N81 G01 X65.0 Y85.0 Z26.0
N82 G01 X65.0 Y130.0 Z26.0
N83 G01 X75.0 Y130.0 Z26.0
N84 G01 X75.0 Y85.0 Z26.0
N85 G01 X85.0 Y85.0 Z26.0
N86 G01 X85.0 Y130.0 Z26.0
N87 G01 X95.0 Y130.0 Z26.0
N88 G01 X95.0 Y85.0 Z26.0
N89 G01 X105.0 Y85.0 Z26.0
N90 G01 X105.0 Y130.0 Z26.0
N91 G01 X115.0 Y130.0 Z26.0
N92 G01 X115.0 Y85.0 Z26.0
N93 G01 X120.0 Y85.0 Z26.0
N94 G01 X120.0 Y130.0 Z26.0
N95 G01 X45.0 Y85.0 Z26.0
N96 G01 X45.0 Y85.0 Z25.0
N97 G01 X45.0 Y130.0 Z25.0
N98 G01 X55.0 Y130.0 Z25.0
N99 G01 X55.0 Y85.0 Z25.0
N100 G01 X65.0 Y85.0 Z25.0
N101 G01 X65.0 Y130.0 Z25.0
N102 G01 X75.0 Y130.0 Z25.0
N103 G01 X75.0 Y85.0 Z25.0
N104 G01 X85.0 Y85.0 Z25.0
N105 G01 X85.0 Y130.0 Z25.0
N106 G01 X95.0 Y130.0 Z25.0
N107 G01 X95.0 Y85.0 Z25.0
N108 G01 X105.0 Y85.0 Z25.0
N109 G01 X105.0 Y130.0 Z25.0
N110 G01 X115.0 Y130.0 Z25.0
N111 G01 X115.0 Y85.0 Z25.0
N112 G01 X120.0 Y85.0 Z25.0
N113 G01 X120.0 Y130.0 Z25.0
N114 G01 X45.0 Y85.0 Z25.0
N115 G01 X45.0 Y85.0 Z24.0
N116 G01 X45.0 Y130.0 Z24.0
N117 G01 X55.0 Y130.0 Z24.0
N118 G01 X55.0 Y85.0 Z24.0
N119 G01 X65.0 Y85.0 Z24.0
N120 G01 X65.0 Y130.0 Z24.0
N121 G01 X75.0 Y130.0 Z24.0
N122 G01 X75.0 Y85.0 Z24.0
N123 G01 X85.0 Y85.0 Z24.0
N124 G01 X85.0 Y130.0 Z24.0
N125 G01 X95.0 Y130.0 Z24.0
N126 G01 X95.0 Y85.0 Z24.0
N127 G01 X105.0 Y85.0 Z24.0
N128 G01 X105.0 Y130.0 Z24.0
N129 G01 X115.0 Y130.0 Z24.0
N130 G01 X115.0 Y85.0 Z24.0
N131 G01 X120.0 Y85.0 Z24.0
N132 G01 X120.0 Y130.0 Z24.0
N133 G01 X45.0 Y85.0 Z24.0
N134 G01 X45.0 Y85.0 Z23.0
N135 G01 X45.0 Y130.0 Z23.0
N136 G01 X55.0 Y130.0 Z23.0
N137 G01 X55.0 Y85.0 Z23.0
N138 G01 X65.0 Y85.0 Z23.0
N139 G01 X65.0 Y130.0 Z23.0
N140 G01 X75.0 Y130.0 Z23.0
N141 G01 X75.0 Y85.0 Z23.0
N142 G01 X85.0 Y85.0 Z23.0
N143 G01 X85.0 Y130.0 Z23.0
N144 G01 X95.0 Y130.0 Z23.0
N145 G01 X95.0 Y85.0 Z23.0
N146 G01 X105.0 Y85.0 Z23.0
N147 G01 X105.0 Y130.0 Z23.0
N148 G01 X115.0 Y130.0 Z23.0
N149 G01 X115.0 Y85.0 Z23.0
N150 G01 X120.0 Y85.0 Z23.0
N151 G01 X120.0 Y130.0 Z23.0
N152 G01 X45.0 Y85.0 Z23.0
N153 G01 X45.0 Y85.0 Z22.0
N154 G01 X45.0 Y130.0 Z22.0
N155 G01 X55.0 Y130.0 Z22.0
N156 G01 X55.0 Y85.0 Z22.0
N157 G01 X65.0 Y85.0 Z22.0
N158 G01 X65.0 Y130.0 Z22.0
N159 G01 X75.0 Y130.0 Z22.0
N160 G01 X75.0 Y85.0 Z22.0
N161 G01 X85.0 Y85.0 Z22.0
N162 G01 X85.0 Y130.0 Z22.0
N163 G01 X95.0 Y130.0 Z22.0
N164 G01 X95.0 Y85.0 Z22.0
N165 G01 X105.0 Y85.0 Z22.0
N166 G01 X105.0 Y130.0 Z22.0
N167 G01 X115.0 Y130.0 Z22.0
N168 G01 X115.0 Y85.0 Z22.0
N169 G01 X120.0 Y85.0 Z22.0
N170 G01 X120.0 Y130.0 Z22.0
N171 G01 X45.0 Y85.0 Z22.0
N172 G01 X45.0 Y85.0 Z21.0
N173 G01 X45.0 Y130.0 Z21.0
N174 G01 X55.0 Y130.0 Z21.0
N175 G01 X55.0 Y85.0 Z21.0
N176 G01 X65.0 Y85.0 Z21.0
N177 G01 X65.0 Y130.0 Z21.0
N178 G01 X75.0 Y130.0 Z21.0
N179 G01 X75.0 Y85.0 Z21.0
N180 G01 X85.0 Y85.0 Z21.0
N181 G01 X85.0 Y130.0 Z21.0
N182 G01 X95.0 Y130.0 Z21.0
N183 G01 X95.0 Y85.0 Z21.0
N184 G01 X105.0 Y85.0 Z21.0
N185 G01 X105.0 Y130.0 Z21.0
N186 G01 X115.0 Y130.0 Z21.0
N187 G01 X115.0 Y85.0 Z21.0
N188 G01 X120.0 Y85.0 Z21.0
N189 G01 X120.0 Y130.0 Z21.0
N190 G01 X45.0 Y85.0 Z21.0
N191 G01 X45.0 Y85.0 Z20.0
N192 G01 X45.0 Y130.0 Z20.0
N193 G01 X55.0 Y130.0 Z20.0
N194 G01 X55.0 Y85.0 Z20.0
N195 G01 X65.0 Y85.0 Z20.0
N196 G01 X65.0 Y130.0 Z20.0
N197 G01 X75.0 Y130.0 Z20.0
N198 G01 X75.0 Y85.0 Z20.0
N199 G01 X85.0 Y85.0 Z20.0
N200 G01 X85.0 Y130.0 Z20.0
N201 G01 X95.0 Y130.0 Z20.0
N202 G01 X95.0 Y85.0 Z20.0
N203 G01 X105.0 Y85.0 Z20.0
N204 G01 X105.0 Y130.0 Z20.0
N205 G01 X115.0 Y130.0 Z20.0
N206 G01 X115.0 Y85.0 Z20.0
N207 G01 X120.0 Y85.0 Z20.0
N208 G01 X120.0 Y130.0 Z20.0
N209 G01 X0.0 Y90.0 Z20.0


N1 G01 X30.0 Y15.0 Z30.0
N2 G01 X40.0 Y15.0 Z30.0
N3 G01 X40.0 Y120.0 Z30.0
N4 G01 X50.0 Y120.0 Z30.0
N5 G01 X50.0 Y15.0 Z30.0
N6 G01 X60.0 Y15.0 Z30.0
N7 G01 X60.0 Y120.0 Z30.0
N8 G01 X70.0 Y120.0 Z30.0
N9 G01 X70.0 Y15.0 Z30.0
N10 G01 X80.0 Y15.0 Z30.0
N11 G01 X80.0 Y120.0 Z30.0
N12 G01 X90.0 Y120.0 Z30.0
N13 G01 X90.0 Y15.0 Z30.0
N14 G01 X100.0 Y15.0 Z30.0
N15 G01 X100.0 Y120.0 Z30.0
N16 G01 X110.0 Y120.0 Z30.0
N17 G01 X110.0 Y15.0 Z30.0
N18 G01 X120.0 Y15.0 Z30.0
N19 G01 X120.0 Y120.0 Z30.0
N20 G01 X30.0 Y15.0 Z30.0
N21 G01 X30.0 Y15.0 Z29.0
N22 G01 X40.0 Y15.0 Z29.0
N23 G01 X40.0 Y120.0 Z29.0
N24 G01 X50.0 Y120.0 Z29.0
N25 G01 X50.0 Y15.0 Z29.0
N26 G01 X60.0 Y15.0 Z29.0
N27 G01 X60.0 Y120.0 Z29.0
N28 G01 X70.0 Y120.0 Z29.0
N29 G01 X70.0 Y15.0 Z29.0
N30 G01 X80.0 Y15.0 Z29.0
N31 G01 X80.0 Y120.0 Z29.0
N32 G01 X90.0 Y120.0 Z29.0
N33 G01 X90.0 Y15.0 Z29.0
N34 G01 X100.0 Y15.0 Z29.0
N35 G01 X100.0 Y120.0 Z29.0
N36 G01 X110.0 Y120.0 Z29.0
N37 G01 X110.0 Y15.0 Z29.0
N38 G01 X120.0 Y15.0 Z29.0
N39 G01 X120.0 Y120.0 Z29.0
N40 G01 X30.0 Y15.0 Z29.0
N41 G01 X30.0 Y15.0 Z28.0
N42 G01 X40.0 Y15.0 Z28.0
N43 G01 X40.0 Y120.0 Z28.0
N44 G01 X50.0 Y120.0 Z28.0
N45 G01 X50.0 Y15.0 Z28.0
N46 G01 X60.0 Y15.0 Z28.0
N47 G01 X60.0 Y120.0 Z28.0
N48 G01 X70.0 Y120.0 Z28.0
N49 G01 X70.0 Y15.0 Z28.0
N50 G01 X80.0 Y15.0 Z28.0
N51 G01 X80.0 Y120.0 Z28.0
N52 G01 X90.0 Y120.0 Z28.0
N53 G01 X90.0 Y15.0 Z28.0
N54 G01 X100.0 Y15.0 Z28.0
N55 G01 X100.0 Y120.0 Z28.0
N56 G01 X110.0 Y120.0 Z28.0
N57 G01 X110.0 Y15.0 Z28.0
N58 G01 X120.0 Y15.0 Z28.0
N59 G01 X120.0 Y120.0 Z28.0
N60 G01 X30.0 Y15.0 Z28.0
N61 G01 X30.0 Y15.0 Z27.0
N62 G01 X40.0 Y15.0 Z27.0
N63 G01 X40.0 Y120.0 Z27.0
N64 G01 X50.0 Y120.0 Z27.0
N65 G01 X50.0 Y15.0 Z27.0
N66 G01 X60.0 Y15.0 Z27.0
N67 G01 X60.0 Y120.0 Z27.0
N68 G01 X70.0 Y120.0 Z27.0
N69 G01 X70.0 Y15.0 Z27.0
N70 G01 X80.0 Y15.0 Z27.0
N71 G01 X80.0 Y120.0 Z27.0
N72 G01 X90.0 Y120.0 Z27.0
N73 G01 X90.0 Y15.0 Z27.0
N74 G01 X100.0 Y15.0 Z27.0
N75 G01 X100.0 Y120.0 Z27.0
N76 G01 X110.0 Y120.0 Z27.0
N77 G01 X110.0 Y15.0 Z27.0
N78 G01 X120.0 Y15.0 Z27.0
N79 G01 X120.0 Y120.0 Z27.0
N80 G01 X30.0 Y15.0 Z27.0
N81 G01 X30.0 Y15.0 Z26.0
N82 G01 X40.0 Y15.0 Z26.0
N83 G01 X40.0 Y120.0 Z26.0
N84 G01 X50.0 Y120.0 Z26.0
N85 G01 X50.0 Y15.0 Z26.0
N86 G01 X60.0 Y15.0 Z26.0
N87 G01 X60.0 Y120.0 Z26.0
N88 G01 X70.0 Y120.0 Z26.0
N89 G01 X70.0 Y15.0 Z26.0
N90 G01 X80.0 Y15.0 Z26.0
N91 G01 X80.0 Y120.0 Z26.0
N92 G01 X90.0 Y120.0 Z26.0
N93 G01 X90.0 Y15.0 Z26.0
N94 G01 X100.0 Y15.0 Z26.0
N95 G01 X100.0 Y120.0 Z26.0
N96 G01 X110.0 Y120.0 Z26.0
N97 G01 X110.0 Y15.0 Z26.0
N98 G01 X120.0 Y15.0 Z26.0
N99 G01 X120.0 Y120.0 Z26.0
N100 G01 X30.0 Y15.0 Z26.0
N101 G01 X30.0 Y15.0 Z25.0
N102 G01 X40.0 Y15.0 Z25.0
N103 G01 X40.0 Y120.0 Z25.0
N104 G01 X50.0 Y120.0 Z25.0
N105 G01 X50.0 Y15.0 Z25.0
N106 G01 X60.0 Y15.0 Z25.0
N107 G01 X60.0 Y120.0 Z25.0
N108 G01 X70.0 Y120.0 Z25.0
N109 G01 X70.0 Y15.0 Z25.0
N110 G01 X80.0 Y15.0 Z25.0
N111 G01 X80.0 Y120.0 Z25.0
N112 G01 X90.0 Y120.0 Z25.0
N113 G01 X90.0 Y15.0 Z25.0
N114 G01 X100.0 Y15.0 Z25.0
N115 G01 X100.0 Y120.0 Z25.0
N116 G01 X110.0 Y120.0 Z25.0
N117 G01 X110.0 Y15.0 Z25.0
N118 G01 X120.0 Y15.0 Z25.0
N119 G01 X120.0 Y120.0 Z25.0
N120 G01 X30.0 Y15.0 Z25.0
N121 G01 X30.0 Y15.0 Z24.0
N122 G01 X40.0 Y15.0 Z24.0
N123 G01 X40.0 Y120.0 Z24.0
N124 G01 X50.0 Y120.0 Z24.0
N125 G01 X50.0 Y15.0 Z24.0
N126 G01 X60.0 Y15.0 Z24.0
N127 G01 X60.0 Y120.0 Z24.0
N128 G01 X70.0 Y120.0 Z24.0
N129 G01 X70.0 Y15.0 Z24.0
N130 G01 X80.0 Y15.0 Z24.0
N131 G01 X80.0 Y120.0 Z24.0
N132 G01 X90.0 Y120.0 Z24.0
N133 G01 X90.0 Y15.0 Z24.0
N134 G01 X100.0 Y15.0 Z24.0
N135 G01 X100.0 Y120.0 Z24.0
N136 G01 X110.0 Y120.0 Z24.0
N137 G01 X110.0 Y15.0 Z24.0
N138 G01 X120.0 Y15.0 Z24.0
N139 G01 X120.0 Y120.0 Z24.0
N140 G01 X30.0 Y15.0 Z24.0
N141 G01 X30.0 Y15.0 Z23.0
N142 G01 X40.0 Y15.0 Z23.0
N143 G01 X40.0 Y120.0 Z23.0
N144 G01 X50.0 Y120.0 Z23.0
N145 G01 X50.0 Y15.0 Z23.0
N146 G01 X60.0 Y15.0 Z23.0
N147 G01 X60.0 Y120.0 Z23.0
N148 G01 X70.0 Y120.0 Z23.0
N149 G01 X70.0 Y15.0 Z23.0
N150 G01 X80.0 Y15.0 Z23.0
N151 G01 X80.0 Y120.0 Z23.0
N152 G01 X90.0 Y120.0 Z23.0
N153 G01 X90.0 Y15.0 Z23.0
N154 G01 X100.0 Y15.0 Z23.0
N155 G01 X100.0 Y120.0 Z23.0
N156 G01 X110.0 Y120.0 Z23.0
N157 G01 X110.0 Y15.0 Z23.0
N158 G01 X120.0 Y15.0 Z23.0
N159 G01 X120.0 Y120.0 Z23.0
N160 G01 X30.0 Y15.0 Z23.0
N161 G01 X30.0 Y15.0 Z22.0
N162 G01 X40.0 Y15.0 Z22.0
N163 G01 X40.0 Y120.0 Z22.0
N164 G01 X50.0 Y120.0 Z22.0
N165 G01 X50.0 Y15.0 Z22.0
N166 G01 X60.0 Y15.0 Z22.0
N167 G01 X60.0 Y120.0 Z22.0
N168 G01 X70.0 Y120.0 Z22.0
N169 G01 X70.0 Y15.0 Z22.0
N170 G01 X80.0 Y15.0 Z22.0
N171 G01 X80.0 Y120.0 Z22.0
N172 G01 X90.0 Y120.0 Z22.0
N173 G01 X90.0 Y15.0 Z22.0
N174 G01 X100.0 Y15.0 Z22.0
N175 G01 X100.0 Y120.0 Z22.0
N176 G01 X110.0 Y120.0 Z22.0
N177 G01 X110.0 Y15.0 Z22.0
N178 G01 X120.0 Y15.0 Z22.0
N179 G01 X120.0 Y120.0 Z22.0
N180 G01 X30.0 Y15.0 Z22.0
N181 G01 X30.0 Y15.0 Z21.0
N182 G01 X40.0 Y15.0 Z21.0
N183 G01 X40.0 Y120.0 Z21.0
N184 G01 X50.0 Y120.0 Z21.0
N185 G01 X50.0 Y15.0 Z21.0
N186 G01 X60.0 Y15.0 Z21.0
N187 G01 X60.0 Y120.0 Z21.0
N188 G01 X70.0 Y120.0 Z21.0
N189 G01 X70.0 Y15.0 Z21.0
N190 G01 X80.0 Y15.0 Z21.0
N191 G01 X80.0 Y120.0 Z21.0
N192 G01 X90.0 Y120.0 Z21.0
N193 G01 X90.0 Y15.0 Z21.0
N194 G01 X100.0 Y15.0 Z21.0
N195 G01 X100.0 Y120.0 Z21.0
N196 G01 X110.0 Y120.0 Z21.0
N197 G01 X110.0 Y15.0 Z21.0
N198 G01 X120.0 Y15.0 Z21.0
N199 G01 X120.0 Y120.0 Z21.0
N200 G01 X30.0 Y15.0 Z21.0
N201 G01 X30.0 Y15.0 Z20.0
N202 G01 X40.0 Y15.0 Z20.0
N203 G01 X40.0 Y120.0 Z20.0
N204 G01 X50.0 Y120.0 Z20.0
N205 G01 X50.0 Y15.0 Z20.0
N206 G01 X60.0 Y15.0 Z20.0
N207 G01 X60.0 Y120.0 Z20.0
N208 G01 X70.0 Y120.0 Z20.0
N209 G01 X70.0 Y15.0 Z20.0
N210 G01 X80.0 Y15.0 Z20.0
N211 G01 X80.0 Y120.0 Z20.0
N212 G01 X90.0 Y120.0 Z20.0
N213 G01 X90.0 Y15.0 Z20.0
N214 G01 X100.0 Y15.0 Z20.0
N215 G01 X100.0 Y120.0 Z20.0
N216 G01 X110.0 Y120.0 Z20.0
N217 G01 X110.0 Y15.0 Z20.0
N218 G01 X120.0 Y15.0 Z20.0
N219 G01 X120.0 Y120.0 Z20.0
N220 G01 X30.0 Y15.0 Z20.0



