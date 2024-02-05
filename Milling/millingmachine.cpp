#include "millingmachine.h"

MillingMachine::MillingMachine(QObject *parent)
    : QObject{parent}, currentCoordinates(0,0,0)
{
    serial = new QSerialPort(this);
}

MillingMachine::~MillingMachine(){
    serial->close();
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
    if(listCoordinates.empty()){
        flagEmptyCoordinates = true;
        return;
    }
    currentCoordinates.x_coordinate = listCoordinates.begin()->x_coordinate;
    currentCoordinates.y_coordinate = listCoordinates.begin()->y_coordinate;
    currentCoordinates.z_coordinate = listCoordinates.begin()->z_coordinate;
    listCoordinates.pop_front();
    //std::cout << currentCoordinates.y_coordinate << std::endl;

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

void MillingMachine::G02(QString& textCoordinates){
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
    Segment segment(point_begin, point_end, point_center);
    CrushingSegmentRight(segment, listCoordinates);
}

void MillingMachine::G03(QString& textCoordinates){
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
    Segment segment(point_begin, point_end, point_center);
    CrushingSegmentLeft(segment, listCoordinates);
}


void MillingMachine::readCommandString(QString& textCoordinates){    
    flagEmptyCoordinates = false;

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

const bool MillingMachine::getFlagEmptyCoordinates(){
    return flagEmptyCoordinates;
}


void MillingMachine::openCOMport(){
    serial->setPortName("COM3");
    serial->setBaudRate(QSerialPort::Baud38400);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);
    connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecieve()));
}

void MillingMachine::closeCOMport(){
    serial->close();
}

void MillingMachine::serialRecieve(){
    command = serial->readAll();
    if(nextCommand() && moving_coordinates){
        emit mySignal();
        sendCoordinate();
    }
}

void MillingMachine::sendCoordinate(){
    char char_crd[18];
    ConvertToChar(char_crd, currentCoordinates);
    serial->write(char_crd, 18);
    serial->waitForBytesWritten();
    command.clear();
}

bool MillingMachine::nextCommand(){
    if(command == "Next" || command == "Start"){
        return true;
    }
    return false;
}

void MillingMachine::change_moving_coordinates(bool mv_crd){
    moving_coordinates = mv_crd;
}

void MillingMachine::WriteChar(char* char_crd, std::string& str_crd){

    if(str_crd[0] == '-'){
            char_crd[0] = '-';
            str_crd.erase(0, 1);
    } else{
            char_crd[0] = '+';
    }

    size_t index_point = str_crd.find('.');
    switch (index_point) {
    case 3:
            *(char_crd + 1) = str_crd[0];
            *(char_crd + 2) = str_crd[1];
            *(char_crd + 3) = str_crd[2];
            break;
    case 2:
            *(char_crd + 1) = '0';
            *(char_crd + 2) = str_crd[0];
            *(char_crd + 3) = str_crd[1];
            break;
    case 1:
            *(char_crd + 1) = '0';
            *(char_crd + 2) = '0';
            *(char_crd + 3) = str_crd[0];
            break;
    default:
            break;
    }
    *(char_crd + 4) = str_crd[index_point+1];
    *(char_crd + 5) = str_crd[index_point+2];
}

void MillingMachine::ConvertToChar(char* char_crd, const Coordinates& coordinates){
    std::string str_crd_x = std::to_string(coordinates.x_coordinate);
    WriteChar(char_crd, str_crd_x);
    std::string str_crd_y = std::to_string(coordinates.y_coordinate);
    WriteChar(char_crd+6, str_crd_y);
    std::string str_crd_z = std::to_string(coordinates.z_coordinate);
    WriteChar(char_crd+12, str_crd_z);
}
