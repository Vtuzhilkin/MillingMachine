#ifndef MILLINGMACHINE_H
#define MILLINGMACHINE_H

#include <QObject>
#include <QtQml/qqml.h>
#include <QtSerialPort/QSerialPort>
#include <QThread>
#include <QQueue>
#include <Message.h>
#include <QTimer>
#include <regex>
#include <string>
#include <QMutex>
#include <QMutexLocker>
#include <Arc.h>

struct GCode {
    int lineNumber;
    int function;
    float X, Y, Z;
    float I, J, K;

    GCode() : lineNumber(0), function(0), X(0.0), Y(0.0), Z(0.0), I(0.0), J(0.0), K(0.0) {}
};

struct Status{
    Status(): calibrated(false), moving(false), xCoordinate(0), yCoordinate(0), zCoordinate(0) {}
    bool calibrated;
    bool moving;
    float xCoordinate, yCoordinate, zCoordinate;
    float velocity;
};

class MillingMachine : public QThread
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
    QML_NAMED_ELEMENT(MillingMachine)
    Q_PROPERTY(bool openedPort READ getStatusCOM NOTIFY updateLeds)
    Q_PROPERTY(bool calibrated READ getCalibrated NOTIFY updateLeds)
    Q_PROPERTY(bool processing READ getProcessing NOTIFY updateLeds)
    Q_PROPERTY(float xCoordinate READ getXCoordiate NOTIFY updateCoordiantes)
    Q_PROPERTY(float yCoordinate READ getYCoordiate NOTIFY updateCoordiantes)
    Q_PROPERTY(float zCoordinate READ getZCoordiate NOTIFY updateCoordiantes)

    public:
        static MillingMachine* create(QQmlEngine *, QJSEngine *);
        static MillingMachine* instance();
        ~MillingMachine();
        Q_INVOKABLE void openCOM(int numberPort);
        Q_INVOKABLE void closeCOM();
        Q_INVOKABLE void startMilling(const QStringList& listCommands);
        Q_INVOKABLE void stopMilling();
        Q_INVOKABLE void continueMilling();
        Q_INVOKABLE void pauseMilling();
        Q_INVOKABLE void changeVelocity(float velocity);

        bool getStatusCOM();
        bool getCalibrated();
        bool getProcessing();
        float getXCoordiate();
        float getYCoordiate();
        float getZCoordiate();
        void operatingMessage(const Message& message);
    private:
        explicit MillingMachine(QObject *parent = nullptr);
        void run() override;
        void sendMessage();
        void addMessage(const GCode& gcode, const GCode& previousGCode);
        void formatedNumber(float number, QVector<unsigned char>&);
        void operatingCommands();
        static MillingMachine* millingMachine;
        QSerialPort* serialPort;
        bool openedPort = false;
        bool mThread = false;
        bool operating = false;
        QQueue<Message> messages;
        QTimer timerUpdate;
        QStringList listCommands;
        QMutex m_Operating;
        GCode previousGCode;
        Status statusMachine;
    signals:
        void updateLeds();
        void updateCoordiantes();
};


#endif // MILLINGMACHINE_H
