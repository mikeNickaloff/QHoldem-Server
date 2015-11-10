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
#include <QTime>
CheckerThread::CheckerThread(QObject *parent) :
   QThread(parent)
   {



      _d_suits.clear();
      _d_faces.clear();
      _d_suits << 1 << 2 << 3 << 4;
      _d_faces << 14 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13;
      QListIterator<int> o(_d_suits);
      QListIterator<int> p(_d_faces);
      QListIterator<int> n(_d_faces);
      o.toFront();
      p.toFront();


      // int idx = 1;
      int cur_suit = o.next();
      for (int idx=1; idx<53; ++idx) {

            if (p.hasNext()) {
                  int e = p.next();
                  this->_m_Faces[idx] = e;
                  // this->_face_indices.insert(e, idx);
               } else {
                  p.toFront();
                  int e = p.next();
                  this->_m_Faces[idx] = e;
                  //  this->_face_indices.insert(e, idx);
                  if (o.hasNext())  {
                        cur_suit = o.next();

                     }
               }

            this->_m_Suits[idx] = cur_suit;


            //  idx++;


         }
      cat_relevance1[0] = 1;
      cat_relevance1[1] = 2;
      cat_relevance1[2] = 2;
      cat_relevance1[3] = 3;
      cat_relevance1[4] = 1;
      cat_relevance1[5] = 0;
      cat_relevance1[6] = 3;
      cat_relevance1[7] = 4;
      cat_relevance1[8] = 0;
      cat_relevance1[9] = 0;

      cat_relevance2[0] = 0;
      cat_relevance2[1] = 0;
      cat_relevance2[2] = 2;
      cat_relevance2[3] = 0;
      cat_relevance2[4] = 0;
      cat_relevance2[5] = 0;
      cat_relevance2[6] = 2;
      cat_relevance2[7] = 0;
      cat_relevance2[8] = 0;
      cat_relevance2[9] = 0;
      match_cat <<
                   hand_HIGHCARD <<
                   hand_PAIR <<
                   hand_TWOPAIR <<
                   hand_THREE <<
                   hand_STRAIGHT <<
                   hand_FLUSH <<
                   hand_FULLHOUSE <<
                   hand_FOUR <<
                   hand_STRAIGHTFLUSH <<
                   hand_ROYALFLUSH;
      this->populate_straights();

      this->m_random_numbers[0] = 0;
      for (int i=1; i<(CARDS_PER_CYCLE * CYCLES_PER_TIER); ++i) {
            m_random_numbers[i] = 0;
         }
      this->bool_reseed = true;
      this->i_rand = this->m_random_numbers.constEnd();

      //        this->start(QThread::NormalPriority);


   }
void CheckerThread::run() {

      //QMutexLocker(&this->m_mutex);
      QMutex mutex;

      qlonglong num_per_cycle = CARDS_PER_CYCLE;
      qlonglong num_per_tier = CYCLES_PER_TIER;
      this->_available_face_indices.clear();
      this->_available_face_indices.unite(this->_face_indices);
      while (this->bool_reseed) {
            mutex.lock();

            qlonglong i[4];
            i[0] = 0;
            qlonglong cur_idx = 0;
            bool db_built = false;
            if (this->m_random_numbers.size() >= (num_per_tier * num_per_cycle))
               db_built = true;
            //      this->m_random_numbers.clear();
            //  qDebug() << "Seeding...";

            //  QList<int> new_seed;
            while ((i[0] < num_per_tier) && (bool_reseed)) {
                  i[1] = 0;
                  while ((i[1] < num_per_cycle) && (bool_reseed)) {
                        qreal num_repititions = ((num_per_tier + 1)  * (num_per_cycle + 1));
                        num_repititions /= 52; num_repititions += 0;
                        cur_idx++;
                        int rand = 0;
                      //  msleep(10);
                        _waitcondition_reseed.wait(&mutex, (qBound(5, QTime::currentTime().second(), 59) + qBound(1, QTime::currentTime().minute(), 59)) * 0.175);

                        rand = (qrand()%52)+1;
                        while (m_random_numbers.values().count(rand) > num_repititions) {
 _waitcondition_reseed.wait(&mutex, (qBound(5, QTime::currentTime().second(), 59) + qBound(1, QTime::currentTime().minute(), 59)) * 0.175);
                              rand = (qrand()%52)+1;
                              //if (db_built)
                             // msleep(QTime::currentTime().second() * 10);
                              // if (!db_built)
                              // msleep(25);

                           }
                        emit this->signal_send_seeding_progress(cur_idx, (num_per_tier * num_per_cycle));
                        //  new_seed << rand;
                        this->m_random_numbers[cur_idx] = rand;
                        i[1]++;



                     }

//                  if (db_built)
//                     msleep(25);
//                  if (!db_built)
//                     msleep(25);
                  i[0]++;
               }
            int num_random = this->m_random_numbers.keys().count();
            qreal half_num = ((qreal) num_random * 0.5);
            int half_int = num_random;
            if (half_num != half_int) {
                  half_num -= 1;
               }
            QHash<qlonglong, int>::const_iterator r[2];
            r[0] = this->m_random_numbers.constEnd();
            r[1] = this->m_random_numbers.constEnd();
            for (int i=0; i<half_num; ++i) { r[1]--; }
            int h_id = 0;
            _waitcondition_reseed.wait(&mutex, (qBound(5, QTime::currentTime().second(), 59) + qBound(1, QTime::currentTime().minute(), 59)) * 0.175);
            while (r[1] != m_random_numbers.constBegin()) {
                   _waitcondition_reseed.wait(&mutex, (qBound(5, QTime::currentTime().second(), 59) + qBound(1, QTime::currentTime().minute(), 59)) * 0.175);
                  h_id++;
                  this->m_shuffled_numbers[h_id] = r[1].value();
                    r[1]--;
 _waitcondition_reseed.wait(&mutex, (qBound(5, QTime::currentTime().second(), 59) + qBound(1, QTime::currentTime().minute(), 59)) * 0.175);
                  if (r[0] != m_random_numbers.constBegin()) {
                        h_id++;
                        this->m_shuffled_numbers[h_id] = r[0].value();
                        r[0]--;
                     }

               }
            _waitcondition_reseed.wait(&mutex, (qBound(5, QTime::currentTime().second(), 59) + qBound(1, QTime::currentTime().minute(), 59)) * 0.175);
            r[0] = this->m_shuffled_numbers.constBegin();
            while (r[0] != m_shuffled_numbers.constEnd()) {
                  _waitcondition_reseed.wait(&mutex, (qBound(5, QTime::currentTime().second(), 59) + qBound(1, QTime::currentTime().minute(), 59)) * 0.175);
                  int rk = r[0].key();
                  int rva = r[0].value();

                  this->m_random_numbers[ rk ] = rva;
                  r[0]++;
               }


            // qDebug() << "Seeded " << cur_idx << "numbers...";
            // bool_reseed = false;
            //  i_rand = this->m_random_numbers.constBegin();
            // this->i_rand = this->m_random_numbers.constBegin();
            //i_rand = this->m_random_numbers.constEnd();
            //msleep(250);
            // bool_reseed = false;
            //  db_built = true;
            //cur_idx = 0;
            //i[0] = 0;
            _waitcondition_reseed.wait(&mutex, (qBound(10, QTime::currentTime().minute(), 59) + qBound(10, QTime::currentTime().second(), 59) * 30));
            mutex.unlock();
         }
      qDebug() << "Reseed Unlocked";
      //this->exit(0);
      //this->terminate();

   }

int CheckerThread::find_face_val(int card_index) {
      int rv = 9999;
      if ((card_index < 53) || (_m_Faces.keys().contains(card_index))) {
            // int rv;
            rv = _m_Faces.value(card_index, -1);
         }
      return rv;
   }
int CheckerThread::find_suit_val(int card_index) {
      int rv = 9999;
      if ((card_index < 53) || (_m_Suits.keys().contains(card_index))) {
            // int  rv;
            rv = _m_Suits.value(card_index, 1);
         }
      return rv;
   }
QList<int> CheckerThread::find_matches(QList<int> &i_list) {


      QList<int> matches;
      foreach (int f, i_list) {
            matches << i_list.count(f);


         }
      return matches;
   }
void CheckerThread::process_score(QList<QPair<int, int> > face_match_list, int category_number, int &score_v) {
      //  QMutexLocker(&this->m_mutex);
      int hf = 0;
      int sf = 0;
      int ki = 0;
      int pt_val = 0;
      pt_val += category_number * 1000;
      //qSort(face_match_list.end(), face_match_list.begin());

      QListIterator<QPair<int, int> >o(face_match_list);
      o.toFront();
      while (o.hasNext()) {
            QPair<int, int> pair = o.next();

            if (pair.second == cat_relevance1[category_number]) {

                  if (pair.first >= hf) { hf = pair.first; }

               }
         }
      o.toFront();
      while (o.hasNext()) {
            QPair<int, int> pair = o.next();

            if (pair.second == cat_relevance2[category_number]) {
                  if ((pair.first >= sf) && (pair.first < hf)) { sf = pair.first; }
               }
            if ((pair.second != cat_relevance1[category_number]) && (pair.second != cat_relevance2[category_number])) {
                  if (pair.first >= ki) { ki = pair.first; }
               }
         }
      pt_val += (hf * 60);
      pt_val += (sf * 6);
      if (ki > 0) { ki--; }
      pt_val += (ki * 1);
      //this->m_category_scores[pt_val]++;
      score_v = qBound(category_number * 1000, pt_val, (category_number * 1000) + 999);

   }
int CheckerThread::find_score(QList<int> idx_list, QList<int> &relevant_values) {
      //QMutexLocker(&this->m_mutex);
      int rv = 0;

      QList<int> face_list;
      QList<int> suit_list;
      foreach (int i, idx_list) {
            face_list << this->find_face_val(i);
            suit_list << this->find_suit_val(i);
         }
      QList<QPair<int, int> > face_match_list;
      QList<QPair<int, int> > suit_match_list;
      foreach (int g, face_list) {
            // matches << i_list.count(g);
            face_match_list << qMakePair(g, face_list.count(g));


         }

      foreach (int g, suit_list) {
            // matches << i_list.count(g);
            suit_match_list << qMakePair(g, suit_list.count(g));


         }
      int score_cat = this->find_category_number(idx_list);
      if (score_cat == hand_FLUSH) {
            rv = 5000;
            QList<int> flush_idx;
            flush_idx << this->find_flush_idx_list(idx_list);
            int bface = -1;
            foreach (int q, flush_idx) {
                  int fvq = find_face_val(q);
                  if (fvq > bface) { bface = fvq; }
               }
            rv += bface * 60;
            return rv;
         } else {
            this->process_score(face_match_list, score_cat, rv);
            relevant_values.clear();
            //qDebug() << "Processed Relevant Values for" << face_match_list << "as...";
            relevant_values << this->find_relevant_values(face_list, this->find_matches(face_list), this->cat_relevance1[score_cat]);
            if (this->cat_relevance1[score_cat] != this->cat_relevance2[score_cat]) {
                  if (this->cat_relevance2[score_cat] > 0) {
                        relevant_values << this->find_relevant_values(face_list, this->find_matches(face_list), this->cat_relevance2[score_cat]);
                     }

               }
            return rv;
         }
      return rv;

   }


int CheckerThread::find_category_number(QList<int> i_idx_list) {
      //      QMutexLocker(&this->m_mutex);
      //     QMutexLocker(&this->m_mutex);
      int rv = 0;
      QList<int> faces;
      QList<int> _faces;
      QList<int> suits;
      QList<int> _suits;
      foreach (int i, i_idx_list) {
            _faces << this->find_face_val(i);
            _suits << this->find_suit_val(i);
         }





      int high_card = 0;
      faces << find_matches(_faces);
      //qDebug() << i_idx_list << "has faces" << _faces;
      suits << find_matches(_suits);
      // QList<bool> has_cat;
      //      has_cat << true << has_pair(faces) << has_two_pair(faces) << has_three(faces) << has_straight(_faces, high_card)
      //              << has_flush(suits) << has_fullhouse(faces) << has_four(faces) <<false/* has_straightflush(_faces, _suits)*/ << /*has_royalflush(_faces, _suits)*/false;



      bool hcheck = has_four(faces);
      int hcat = 7;
      if (!hcheck) {
            hcheck = has_fullhouse(faces);
            hcat--;
            if (!hcheck) {
                  /* hcheck = has_flush(suits); */
                  hcheck = false;
                  hcat--;
                  if (!hcheck) {
                        hcheck = has_straight(_faces, high_card);
                        hcat--;
                        if (!hcheck) {
                              hcheck = has_three(faces);
                              hcat--;
                              if (!hcheck) {
                                    hcheck = has_two_pair(faces);
                                    hcat--;
                                    if (!hcheck) {
                                          hcheck = has_pair(faces);
                                          hcat--;
                                          if (!hcheck) {
                                                hcat--;
                                             }
                                       }
                                 }
                           }
                     }
               }
         }
      rv = hcat;


      return rv;


   }


bool CheckerThread::has_pair(QList<int> &i_faces) {

      // bool rv = false;
      //  QList<int> matches;

      //     foreach (int f, i_faces) {
      //         matches << i_faces.count(f);
      ////         if (i_faces.count(f) >= 2) {
      ////             rv = true;
      ////         }
      //     }
      if (i_faces.count(2) == 2) { return true; }
      return false;
   }
bool  CheckerThread::has_two_pair(QList<int> &i_faces) {


      //     QList<int> matches;
      //     foreach (int f, i_faces) {
      //         matches << i_faces.count(f);
      ////         if (i_faces.count(f) >= 2) {
      ////             rv = true;
      ////         }
      //     }
      if (i_faces.count(2) == 4) { return true; }
      return false;
   }
bool CheckerThread::has_three(QList<int> &i_faces) {


      //     QList<int> matches;
      //     foreach (int f, i_faces) {
      //         matches << i_faces.count(f);
      ////         if (i_faces.count(f) >= 2) {
      ////             rv = true;
      ////         }
      //     }
      if (i_faces.count(3) == 3) { return true; }
      return false;
   }

bool CheckerThread::has_straight(QList<int> &i_faces, int &high_card) {


      high_card = -1;

      //     if (this->m_straightCalculator->is_straight(i_faces, high_card, missing)) {
      //         rv = true;
      //     }
      //     if (this->m_straightCalculator->is_straight(i_faces, high_card)) {
      //         return true;
      //     }
      bool rv =  false;
      QHashIterator<int, QList<int> > o(this->m_Straights);
      o.toFront();
      while ((o.hasNext()) && (rv == false)) {
            o.next();
            QList<int> sv; sv << o.value();
            bool valid = true;
            foreach (int i, sv) {
                  if (!i_faces.contains(i)) {
                        valid = false;
                     }
               }
            if (valid == true) {
                  rv = true;
               }

         }
      return rv;
      //     if (rv == false) {
      //         if (i_faces.contains(2)) {
      //             if (i_faces.contains(14)) {
      //                 i_faces.removeOne(14);
      //                 i_faces.append(1);
      //                 if (this->m_straightCalculator->is_straight(i_faces, high_card, missing)) {
      //                     rv = true;
      //                 }
      //             }

      //         }
      //     }


   }

bool CheckerThread::has_flush(QList<int> &i_suits) {


      //   if (i_suits.count(5) == 5) { return true; }
      return false;
   }
bool CheckerThread::has_fullhouse(QList<int> &i_faces) {

      if (has_pair(i_faces)) {
            if (has_three(i_faces)) {
                  return true;
               }


         }
      return false;
   }
bool CheckerThread::has_fullhouse(QList<int> &i_faces, int &high_three) {

      bool rv = false;
      high_three = 0;
      if (has_three(i_faces)) {
            if (has_pair(i_faces)) { rv = true; }
            if (has_two_pair(i_faces)) { rv = true; }

         }
      if (i_faces.count(3) > 3) {
            rv = true;

         }
      if (rv == true) {
            QList<int> rel;
            rel << this->find_relevant_values(i_faces, i_faces, 3);
            qSort(rel.begin(), rel.end());
            high_three = rel.last();
         }
      return rv;
   }
bool CheckerThread::has_four(QList<int> &i_faces) {

      //bool rv = false;
      //     QList<int> matches;
      //     foreach (int f, i_faces) {
      //         matches << i_faces.count(f);
      ////         if (i_faces.count(f) >= 2) {
      ////             rv = true;
      ////         }
      //     }
      if (i_faces.count(4) == 4) { return true; }
      return false;
   }


QList<int>  CheckerThread::find_relevant_values(QList<int> &i_list, QList<int> &match_list, int num_matches) {

      QList<int> rv;
      QListIterator<int> o(i_list);
      QListIterator<int> p(match_list);
      int i1 = 0; int i2 = 0;
      o.toFront();
      p.toFront();
      while (o.hasNext()) {

            i1 = o.next();
            if (p.hasNext()) { i2 = p.next(); }
            if (i2 == num_matches) {
                  if (!rv.contains(i1)) {
                        rv << i1;
                     }
               }

         }
      qSort(rv.begin(), rv.end());
      return rv;
   }
void CheckerThread::populate_straights() {
      QList<int> current_straight;
      int current_low = 1;
      int current_high = 5;
      int q = 0;
      while (current_low < 10) {
            current_low++;
            current_high++;
            current_straight.clear();
            for (int i=current_low; i<current_high; ++i) {
                  current_straight << i;
               }
            current_straight << current_high;
            //qDebug() << "Checking For Straights Matching" << current_straight;
            this->m_Straights[q] = current_straight;
            q++;
         }
   }
QList<int> CheckerThread::find_flush_idx_list(QList<int> idx_list) {

      QList<int> face_list;
      QList<int> suit_list;
      foreach (int i, idx_list) {
            face_list << this->find_face_val(i);
            suit_list << this->find_suit_val(i);
         }
      bool got_flush = false;
      int flush_suit = -1;
      foreach (int i, suit_list) {
            if ((suit_list.count(i) >= 5) && (got_flush == false)) {
                  got_flush = true;
                  flush_suit = i;
               }
         }
      QList<int> rv;
      if (got_flush == true) {
            foreach (int i, idx_list) {
                  if (this->find_suit_val(i) == flush_suit) {
                        rv << i;
                     }
               }

         }
      return rv;
   }
void CheckerThread::slot_request_random_number(int entry_number) {
      int return_number = 0;
      //if (this->bool_reseed) {
     // int rand = 0;
      //rand = (qrand()%52)+1;
      // for (int i=0; i<rand; ++i) {
      while (return_number == 0) {
            if (this->i_rand != this->m_random_numbers.constBegin()) {
                  return_number = i_rand.value();
                  int ikey = i_rand.key();
                  i_rand--;
                  //this->m_random_numbers[ikey] = 0;

               } else {
                  i_rand = m_random_numbers.constEnd();
                  return_number = i_rand.value();
                  int ikey = i_rand.key();
                  i_rand--;
                  //this->m_random_numbers[ikey] = 0;
                  //this->_waitcondition_reseed.wakeAll();
               }
         }
      //  }

      this->bool_reseed = true;
      //  this->start(QThread::IdlePriority);
      // }
      //if (return_number < 1) { return_number = (qrand()%52)+1; }
      emit this->signal_send_seed_access_progress(m_random_numbers.size() - i_rand.key(), m_random_numbers.size());
      // if ((m_random_numbers.size() - i_rand.key()) == i_rand.key()) {

      //    }
      //         } else {

      //         }
      //if (return_number < 1) { return_number = (qrand()%52)+1; }
      emit this->signal_return_random_number(entry_number, return_number);

   }
