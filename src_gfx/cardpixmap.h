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

#ifndef CARDPIXMAP_H
#define CARDPIXMAP_H

#include <QGraphicsPixmapItem>
#include <QtGui>
#include <QtCore/qstate.h>
#include <QGraphicsScene>
#include <QObject>
class CardPixmap : public QObject, public QGraphicsPixmapItem
   {
      Q_OBJECT
      Q_PROPERTY(QPointF pos READ pos WRITE setPos)
      Q_PROPERTY(QPointF original_pos READ original_pos WRITE setOriginalPos)
      Q_PROPERTY(int reel_duration READ reel_duration WRITE setReelDuration)
      Q_PROPERTY(int entry_number READ entry_number WRITE setEntryNumber)
      Q_PROPERTY(int reel_end_y READ reel_end_y WRITE setReelEndY)
   public:
     CardPixmap(QPixmap &i_pixmap, QGraphicsScene *i_scene = 0);
     QPointF original_pos() { return _original_point; }
     int reel_duration() { return _reel_duration; }
     int card_index() { return _card_index; }
     int entry_number() { return _entry_number; }
     int reel_end_y() { return _reel_end_y; }
     QPair<int,int> board_position;

   signals:
     void signal_reset_position(QPointF original_point);
     void signal_report_index_change(int entry_number, int new_index);
     void signal_request_random_index(int my_entry_number);
   public slots:
     void slot_randomize();
     void setOriginalPos(QPointF new_original_point) { _original_point = new_original_point; }
     void slot_reset_position();
     void setReelDuration(int new_duration) { _reel_duration = new_duration; _start_seed += new_duration; }
     void setEntryNumber(int new_number) { _entry_number = new_number; }
     void slot_reel_begin();
     void set_pos(QPointF new_pos) { setPos(new_pos); }
     void slot_set_card_index(int new_idx) { _card_index = new_idx; _start_seed += new_idx; }
     void slot_received_invalid_index_change(int entry);
     void slot_receive_random_index(int entry, int index);
     void setReelEndY(int new_end) { _reel_end_y = new_end; }
     void slot_start_scale_animation(QPair<int, int> dest_board_pos);
     void slot_start_border_animation(QPair<int, int> dest_board_pos);
   protected:
     void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);


   private:
     QPointF _original_point;
     int _reel_duration;
     int _card_index;
     uint _start_seed;
     int _entry_number;
     void _slot_set_index(int index);
     int _reel_end_y;
     bool _scale_anim_invert_delta;
     QGraphicsScene* _scene;


   };
#endif // CARDPIXMAP_H
