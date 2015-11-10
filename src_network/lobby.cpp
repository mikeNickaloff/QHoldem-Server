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

#include "network.h"
#include "../src_threads/threads.h"
#include <QtDebug>
Lobby::Lobby(QObject *parent) :
   QObject(parent), QTcpServer(parent)
   {
      this->_lastId = 1;
      this->listen(QHostAddress::Any, 2222);
   }
void Lobby::incomingConnection(int socketDescriptor) {

      //qDebug() << "Incoming Connection" << socketDescriptor;
      int new_id = nextId();
      new_clientThread = new ClientThread(0, socketDescriptor, new_id);
      new_thread = new Thread(0);
      new_clientThread->moveToThread(new_thread);
      this->m_threads[new_id] = new_thread;
      QObject::connect(new_thread, SIGNAL(started()), new_clientThread, SLOT(run()));
      QObject::connect(new_clientThread, SIGNAL(finished()), new_thread, SLOT(quit()));
      QObject::connect(new_clientThread, SIGNAL(finished()), new_clientThread, SLOT(deleteLater()));
      QObject::connect(new_thread, SIGNAL(finished()), new_thread, SLOT(deleteLater()));


    //  new_clientThread->start();
      QObject::connect(this, SIGNAL(signal_index_change(int,int)), new_clientThread, SIGNAL(signal_index_change(int,int)));
      QObject::connect(new_clientThread, SIGNAL(signal_new_login(int,int)), this, SLOT(slot_new_login(int,int)));
      QObject::connect(new_clientThread, SIGNAL(signal_disconnected(int)), this, SLOT(slot_client_disconnected(int)));
      QObject::connect(new_clientThread, SIGNAL(signal_receive_check(int)), this, SLOT(slot_receive_check(int)));
      QObject::connect(new_clientThread, SIGNAL(signal_receive_fold(int)), this, SLOT(slot_receive_fold(int)));
      QObject::connect(new_clientThread, SIGNAL(signal_sent_bytes(int)), this, SIGNAL(signal_bytes_sent(int)));
      QObject::connect(new_clientThread, SIGNAL(signal_receive_bet(int,int)), this, SLOT(slot_receive_bet(int,int)));
     // qDebug() << "Connected Client is at thread" << new_clientThread->thread();
      this->m_clients[new_id] = new_clientThread;
      new_thread->start(QThread::IdlePriority);
//      QHashIterator<int, ClientThread*> o(this->m_clients);
//      o.toFront();
//      while (o.hasNext()) {
//            o.next();
//            qDebug() << "-Client at ID" << o.key() << "is" << o.value();
//         }

   }
qlonglong Lobby::nextId() {
      this->_lastId++;
      return this->_lastId;
   }
void Lobby::slot_new_login(int uid, int connection_id) {
      //qDebug() << "New Login" << uid << "from" << connection_id;
      emit this->signal_new_login(uid, connection_id);
   }
void Lobby::slot_client_disconnected(int connection_id) {

      emit this->signal_client_disconnected(connection_id);
       this->m_clients.remove(connection_id);
   }
void Lobby::slot_receive_check(int connect_id) {
      emit this->signal_receive_turn("CHECK", connect_id);
    //  qDebug() << "Sending CHECK from Lobby" << "from connection" << connect_id;
   }

void Lobby::slot_receive_fold(int connect_id) {
      emit this->signal_receive_turn("FOLD", connect_id);
   }
void Lobby::slot_receive_bet(int connect_id, int bet_amt) {
      //qDebug() << "ID" << connect_id << "bet" << bet_amt;
      emit this->signal_receive_turn_amt("BET", connect_id, bet_amt);
   }
