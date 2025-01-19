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

struct GCode {
    int lineNumber;
    int function;
    float X, Y, Z;
    float I, J, K;

    GCode() : lineNumber(0), function(0), X(0.0), Y(0.0), Z(0.0), I(0.0), J(0.0), K(0.0) {}
};

class MillingMachine : public QThread
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
    QML_NAMED_ELEMENT(MillingMachine)
    Q_PROPERTY(bool openedPort READ getStatusCOM NOTIFY updateLeds)
    public:
        static MillingMachine* create(QQmlEngine *, QJSEngine *);
        static MillingMachine* instance();
        ~MillingMachine();
        Q_INVOKABLE void openCOM(int numberPort);
        Q_INVOKABLE void closeCOM();
        Q_INVOKABLE bool getStatusCOM();
        Q_INVOKABLE void startMilling(const QStringList& listCommands);
    private:
        explicit MillingMachine(QObject *parent = nullptr);
        void run() override;
        void sendMessage();
        void addMessage(const GCode& gcode);
        void formatedNumber(float number, QVector<unsigned char>&);
        static MillingMachine* millingMachine;
        QSerialPort* serialPort;
        bool openedPort = false;
        bool mThread = false;
        QQueue<Message> messages;
        QTimer timerUpdate;
    signals:
        void updateLeds();
};


#endif // MILLINGMACHINE_H
