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

//#ifndef TOPLEVEL_H
//#define TOPLEVEL_H

//#endif // TOPLEVEL_H
#include <QPixmap>
#include <QString>
static QPixmap makePixmap(int card_id) {

      QString pmstr = QString("%1").arg(qBound(1, card_id, 52));
      while ((pmstr.length() < 2) && (card_id != 0)) { pmstr.prepend(QString("%1").arg('0')); }
      QPixmap pm(QString(":/cards/%1.png").arg(pmstr));
      return pm;
   }

#include "../mainwindow.h"
