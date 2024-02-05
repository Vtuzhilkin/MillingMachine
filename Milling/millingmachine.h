
#ifndef MILLINGMACHINE_H
#define MILLINGMACHINE_H

#include <QObject>
#include <QQueue>
#include <segment.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


class MillingMachine : public QObject
{
    Q_OBJECT
private:
    Coordinates currentCoordinates;
    QQueue<Coordinates> listCoordinates;
    int g_code_command;
    bool flagEmptyCoordinates = true;
    bool moving_coordinates = false;
    QSerialPort *serial;
    QByteArray command;
public:
    explicit MillingMachine(QObject *parent = nullptr);
    ~MillingMachine();

signals:
    void mySignal();
public slots:
    Coordinates transferTextToCoordinate(QString&);
    void readCoordinates(QString);
    const double getCoordinateX();
    const double getCoordinateY();
    const double getCoordinateZ();
    void getNextCoordinate();
    void readCommandString(QString&);
    const bool getFlagEmptyCoordinates();

    void G01(QString& textCoordinates);
    void G02(QString& textCoordinates);
    void G03(QString& textCoordinates);

    void openCOMport();
    void closeCOMport();
    void serialRecieve();
    void sendCoordinate();
    bool nextCommand();
    void change_moving_coordinates(bool);
    void ConvertToChar(char*, const Coordinates&);
    void WriteChar(char*, std::string&);

};

#endif // MILLINGMACHINE_H
