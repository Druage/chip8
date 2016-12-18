#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "videooutput.h"
#include "qqml.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<VideoOutput>( "Chip8", 1, 0, "VideoOutput" );

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
