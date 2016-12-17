#include "chip8.h"

#include <QSGTexture>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>

Chip8::Chip8(QQuickItem* parent)
    : QQuickItem( parent )
{
    setFlag( QQuickItem::ItemHasContents );
    m_cpu.load( "C:/Users/leewee/Downloads/c8games/INVADERS" );
}

QSGNode* Chip8::updatePaintNode(QSGNode* t_node, QQuickItem::UpdatePaintNodeData * ) {

    QSGSimpleTextureNode *node = static_cast<QSGSimpleTextureNode *>( t_node );
    if ( !node ) {
        node = new QSGSimpleTextureNode;
    }

    if ( !window() ) {
        return node;
    }

    m_cpu.run();
    if ( m_cpu.videoFrameReady() ) {

        // Convert the mono image to RGB32. It seems to be better supported
        // than QImage::Format_Mono is.
        QImage img( 64, 32, QImage::Format_RGB32 );

        for ( int h=0; h < 32; ++h) {
            for ( int w=0; w < 64; ++w ) {
                if (  m_cpu.gfx()[ ( w  + ( h * 64 ) ) ] == 0 ) {
                    img.setPixel( w, h, qRgb( 0, 0, 0 ) );
                } else {
                    img.setPixel( w, h, qRgb( 255, 255, 255) );
                }
            }
        }

        QSGTexture *texture = window()->createTextureFromImage( img,  QQuickWindow::TextureOwnsGLTexture );
        texture->setFiltering( QSGTexture::Nearest );

        node->setRect( boundingRect() );
        node->setTexture( texture );
        node->setOwnsTexture( true );

    }

    update();

    return node;
}
