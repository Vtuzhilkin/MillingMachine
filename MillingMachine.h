
#ifndef MILLINGMACHINE_H
#define MILLINGMACHINE_H


#include <QObject>
#include <QtQml/qqml.h>
#include <QtSerialPort/QSerialPort>

class MillingMachine : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
    QML_NAMED_ELEMENT(Miller)
    public:
    static MillingMachine* create(QQmlEngine *, QJSEngine *);
    static MillingMachine* instance();
    private:
        explicit MillingMachine(QObject *parent = nullptr);
        static MillingMachine* millingMachine;
        QSerialPort* serialPort;
        bool openedPort = false;
    private:
        void connect(int numberPort);
signals:

};


#endif // MILLINGMACHINE_H
