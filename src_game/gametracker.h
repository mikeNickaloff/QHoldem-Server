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

#ifndef GAMETRACKER_H
#define GAMETRACKER_H

#include <QObject>
#include <QtGui>
class GameTable;
class GamePlayer;
class MainWindow;
class Lobby;
class ClientThread;
class Database;

class GameTracker : public QObject
   {
      Q_OBJECT
   public:
      GameTracker(QObject *parent = 0, MainWindow* i_main = 0);
      QHash<int, GameTable*> m_tables;
      GameTable* new_table;
      QHash<int, GamePlayer*> m_players;
      GamePlayer* new_player;
      MainWindow* m_main;


   signals:
      void signal_remove_player(GamePlayer* i_player);
   public slots:
      void slot_create_table(int &ret_table_id);
      void slot_create_player(int player_uid, int connection_id);
      void slot_remove_player(int connection_id);
      ClientThread* find_client(int connection_id);
      ClientThread* find_client(GamePlayer* i_player);
      void slot_receive_turn(QString turn, int connect_id);
      void slot_player_bet(GamePlayer* i_player, int amount);
      void slot_award_chips(GamePlayer* i_player, int amount);
      void slot_receive_turn_amt(QString turn, int connect_id, int amount);
   };

#endif // GAMETRACKER_H
