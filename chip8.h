#pragma once

#include <QByteArray>
#include <QVector>
#include <QFile>
#include <QImage>

class QKeyEvent;

class Chip8 : public QObject
{
    Q_OBJECT
public:
    explicit Chip8( QObject *parent = nullptr );

    bool load( QString t_romPath );

    void drawToConsole();

    const quint8 *gfx() const {
        return m_gfx.data();
    }

public slots:
    void run();

signals:
    void renderVideoFrame( const quint8 *t_data, int width, int height );
    void renderAudioFrame();

private slots:
    void keyEvent( int t_key, bool t_state );

private:
    QVector<quint8> m_memory;
    QVector<quint8> m_registers;
    QVector<quint16> m_stack;
    QVector<quint8> m_input;
    QVector<quint8> m_gfx;

    quint16 opcode;
    quint16 m_I;
    quint16 m_pc;
    quint16 m_sp;

    quint8 m_delayTimer;
    quint8 m_soundTimer;

    QFile m_romFile;

    void incCounter();
    void skipNext();

    quint8 extractX( quint16 opcode );

    quint8 extractY( quint16 opcode );

};

