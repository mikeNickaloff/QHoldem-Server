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
#include <QtDebug>
#include "../src_headers/toplevel.h"
#include "../src_network/network.h"
#include "../src_threads/threads.h"
#include "../src_data/database.h"
GameTracker::GameTracker(QObject *parent, MainWindow *i_main) :
   QObject(parent), m_main(i_main)
   {
      int last_table = 0;
      this->slot_create_table(last_table);
   }
void GameTracker::slot_create_table(int &ret_table_id) {
      int i = 1;
      while (this->m_tables.keys().contains(i)) {
            i++;

         }
      this->new_table = new GameTable(this, this, i);
      this->m_tables[i] = new_table;
      this->connect(this, SIGNAL(signal_remove_player(GamePlayer*)), new_table, SLOT(slot_remove_player(GamePlayer*)));
      this->connect(new_table, SIGNAL(signal_player_bet(GamePlayer*,int)), this, SLOT(slot_player_bet(GamePlayer*,int)));
      // this->connect(new_table, SIGNAL(signal_player_bet(GamePlayer*,int)), new_table, SLOT(slot_player_bet(GamePlayer*,int)));
      ret_table_id = i;

   }
void GameTracker::slot_create_player(int player_uid, int connection_id) {
      if (this->m_players.keys().contains(connection_id)) {
            qDebug() << "player is already logged in under connection id" << connection_id;
         } else {
            this->new_player = new GamePlayer(this, player_uid, connection_id, this);
            this->m_players[connection_id] = new_player;
            QHashIterator<int, GameTable*> o(m_tables);
            o.toFront();
            bool found_table = false;
            int last = 0;
            while (o.hasNext()) {
                  o.next();
                  if (found_table == false) {
                        GameTable* table = o.value();
                        found_table = table->slot_player_try_join_table(new_player);
                        if (found_table) { last = o.key(); }
                     }

               }
            if (!found_table) {

                  this->slot_create_table(last);
                  found_table = this->m_tables.value(last)->slot_player_try_join_table(new_player);
               }
            if (found_table) {
                  if (m_tables.value(last)->m_players.keys().count() >= 2) {
                        if (!m_tables.value(last)->m_game_started) {
                              m_tables.value(last)->slot_reset_game();
                              m_tables.value(last)->slot_deal_cards();
                           }
                     }
               }
         }
   }
void GameTracker::slot_remove_player(int connection_id) {
      if (this->m_players.keys().contains(connection_id)) {
            GamePlayer* player  = m_players.value(connection_id);
            emit this->signal_remove_player(player);
            this->m_players.remove(connection_id);
            qDebug() << "Player from Connection" << player->m_player_uid << "disconnected";
         }
   }
ClientThread* GameTracker::find_client(int connection_id) {
      Lobby* lobby = this->m_main->m_lobby;
      ClientThread* rv;
      if (lobby) {
            if (lobby->m_clients.contains(connection_id)) {
                  rv = lobby->m_clients.value(connection_id);
                  return rv;
               }
         }
      return 0;
   }
ClientThread* GameTracker::find_client(GamePlayer *i_player) {
      if (i_player) {
            ClientThread* rv = this->find_client(i_player->m_connection_id);
            return rv;
         }
      return 0;
   }
void GameTracker::slot_receive_turn(QString turn, int connect_id) {
      if (this->m_players.keys().contains(connect_id)) {
            GamePlayer* player  = m_players.value(connect_id);
            QHashIterator<int, GameTable*> o(m_tables);
            o.toFront();
            while (o.hasNext()) {
                  o.next();
                  GameTable* table = o.value();
                  if (table->m_players.values().contains(player)) {
                        table->slot_receive_turn(turn, connect_id);
                     }
               }
         }
   }

void GameTracker::slot_receive_turn_amt(QString turn, int connect_id, int amount) {
      if (this->m_players.keys().contains(connect_id)) {
            GamePlayer* player  = m_players.value(connect_id);
            QHashIterator<int, GameTable*> o(m_tables);
            o.toFront();
            while (o.hasNext()) {
                  o.next();
                  GameTable* table = o.value();
                  if (table->m_players.values().contains(player)) {
                        table->slot_receive_turn_amt(turn, connect_id, amount);
                     }
               }
         }
   }

void GameTracker::slot_player_bet(GamePlayer *i_player, int amount) {
      if (i_player) {
            int old_count = this->m_main->m_db->get_chips(i_player->m_player_uid).toInt();
            if (amount > old_count) { amount = old_count; }
            int new_amount = old_count - amount;
            this->m_main->m_db->slot_modify_element_text(i_player->m_player_uid, "chips", QString("%1").arg(new_amount) );
         }
   }
void GameTracker::slot_award_chips(GamePlayer *i_player, int amount) {
      if (i_player) {
            int old_count = this->m_main->m_db->get_chips(i_player->m_player_uid).toInt();
            int new_amount = old_count + amount;
            this->m_main->m_db->slot_modify_element_text(i_player->m_player_uid, "chips", QString("%1").arg(new_amount) );
         }
   }
