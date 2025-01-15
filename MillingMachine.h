
#ifndef MILLINGMACHINE_H
#define MILLINGMACHINE_H


#include <QObject>
#include <QtQml/qqml.h>
#include <QtSerialPort/QSerialPort>
#include <QThread>

class MillingMachine : public QThread
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
    QML_NAMED_ELEMENT(MillingMachine)
    public:
        static MillingMachine* create(QQmlEngine *, QJSEngine *);
        static MillingMachine* instance();
        ~MillingMachine();
        Q_INVOKABLE void openCOM(int numberPort);
        Q_INVOKABLE void closeCOM(int numberPort);
        Q_INVOKABLE bool getStatusCOM();

    private:
        explicit MillingMachine(QObject *parent = nullptr);
        void run() override;
        static MillingMachine* millingMachine;
        QSerialPort* serialPort;
        bool openedPort = false;
        bool mThread = false;
    signals:
        void updateLeds();
};


#endif // MILLINGMACHINE_H
