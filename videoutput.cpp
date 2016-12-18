#include "videooutput.h"
#include "chip8.h"
#include "audiooutput.h"

#include <QSGTexture>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>
#include <QThread>
#include <QTimer>

VideoOutput::VideoOutput(QQuickItem* parent)
    : QQuickItem( parent ),

      m_audioOutput( new AudioOutput ),
      m_chip8( new Chip8 ),

      m_thread( new QThread( this ) ),
      m_audioThread( new QThread( this ) ),

      m_timer( new QTimer ),

      m_texture( 64, 32, QImage::Format_ARGB32 )
{

    // I think it looks kinda cool with garbage data starting out. Uncomment this
    // if you want the video frame to initially look "correct".

    //m_texture.fill( Qt::black );
    setFlag( QQuickItem::ItemHasContents );

    connect( m_chip8, &Chip8::renderAudioFrame, m_audioOutput, &AudioOutput::renderAudioFrame );

    connect( m_chip8, &Chip8::renderVideoFrame, this, [this] ( const quint8 *t_data, int width, int height ) {

        // Copy the video texture.

        // Current things I notice that will cause an issue.
        // If the chip8 class modifies the pointer data during this write, we're in for a suprise.
        // Lock that shit!
        for ( int h=0; h < height; ++h) {
            for ( int w=0; w < width; ++w ) {
                quint8 state = t_data[ ( w  + ( h * width ) ) ];
                if ( state == 0 ) {
                    m_texture.setPixel( w, h, qRgba( 0, 0, 0, 0 ) );
                } else {
                    m_texture.setPixel( w, h, qRgba( 255, 255, 255, 255 ) );
                }
            }
        }

        // Render frame.
        update();
    });

    // Add your game path.
    m_chip8->load( "C:/Users/leewee/Downloads/c8games/INVADERS" );

    // Set up timers and threads and all the gibberish.
    m_timer->setTimerType( Qt::PreciseTimer );

    // Chip8 is really slow, so we shall speed up the gameplay.
    m_timer->setInterval( 16 / 2 );

    // Move objects to the proper threads.
    m_audioOutput->moveToThread( m_audioThread );
    m_chip8->moveToThread( m_thread );

    connect( m_thread, &QThread::started, m_timer, static_cast<void (QTimer::*)(void)>(&QTimer::start) );
    connect( m_thread, &QThread::finished, m_timer, &QTimer::stop );

    // Connect delete signals for object cleanup.
    connect( m_audioThread, &QThread::destroyed, m_audioOutput, &Chip8::deleteLater );
    connect( m_thread, &QThread::destroyed, m_chip8, &Chip8::deleteLater );
    connect( m_thread, &QThread::destroyed, m_timer, &QTimer::deleteLater );


    connect( m_timer, &QTimer::timeout, m_chip8, &Chip8::run );

    // Start dat timer baby and thread!
    m_thread->start( QThread::HighestPriority );
    m_audioThread->start( QThread::HighestPriority );

}

VideoOutput::~VideoOutput() {
    m_thread->quit();
    m_thread->wait();

    m_audioThread->quit();
    m_audioThread->wait();
}

QSGNode* VideoOutput::updatePaintNode(QSGNode* t_node, QQuickItem::UpdatePaintNodeData * ) {

    QSGSimpleTextureNode *node = static_cast<QSGSimpleTextureNode *>( t_node );
    if ( !node ) {
        node = new QSGSimpleTextureNode;
    }

    if ( !window() ) {
        return node;
    }

    // Convert the mono image to RGB32. It seems to be better supported
    // than QImage::Format_Mono is.
    QSGTexture *texture = window()->createTextureFromImage( m_texture,  QQuickWindow::TextureOwnsGLTexture );
    texture->setFiltering( QSGTexture::Nearest );

    node->setRect( boundingRect() );
    node->setTexture( texture );
    node->setOwnsTexture( true );

    return node;
}

void VideoOutput::keyPressEvent(QKeyEvent *t_event) {

    QMetaObject::invokeMethod( m_chip8, "keyEvent", Q_ARG( int, t_event->key() )
                                                  , Q_ARG( bool, true )) ;

    QQuickItem::keyPressEvent( t_event );
}

void VideoOutput::keyReleaseEvent(QKeyEvent *t_event) {

    QMetaObject::invokeMethod( m_chip8, "keyEvent", Q_ARG( int, t_event->key() )
                                                  , Q_ARG( bool, false )) ;
    QQuickItem::keyReleaseEvent( t_event );
}
