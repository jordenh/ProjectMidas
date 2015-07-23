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

    /**
     * Basic Constructor/Destructor
     */
    MidasThread(std::vector<ringData> *kybrdRingData);
    ~MidasThread();

    /**
     * Accessor
     *
     * @return a pointer to the vector of ringData data initially loaded
     */
	std::vector<ringData>* getKybrdRingData();

    /**
     * Initialization function
     *
     * @param Handle to the mainGUI
     */
    void setMainGuiHandle(MainGUI *mainGui);

    /**
     * Initialization function
     *
     * @param Handle to the profile Manager
     */
    void setProfileManagerHandle(ProfileManager *profileManager);

    /**
     * Main thread functionality 
     */
    void run();

private:
    MainGUI *mainGui; // not owned
    ProfileManager *profileManager; // not owned

	std::vector<ringData> *kybrdRingData; // not owned
signals:
	/**
     * Emit the Keyboard GUI index (which panel of characters to display that were initially loaded),
     * The current angle of the user, relative to their initial angle,
     * Whether or not they are in the center region
     * And whenter of not the action has been held
     */
    void emitUpdateKeyboard(int, double, bool, bool);  // kybdGUISel, angle, center, held

    /**
     * Emit the current signal strength
     */
    void emitRssi(float);

    /**
     * Emit the current cursor velocity as an X,Y vector
     */
    void emitVeloc(int, int);

    /**
     * Emit misc debug info
     */
    void emitDebugInfo(int, int);

    /**
     * Emit connection info
     */
    void emitDisconnect(bool);
};

#endif // MIDASTHREAD_H
