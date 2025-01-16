#ifndef MILLINGMACHINE_H
#define MILLINGMACHINE_H

#include <QObject>
#include <QtQml/qqml.h>
#include <QtSerialPort/QSerialPort>
#include <QThread>
#include <QQueue>
#include <Message.h>
#include <QTimer>

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
    private:
        explicit MillingMachine(QObject *parent = nullptr);
        void run() override;
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
