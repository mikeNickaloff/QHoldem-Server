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

#include "gfx.h"

CardPixmap::CardPixmap(QPixmap &i_pixmap, QGraphicsScene *i_scene) :
   QObject(), QGraphicsPixmapItem(i_pixmap), _scene(i_scene)
   {
      setCacheMode(DeviceCoordinateCache);
      this->setOriginalPos(QPointF(0, 0));
      this->setReelDuration(0);
      this->slot_set_card_index(2);
      this->setEntryNumber(-1);
      this->board_position = qMakePair(-1, -1);
      this->_start_seed = QTime::currentTime().msec();
      this->_reel_end_y = 0;
    //  qsrand(_start_seed);
   }
void CardPixmap::slot_randomize() {

      emit this->signal_request_random_index(this->entry_number());
//      int i = (qrand()%52)+1;
//      QString pmstr = QString("%1").arg(i);
//      while ((pmstr.length() < 2) && (i != 0)) { pmstr.prepend(QString("%1").arg('0')); }
//      QPixmap pm(QString(":/cards/%1.png").arg(pmstr));
//      this->setPixmap(pm);
//      this->slot_set_card_index(i);
//      if (this->board_position.first > -1) {
//            emit this->signal_report_index_change(board_position.first, board_position.second, this->card_index());
//         }

     // if (_start_seed > 999999) { _start_seed -= 99999; } else { this->_start_seed += QTime::currentTime().second(); }

   }
void CardPixmap::slot_receive_random_index(int entry, int index) {
      if (this->entry_number() == entry) {
            this->_slot_set_index(index);
         }
   }
void CardPixmap::_slot_set_index(int index) {
      int i = index;
      QString pmstr = QString("%1").arg(i);
      while ((pmstr.length() < 2) && (i != 0)) { pmstr.prepend(QString("%1").arg('0')); }
      QPixmap pm(QString(":/cards/%1.png").arg(pmstr));
      this->setPixmap(pm);
      this->slot_set_card_index(i);
//      if (this->board_position.first > -1) {
//            emit this->signal_report_index_change(board_position.first, board_position.second, this->card_index());
//         }
      emit this->signal_report_index_change(this->entry_number(), this->card_index());
      this->update();
   }

void CardPixmap::slot_reset_position() {
      //qDebug() << "Changing Position To" << original_pos();
     // emit this->signal_reset_position(this->original_pos());
      //this->slot_randomize();
      this->setPos(original_pos());
     // this->update(this->boundingRect());
   }
void CardPixmap::slot_reel_begin() {
     // qDebug() << "Started Reel WIth Duration" << this->reel_duration();
    //  QTimer::singleShot(this->reel_duration() * 0.25, this, SLOT(slot_randomize()));
      this->slot_randomize();
      this->_scale_anim_invert_delta = false;
      this->setScale(CARD_SCALE);
    //  QTimer::singleShot(this->reel_duration(), this, SLOT(slot_reset_position()));
   }
 void CardPixmap::slot_received_invalid_index_change(int entry) {
       if (entry == this->entry_number()) {
             this->slot_randomize();
          }
    }
void CardPixmap::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
      qDebug() << "slot_insert_line(line, qMakePair(" << board_position.first << "," << board_position.second << "));";

   }
void CardPixmap::slot_start_scale_animation(QPair<int, int> dest_board_pos) {
      if (dest_board_pos.first == board_position.first) {
            if (dest_board_pos.second == board_position.second) {
                  qreal cur_scale = this->scale();
                  if (!this->_scale_anim_invert_delta) {
                        if (cur_scale >= PIXMAP_SCALE_ANIM_MAX) {
                              this->_scale_anim_invert_delta = true;
                           } else {
                              cur_scale += PIXMAP_SCALE_ANIM_DELTA;
                           }
                     } else {
                        if (cur_scale <= PIXMAP_SCALE_ANIM_MIN) {
                              this->_scale_anim_invert_delta = false;
                           } else {
                              cur_scale -= PIXMAP_SCALE_ANIM_DELTA;
                           }
                     }
                  this->setScale(cur_scale);
                  //this->setScale(CARD_SCALE * 1.05);
                  //qDebug() << "Scaling Card at " << dest_board_pos;
               }
         }
   }
void CardPixmap::slot_start_border_animation(QPair<int, int> dest_board_pos) {
      if (dest_board_pos.first == board_position.first) {
            if (dest_board_pos.second == board_position.second) {
                //  this->_scene->addRect(this->boundingRect(), QPen(), QBrush(QColor(Qt::yellow)));
                  this->setScale(PIXMAP_SCALE_BORDER_SCALE);

               }
         }
   }
