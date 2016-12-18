#pragma once

#include <QQuickItem>
#include <QImage>

class AudioOutput;
class QTimer;
class QThread;
class Chip8;

class QSGSimpleTextureNode;

class VideoOutput : public QQuickItem
{
    Q_OBJECT
public:
    explicit VideoOutput( QQuickItem* parent = nullptr );
    ~VideoOutput();

    QSGNode *updatePaintNode( QSGNode *t_node, UpdatePaintNodeData *t_paintData ) override;

    void keyPressEvent( QKeyEvent *t_event ) override;
    void keyReleaseEvent( QKeyEvent *t_event ) override;

private:
    AudioOutput *m_audioOutput;
    Chip8 *m_chip8;
    QThread *m_thread;
    QThread *m_audioThread;
    QTimer *m_timer;

    QImage m_texture;

};
