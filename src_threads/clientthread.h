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

#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QtNetwork>
#include <QAbstractSocket>
class Database;

class TCPSocket : public  QTcpSocket {
      Q_OBJECT
   public:
      TCPSocket(QObject* parent = 0);
   signals:
      void signal_sent_bytes(int num_bytes);
   public slots:
      bool sendMessage(const QString &message);
      void slot_send_index_change(int entry, int index);

   };
class Thread : public QThread {
      Q_OBJECT
   public:
      Thread(QObject *parent = 0);
   signals:
      void signal_index_change(int entry, int index);
   protected:
      void run();
   };

class ClientThread : public QObject
   {
      Q_OBJECT
   public:
      explicit ClientThread(QObject *parent = 0, int socketDescriptor = 0, int connectionId = 0);
      int m_connectionId;
      int m_socketDescriptor;
      ~ClientThread();
      bool _socketConnected;
      TCPSocket tcpSocket;
   signals:
      void signal_index_change(int entry, int index);
      void signal_new_login(int uid, int connection_id);
      void signal_disconnected(int connection_id);
      void signal_send_socket_msg(const QString &message);
      void signal_receive_check(int connect_id);
      void signal_receive_fold(int connect_id);
      void signal_sent_bytes(int num_bytes);
      void signal_receive_bet(int connect_id, int bet_amt);
      void finished();
   public slots:
      void slot_bytes_written(qint64 bytes);
      void slot_parse_data(QByteArray i_data, TCPSocket &i_socket);
      bool slot_send_data(QString i_data, TCPSocket &i_socket);
      void run();
      void slot_disconnected();
      void slot_parse_error(QAbstractSocket::SocketError i_error);
      void slot_index_change(int entry, int index);
      void slot_specify_full_index_list(QList<QPair<int, int> > i_entry_index_list);
      void slot_announce(QString announce_type, QString value);
      //void slot_process_bet();
   protected:

   };

#endif // CLIENTTHREAD_H
