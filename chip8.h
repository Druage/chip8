#pragma once

//#include <QQuickItem>
#include <QQuickItem>

#include "cpu.h"

#include <QPainter>

class QSGSimpleTextureNode;

class Chip8 : public QQuickItem
{
    Q_OBJECT
public:
    explicit Chip8( QQuickItem* parent = nullptr );
    ~Chip8() = default;

    QSGNode *updatePaintNode( QSGNode *t_node, UpdatePaintNodeData *t_paintData );

private:
    CPU m_cpu;
};

