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

#ifndef LOBBY_H
#define LOBBY_H

#include <QTcpServer>
#include <QtGui>
class ClientThread;
class Thread;
class Lobby : public QObject, QTcpServer
   {
      Q_OBJECT
   public:
      Lobby(QObject *parent = 0);
      qlonglong nextId();
      ClientThread* new_clientThread;
      QHash<int, ClientThread*> m_clients;
      Thread* new_thread;
      QHash<int, Thread*> m_threads;
   signals:
      void signal_index_change(int entry, int index);
      void signal_new_login(int uid, int connection_id);
      void signal_client_disconnected(int connection_id);
      void signal_receive_turn(QString turn, int connect_id);
       void signal_receive_turn_amt(QString turn, int connect_id, int amt);
      void signal_bytes_sent(int num_bytes);
   protected:
      void incomingConnection(int socketDescriptor);
   public slots:
      void slot_new_login(int uid, int connection_id);
      void slot_client_disconnected(int connection_id);
      void slot_receive_check(int connect_id);
      void slot_receive_fold(int connect_id);
      void slot_receive_bet(int connect_id, int bet_amt);
   private:
      qlonglong _lastId;
      
   };

#endif // LOBBY_H
