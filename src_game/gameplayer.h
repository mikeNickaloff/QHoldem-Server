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

#ifndef GAMEPLAYER_H
#define GAMEPLAYER_H

#include <QObject>
#include <QHash>
class Database;
class MainWindow;
class GameTable;
class GameTracker;
class GamePlayer : public QObject
   {
      Q_OBJECT

   public:
      GamePlayer(QObject *parent = 0, int i_player_uid = 0, int i_conn_id = 0, GameTracker* i_tracker = 0);
      int m_player_uid;
      int m_connection_id;
      MainWindow* m_main;
      GameTable* table();
      GameTracker* m_tracker;
      int chipCount();
      bool hasTurn;
      int m_score;
      QString nickName();
      QHash<int, int> m_last_entries;
      QPair<int, int> m_table_and_pos;
      QPair<int, int> m_pocket;
      int m_current_bet;
   signals:
      
   public slots:
      void slot_clear_last_entries();
      void slot_set_table(int table_num);
      void slot_set_table_pos(int position);
      void slot_set_table_and_pos(int table_num, int position);
   };

#endif // GAMEPLAYER_H
