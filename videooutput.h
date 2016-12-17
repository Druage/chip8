#pragma once


#include "chip8.h"

#include <QQuickItem>
#include <QThread>
#include <QTimer>

class QSGSimpleTextureNode;

class VideoOutput : public QQuickItem
{
    Q_OBJECT
public:
    explicit VideoOutput( QQuickItem* parent = nullptr );
    ~VideoOutput();

    QSGNode *updatePaintNode( QSGNode *t_node, UpdatePaintNodeData *t_paintData );
    void keyPressEvent( QKeyEvent *t_event ) override {


        QQuickItem::keyPressEvent( t_event );
    }

private:
    Chip8 *m_chip8;
    QThread m_thread;
    QTimer m_timer;
    QVector<bool> m_keys;

    QImage m_texture;

};

