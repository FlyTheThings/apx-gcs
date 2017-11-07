/*
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
#ifndef FactTreeModel_H
#define FactTreeModel_H
//=============================================================================
#include <QtCore>
#include <FactSystem.h>
//=============================================================================
class FactTreeModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  explicit FactTreeModel(Fact *root);

  Fact * fact(const QModelIndex &index) const;
  QModelIndex factIndex(FactTree *item, int column=0) const;

  enum { //model columns
    FACT_MODEL_COLUMN_NAME=0,
    FACT_MODEL_COLUMN_VALUE,
    FACT_MODEL_COLUMN_DESCR,

    FACT_MODEL_COLUMN_CNT,
  };

protected:
  Fact * root;

  virtual void checkConnections(Fact *fact) const;

  //override
  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex & index) const;
  QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

private slots:
  void factTextChanged();
  void factTitleChanged();
  void factDescrChanged();

};
//=============================================================================
#endif
