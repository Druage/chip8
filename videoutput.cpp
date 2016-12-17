#include "videooutput.h"

#include <QSGTexture>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>

VideoOutput::VideoOutput(QQuickItem* parent)
    : QQuickItem( parent ),
      m_keys( 16, 0 ),
      m_chip8( new Chip8 ),
      m_texture( 64, 32, QImage::Format_RGB32 )
{

    // I think it looks kinda cool with garbage data starting out. Uncomment this
    // if you want the video frame to initially look "correct".

    //m_texture.fill( Qt::black );
    setFlag( QQuickItem::ItemHasContents );

    connect( m_chip8, &Chip8::renderVideoFrame, this, [this] ( const quint8 *t_data, int width, int height ) {

        // Copy the video texture.

        // Current things I notice that will cause an issue.
        // If the chip8 class modifies the pointer data during this write, we're in for a suprise.
        // Lock that shit!
        for ( int h=0; h < height; ++h) {
            for ( int w=0; w < width; ++w ) {
                if (  t_data[ ( w  + ( h * width ) ) ] == 0 ) {
                    m_texture.setPixel( w, h, qRgb( 0, 0, 0 ) );
                } else {
                    m_texture.setPixel( w, h, qRgb( 255, 255, 255) );
                }
            }
        }

        // Render frame.
        update();
    });

    // Add your game path.
    m_chip8->load( "C:/Users/leewee/Downloads/c8games/INVADERS" );

    // Set up timers and threads and all the gibberish.
    m_timer.setTimerType( Qt::PreciseTimer );
    m_timer.setInterval( 16 );

    m_thread.moveToThread( &m_thread );
    m_chip8->moveToThread( &m_thread );

    connect( &m_thread, &QThread::started, &m_timer, static_cast<void (QTimer::*)(void)>(&QTimer::start) );
    connect( &m_thread, &QThread::finished, &m_timer, &QTimer::stop );

    connect( &m_timer, &QTimer::timeout, m_chip8, &Chip8::run );

    // Start dat timer baby and thread!
    m_thread.start( QThread::HighestPriority );

}

VideoOutput::~VideoOutput() {
    m_thread.quit();
    m_thread.wait();
    m_chip8->deleteLater();
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
