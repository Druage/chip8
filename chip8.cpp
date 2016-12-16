#include "chip8.h"

#include <QSGTexture>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>
#include <QDebug>

Chip8::Chip8(QQuickItem* parent)
    : QQuickItem( parent )
{
    setFlag( QQuickItem::ItemHasContents );
    m_cpu.load( "C:/Users/leewee/Downloads/c8games/PONG" );
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
        m_cpu.drawToConsole();

        // For whatever reason this image is not correct. I can't figure out why.

        QImage img( m_cpu.gfx(), 64, 32, 64, QImage::Format_Mono);

        QSGTexture *texture = window()->createTextureFromImage( img,  QQuickWindow::TextureOwnsGLTexture );
        texture->setFiltering( QSGTexture::Nearest );

        node->setRect( boundingRect() );
        node->setTexture( texture );
        node->setOwnsTexture( true );

    }


    update();

    return node;
}
