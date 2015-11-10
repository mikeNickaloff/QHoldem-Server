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
#include "../src_network/network.h"
#include "../src_threads/threads.h"
#include "../src_headers/toplevel.h"
#include "../src_gfx/gfx.h"
#include "../src_data/database.h"

#include <QtDebug>
GameTable::GameTable(QObject *parent, GameTracker *i_tracker, int i_table_id) :
   QObject(parent), m_tracker(i_tracker), m_tableId(i_table_id)
   {
      qDebug() << "Table Created!";
      int e = 1;
      this->m_tablePos_pocket_entry_numbers[e] = qMakePair(7, 8);
      e++;
      this->m_tablePos_pocket_entry_numbers[e] = qMakePair(5, 6);
      e++;
      this->m_tablePos_pocket_entry_numbers[e] = qMakePair(3, 4);
      e++;
      this->m_tablePos_pocket_entry_numbers[e] = qMakePair(1, 2);
      e++;
      this->m_tablePos_pocket_entry_numbers[e] = qMakePair(9, 10);
      for (int z=1; z<16; ++z) {
            this->m_table_cards[z] = 0;
         }
      this->m_game_started = false;
      this->m_double_blind_chip = 1;
      this->m_river_cards_dealt = 0;
      m_current_bet = 0;
      for (int i=1; i<6; ++i) {
            this->m_playing[i] = false;
         }

   }
bool GameTable::slot_player_try_join_table(GamePlayer* i_player) {
      int i = 0;
      bool rv = false;
      while ((i < TABLE_MAX_PLAYERS) && (rv == false)) {
            i++;
            if (!m_players.keys().contains(i)) {
                  rv = true;
                  //qDebug() << "Player from Connection ID" << i_player->m_connection_id << "and uid" << i_player->m_player_uid << "joined table" << m_tableId;
                  this->m_players[i] = i_player;
                  i_player->slot_set_table_and_pos(this->m_tableId, i);
                  i_player->m_current_bet = 0;
                  i_player->hasTurn = false;
                  if (this->m_game_started) {
                        this->m_playing[i] = false;
                     } else {

                        this->m_playing[i] = true;

                     }

               }
         }
      if (rv) {
            this->slot_send_game_seats();
         }
      return rv;
   }
void GameTable::slot_remove_player(GamePlayer *i_player) {
      QHashIterator<int, GamePlayer*> o(m_players);
      o.toFront();
      int remove_num = 0;
      bool found_remove = false;
      while ((o.hasNext()) && (found_remove == false)) {
            o.next();
            GamePlayer* player = o.value();
            if (player == i_player) {
                  found_remove = true;
                  remove_num = o.key();
               }
         }
      if (found_remove) {
            //qDebug() << "Removing Player with Connection ID" << i_player->m_connection_id << "from Table" << this->m_tableId;
            if (remove_num == this->m_game_turn) { this->slot_move_turn(); }

            this->m_playing[remove_num] = false;
            if (m_playing.values().count(true) < 2) {
                  // this->m_game_started = false;

               }
         }
      this->m_players.remove(remove_num);
      QTimer::singleShot(2000, this, SLOT(slot_send_game_turns()));
      this->slot_send_game_seats();
      //this->slot_send_game_turns();
   }
void GameTable::slot_reset_game() {
      for (int z=1; z<16; ++z) {
            this->m_table_cards[z] = this->m_tracker->m_main->m_cards.value(z)->card_index();
         }
      this->m_game_started = false;
      this->m_river_cards_dealt = 0;
      this->m_current_bet = 0;
      this->m_pot = 0;


   }
void GameTable::slot_deal_cards() {

//      QMutex mutex;
//      QWaitCondition wait;
//      mutex.lock();
//      wait.wait(&mutex, 2000);
//      mutex.unlock();

      if (!this->m_game_started) {
            this->slot_move_double_blind_chip();
            this->m_game_turn = this->m_double_blind_chip;
            this->m_river_cards_dealt = 0;
            this->m_current_bet = TABLE_ANTE;
            QHashIterator<int, GamePlayer*> o(m_players);
            o.toFront();
            this->m_pot = 0;
            while (o.hasNext()) {
                  o.next();
                  int i = o.key();


                  GamePlayer* player = o.value();
                  if (player->chipCount() > 0) {
                        if (i != this->m_game_turn) {
                              if (i == this->big_blind()) {
                                    emit this->signal_player_bet(player, qRound(TABLE_ANTE * 0.5));
                                    m_pot += qRound(TABLE_ANTE);
                                 } else {
                                    emit this->signal_player_bet(player, TABLE_ANTE);
                                    m_pot += qRound(TABLE_ANTE);
                                 }
                           } else {
                              // emit this->signal_player_bet(player, TABLE_ANTE);
                              //   m_pot += qRound(TABLE_ANTE);

                           }
                        this->m_playing[i] = true;
                        player->hasTurn = true;
                        player->m_score = 0;
                        player->slot_clear_last_entries();
                        if (i == this->m_game_turn) {
                              m_tracker->find_client(player)->slot_announce("CHIPS", QString("%1").arg(player->chipCount()));
                              m_tracker->find_client(player)->slot_announce("TURN", QString("%1").arg(TABLE_ANTE));
                           } else {
                              m_tracker->find_client(player)->slot_announce("CHIPS", QString("%1").arg(player->chipCount()));
                              m_tracker->find_client(player)->slot_announce("WAIT", QString("%1").arg(this->m_game_turn));
                           }
                     } else {
                        this->m_playing[i] = false;
                        player->hasTurn = false;
                        player->m_score = 0;
                        player->slot_clear_last_entries();
                        m_tracker->find_client(player)->slot_announce("WAIT", QString("%1").arg(this->m_game_turn));
                        //this->slot_remove_player(player);
                        this->slot_remove_player(player);

                     }
               }
            this->slot_send_game_cards();
            this->m_game_started = true;
            //this->m_current_bet = 0;
         }


   }

void GameTable::slot_move_double_blind_chip() {
      if (this->m_players.keys().count() > 1) {
            int cur_pos = this->m_double_blind_chip;
            if (cur_pos == TABLE_MAX_PLAYERS) { cur_pos = 1; } else { cur_pos++; }
            while (this->m_playing.value(cur_pos) == false) {
                  if (cur_pos == TABLE_MAX_PLAYERS) { cur_pos = 1; } else { cur_pos++; }
               }
            this->m_double_blind_chip = cur_pos;
         } else {
            this->m_game_started = false;
         }
   }
int GameTable::big_blind() {
      int rv = 0;
      if (this->m_players.keys().count() > 1) {
            int cur_pos = this->m_double_blind_chip;
            if (cur_pos == 1) { cur_pos = 5; } else { cur_pos--; }
            while (this->m_playing.value(cur_pos) == false) {
                  if (cur_pos == 1) { cur_pos = 5; } else { cur_pos--; }
               }
            rv = cur_pos;
         } else {
            this->m_game_started = false;
         }
      return rv;
   }

void GameTable::slot_move_turn() {
      if (this->m_players.keys().count() > 1) {
            int cur_pos = this->m_game_turn;
            if (cur_pos == TABLE_MAX_PLAYERS) { cur_pos = 1; } else { cur_pos++; }
            while (this->m_playing.value(cur_pos) == false) {
                  if (cur_pos == TABLE_MAX_PLAYERS) { cur_pos = 1; } else { cur_pos++; }
               }
            this->m_game_turn = cur_pos;

         } else {
            this->m_game_started = false;
         }
   }
void GameTable::slot_receive_turn(QString turn, int connect_id) {
//      QMutex mutex;
//      QWaitCondition wait;
//      mutex.lock();
//      wait.wait(&mutex, 2000);
//      mutex.unlock();


      this->slot_move_turn();

      QHashIterator<int, GamePlayer*> o(m_players);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();
            GamePlayer* player = o.value();
            if (turn == "FOLD") {
                  if (connect_id == player->m_connection_id) {
                        this->m_playing[i] = false;
                        player->hasTurn = false;
                        if (i == m_game_turn) {
                              this->slot_move_turn();
                           }
                     }

               }
            if (turn == "CHECK") {
                  if (connect_id == player->m_connection_id) {
                        if (player->hasTurn) {
                              player->hasTurn = false;

                           }

                     }
               }

         }
      this->slot_send_game_seats();
      QTimer::singleShot(2000, this, SLOT(slot_send_game_turns()));
     // this->slot_send_game_turns();

   }
void GameTable::slot_receive_turn_amt(QString turn, int connect_id, int amount) {
//      QMutex mutex;
//      QWaitCondition wait;
//      mutex.lock();
//      wait.wait(&mutex, 2000);
//      mutex.unlock();
      this->slot_move_turn();

      QHashIterator<int, GamePlayer*> o(m_players);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();
            GamePlayer* player = o.value();
            if (turn == "BET") {
                  if (connect_id == player->m_connection_id) {
                        if (player->hasTurn) {
                              player->hasTurn = false;
                              //  emit this->signal_player_bet(player, amount );
                              this->slot_player_bet(player, amount);
                              //  this->m_current_bet = amount;

                           }

                     }
               }
         }
      this->slot_send_game_seats();
       QTimer::singleShot(2000, this, SLOT(slot_send_game_turns()));
      //this->slot_send_game_turns();


   }

void GameTable::slot_send_game_turns() {
      QHashIterator<int, GamePlayer*> o(m_players);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();

            GamePlayer* player = o.value();
            if (i == this->m_game_turn) {
                  if (player->hasTurn) {
                        m_tracker->find_client(player)->slot_announce("TURN", QString("%1").arg(m_current_bet));
                        m_tracker->find_client(player)->slot_announce("CURRENT_TURN", QString("%1").arg(QString("%1").arg(this->m_game_turn)));
                     } else {
                        //qDebug() << "PROCESSING RIVER!!";
                        this->m_current_bet = 0;
                        this->slot_process_river();
                        m_tracker->find_client(player)->slot_announce("TURN", QString("%1").arg(m_current_bet));
                        m_tracker->find_client(player)->slot_announce("CURRENT_TURN", QString("%1").arg(QString("%1").arg(this->m_game_turn)));

                     }
               } else {
                  m_tracker->find_client(player)->slot_announce("WAIT", QString("%1").arg(i));
                  m_tracker->find_client(player)->slot_announce("CURRENT_TURN", QString("%1").arg(QString("%1").arg(this->m_game_turn)));

               }
         }
   }

void GameTable::slot_send_game_cards() {
      QHashIterator<int, GamePlayer*> o(m_players);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();

            GamePlayer* player = o.value();

            QList<QPair<int, int> > full_list;
            QPair<int, int> pocket = this->m_tablePos_pocket_entry_numbers.value(i);
            int p1 = pocket.first;
            int p2 = pocket.second;
            player->m_pocket = qMakePair(this->m_table_cards.value(p1), this->m_table_cards.value(p2));
            QList<int> river_entries;
            int g = 11;
            for (int m=0; m<this->m_river_cards_dealt; ++m) {
                  river_entries << g;
                  g++;
               }

            for (int e=1; e<16; ++e) {
                  if ((e != p1) && (e != p2)) {
                        if (river_entries.contains(e)) {
                              if (player->m_last_entries.value(e) != m_table_cards.value(e)) {
                                    full_list << qMakePair(e, m_table_cards.value(e));
                                    player->m_last_entries[e] = m_table_cards.value(e);
                                 }
                           } else {
                              if (player->m_last_entries.value(e) != 0) {
                                    full_list << qMakePair(e, 0);
                                    player->m_last_entries[e] = 0;
                                 }
                           }
                     }
               }
            if (player->m_last_entries.value(p1) != m_table_cards.value(p1)) {
                  full_list << qMakePair(p1, this->m_table_cards.value(p1));
                  player->m_last_entries[p1] = m_table_cards.value(p1);
               }
            if (player->m_last_entries.value(p2) != m_table_cards.value(p2)) {
                  full_list << qMakePair(p2, this->m_table_cards.value(p2));
                  player->m_last_entries[p2] = m_table_cards.value(p2);
               }
            //full_list << qMakePair(p2, this->m_table_cards.value(p2));



            if (full_list.size() > 0) {
                  this->m_tracker->find_client(player)->slot_specify_full_index_list(full_list);
               }

         }
   }
void GameTable::slot_process_river() {
      if (this->m_river_cards_dealt == 0) {
            this->m_river_cards_dealt = 3;
         } else {
            if (m_river_cards_dealt == 3) {
                  m_river_cards_dealt++;
               } else {
                  if (m_river_cards_dealt == 4) {
                        this->m_river_cards_dealt++;

                     } else {
                        if (m_river_cards_dealt == 5) {
                              //qDebug() << "GAME OVER!" << "CHECK HANDS";
                              QList<GamePlayer*> winners;
                              winners << this->get_pot_winners();
                              int num_winners = winners.count();
                              foreach (GamePlayer* player, winners) {
                                    qreal win_amt = (qreal) m_pot;
                                    win_amt /= (qreal) num_winners;
                                    this->m_tracker->find_client(player)->slot_announce("WIN", QString("%1").arg(qRound(win_amt)));
                                    this->m_tracker->slot_award_chips(player, qRound(win_amt));
                                 }

                              this->slot_reset_game();
                              QTimer::singleShot(3000, this, SLOT(slot_deal_cards()));
                           }
                     }

               }

         }
      this->slot_send_game_cards();
      QHashIterator<int, GamePlayer*> o(m_players);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();

            GamePlayer* player = o.value();
            if (m_playing.value(i) == true) {
                  player->hasTurn = true;
               }
         }
      // this->slot_send_game_turns();

      /*  END OF RIVER PROCESSING */
   }
int GameTable::find_score(QList<int> idx_list) {
      int rv = 0;
      QList<int> rel_vals;
      rv = this->m_tracker->m_main->m_checker->find_score(idx_list, rel_vals);
      return rv;
   }

QList<int> GameTable::get_idx_list(GamePlayer* i_player) {
      QList<int> idx_list;
      QHashIterator<int, GamePlayer*> o(m_players);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();

            GamePlayer* player = o.value();
            if (player == i_player) {


                  QPair<int, int> pocket = this->m_tablePos_pocket_entry_numbers.value(i);
                  int p1 = pocket.first;
                  int p2 = pocket.second;
                  idx_list << this->m_table_cards.value(p1);
                  idx_list << this->m_table_cards.value(p2);
                  if (this->m_river_cards_dealt >= 3) {
                        idx_list << this->m_table_cards.value(11);
                        idx_list << this->m_table_cards.value(12);
                        idx_list << this->m_table_cards.value(13);
                     }
                  if (m_river_cards_dealt >= 4) {
                        idx_list << this->m_table_cards.value(14);
                     }
                  if (m_river_cards_dealt >= 5) {
                        idx_list << this->m_table_cards.value(15);
                     }
               }

         }
      return idx_list;
   }
void GameTable::slot_populate_scores() {
      QHashIterator<int, GamePlayer*> o(m_players);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();

            GamePlayer* player = o.value();
            if (m_playing.value(i) == true) {
                  QList<int> idx_list;
                  idx_list << this->get_idx_list(player);
                  int score = this->find_score(idx_list);
                  player->m_score = score;
               } else {
                  player->m_score = 0;
               }
         }
   }
QList<GamePlayer*> GameTable::get_pot_winners() {
      this->slot_populate_scores();
      QList<GamePlayer*> rv;
      int best_score = 0;
      QHashIterator<int, GamePlayer*> o(m_players);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();

            GamePlayer* player = o.value();
            if (m_playing.value(i) == true) {
                  if (player->m_score > best_score) {
                        best_score = player->m_score;
                     }
               }

         }
      this->slot_winning_score(best_score);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();

            GamePlayer* player = o.value();
            if (m_playing.value(i) == true) {
                  if (player->m_score == best_score) {
                        rv << player;
                     }

               }
         }
      return rv;
   }
void GameTable::slot_send_game_seats() {
      QHashIterator<int, GamePlayer*> o(m_players);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();

            GamePlayer* player = o.value();
            QHashIterator<int, GamePlayer*> p(m_players);
            p.toFront();
            while (p.hasNext()) {
                  p.next();
                  int j = p.key();
                  GamePlayer* d_player = p.value();
                  if (this->m_playing.value(j) == true) {
                        QString seat_data;
                        seat_data = QString("%1:%2:%3").arg(j).arg(d_player->nickName()).arg(d_player->chipCount());
                        this->m_tracker->find_client(player)->slot_announce("SEAT", seat_data);
                     } else {
                        QString seat_data;
                        seat_data = QString("%1:%2:%3").arg(j).arg(" ").arg(0);
                        this->m_tracker->find_client(player)->slot_announce("SEAT", seat_data);
                     }
               }
         }
      QHash<int, bool>::const_iterator i;
      i = this->m_playing.constBegin();
      while (i != m_playing.constEnd()) {
            if (!m_players.keys().contains(i.key())) {
                  QHashIterator<int, GamePlayer*> p(m_players);
                  p.toFront();
                  while (p.hasNext()) {
                        p.next();
                        int j = p.key();
                        GamePlayer* d_player = p.value();


                        QString seat_data;
                        seat_data = QString("%1:%2:%3").arg(i.key()).arg(" ").arg(0);
                        this->m_tracker->find_client(d_player)->slot_announce("SEAT", seat_data);
                     }


               }
            i++;
         }
   }
void GameTable::slot_winning_score(int i_score) {
     // this->m_tracker->m_main->m_db->slot_winning_score(i_score);
   }
void GameTable::slot_player_bet(GamePlayer *i_player, int amt) {
      //qDebug() << "Player" << i_player->nickName() << "bet" << amt;
      this->m_pot += amt;
      QHashIterator<int, GamePlayer*> p(m_players);
      p.toFront();
      while (p.hasNext()) {
            p.next();
            int j = p.key();
            GamePlayer* d_player = p.value();
            if (d_player != i_player) {
                  if (this->m_playing.value(j)) {
                        if (amt > m_current_bet) {
                              d_player->hasTurn = true;

                           } else {

                           }
                     }
               }
         }



      emit this->signal_player_bet(i_player, amt);
      if (amt > m_current_bet) { m_current_bet = amt; }



   }
