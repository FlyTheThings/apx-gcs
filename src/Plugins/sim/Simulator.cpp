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
#include "Simulator.h"
#include <App/AppSettings.h>
#include <ApxLog.h>
#include <QDesktopServices>
//#include <QtConcurrent>

APX_LOGGING_CATEGORY(SimLog, "sim")
//=============================================================================
Simulator::Simulator(Fact *parent)
    : Fact(parent, "sim", tr("Simulator"), tr("Software in the loop simulation"), Group)
{
    setIcon("fan");
    //parent->insertIntoSection(FactSystem::ApplicationSection,this);

    ApxLog::add(SimLog().categoryName(), "sim.txt", true);

    f_launch = new FactAction(this,
                              "launch",
                              tr("Launch"),
                              tr("Start simulation"),
                              "play",
                              FactAction::ActionApply);
    connect(f_launch, &FactAction::triggered, this, &Simulator::launch);
    //connect(parent,&Vehicles::vehicleSelected,this,[=](Vehicle *v){ f_select->setEnabled(v!=this); });

    f_stop = new FactAction(this,
                            "stop",
                            tr("Stop"),
                            tr("Stop simulation"),
                            "stop",
                            FactAction::ActionStop);
    f_stop->setEnabled(false);
    connect(f_stop, &FactAction::triggered, &pShiva, &QProcess::terminate);

    QSettings *settings = AppSettings::settings();

    /*QMetaEnum m=QMetaEnum::fromType<QStandardPaths::StandardLocation>();
  for(int i=0;i<m.keyCount();++i) {
    qDebug()<<m.key(i)<<QStandardPaths::standardLocations((QStandardPaths::StandardLocation)m.value(i));
  }*/
    //QStandardPaths::GenericConfigLocation
    f_type = new AppSettingFact(settings, this, "type", tr("Type"), tr("Simulator type"), Enum, 0);
    f_type->setIcon("package-variant");

    f_oXplane = new Fact(this, "oxplane", tr("X-Plane"), tr("Run X-Plane on start"), Bool);
    f_oXplane->setValue(true);
    f_oAHRS = new Fact(this, "oahrs", tr("AHRS"), tr("Enable AHRS XKF simulation"), Bool);
    f_oNoise = new Fact(this, "onise", tr("Noise"), tr("Sensors noise simulation"), Bool);
    f_oDLHD = new Fact(this, "odlhd", tr("DLHD"), tr("Higher precision downlink"), Bool);
    f_oDLHD->setValue(true);

    //shiva
    pShiva.setProgram(QCoreApplication::applicationDirPath() + "/shiva");
    connect(&pShiva,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            &Simulator::pShivaFinished);

    QTimer::singleShot(2000, this, &Simulator::detectXplane);
}
Simulator::~Simulator()
{
    //QProcess::execute("killall", QStringList() << "shiva");
    pShiva.terminate();
}
//=============================================================================
void Simulator::detectXplane()
{
    QStringList st;
    QFileInfoList files(QDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation),
                             "x-plane_install*.txt")
                            .entryInfoList());
    foreach (QFileInfo fi, files) {
        QString s = fi.baseName();
        int ver = 0;
        QString sv = s.split('_').last();
        if (!sv.isEmpty())
            ver = sv.toUInt();
        if (ver < 9)
            ver = 9;
        //read dir
        QFile file(fi.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            apxConsoleW() << "Can't open file for reading:" << file.fileName();
            continue;
        }
        QDir dir(file.readLine().trimmed());
        if (!dir.exists()) {
            apxConsoleW() << "Error reading X-Plane config:" << file.fileName()
                          << dir.absolutePath();
            continue;
        }
        xplaneDirs.append(dir.absolutePath());
        st.append(QString("XPlane %1").arg(ver));
    }
    //qDebug()<<st<<xplaneDirs;
    f_type->setEnumStrings(st);
    if (st.isEmpty())
        f_type->setEnabled(false);
    else {
        f_type->setEnabled(true);
        f_type->load();
    }
}
//=============================================================================
void Simulator::launch()
{
    apxMsg() << tr("Launching SIL simulation").append("...");

    QString xplaneDir;
    xplaneDir = xplaneDirs.value(f_type->value().toInt());
    while (!xplaneDir.isEmpty()) {
        //install xpl plugins
        QDir d(xplaneDir);
        if (d.cd("Resources") && d.cd("plugins")) {
            QFileInfoList fiSrcList(
                QDir(ApxDirs::res().absoluteFilePath("xplane"), "*.xpl").entryInfoList());
            foreach (QFileInfo fi, fiSrcList) {
                QString dest = d.absoluteFilePath(fi.fileName());
                QFileInfo fiDest(dest);
                //qDebug()<<dest;
                if (fiDest.isSymLink() && fiDest.symLinkTarget() == fi.absoluteFilePath())
                    continue;
                //qDebug()<<fi.absoluteFilePath()<<dest<<QFile::exists(dest);
                if (fiDest.isSymLink() || QFile::exists(dest))
                    QFile::remove(dest);
                //qDebug()<<fi.absoluteFilePath()<<dest<<QFile::exists(dest);
                if (QFile::link(fi.absoluteFilePath(), dest)) {
                    apxMsg() << tr("XPL Plugin installed").append(":") << dest;
                } else {
                    apxMsgW() << tr("XPL Plugin error").append(":") << dest;
                }
            }
        }
        //install xpl aircrafts
        if (d.cd(xplaneDir) && d.cd("Aircraft")) {
            d.mkpath("APX");
            d.cd("APX");
            QFileInfoList fiSrcList(QDir(ApxDirs::res().absoluteFilePath("xplane/models"),
                                         "",
                                         QDir::Unsorted,
                                         QDir::AllDirs | QDir::NoDotAndDotDot)
                                        .entryInfoList());
            foreach (QFileInfo fi, fiSrcList) {
                QFileInfo fiDest(d.absoluteFilePath(fi.fileName()));
                //qDebug()<<fiDest.absoluteFilePath();
                if (fiDest.exists())
                    continue;
                ApxDirs::copyPath(fi.absoluteFilePath(), fiDest.absoluteFilePath());
                apxMsg() << tr("XPL Aircraft installed").append(":") << fiDest.absoluteFilePath();
            }
        }

        break;
    }

    //launch shiva
    if (QProcess::execute("killall", QStringList() << "shiva") == 0) {
        apxMsgW() << tr("SIL simulation session restart");
    }
    pShiva.kill();
    QStringList args;
    if (f_oDLHD->value().toBool())
        args << "-a";
    if (f_oAHRS->value().toBool())
        args << "-i";
    if (f_oNoise->value().toBool())
        args << "-n";
    args << "-s"
         << "-u";
    pShiva.setArguments(args);

    f_launch->setEnabled(false);
    f_stop->setEnabled(true);

    pShiva.start();

    if (f_oXplane->value().toBool() && (!xplaneDir.isEmpty())) {
        //QTimer::singleShot(1000, this, [xplaneDir]() {
        //QtConcurrent::run(&Simulator::launchXplane, xplaneDir);
        //});
        launchXplane(xplaneDir);
    }
}
//=============================================================================
void Simulator::launchXplane(QString xplaneDir)
{
    //mount image
    QFileInfoList fiList(QDir(xplaneDir, "*.img").entryInfoList());
    while (!fiList.isEmpty()) {
        QString imgFile = fiList.first().absoluteFilePath();
#if defined Q_OS_MAC
        if (QProcess::execute("hdiutil", QStringList() << "attach" << imgFile) == 0)
            break;
#elif defined Q_OS_LINUX
        QDir("/media/cdrom").mkpath(".");
        if (QProcess::execute("mount",
                              QStringList() << imgFile << "/media/cdrom"
                                            << "-o loop,ro")
            == 0)
            break;
#endif
        apxMsgW() << tr("Failed to mount X-Plane image") << imgFile;
        break;
    }

    //launch xplane
    QString xplaneApp("X-Plane");
#if defined Q_OS_MAC
    xplaneApp.append(".app");
#elif defined Q_OS_WIN
    xplaneApp.append(".exe");
#endif
    QUrl xplaneUrl(QUrl::fromLocalFile(xplaneDir + "/X-Plane.app"));
    if (!QDesktopServices::openUrl(xplaneUrl)) {
        apxMsgW() << tr("Failed to start X-Plane app") << xplaneUrl;
    }
}
//=============================================================================
//=============================================================================
void Simulator::pShivaFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    apxMsg() << tr("SIL simulation stopped");
    f_launch->setEnabled(true);
    f_stop->setEnabled(false);
}
//=============================================================================