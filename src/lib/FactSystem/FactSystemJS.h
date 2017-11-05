﻿/*
 * Copyright (C) 2011 Aliaksei Stratsilatau <sa@uavos.com>
 *
 * This file is part of the UAV Open System Project
 *  http://www.uavos.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 *
 */
#ifndef FactSystemJS_H
#define FactSystemJS_H
//=============================================================================
#include <QtCore>
#include <QtQml>
#include <QJSEngine>
#include "FactSystemApp.h"
//=============================================================================
class FactSystemJS: public FactSystemApp
{
  Q_OBJECT
public:
  explicit FactSystemJS(QObject *parent);

  QJSValue jsexec(const QString &s);

  void jsSyncObject(QObject *obj);
  void jsSync(Fact *item);

  Q_INVOKABLE void alias(FactTree *item,QString aliasName);

  QQmlEngine *engine() {return js;}

  Q_INVOKABLE void help();
  Q_INVOKABLE void sleep(quint16 ms);

  Q_INVOKABLE QByteArray jsToArray(QJSValue data);

protected:
  //js engine
  QQmlEngine *js;
  QHash<QString,QString> js_descr; //helper commands alias,descr

  QJSValue jsSync(Fact *factItem, QJSValue parent); //recursive

  void jsRegister(QString fname,QString description,QString body);

  Fact *f_m;

private:
  void jsRegisterFunctions();

public slots:
  void jsAddItem(FactTree *item);
  void jsRemoveItem(FactTree *item);
};
//=============================================================================
#endif
