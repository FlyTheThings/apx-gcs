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
#include "AppRoot.h"
#include "AppWindow.h"
#include <ApxApp.h>
#include <ApxLog.h>
#include <QtCore>
//=============================================================================
AppRoot *AppRoot::_instance = nullptr;
AppRoot::AppRoot(QObject *parent)
    : Fact(nullptr, "apx", tr("Application"), QCoreApplication::applicationName(), Root)
{
    _instance = this;
    if (parent)
        setParent(parent);
    model()->setFlat(true);

    f_settings = new AppSettings(this);

    createTools();
}
//=============================================================================
void AppRoot::sound(const QString &v)
{
    ApxApp::sound(v);
}
//=============================================================================
void AppRoot::createTools()
{
    //plugins management facts
    f_tools = new Fact(this, "tools", tr("Tools"), tr("Application tools"), Group | Const);
    f_tools->setIcon("dialpad");
    f_tools->setVisible(false);

    f_controls = new Fact(f_tools,
                          "controls",
                          tr("Controls"),
                          tr("Instruments and controls"),
                          Group | Const);
    f_controls->setIcon("animation");
    f_controls->setVisible(false);

    f_windows = new Fact(this, "windows", tr("Windows"), tr("Application windows"), Group | Const);
    f_windows->setIcon("monitor");
    f_windows->setVisible(false);

    ApxApp::jsync(this);

    f_pluginsSettings = new Fact(f_settings->f_application,
                                 "plugins",
                                 tr("Plugins"),
                                 tr("Application PligIns"),
                                 Group | Const);
    ApxApp::jsync(f_settings);
}
//=============================================================================
void AppRoot::addToolPlugin(AppPlugin *plugin)
{
    Fact *f = qobject_cast<Fact *>(plugin->control);
    if (!f)
        return;
    if (f->parentFact())
        return;
    f->setParentFact(f_tools);
    f_tools->setVisible(true);
    ApxApp::jsync(f_tools);
}
//=============================================================================
void AppRoot::addWindowPlugin(AppPlugin *plugin)
{
    new AppWindow(f_windows, plugin);
    f_windows->setVisible(true);
    ApxApp::jsync(f_windows);
}
//=============================================================================
void AppRoot::addControlPlugin(AppPlugin *plugin)
{
    Fact *f = qobject_cast<Fact *>(plugin->control);
    if (!f)
        return;
    f->setParentFact(f_controls);
    f_controls->setVisible(true);
    ApxApp::jsync(f_controls);
}
//=============================================================================
//=============================================================================
void AppRoot::factRequestMenu(Fact *fact, QVariantMap opts)
{
    emit menuRequested(fact, opts);
}
//=============================================================================
void AppRoot::factNotify(Fact *fact)
{
    emit notify(fact);
}
//=============================================================================
//=============================================================================
// utils library
//=============================================================================
QString AppRoot::latToString(double v)
{
    double lat = fabs(v);
    double lat_m = 60 * (lat - floor(lat)), lat_s = 60 * (lat_m - floor(lat_m)),
           lat_ss = 100 * (lat_s - floor(lat_s));
    return QString().sprintf("%c %g%c%02g'%02g.%02g\"",
                             (v >= 0) ? 'N' : 'S',
                             floor(lat),
                             176,
                             floor(lat_m),
                             floor(lat_s),
                             floor(lat_ss));
}
QString AppRoot::lonToString(double v)
{
    double lat = fabs(v);
    double lat_m = 60 * (lat - floor(lat)), lat_s = 60 * (lat_m - floor(lat_m)),
           lat_ss = 100 * (lat_s - floor(lat_s));
    return QString().sprintf("%c %g%c%02g'%02g.%02g\"",
                             (v >= 0) ? 'E' : 'W',
                             floor(lat),
                             176,
                             floor(lat_m),
                             floor(lat_s),
                             floor(lat_ss));
}
double AppRoot::latFromString(QString s)
{
    bool ok;
    int i;
    s = s.simplified();
    if (QString("NS").contains(s.at(0))) {
        bool bN = s.at(0) == 'N';
        s = s.remove(0, 1).trimmed();
        i = s.indexOf(QChar(176));
        double deg = s.left(i).toDouble(&ok);
        if (!ok)
            return 0;
        s = s.remove(0, i + 1).trimmed();
        i = s.indexOf('\'');
        double min = s.left(i).toDouble(&ok);
        if (!ok)
            return 0;
        s = s.remove(0, i + 1).trimmed();
        i = s.indexOf('\"');
        double sec = s.left(i).toDouble(&ok);
        if (!ok)
            return 0;
        deg = deg + min / 60.0 + sec / 3600.0;
        return bN ? deg : -deg;
    }
    return s.toDouble();
}
double AppRoot::lonFromString(QString s)
{
    s = s.simplified();
    if (QString("EW").contains(s.at(0)))
        s[0] = (s.at(0) == 'E') ? 'N' : 'S';
    return latFromString(s);
}
QString AppRoot::distanceToString(uint v, bool units)
{
    QString s, su = "km";
    if (v >= 1000000)
        s = QString("%1").arg(v / 1000.0, 0, 'f', 0);
    else if (v >= 1000)
        s = QString("%1").arg(v / 1000.0, 0, 'f', 1);
    else {
        s = QString("%1").arg((uint) v);
        su = "m";
    }
    if (units)
        s.append(su);
    return s;
}
QString AppRoot::timeToString(quint64 v, bool seconds)
{
    //if(v==0)return "--:--";
    qint64 d = (qint64) v / (24 * 60 * 60);
    const char *sf = seconds ? "hh:mm:ss" : "hh:mm";
    if (d <= 0)
        return QString("%1").arg(QTime(0, 0, 0).addSecs(v).toString(sf));
    return QString("%1d%2").arg(d).arg(QTime(0, 0, 0).addSecs(v).toString(sf));
}
QString AppRoot::timemsToString(quint64 v)
{
    qint64 ts = v / 1000;
    QString s;
    if (ts == 0)
        s = timeToString(1, false) + ":00";
    else
        s = timeToString(ts, true);
    s += QString(".%1").arg(v % 1000, 3, 10, QLatin1Char('0'));
    return s;
}
quint64 AppRoot::timeFromString(QString s)
{
    quint64 t = 0;
    s = s.trimmed().toLower();
    if (s.contains('d')) {
        QString ds = s.left(s.indexOf('d')).trimmed();
        s = s.remove(0, s.indexOf('d') + 1).trimmed();
        bool ok = false;
        double dv = ds.toDouble(&ok);
        if (ok && dv > 0)
            t += floor(dv * (double) (24 * 60 * 60));
    }
    if (s.contains('h')) {
        QString ds = s.left(s.indexOf('h')).trimmed();
        s = s.remove(0, s.indexOf('h') + 1).trimmed();
        bool ok = false;
        double dv = ds.toDouble(&ok);
        if (ok && dv > 0)
            t += floor(dv * (double) (60 * 60));
    }
    if (s.contains('m')) {
        QString ds = s.left(s.indexOf('m')).trimmed();
        s = s.remove(0, s.indexOf('m') + 1).trimmed();
        bool ok = false;
        double dv = ds.toDouble(&ok);
        if (ok && dv > 0)
            t += floor(dv * (double) (60));
    }
    if (s.contains('s')) {
        QString ds = s.left(s.indexOf('s')).trimmed();
        s = s.remove(0, s.indexOf('s') + 1).trimmed();
        bool ok = false;
        double dv = ds.toDouble(&ok);
        if (ok && dv > 0)
            t += floor(dv);
        s.clear();
    }
    if (s.contains(':')) {
        QString ds = s.left(s.indexOf(':')).trimmed();
        s = s.remove(0, s.indexOf(':') + 1).trimmed();
        bool ok = false;
        double dv = ds.toDouble(&ok);
        if (ok && dv > 0)
            t += floor(dv * (double) (60 * 60));
        if (s.contains(':')) {
            QString ds = s.left(s.indexOf(':')).trimmed();
            s = s.remove(0, s.indexOf(':') + 1).trimmed();
            bool ok = false;
            double dv = ds.toDouble(&ok);
            if (ok && dv > 0)
                t += floor(dv * (double) (60));
        } else {
            bool ok = false;
            double dv = s.toDouble(&ok);
            if (ok && dv > 0)
                t += floor(dv * (double) (60));
            s.clear();
        }
    }
    if (!s.isEmpty()) {
        bool ok = false;
        double dv = s.toDouble(&ok);
        if (ok && dv > 0)
            t += floor(dv);
    }
    return t;
}
//=============================================================================
QString AppRoot::capacityToString(quint64 v)
{
    QString s, su;
    if (v >= (1024 * 1024 * 1024)) {
        s = QString("%1").arg(v / (double) (1024 * 1024 * 1024), 0, 'f', 2);
        su = "GB";
    } else if (v >= (1024 * 1024)) {
        s = QString("%1").arg(v / (double) (1024 * 1024), 0, 'f', 1);
        su = "MB";
    } else {
        s = QString("%1").arg(v / (double) (1024), 0, 'f', 0);
        su = "kB";
    }
    s.append(su);
    return s;
}
//=============================================================================
double AppRoot::limit(double v, double min, double max)
{
    if (v < min)
        return min;
    if (v > max)
        return max;
    return v;
}
double AppRoot::angle(double v)
{
    const double span = 180.0;
    const double dspan = span * 2.0;
    return v - floor(v / dspan + 0.5) * dspan;
}
double AppRoot::angle360(double v)
{
    while (v < 0)
        v += 360.0;
    while (v >= 360.0)
        v -= 360.0;
    return v;
}
double AppRoot::angle90(double v)
{
    const double span = 90.0;
    const double dspan = span * 2.0;
    return v - floor(v / dspan + 0.5) * dspan;
}
//=============================================================================
QPointF AppRoot::rotate(const QPointF &p, double a)
{
    const double psi_r = qDegreesToRadians(a);
    double cos_theta = cos(psi_r);
    double sin_theta = sin(psi_r);
    return QPointF(p.x() * cos_theta + p.y() * sin_theta, p.y() * cos_theta - p.x() * sin_theta);
}
//=============================================================================
QPointF AppRoot::seriesBounds(const QVariantList &series)
{
    //qDebug()<<v;
    double min, max;
    for (int i = 0; i < series.size(); ++i) {
        double v = series.at(i).toDouble();
        if (i == 0) {
            min = max = v;
        } else {
            if (min > v)
                min = v;
            if (max < v)
                max = v;
        }
    }
    return QPointF(min, max);
}
//=============================================================================