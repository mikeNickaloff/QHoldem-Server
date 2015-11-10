/****************************************************************************
**
** Copyright (C) 2012 Mike Nickaloff.
** All rights reserved.
** Contact: seppfuv@gmail.com  (http://www.alamedasbestcatering.com/mike)
**
**
** GNU General Public License Usage
** This file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
****************************************************************************/

//#ifndef GFX_H
//#define GFX_H

//#endif // GFX_H

#include "graphicsscene.h"
#include <QtGui>
#include "cardpixmap.h"
#include <QAbstractTransition>
#include <QState>
#include <QParallelAnimationGroup>
#include <QStateMachine>

#define CARD_SCALE 1.5
#define CARDS_PER_REEL 27
#define PIXMAP_SCALE_ANIM_MAX 1.60
#define PIXMAP_SCALE_ANIM_MIN 1.40
#define PIXMAP_SCALE_ANIM_DELTA 0.025
#define PIXMAP_SCALE_BORDER_SCALE 1.75



class View : public QGraphicsView
 {
 public:
     View(QGraphicsScene *scene) : QGraphicsView(scene) { }

 protected:
     void resizeEvent(QResizeEvent *event)
     {
         QGraphicsView::resizeEvent(event);
         fitInView(sceneRect(), Qt::KeepAspectRatio);
     }
 };
