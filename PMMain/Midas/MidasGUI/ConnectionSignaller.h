/*
Copyright (C) 2015 Midas

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
USA
*/

#ifndef CONNECTION_SIGNALLER_H
#define CONNECTION_SIGNALLER_H

#include <QObject>
#include "MidasCommon.h"
#include "MyoCommon.h"

class ConnectionSignaller : public QObject
{
    Q_OBJECT

public:
    ConnectionSignaller(QObject *parent = 0, bool currentlyConnected = false, bool currentlySynched = false);
    ~ConnectionSignaller();

    void setCurrentlyConnected(bool conn) { currentlyConnected = conn; }
    bool getCurrentlyConnected() { return currentlyConnected; }

    void setCurrentlySynched(bool sync) { currentlySynched = sync; }
    bool getCurrentlySynched() { return currentlySynched; }
signals:
    void emitDisconnect();
    void emitConnect();
    void emitSync();
    void emitUnsync();

private:
    bool currentlyConnected;
    bool currentlySynched;
};

#endif // SETTINGS_SIGNALLER_H
