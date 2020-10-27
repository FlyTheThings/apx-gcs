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

#include "NodeField.h"
#include "NodeTools.h"

#include <Protocols/ProtocolNode.h>
#include <Protocols/ProtocolViewBase.h>

#include <App/AppNotify.h>

#include <QtCore>

class Nodes;

class NodeItem : public ProtocolViewBase<ProtocolNode>
{
    Q_OBJECT

public:
    explicit NodeItem(Fact *parent, Nodes *nodes, ProtocolNode *protocol);

    NodeTools *tools;

    //int loadConfigValues(QVariantMap values);
    bool loadConfigValue(const QString &name, const QString &value);

    const QList<NodeField *> &fields() const;

    Q_INVOKABLE void message(QString msg,
                             AppNotify::NotifyFlags flags = AppNotify::FromVehicle
                                                            | AppNotify::Important);

    inline Nodes *nodes() const { return _nodes; }

protected:
    QTimer statusTimer;

    QVariant data(int col, int role) const override;

private:
    Nodes *_nodes{nullptr};

    QList<NodeField *> m_fields;

    QJsonArray _parameters;
    void updateMetadataAPXFW(Fact *root, Fact *group, QJsonValue json);

    NodeField *m_status_field{nullptr};
    void groupArrays();
    void groupArrays(Fact *group);
    void updateArrayRowDescr(Fact *fRow);
    void removeEmptyGroups(Fact *f);
    void linkGroupValues(Fact *f);

private slots:

    void validateDict();
    void validateData();

    void updateDescr();
    void updateStatus();

public slots:
    void upload();
    void clear();

    //protocols:
private slots:
    void identReceived();
    void dictReceived(const ProtocolNode::Dict &dict);
    void confReceived(const QVariantMap &values);
    void confSaved();

    void messageReceived(xbus::node::msg::type_e type, QString msg);
    void statusReceived(const xbus::node::status::status_s &status);

signals:
    void saveValues();
    void shell(QStringList commands);
};
