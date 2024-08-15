#include "millingmachine.h"

MillingMachine::MillingMachine(QObject *parent)
    : QObject{parent}, currentCoordinates(0,0,0)
{
    serial = new QSerialPort(this);
    serial->setPortName("COM3");
    serial->setBaudRate(QSerialPort::Baud38400);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    connect(serial, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
}


MillingMachine::~MillingMachine(){
    closeCOMport();
    delete serial;
}

Coordinates MillingMachine::transferTextToCoordinate(QString& textCoordinates){
    Coordinates crds;
    crds.x_coordinate  = (textCoordinates.mid(1, textCoordinates.indexOf('Y') - 1)).toDouble();
    crds.y_coordinate = textCoordinates.mid(textCoordinates.indexOf('Y') + 1,
                                            textCoordinates.indexOf('Z') - textCoordinates.indexOf('Y') - 1).toDouble();
    crds.z_coordinate = textCoordinates.mid(textCoordinates.indexOf('Z') + 1,
                                                textCoordinates.length() - textCoordinates.indexOf('Z') - 1).toDouble();
    return std::move(crds);
}

void MillingMachine::readCoordinates(QString textCoordinates){
    Coordinates currentCrds = transferTextToCoordinate(textCoordinates);
    currentCoordinates.x_coordinate = std::move(currentCrds.x_coordinate);
    currentCoordinates.y_coordinate = std::move(currentCrds.y_coordinate);
    currentCoordinates.z_coordinate = std::move(currentCrds.z_coordinate);

}

const double MillingMachine::getCoordinateX(){
    return currentCoordinates.x_coordinate;
}
const double MillingMachine::getCoordinateY(){
    return currentCoordinates.y_coordinate;
}
const double MillingMachine::getCoordinateZ(){
    return -currentCoordinates.z_coordinate;
}

void MillingMachine::getNextCoordinate(){
    currentCoordinates.x_coordinate = listCoordinates.front().x_coordinate;
    currentCoordinates.y_coordinate = listCoordinates.front().y_coordinate;
    currentCoordinates.z_coordinate = listCoordinates.front().z_coordinate;
    listCoordinates.pop_front();
}

bool MillingMachine::emptyListCoordinates() const{
    return listCoordinates.empty();
}

void MillingMachine::readCommandString(QString& textCoordinates){
    QString number_string = textCoordinates.mid(0, textCoordinates.indexOf(' ')+1);
    textCoordinates.remove(0, textCoordinates.indexOf(' ')+1);
    if(textCoordinates[0] == 'G'){
        g_code_command = textCoordinates.mid(1, textCoordinates.indexOf(' ')).toInt();
        textCoordinates.remove(0, textCoordinates.indexOf(' ')+1);
    }
    switch(g_code_command){
        case 1:{
            G01(textCoordinates);
            break;
        }
        case 2:{
            G02(textCoordinates);
            break;
        }
        case 3:{
            G03(textCoordinates);
            break;
        }
    }
}

void MillingMachine::G01(QString& textCoordinates){
    double coordinate_x = currentCoordinates.x_coordinate;
    double coordinate_y = currentCoordinates.y_coordinate;
    double coordinate_z = currentCoordinates.z_coordinate;

    while(textCoordinates.length() != 0){
            int number_space = textCoordinates.indexOf(' ');
            if(-1 != number_space){
                number_space += 1;
            }else{
                number_space = textCoordinates.length();
            }

            if(textCoordinates[0] == 'X'){
                coordinate_x = textCoordinates.mid(1, textCoordinates.indexOf(' ')).toDouble();
                textCoordinates.remove(0, number_space);
            }else if(textCoordinates[0] == 'Y'){
                coordinate_y = textCoordinates.mid(1, textCoordinates.indexOf(' ')).toDouble();
                textCoordinates.remove(0, number_space);
            }else if(textCoordinates[0] == 'Z'){
                coordinate_z = textCoordinates.mid(1, textCoordinates.indexOf(' ')).toDouble();
                textCoordinates.remove(0, number_space);
            }
    }
    listCoordinates.append(Coordinates(coordinate_x, coordinate_y, coordinate_z));
}

Segment MillingMachine::createSegment(QString& textCoordinates) const{
    double coordinate_x = textCoordinates.mid(1, textCoordinates.indexOf(' ')).toDouble();
    textCoordinates.remove(0, textCoordinates.indexOf(' ')+1);
    double coordinate_y = textCoordinates.mid(1, textCoordinates.indexOf(' ')).toDouble();
    textCoordinates.remove(0, textCoordinates.indexOf(' ')+1);
    double coordinate_i = textCoordinates.mid(1, textCoordinates.indexOf(' ')).toDouble();
    textCoordinates.remove(0, textCoordinates.indexOf(' ')+1);
    double coordinate_j = textCoordinates.mid(1, textCoordinates.indexOf('\n')).toDouble();
    textCoordinates.remove(0, textCoordinates.length());
    Coordinates point_begin(currentCoordinates.x_coordinate, currentCoordinates.y_coordinate, currentCoordinates.z_coordinate);
    Coordinates point_end(coordinate_x, coordinate_y, currentCoordinates.z_coordinate);
    Coordinates point_center(coordinate_i, coordinate_j, 0);
    return Segment(point_begin, point_end, point_center);
}

void MillingMachine::G02(QString& textCoordinates){
    CrushingSegmentRight(createSegment(textCoordinates), listCoordinates);
}

void MillingMachine::G03(QString& textCoordinates){
    CrushingSegmentLeft(createSegment(textCoordinates), listCoordinates);
}


void MillingMachine::openCOMport(){
    serial->open(QIODevice::ReadWrite);
    QByteArray open = QString("Open").toUtf8();
    sendMessage(open);
}

void MillingMachine::closeCOMport(){
    QByteArray close = QString("Close").toUtf8();
    sendMessage(close);
}

void MillingMachine::receiveMessage(){
    command = serial->readAll();
    if(command == "Open"){
        emit signalUnlock(true);
    }else if(command == "Close"){
        emit signalUnlock(false);
        serial->close();
    }else if(command == "Next"){
        emit startProcessing();
    }
}

void MillingMachine::sendMessage(QByteArray& message){
    short int checkSumm = crc16(message);
    message.append(checkSumm & 0xFF);
    message.append((checkSumm >> 8) & 0xFF);
    serial->write(message, 20);
}

void MillingMachine::sendCoordinate(){
    QByteArray byteCoordinates;
    ConvertToByte(byteCoordinates, currentCoordinates);
    sendMessage(byteCoordinates);
}

void MillingMachine::ConvertToByte(QByteArray& byteCrd, const Coordinates& coordinates){
    std::string str_crd_x = std::to_string(coordinates.x_coordinate);
    WriteChar(byteCrd, str_crd_x);
    std::string str_crd_y = std::to_string(coordinates.y_coordinate);
    WriteChar(byteCrd, str_crd_y);
    std::string str_crd_z = std::to_string(coordinates.z_coordinate);
    WriteChar(byteCrd, str_crd_z);
}

void MillingMachine::WriteChar(QByteArray& byteCrd, std::string& str_crd) const{

    if(str_crd[0] == '-'){
        byteCrd.append('-');
        str_crd.erase(0, 1);
    } else{
        byteCrd.append('+');
    }

    size_t index_point = str_crd.find('.');
    for(size_t i = 0; i < 3 - index_point; i++){ // заполняет нулями первые символы
        byteCrd.append('0');
    }
    for(size_t i = 0; i < index_point; i++){ // заполняет числами
        byteCrd.append(str_crd.at(i));
    }
    byteCrd.append(str_crd.at(index_point+1));
    byteCrd.append(str_crd.at(index_point+2));
}


