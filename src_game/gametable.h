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

#ifndef GAMETABLE_H
#define GAMETABLE_H

#include <QObject>
#include <QtGui>
#define TABLE_MAX_PLAYERS 5
#define TABLE_ANTE 2500
#define TABLE_MAX_BET 50000
class GamePlayer;
class GameTracker;
class MainWindow;
class Lobby;
class ClientThread;
class CardPixmap;
class CheckerThread;
class Database;
class GameTable : public QObject
   {
      Q_OBJECT
   public:
       GameTable(QObject *parent = 0, GameTracker* i_tracker = 0, int i_table_id = 0);
      GameTracker* m_tracker;
      QHash<int, GamePlayer*> m_players;
      QHash<int, QPair<int, int> > m_tablePos_pocket_entry_numbers;
      int m_tableId;
      QHash<int, int> m_table_cards;
      bool m_game_started;
      QHash<int, bool> m_playing;
      int m_double_blind_chip;
      int m_game_turn;
      int m_river_cards_dealt;
      int big_blind();
      int m_pot;
      int m_current_bet;
   signals:
      void signal_player_bet(GamePlayer* i_player, int amount);

   public slots:
      bool slot_player_try_join_table(GamePlayer* i_player);
      void slot_remove_player(GamePlayer* i_player);
      void slot_reset_game();
      void slot_deal_cards();
      void slot_move_double_blind_chip();
      void slot_move_turn();
      void slot_receive_turn(QString turn, int connect_id);
      void slot_receive_turn_amt(QString turn, int connect_id, int amount);
      void slot_send_game_cards();
      void slot_send_game_turns();
      void slot_process_river();
      int find_score(QList<int> idx_list);
      QList<int> get_idx_list(GamePlayer* i_player);
      void slot_populate_scores();
      QList<GamePlayer*> get_pot_winners();
      void slot_send_game_seats();
      void slot_winning_score(int i_score);
      void slot_player_bet(GamePlayer* i_player, int amt);
   };

#endif // GAMETABLE_H
