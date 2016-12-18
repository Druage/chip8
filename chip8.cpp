#include "chip8.h"

#include <QDebug>
#include <ctime>

unsigned char fonts[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


Chip8::Chip8( QObject *parent )
    : QObject( parent ),
      m_memory( 4096, 0 ),
      m_registers( 16, 0 ),
      m_stack( 16, 0 ),
      m_input( 16, 0 ),
      m_gfx( 64 * 32, 0 ),

      opcode( 0 ),
      m_I( 0 ),
      m_pc( 0x200 ),
      m_sp( 0 ),

      m_delayTimer( 0 ),
      m_soundTimer( 0 )

{

    for ( int i=0; i < 80; ++i ) {
        m_memory[ i ] = fonts[ i ];
    }

    qsrand( std::time( nullptr ) );

}

bool Chip8::load(QString t_romPath) {
    if ( m_romFile.isOpen() ) {
        m_romFile.close();
    }

    m_romFile.setFileName( t_romPath );
    if ( m_romFile.open( QIODevice::ReadOnly ) ) {
        QByteArray data = m_romFile.readAll();
        for ( int i=0; i < data.size(); ++i ) {
            m_memory[ i + m_pc ] = data[ i ];
        }

        return true;
    }

    return false;

}

void Chip8::run() {
    opcode = m_memory[ m_pc ] << 8 | m_memory[ m_pc + 1 ];
    //qDebug() << QByteArray::number( opcode, 16 );
    switch( opcode & 0xF000 ) {
        case 0x0000:
            switch( opcode & 0x000F ) {
                case 0x0000: {
                    for ( int i=0; i < m_gfx.size(); ++i ) {
                        m_gfx[ i ] = 0;
                    }
                    // Execute opcode
                    //m_gfxImage.fill( 0 );
                    emit renderVideoFrame( m_gfx.data(), 64, 32 );
                    incCounter();
                    break;
                }
                case 0x000E:
                    // Return from subroutine
                    --m_sp;
                    m_pc = m_stack[ m_sp ];
                    incCounter();
                    break;
                default:
                    qDebug() << "Unknown OPcode discovered:" << QByteArray::number( opcode, 16 );
                    break;
            }
            break;

        case 0x1000:
            m_pc = opcode & 0x0FFF;
            break;

        case 0x2000:
            m_stack[ m_sp ] = m_pc;
            ++m_sp;
            m_pc = opcode & 0x0FFF;
            break;

        case 0x3000:
            if ( m_registers[ extractX( opcode )] == ( opcode & 0x00FF ) ) {
                skipNext();
            } else {
                incCounter();
            }
            break;

        case 0x4000:
            if ( m_registers[ extractX( opcode ) ] != ( opcode & 0x00FF ) ) {
                skipNext();
            } else {
                incCounter();
            }
            break;

        case 0x5000:
            if ( m_registers[ extractX( opcode ) ] == m_registers[ extractY( opcode ) ] ) {
                skipNext();
            } else {
                incCounter();
            }
            break;

        case 0x6000: {
            m_registers[ extractX( opcode ) ] = opcode & 0x00FF;
            incCounter();
            break;
        }

        case 0x7000: {
            m_registers[ extractX( opcode ) ] += opcode & 0x00FF;
            incCounter();
            break;
        }

        case 0x8000: {
            switch( opcode & 0x000F ) {
                case 0x0000:
                    m_registers[ extractX( opcode ) ] = m_registers[ extractY( opcode ) ];
                    incCounter();
                    break;
                case 0x0001:
                    m_registers[ extractX( opcode ) ] |= m_registers[ extractY( opcode ) ];
                    incCounter();
                    break;
                case 0x0002:
                    m_registers[ extractX( opcode ) ] &= m_registers[  extractY( opcode ) ];
                    incCounter();
                    break;
                case 0x0003:
                    m_registers[ extractX( opcode ) ] ^= m_registers[  extractY( opcode ) ];
                    incCounter();
                    break;
                case 0x0004: {
                    // This is a tricky and interesting part, study this more, on how to find bit overflows
                    if ( m_registers[ extractY( opcode ) ] > ( 0xFF - m_registers[ extractX( opcode ) ]  ) ) {
                        m_registers[ 0xF ] = 1; // carry
                    } else {
                        m_registers[ 0xF ] = 0;
                    }

                    m_registers[ extractX( opcode ) ] += m_registers[ extractY( opcode ) ];
                    incCounter();
                    break;
                }
                case 0x0005:
                    if ( m_registers[ extractY( opcode ) ] > m_registers[ extractX( opcode ) ] ) {
                        m_registers[ 0xF ] = 0; // borrow
                    } else {
                        m_registers[ 0xF ] = 1;
                    }
                    m_registers[ extractX( opcode ) ] -= m_registers[ extractY( opcode ) ];
                    incCounter();
                    break;
                case 0x0006:
                    m_registers[ 0xF ] = m_registers[ extractX( opcode ) ] & 0x1;
                    m_registers[ extractX( opcode ) ] >>= 1;
                    incCounter();
                    break;
                case 0x0007:
                    if ( m_registers[ extractX( opcode ) ] > m_registers[ extractY( opcode ) ] ) {
                        m_registers[ 0xF ] = 0; // borrow
                    } else {
                        m_registers[ 0xF ] = 1;
                    }

                    m_registers[ extractX( opcode ) ] = m_registers[ extractY( opcode ) ] - m_registers[ extractX( opcode ) ];
                    incCounter();
                    break;
                case 0x000E:
                     m_registers[ 0xF ] = m_registers[ extractX( opcode ) ] >> 7;
                     m_registers[ extractX( opcode ) ] <<= 1;
                     incCounter();
                    break;
                default:
                    qDebug() << "Unknown OPcode discovered:" << QByteArray::number( opcode, 16 );
                    break;
            }

            break;
        }

        case 0x9000:
            if ( m_registers[ extractX( opcode ) ] != m_registers[ extractY( opcode ) ] ) {
                skipNext();
            } else {
                incCounter();
            }
            break;

        case 0xA000: {
            // Sets I to the address NNN.
            m_I = opcode & 0x0FFF;
            incCounter();
            break;
        }

        case 0xB000:
            m_pc = m_registers[ 0 ] + ( opcode & 0x0FFF );
            break;

        case 0xC000:
            m_registers[ extractX( opcode ) ] = ( qrand() % 255 )  &  ( opcode & 0x00FF );
            incCounter();
            break;

        case 0xD000: {
            quint16 x = m_registers[ extractX( opcode ) ];
            quint16 y = m_registers[ extractY( opcode ) ];

            quint16 height = opcode & 0x000F;
            quint16 pixel;

            m_registers[ 0xF ] = 0;
            for ( int col=0; col < height; ++col ) {
                pixel = m_memory[ m_I + col ];

                for ( int row=0; row < 8; ++row ) {
                    if ( ( pixel & ( 0x80 >> row ) ) != 0 ) {

                        if ( m_gfx[ ( x + row + ( ( y + col ) * 64 ) ) ] == 1 ) {
                         m_registers[ 0xF ] = 1;
                        }
                        m_gfx[ ( x + row + ( ( y + col ) * 64 ) ) ] ^= 1;
                    }
                }
            }

            emit renderVideoFrame( m_gfx.data(), 64, 32 );
            incCounter();
            break;
        }

        case 0xE000: {
            switch( opcode & 0x00FF ) {
                case 0x009E:
                    if ( m_input[ m_registers[ extractX( opcode ) ] ] == 1 ) {
                        skipNext();
                    } else {
                        incCounter();
                    }
                    break;
                case 0x00A1:
                    if ( m_input[ m_registers[ extractX( opcode ) ] ] == 0 ) {
                        skipNext();
                    } else {
                        incCounter();
                    }
                    break;
                default:
                    qDebug() << "Unknown OPcode discovered:" << QByteArray::number( opcode, 16 );
                    break;
            }

            break;
        }

        case 0xF000: {

            switch( opcode & 0x00FF ) {
                case 0x0007:
                    // Sets VX to the value of the delay timer.
                    m_registers[ extractX( opcode ) ] = m_delayTimer;
                    incCounter();
                    break;
                case 0x000A: {
                    // A key press is awaited, and then stored in VX.
                    // (Blocking Operation. All instruction halted until
                    // next key event)

                    bool pressed = false;
                    for ( int i=0; i < m_input.size(); ++i ) {
                        if ( m_input[ i ] == 1 ) {
                            m_registers[ extractX( opcode ) ] = i;
                            pressed = true;
                        }
                    }

                    if ( !pressed ) {
                        return;
                    }

                    incCounter();
                    break;
                }
                case 0x0015:
                    // Sets the delay timer to VX.
                    m_delayTimer = m_registers[ extractX( opcode ) ];
                    incCounter();
                    break;
                case 0x0018:
                    // 	Sets the sound timer to VX.
                    m_soundTimer = m_registers[ extractX( opcode ) ];
                    incCounter();
                    break;
                case 0x001E: {
                    // Adds VX to I.
                    if ( m_I + m_registers[ extractX( opcode ) ] > 0xFFF ) {
                        m_registers[ 0xF ] = 1;
                    } else {
                        m_registers[ 0xF ] = 0;
                    }

                    m_I += m_registers[ extractX( opcode ) ];
                    incCounter();
                    break;
                }
                case 0x0029:
                    // Sets I to the location of the sprite for the
                    // character in VX. Characters 0-F (in hexadecimal)
                    // are represented by a 4x5 font.

                    m_I = m_registers[ extractX( opcode ) ] * 0x5;
                    incCounter();
                    break;
                case 0x0033:
                    m_memory[ m_I + 0 ] = m_registers[ extractX( opcode ) ] / 100;
                    m_memory[ m_I + 1 ] = ( m_registers[ extractX( opcode ) ] / 10) % 10;
                    m_memory[ m_I + 2] = ( m_registers[ extractX( opcode ) ] % 100 ) % 10;
                    incCounter();
                    break;
                case 0x0055: {
                    int range = extractX( opcode );
                    for ( int i=0; i < range; ++i ) {
                        m_memory[ m_I + i ] = m_registers[ i ];
                    }
                    m_I += range + 1;
                    incCounter();
                    break;
                }
                case 0x0065: {
                    int range = extractX( opcode );
                    for ( int i=0; i < range; ++i ) {
                         m_registers[ i ] = m_memory[ m_I + i ];
                    }
                    m_I += range + 1;
                    incCounter();
                    break;
                }
                default:
                    qDebug() << "Unknown OPcode discovered:" << QByteArray::number( opcode, 16 );
                    break;
            }

            break;

        }

        default:
            qDebug() << "Unknown OPcode discovered:" << QByteArray::number( opcode & 0xF000, 16 );
            break;
    }

    // Update timers
    if ( m_delayTimer > 0 ) {
        --m_delayTimer;
    }

    if ( m_soundTimer > 0 ) {
        if ( m_soundTimer == 1 ) {
            renderAudioFrame();
        }
        --m_soundTimer;
    }

}

void Chip8::keyEvent( int t_key, bool t_state) {
    switch( t_key ) {
        case Qt::Key_1:
            m_input[ 1 ] = t_state;
            break;
        case Qt::Key_2:
            m_input[ 2 ] = t_state;
            break;
        case Qt::Key_3:
            m_input[ 3 ] = t_state;
            break;
        case Qt::Key_4:
            m_input[ 0xC ] = t_state;
            break;
        case Qt::Key_Q:
            m_input[ 4 ] = t_state;
            break;
        case Qt::Key_W:
            m_input[ 5 ] = t_state;
            break;
        case Qt::Key_E:
            m_input[ 6 ] = t_state;
            break;
        case Qt::Key_R:
            m_input[ 0xD ] = t_state;
            break;
        case Qt::Key_A:
            m_input[ 7 ] = t_state;
            break;
        case Qt::Key_S:
            m_input[ 8 ] = t_state;
            break;
        case Qt::Key_D:
            m_input[ 9 ] = t_state;
        case Qt::Key_F:
            m_input[ 0xE ] = t_state;
            break;
        case Qt::Key_Z:
            m_input[ 0xA ] = t_state;
            break;
        case Qt::Key_X:
            m_input[ 0 ] = t_state;
            break;
        case Qt::Key_C:
            m_input[ 0xB ] = t_state;
            break;
        case Qt::Key_V:
            m_input[ 0xF ] = t_state;
            break;
        default:
            break;
    }
}

void Chip8::drawToConsole() {
    // Draw
    auto debug = qDebug();
    for(int y = 0; y < 32; ++y)
    {
        for(int x = 0; x < 64; ++x)
        {
            if( m_gfx[(y*64) + x] == 0)
                debug << " ";
            else
                debug << "X";
        }
        debug << "\n";
    }

}

void Chip8::incCounter() {
    m_pc += 2;
}

void Chip8::skipNext() {
        m_pc += 4;
    }

quint8 Chip8::extractX(quint16 opcode) {
    return ( opcode & 0x0F00 ) >> 8;
}

quint8 Chip8::extractY(quint16 opcode) {
    return ( opcode & 0x00F0 ) >> 4;
}
