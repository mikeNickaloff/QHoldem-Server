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

#include "game.h"
#include "../src_data/database.h"
#include "../src_headers/toplevel.h"
GamePlayer::GamePlayer(QObject *parent, int i_player_uid, int i_conn_id, GameTracker *i_tracker) :
   QObject(parent), m_player_uid(i_player_uid), m_connection_id(i_conn_id), m_tracker(i_tracker)
   {
      this->slot_set_table_and_pos(-1, -1);
   }
int GamePlayer::chipCount() {
      int rv = 0;
      if (this->m_tracker) {
            rv = m_tracker->m_main->m_db->get_chips(m_player_uid).toInt();
         }
      return rv;
   }
QString GamePlayer::nickName() {
      QString rv;
      if (this->m_tracker) {
            rv = m_tracker->m_main->m_db->get_nickname(m_player_uid);
         }
      return rv;
   }
void GamePlayer::slot_clear_last_entries() {
      for (int i=1; i<16; ++i) {
            this->m_last_entries[i] = -1;
         }
   }

void GamePlayer::slot_set_table(int table_num) {
      this->m_table_and_pos.first = table_num;
   }

void GamePlayer::slot_set_table_pos(int position) {
      this->m_table_and_pos.second = position;
   }

void GamePlayer::slot_set_table_and_pos(int table_num, int position) {
      this->m_table_and_pos = qMakePair(table_num, position);
   }
GameTable* GamePlayer::table() {

      //if (this->m_tracker->m_tables.keys().contains(this->m_table_and_pos.first)) {
            return  this->m_tracker->m_tables.value(this->m_table_and_pos.first, 0);
        // }
   }
