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

#ifndef CHECKERTHREAD_H
#define CHECKERTHREAD_H


#include <QThread>
#include <QObject>
#include <QHash>
#include <QtDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#define CARDS_PER_CYCLE 15
#define CYCLES_PER_TIER 1
class CheckerThread : public QThread
   {
      Q_OBJECT
   public:

      enum HAND_CATEGORIES {
          hand_HIGHCARD,
          hand_PAIR,
          hand_TWOPAIR,
          hand_THREE,
          hand_STRAIGHT,
          hand_FLUSH,
          hand_FULLHOUSE,
          hand_FOUR,
          hand_STRAIGHTFLUSH,
          hand_ROYALFLUSH
      };

      explicit CheckerThread(QObject *parent = 0);
      ~CheckerThread() { bool_reseed = false; terminate(); exit(0);  }
      int find_category_number(QList<int> i_idx_list);
       int find_score(QList<int> idx_list, QList<int> &relevant_values);
       bool bool_reseed;
       QMutex m_mutex;
       QWaitCondition _waitcondition_reseed;
        int find_face_val(int card_index);
   signals:
       void signal_return_random_number(int entry_number, int result);
       void signal_send_seeding_progress(int current_index, int max_index);
       void signal_send_seed_access_progress(int current_index, int max_index);
   public slots:
       void slot_request_random_number(int entry_number);

   protected:
      void run();
   private:

       bool has_pair(QList<int> &i_faces);
        bool has_two_pair(QList<int> &i_faces);
         bool has_three(QList<int> &i_faces);
         bool has_straight(QList<int> &i_faces, int &high_card);
         bool has_flush(QList<int> &i_suits);
         bool has_fullhouse(QList<int> &i_faces, int &high_three);
         bool has_fullhouse(QList<int> &i_faces);
         bool has_four(QList<int> &i_faces);
         QList<int> find_matches(QList<int> &i_list);
         QList<int> find_relevant_values(QList<int> &i_list, QList<int> &match_list,int num_matches);


         int find_suit_val(int card_index);
          void process_score(QList<QPair<int, int> > face_match_list, int category_number, int &score_v);
          int cat_relevance1[10];
          int cat_relevance2[10];
          QHash<int, QList<int> > m_Straights;
          void populate_straights();
          QList<int> _d_suits;
          QList<int> _d_faces;
          QHash<int, int> _m_Faces;
          QHash<int, int> _m_Suits;
          QList<int> match_cat;
          QList<int> find_flush_idx_list(QList<int> idx_list);
          QHash<qlonglong, int> m_random_numbers;
          QHash<qlonglong, int> m_shuffled_numbers;
          QHash<qlonglong, int>::const_iterator i_rand;
          QMultiHash<int, int> _face_indices;
          QMultiHash<int, int> _available_face_indices;


   };


#endif // CHECKERTHREAD_H
