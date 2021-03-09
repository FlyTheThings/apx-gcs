/*
 * APX Autopilot project <http://docs.uavos.com>
 *
 * Copyright (c) 2003-2020, Aliaksei Stratsilatau <sa@uavos.com>
 * All rights reserved
 *
 * This file is part of APX Ground Control.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "ShareXml.h"
#include <Database/MissionsDB.h>

class MissionsXmlExport : public ShareXmlExport
{
    Q_OBJECT
public:
    explicit MissionsXmlExport(QString hash, QString fileName);

protected:
    bool run(QSqlQuery &query);
    bool write(QDomNode &dom);

private:
    DBReqMissionsLoad req;
    QVariantMap details;
    void write(QDomNode &dom,
               const QString &sectionName,
               const QString &elementName,
               const QList<ProtocolMission::Item> &items);
};

class MissionsXmlImport : public ShareXmlImport
{
    Q_OBJECT
public:
    explicit MissionsXmlImport(QString fileName);

protected:
    bool read(const QDomNode &dom);
    bool save(QSqlQuery &query);

private:
    ProtocolMission::Mission mission;
    QVariantMap details;

    int read(const QDomNode &dom,
             const QString &sectionName,
             const QString &elementName,
             QList<ProtocolMission::Item> &items);
};