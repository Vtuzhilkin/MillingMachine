
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <millingmachine.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    MillingMachine millingMachine;

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/Milling/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    engine.rootContext()->setContextProperty("millingMachine", &millingMachine);

    return app.exec();
}
