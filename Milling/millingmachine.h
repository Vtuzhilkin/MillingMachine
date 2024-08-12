
#ifndef MILLINGMACHINE_H
#define MILLINGMACHINE_H

#include <QObject>
#include <QQueue>
#include <segment.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <CRC16.h>
#include <QThread>


class MillingMachine : public QObject
{
    Q_OBJECT
private:
    Coordinates currentCoordinates;
    QQueue<Coordinates> listCoordinates;
    int g_code_command;
    QSerialPort *serial;
    QByteArray command;
    bool openCOMPort = false;

    Segment createSegment(QString &textCoordinates) const;

public:
    explicit MillingMachine(QObject *parent = nullptr);
    ~MillingMachine();

signals:
    void startProcessing();
    void signalUnlock(bool);

public slots:
    Coordinates transferTextToCoordinate(QString&);
    void readCoordinates(QString);
    void receiveMessage();
    const double getCoordinateX();
    const double getCoordinateY();
    const double getCoordinateZ();
    void getNextCoordinate();
    void readCommandString(QString&);
    bool emptyListCoordinates() const;

    void G01(QString& textCoordinates);
    void G02(QString& textCoordinates);
    void G03(QString& textCoordinates);

    void openCOMport();
    void closeCOMport();
    void sendMessage(QByteArray&);
    void sendCoordinate();
    void ConvertToByte(QByteArray&, const Coordinates&);
    void WriteChar(QByteArray& byteCrd, std::string&) const;


};

#endif // MILLINGMACHINE_H
