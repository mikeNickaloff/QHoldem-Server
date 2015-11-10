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

#include "src_headers/toplevel.h"
#include "src_gfx/gfx.h"
#include "src_threads/threads.h"
#include "src_network/network.h"
#include "src_data/database.h"
#include "src_game/game.h"
#include <QApplication>
#include <QtGui>
#include <QtCore/qstate.h>
#include <QSignalTransition>
#include <QAbstractTransition>
#include <QObject>

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
   {

      m_scene = new GraphicsScene(-350, -350, 700, 700, 0, 0, this);
      m_view = new View(m_scene);
      m_scene->_view = m_view;
      m_view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
      m_view->setCacheMode(QGraphicsView::CacheBackground);
      m_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
      m_view->setParent(this);

      m_view->show();
      this->m_label_bytes_sent = new QLabel(this);
      this->m_label_bytes_sent->move(m_view->geometry().bottomLeft());
      //m_label_bytes_sent->setGeometry(QRect(m_view->geometry().bottomLeft().x(), m_view->geometry().bottomLeft().y(), m_view->geometry().width(), this->geometry().height() - m_view->geometry().height()));
      m_label_bytes_sent->resize(QSize(500, 75));
      QFont font;
      font.setPointSize(16);

      m_label_bytes_sent->setFont(font);
      m_label_bytes_sent->setText("Bytes Sent: 0");
      m_label_bytes_sent->show();
      this->m_lobby = new Lobby(this);
      this->m_db = new Database(this, true);
      m_checker = new CheckerThread(this);
      this->m_checker->start(QThread::InheritPriority);
      int g_num = 1;
      int g_count = 0;
      for (int i=1; i<16; ++i) {
            int d = i;
            while (d > 52) { d -= 52; }

            QPixmap pm = makePixmap(d);
            new_cardpixmap = new CardPixmap(pm, m_scene);
            new_cardpixmap->slot_set_card_index(d);
            g_count++;
            if (g_count > 15) {
                  g_num++;
                  g_count = 1;
               }
            QList<CardPixmap*> clist;
            clist << this->m_game_cards.value(g_num, QList<CardPixmap*>());
            clist << new_cardpixmap;
            this->m_game_cards[g_num] = clist;

            new_cardpixmap->setOffset(-pm.width() / 2, -pm.height() / 2);
            new_cardpixmap->setScale(CARD_SCALE);
            // new_cardpixmap->setPos(0 + ((0 + (i + (t * CARDS_PER_REEL))) * 5), -950);
            new_cardpixmap->setPos(-350 + (((i - 1) % 15) * ((pm.width() * CARD_SCALE) / 2)), -250 + ((i - 1) / 15) * (pm.height() / 2));
            new_cardpixmap->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
            // item->setZValue((6 * 27) - ((t * 27) + i));
            new_cardpixmap->setZValue(i);
            m_scene->addItem(new_cardpixmap);
            m_cards[i] = new_cardpixmap;

            new_cardpixmap->setEntryNumber(i);
            m_checker->connect(new_cardpixmap, SIGNAL(signal_request_random_index(int)), m_checker, SLOT(slot_request_random_number(int)));
            new_cardpixmap->connect(m_checker, SIGNAL(signal_return_random_number(int,int)), new_cardpixmap, SLOT(slot_receive_random_index(int,int)));
            this->connect(new_cardpixmap, SIGNAL(signal_report_index_change(int,int)), this, SLOT(slot_receive_index_change(int,int)));
            new_cardpixmap->connect(this, SIGNAL(signal_send_invalid_index(int)), new_cardpixmap, SLOT(slot_received_invalid_index_change(int)));
            new_cardpixmap->show();
            // new_cardpixmap->slot_randomize();
          //  connect(this, SIGNAL(signal_valid_index_change(int,int)), m_lobby, SIGNAL(signal_index_change(int,int)));

         }

      this->createActions();
      this->createTrayIcon();

      connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
              this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

      this->hide();
      this->m_timer_randomize = new QTimer(this);
      m_timer_randomize->connect(m_timer_randomize, SIGNAL(timeout()), this, SLOT(slot_randomize_all()));
      m_timer_randomize->start((99 * (4 * 15)) + (99 + 59));
      this->m_tracker = new GameTracker(this, this);
      this->connect(m_lobby, SIGNAL(signal_new_login(int,int)), m_tracker, SLOT(slot_create_player(int,int)));
      this->connect(m_lobby, SIGNAL(signal_client_disconnected(int)), m_tracker, SLOT(slot_remove_player(int)));
      this->connect(m_lobby, SIGNAL(signal_receive_turn(QString,int)), m_tracker, SLOT(slot_receive_turn(QString,int)));
      this->connect(m_lobby, SIGNAL(signal_receive_turn_amt(QString,int,int)), m_tracker, SLOT(slot_receive_turn_amt(QString,int,int)));
      this->connect(m_lobby, SIGNAL(signal_bytes_sent(int)), this, SLOT(slot_sent_bytes(int)));
      this->m_bytes_sent = 0;
   }

MainWindow::~MainWindow()
   {
      
   }
void MainWindow::slot_receive_index_change(int entry_number, int index_number) {
      if (this->m_cards.keys().contains(entry_number)) {
            CardPixmap* card = this->m_cards.value(entry_number);

            QHashIterator<int, QList<CardPixmap*> > p(this->m_game_cards);
            p.toFront();
            while (p.hasNext()) {
                  p.next();
                  QList<CardPixmap*> elist;
                  elist << p.value();
                  if (elist.contains(card)) {
                        QListIterator<CardPixmap*> o(elist);
                        o.toFront();
                        bool found_invalid = false;
                        while ((o.hasNext()) && (found_invalid == false)) {
                              CardPixmap* c = o.next();
                              if (c != card) {
                                    if (c->card_index() == index_number) {
                                          emit this->signal_send_invalid_index(entry_number);
                                          found_invalid = true;
                                       }
                                 }
                           }
                        if (found_invalid == false) {
                              emit this->signal_valid_index_change(entry_number, index_number);
                           }
                        p.toBack();
                     }


               }

         }
   }

void MainWindow::slot_randomize_all() {

      QHashIterator<int, CardPixmap*> o(this->m_cards);
      o.toFront();
      while (o.hasNext()) {
            o.next();
            int i = o.key();
            CardPixmap* c = o.value();
            QTimer::singleShot(((qBound(20, QTime::currentTime().second(), 59) * (qBound(2, m_cards.values().count() - i, m_cards.values().count() + 2))) + (qBound(20, QTime::currentTime().minute(), 59) + qBound(5, QTime::currentTime().hour(), 23))), c, SLOT(slot_randomize()));
          //  c->slot_randomize();
      }

   }
void MainWindow::slot_sent_bytes(int num_bytes) {
      this->m_bytes_sent += (qlonglong) num_bytes;

      this->m_label_bytes_sent->setText(QString("Sent: %1").arg(humanize_bytes(m_bytes_sent)));
   }
QString MainWindow::humanize_bytes(qlonglong bytes) {
      qreal new_val = (qreal) bytes;
      QStringList ext_list;
      ext_list << "bytes" << "KB" << "MB" << "GB";
      QStringListIterator o(ext_list);
      o.toFront();
      QString ext = o.next();
      while ((new_val > 1024) && (o.hasNext())) {
            new_val *= 0.0009765625;
            ext = o.next();
         }
      QString rv = QString("%1 %2").arg(new_val).arg(ext);
      return rv;
   }
void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    QIcon icon;
    icon.addPixmap(this->m_cards.value(1)->pixmap());
    trayIcon->setIcon(icon);
    trayIcon->show();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
//        QMessageBox::information(this, tr("Systray"),
//                                 tr("The program will keep running in the "
//                                    "system tray. To terminate the program, "
//                                    "choose <b>Quit</b> in the context menu "
//                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
//        iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1)
//                                      % iconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        //showMessage();
        break;
    default:
        ;
    }
}
