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

#include "../src_headers/toplevel.h"
#include "gfx.h"

GraphicsScene::GraphicsScene(qreal x, qreal y, qreal w, qreal h, QObject *parent, QGraphicsView *i_view, MainWindow *i_mainWindow) :
   QGraphicsScene(x, y, w, h, parent), _view(i_view), _main(i_mainWindow)
   {
      this->setBackgroundBrush(QBrush(QColor(Qt::black)));
      this->update(this->sceneRect());
   }
