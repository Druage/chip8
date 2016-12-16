#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "chip8.h"
#include "qqml.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Chip8>( "emulator", 1, 0, "Chip8" );

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));


    /*
     * CPU cpu;
    bool loaded = cpu.load( "C:/Users/leewee/Downloads/c8games/PONG" );

    if ( loaded  ) {
        while ( true ) {
            cpu.run();
            if ( cpu.draw() ) {
                qDebug() << "draw";
                QPixmap::fromImage( cpu.videoFrame() );
            }
        }
    }
     *
     */

    return app.exec();
}
