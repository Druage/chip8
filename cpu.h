#pragma once

#include <QByteArray>
#include <QVector>
#include <QFile>
#include <QImage>

class CPU
{
public:
    CPU();

    bool load( QString t_romPath );

    void run();

    void drawToConsole();

    bool videoFrameReady() const {
        return m_draw;
    }

    const quint8 *gfx() const {
        return m_gfx.data();
    }

private:
    QVector<quint8> m_memory;
    QVector<quint8> m_registers;
    QVector<quint16> m_stack;
    QVector<quint8> m_input;
    QVector<quint8> m_gfx;

    QImage m_gfxImage;

    quint16 opcode;
    quint16 m_I;
    quint16 m_pc;
    quint16 m_sp;

    quint8 m_delayTimer;
    quint8 m_soundTimer;

    QFile m_romFile;

    bool m_draw;

    void incCounter();
    void skipNext();

    quint8 extractX( quint16 opcode ) {
        return ( opcode & 0x0F00 ) >> 8;
    }

    quint8 extractY( quint16 opcode ) {
        return ( opcode & 0x00F0 ) >> 4;
    }

};

