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
#ifndef SystreePlugin_H
#define SystreePlugin_H

#include <App/AppRoot.h>
#include <ApxPluginInterface.h>
#include <Fact/Fact.h>
#include <TreeModel/FactTreeView.h>
#include <QtCore>
//=============================================================================
class SystreePlugin : public ApxPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.uavos.gcs.ApxPluginInterface/1.0")
    Q_INTERFACES(ApxPluginInterface)
public:
    QObject *createControl() { return new FactTreeWidget(AppRoot::instance(), true, true); }
    int flags() { return Widget; }
    QString title() { return tr("System tree"); }
    QString descr() { return tr("Application tree view"); }
    QString icon() { return "file-tree"; }
};
//=============================================================================
#endif