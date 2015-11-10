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

#include <QApplication>
#include "src_headers/toplevel.h"

int main(int argc, char *argv[])
   {
      QApplication a(argc, argv);
       QApplication::setQuitOnLastWindowClosed(false);
      MainWindow w;
      w.resize(800, 600);

      w.setWindowState(Qt::WindowMinimized);
      w.show();
      
      return a.exec();
   }
