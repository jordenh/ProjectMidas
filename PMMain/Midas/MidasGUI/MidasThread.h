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

#ifndef MIDASTHREAD_H
#define MIDASTHREAD_H

#include <QThread>

#include <vector>
class ringData;
class KeyboardWidget;

class MainGUI;
class ProfileManager;

class MidasThread : public QThread
{
    Q_OBJECT

public:
    ~MidasThread();

	MidasThread(std::vector<ringData> *kybrdRingData);
	std::vector<ringData>* getKybrdRingData();

    void setMainGuiHandle(MainGUI *mainGui);
    void setProfileManagerHandle(ProfileManager *profileManager);
    void run();

private:
    MainGUI *mainGui; // not owned
    ProfileManager *profileManager; // not owned

	std::vector<ringData> *kybrdRingData; // not owned
signals:
	void emitUpdateKeyboard(int, double, bool, bool);  // kybdGUISel, angle, center, held
	void emitRssi(float);

signals:
    void emitVeloc(int, int);

    void emitDebugInfo(int, int);

    void emitDisconnect(bool);
};

#endif // MIDASTHREAD_H
