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
#include <QtCore>
//============================================================================
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Ground Control Software by UAVOS (C) Aliaksei Stratsilatau "
                                     "<sa@uavos.com>. Plugin test utility.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("plugins", "Plugin[s] to load and test.");
    parser.process(*qApp);

    const QStringList args = parser.positionalArguments();

    for (auto fname : args) {
        QLibrary lib(fname);
        if (!lib.load()) {
            const char *err = lib.errorString().toUtf8();
            qFatal("%s", err);
        }
    }

    return 0;
}
//============================================================================
