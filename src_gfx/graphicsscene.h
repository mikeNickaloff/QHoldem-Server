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

#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
class MainWindow;
class GraphicsScene : public QGraphicsScene
   {
      Q_OBJECT
   public:
     explicit GraphicsScene(qreal x, qreal y, qreal w, qreal h, QObject *parent = 0, QGraphicsView *i_view =  0, MainWindow* i_mainWindow = 0);
      QGraphicsView* _view;
      MainWindow* _main;
   signals:
      
   public slots:
      
   };

#endif // GRAPHICSSCENE_H
