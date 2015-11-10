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

#include "threads.h"
#include <QtNetwork>
#include <QtDebug>
#include <QByteArray>
#include <QtGui>
#include <QTextStream>
#include <QAbstractSocket>
#include "../src_data/database.h"
ClientThread::ClientThread(QObject *parent, int socketDescriptor, int connectionId) :
   QObject(parent), m_socketDescriptor(socketDescriptor), m_connectionId(connectionId)
   {
      qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
      // qDebug() << "Parent Thread is " << currentThread();
      connect(&tcpSocket, SIGNAL(signal_sent_bytes(int)), this, SIGNAL(signal_sent_bytes(int)));

   }
void ClientThread::run() {
      // this->moveToThread(thread());

      // tcpSocket.moveToThread(thread());
      //QObject* obj = thread();
      //connect(qobject_cast<ClientThread*>(thread()), SIGNAL(signal_index_change(int,int)), &tcpSocket, SLOT(slot_send_index_change(int,int)));
      // Thread* thr = qobject_cast<Thread*>(obj);

      connect(this, SIGNAL(signal_index_change(int,int)), &tcpSocket, SLOT(slot_send_index_change(int,int)));
      connect(this, SIGNAL(signal_send_socket_msg(QString)), &tcpSocket, SLOT(sendMessage(QString)));
      connect(&tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_parse_error(QAbstractSocket::SocketError)));

      // qDebug() << "Current Thread is " << currentThread() << thread();

      if (!tcpSocket.setSocketDescriptor(m_socketDescriptor)) {
            qDebug() << "Could not establish connection!";
            terminate();
         }
      this->_socketConnected = true;
      //this->connect(this, SIGNAL(terminated()), &tcpSocket, SLOT(disconnectFromHostImplementation());
      this->slot_send_data("!REQ:Login", tcpSocket);

      const int Timeout = 9 * 1111;
      while (_socketConnected) {

            //! [1] //! [2]
            //            while (tcpSocket.bytesAvailable() < (int)sizeof(quint16)) {
            //                  if (!tcpSocket.waitForReadyRead(Timeout)) {
            //                        qDebug() << "error(" << tcpSocket.error() << tcpSocket.errorString();
            //                        this->_socketConnected = false;
            //                        this->slot_disconnected();
            //                        return;
            //                     }
            //                  //! [9] //! [10]
            //               }
            tcpSocket.waitForReadyRead(99);
            QByteArray data = tcpSocket.read(tcpSocket.bytesAvailable());
            if (data.size() > 0) {
                  this->slot_parse_data(data, tcpSocket);
               }
         }
      emit this->finished();
      tcpSocket.disconnectFromHost();
   }
ClientThread::~ClientThread() {

      this->_socketConnected = false;
   }

void ClientThread::slot_bytes_written(qint64 bytes) {
      qDebug() << "Received" << bytes << "from Client";
   }
void ClientThread::slot_parse_data(QByteArray i_data, TCPSocket &i_socket) {
      QList<QByteArray> clist; clist << i_data.split('!');
      foreach (QByteArray s_data, clist) {
            QList<QByteArray> slist; slist << s_data.split(':');
            //   qDebug() << "parsed data to" << slist;
            if (slist.first() == QByteArray().append("LOGIN")) {
                  //            if (slist.last() == QByteArray().append("Login")) {
                  // this->slot_send_data(QString("mike"), i_socket);
                  //  qDebug() << "Attempted Login as" << slist.last();


                  QString str; str = QString::fromUtf8(slist.last());
                  str = str.left(str.size() - 1);
                  // qDebug() << "Checking for UID of" << str << slist.last().toPercentEncoding();
                  int uid = -1;
                  Database* db = new Database(this, false);
                  uid = db->find_uid(str);
                  // qDebug() << "uid of" << str << "is" << uid;
                  if (uid != -1) {
                        this->slot_send_data(QString("!HELLO:%1:%2").arg(db->get_nickname(uid)).arg(db->get_chips(uid)), i_socket);
                        emit this->signal_new_login(uid, this->m_connectionId);
                     } else {
                        slot_send_data("!GOODBYE:GOODBYE", i_socket);
                     }
                  delete db;

                  // }

               }
            if (slist.first() == QByteArray().append("CHECK")) {
                  emit this->signal_receive_check(this->m_connectionId);
               }
            if (slist.first() == QByteArray().append("FOLD")) {
                  emit this->signal_receive_fold(this->m_connectionId);
               }
            if (slist.first() == QByteArray().append("BET")) {
                  int amount = slist.at(1).toInt();
                  //qDebug() << "Parsed a BET Command of"  << slist;
                  emit this->signal_receive_bet(this->m_connectionId, amount);
               }
         }
   }

bool ClientThread::slot_send_data(QString i_data, TCPSocket &i_socket)
   {
      if (i_data.isEmpty())
         return false;

      //   QByteArray data = i_data.toUtf8();
      //    bool ok = i_socket.write(data) == data.size();
      bool ok = i_socket.sendMessage(i_data);
      // qDebug() << "OUT" << i_data << ok;
      return ok;
   }
TCPSocket::TCPSocket(QObject *parent) :
   QTcpSocket()
   {

   }
Thread::Thread(QObject *parent) :
   QThread()
   {

   }
void Thread::run() {

}

bool TCPSocket::sendMessage(const QString &message)
   {
      //    if (message.isEmpty())
      //        return false;
      //    QString text;
      //    QTextStream out(&text, QIODevice::WriteOnly);
      //   // out.setVersion(QDataStream::Qt_4_0);
      //    //out << (quint16)4;
      //    out << message;
      //    out << endl;
      //   // out.device()->seek(0);
      //   // out << (quint16)(block.size() - sizeof(quint16));
      //    qDebug() << "OUT" << text;
      //    return write(text.toLocal8Bit()) == text.size();
      if (message.isEmpty())
         return false;

      QByteArray data = message.toUtf8();
      //this->flush();
      bool ok = this->write(data) == data.size();
        //qDebug() << "OUT" << data << ok;
      emit this->signal_sent_bytes(data.size());
      this->flush();
      this->waitForBytesWritten(10);


      return ok;


      //    QByteArray msg = message.toUtf8();
      //    QByteArray data = "MESSAGE " + QByteArray::number(msg.size()) + ' ' + msg;
      //    return write(data) == data.size();
   }

void TCPSocket::slot_send_index_change(int entry, int index) {
      QString str;
      QTextStream out(&str);
      //  out << endl;
      out << QString("!INDEX CHANGE:%1:%2:").arg(entry).arg(index);

      out << endl;
      // this->waitForBytesWritten(999);
      this->sendMessage(str);
   }
void ClientThread::slot_disconnected() {
      emit this->signal_disconnected(this->m_connectionId);
   }
void ClientThread::slot_parse_error(QAbstractSocket::SocketError i_error) {

      if (i_error == QAbstractSocket::RemoteHostClosedError ) {
            qDebug() << "Received ERROR " << i_error;

            this->_socketConnected = false;
            this->slot_disconnected();

         }
   }
void ClientThread::slot_index_change(int entry, int index) {
      emit this->signal_index_change(entry, index);
   }
void ClientThread::slot_specify_full_index_list(QList<QPair<int, int> > i_entry_index_list) {
      QString msg = QString("!INDEX CHANGE:");
      QListIterator<QPair<int, int> > o(i_entry_index_list);
      o.toFront();
      while (o.hasNext()) {
            QPair<int, int> pair = o.next();
            msg.append(QString("%1,%2").arg(pair.first).arg(pair.second));
            if (o.hasNext()) { msg.append(":"); }
         }
      //qDebug() << "Full Entry List is" << msg;
      emit this->signal_send_socket_msg(msg);
   }
void ClientThread::slot_announce(QString announce_type, QString value) {
      if (announce_type == "TURN") {
            int current_bet = value.toInt();
            QString msg = QString("!TURN:%1").arg(current_bet);
            emit this->signal_send_socket_msg(msg);
            emit this->signal_sent_bytes(msg.size());
         }
      if (announce_type == "WAIT") {
            QString msg = QString("!WAIT:%1").arg(value);
            emit this->signal_send_socket_msg(msg);
            emit this->signal_sent_bytes(msg.size());
         }
      if (announce_type == "WIN") {
            QString msg = QString("!WIN:%1").arg(value);
            emit this->signal_send_socket_msg(msg);
            emit this->signal_sent_bytes(msg.size());
         }
      if (announce_type == "SEAT") {
            QString msg = QString("!SEAT:%1").arg(value);
            emit this->signal_send_socket_msg(msg);
            emit this->signal_sent_bytes(msg.size());
         }
      if (announce_type == "CHIPS") {
            QString msg = QString("!CHIPS:%1").arg(value);
            emit this->signal_send_socket_msg(msg);
            emit this->signal_sent_bytes(msg.size());
         }
      if (announce_type == "CURRENT_TURN") {
            QString msg = QString("!CURRENT_TURN:%1").arg(value);
            emit this->signal_send_socket_msg(msg);
            emit this->signal_sent_bytes(msg.size());
         }

   }
