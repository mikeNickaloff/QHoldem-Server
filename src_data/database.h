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

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QDomDocument>
#include <QHash>
#include <QDomNode>
#include <QtGui>

class Database : public QObject
   {
      Q_OBJECT
   public:
      explicit Database(QObject *parent = 0, bool load_scores = true);
      QHash<int, QDomNode> m_users;
      QString get_text(int uid, QString element);
      QString get_chips(int uid) { return get_text(uid, "chips"); }
      QString get_nickname(int uid) { return get_text(uid, "nickname"); }
      QString get_realname(int uid) { return get_text(uid, "realname"); }
      QString get_login(int uid) { return get_text(uid, "login"); }
      QHash<int, qlonglong> m_winning_scores;
      QHash<int, QDomNode> m_score_nodes;
      QTimer* m_timer_save_scores;

   signals:
      
   public slots:
      void slot_load_xml();
      void slot_save_xml();
      void slot_load_scores_xml();
      void slot_save_scores_xml();
      int find_uid(QString login_name);
      void slot_modify_element_text(int uid, QString element, QString i_new_text);
      void slot_winning_score(int i_score);
      void slot_modify_score_element(int score_id, int new_value);
      qreal find_pct(qlonglong _number, qlonglong _total) {
            int number = (int) _number;
            int total = (int) _total;
                           if (number == 0) { qreal rv = 0.001; return rv; }
                           if (total <= number) { qreal rv = 100; return rv; }
                           if (total == 0) { qreal rv = 100; return rv; }
                           qreal pct = qreal(qRound(qreal((qreal)qBound(0, number, (number + 1)) / (qreal)qBound(0, total, (total + 1))) * 1000) * .1);
                           return pct;

         }
      void slot_display_score_percentages();
   private:
      QDomDocument m_user_data;
      QDomDocument m_score_data;
      
   };

#endif // DATABASE_H
