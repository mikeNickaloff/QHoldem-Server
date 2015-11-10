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

#include "database.h"
#include <QtGui>
#include <QtXml>
#include <QtDebug>
Database::Database(QObject *parent, bool load_scores) :
   QObject(parent)
   {
      this->slot_load_xml();
      this->slot_modify_element_text(1, "nickname", "Big Mike" );
      this->slot_save_xml();
      if (load_scores) {
         //   this->slot_load_scores_xml();
           //  this->slot_save_scores_xml();
         }
      this->m_timer_save_scores = new QTimer(this);
      m_timer_save_scores->connect(m_timer_save_scores, SIGNAL(timeout()), this, SLOT(slot_save_scores_xml()));
//      this->m_timer_save_scores->setInterval(180000);
//      m_timer_save_scores->start();

   }
void Database::slot_load_xml() {
      QFile file(QApplication::applicationDirPath().append("/db/tmp_users.xml"));
      if (!file.open(QIODevice::ReadOnly))
         return;
      if (!this->m_user_data.setContent(&file)) {
            file.close();
            return;
         }
      file.close();

      QDomNodeList users = this->m_user_data.elementsByTagName("user");
      for (int i = 0; i < users.count(); i++) {
            QDomNode user = users.item(i);
            int id = user.toElement().attribute("id").toInt();
            this->m_users[id] = user;

         }
   }
void Database::slot_load_scores_xml() {

      QFile file(QApplication::applicationDirPath().append("/db/tmp_scores.xml"));
      if (!file.open(QIODevice::ReadOnly))
         return;
      if (!this->m_score_data.setContent(&file)) {
            file.close();
            return;
         }
      file.close();

      QDomNodeList users = this->m_score_data.elementsByTagName("score");
      for (int i = 0; i < users.count(); i++) {
            QDomNode user = users.item(i);
            int id = user.toElement().attribute("id").toInt();

            if (id > 2000) {
                  this->m_score_nodes[id] = user;
                  // qDebug() << "Loaded Score Node" << id;
                  this->m_winning_scores[id] = user.toElement().text().toLongLong();
                  if (this->m_winning_scores.value(id) > 0) {
                        //    qDebug() << "loaded Score" << this->m_winning_scores.value(id);
                     }
               }
         }
      for (int i=0; i<10000; ++i) {
            if (!this->m_score_nodes.keys().contains(i)) {
                  this->m_winning_scores[i] = 0;
               }
         }
      this->slot_display_score_percentages();
   }
void Database::slot_winning_score(int i_score) {
      qlonglong tmp = this->m_winning_scores.value(i_score);
      tmp += 1;
      this->m_winning_scores[i_score] = tmp;
      //  qDebug() << "Score" << i_score << "has" << tmp << "wins";
      this->slot_modify_score_element(i_score, (int) tmp);
      //  this->slot_save_scores_xml();
   }
void Database::slot_save_scores_xml() {

      QHashIterator<int, qlonglong> e(this->m_winning_scores);
      while (e.hasNext()) {
            e.next();
            //if (!this->m_score_nodes.keys().contains(e.key())) {
            QDomElement element = m_score_data.createElement("score");
            element.setAttribute("id", e.key());
            //m_score_data.appendChild(element);
            //  QDomElement v_element = m_score_data.createElement("value");
            QDomText text = m_score_data.createTextNode(QString("%1").arg(e.value()));
            element.appendChild(text);
            if (this->m_score_nodes.keys().contains(e.key()))  {
                  m_score_data.firstChild().replaceChild(element, this->m_score_nodes.value(e.key()));
               } else {
                  m_score_data.firstChild().appendChild(element);
               }
            //    }
         }



      QFile file(QApplication::applicationDirPath().append("/db/tmp_scores.xml"));
      if (!file.open(QIODevice::WriteOnly))
         return;
      QTextStream out(&file);
      out << this->m_score_data.toString(5);
      file.close();
      this->slot_load_scores_xml();

      //   m_score_data.clear();
      //      QHashIterator<int, qlonglong> o(this->m_winning_scores);
      //      while (o.hasNext()) {
      //            o.next();
      //            QDomElement element = m_score_data.createElement("score");
      //            element.setAttribute("id", o.key());
      //            QDomText text = m_score_data.createTextNode(QString("%1").arg(o.value()));
      //            element.appendChild(text);
      //            m_score_data.appendChild(element);
      //         }
      //      QFile file(QApplication::applicationDirPath().append("/db/scores.xml"));
      //      if (!file.open(QIODevice::WriteOnly))
      //         return;
      //      QTextStream out(&file);
      //      out << this->m_score_data.toString(5);
      //      file.close();
   }
QString Database::get_text(int uid, QString element) {
      QString rv;
      if (m_users.keys().contains(uid)) {
            rv.append(m_users.value(uid).firstChildElement(element).text());
         }
      return rv;
   }
int Database::find_uid(QString login_name) {
      QHashIterator<int, QDomNode> o(m_users);
      o.toFront();
      int rv = -1;
      while (o.hasNext()) {
            o.next();

            int tuid = o.key();

            QString gl = get_login(tuid);
            QString ln = login_name;
            gl.toAscii();
            ln.toAscii();

            // qDebug() << "Comparing UID" << tuid << "Login" << gl << "with" << ln;
            if (gl == ln) {
                  //qDebug() << "Found matching UID" << tuid;
                  rv = tuid;
               }
         }
      return rv;
   }
void Database::slot_modify_element_text(int uid, QString element, QString i_new_text) {
      if (this->m_users.keys().contains(uid)) {
            QDomNode old_node(m_users.value(uid));
            QDomElement old_element = old_node.firstChildElement(element);
            QDomText old_text = m_user_data.createTextNode(i_new_text);
            old_element.replaceChild(old_text, old_element.firstChild());
            // qDebug() << "Attempting to replace" << this->get_text(uid, element) << "with" << i_new_text;
            this->slot_save_xml();
         }
   }
void Database::slot_modify_score_element(int score_id, int new_value) {
      if (this->m_score_nodes.keys().contains(score_id)) {
            QDomNode old_node(m_score_nodes.value(score_id));
            QDomElement old_element = old_node.firstChildElement("value");

            QDomText old_text = m_score_data.createTextNode(QString("%1").arg(new_value));
            old_element.replaceChild(old_text, old_element.firstChild());
            //  qDebug() << "Modified Score" << score_id << "to have value" << new_value;
            // qDebug() << "Attempting to replace" << this->get_text(uid, element) << "with" << i_new_text;
            //this->slot_save_scores_xml();
         }
   }

void Database::slot_save_xml() {
      QFile file(QApplication::applicationDirPath().append("/db/tmp_users.xml"));
      if (!file.open(QIODevice::WriteOnly))
         return;
      QTextStream out(&file);
      out << this->m_user_data.toString(5);
      file.close();
   }
void Database::slot_display_score_percentages() {
      qlonglong total = 0;
      QHashIterator<int, qlonglong> e(this->m_winning_scores);
      qlonglong cat_count[10];
      for (int i=0; i<10; ++i) { cat_count[i] = 0; }
      while (e.hasNext()) {
            e.next();
            qlonglong val = e.value();
            if (e.key() > 2000) {
                  total += val;
                  int cur_cat = 0;
                  qlonglong t_val = 0;
                  while (val > t_val) {
                        t_val += 1000;
                        cur_cat++;
                     }
                  cat_count[cur_cat] += val;
               }
         }
      QString tstr;
      for (int i=2; i<10; ++i) {
            tstr.append(QString(" [%1 : %2%3]").arg(i).arg(this->find_pct(cat_count[i], total)).arg(QChar('%')));
         }
      qDebug() << "Current Category Percentages";
      qDebug() << tstr;
   }
