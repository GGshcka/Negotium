//
// Created by silly on 08.01.2025.
//

#ifndef NEGOTIUM_ANIMATEDGRAPHICSITEM_H
#define NEGOTIUM_ANIMATEDGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>

class AnimatedGraphicsItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    AnimatedGraphicsItem(QPixmap pixmap, QGraphicsItem *parent = nullptr)
            : QGraphicsPixmapItem(pixmap, parent) {}
};

#endif //NEGOTIUM_ANIMATEDGRAPHICSITEM_H
