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

#ifndef PROFILEDISPLAYER_H
#define PROFILEDISPLAYER_H

#include <QFrame.h>
#include "MidasCommon.h"

class ProfilesDisplayer;
class QGridLayout;


class ProfileDisplayer : public QFrame
{
    Q_OBJECT

public:
    ProfileDisplayer(std::string name, int widgetWidth = PROF_INDICATOR_WIDTH, 
        int widgetHeight = PROF_INDICATOR_HEIGHT, QWidget *parent = 0);
    ~ProfileDisplayer();

    void setActiveProfile(bool active) { activeProfile = active; }
    bool getActiveProfile() { return activeProfile; }

    QSize sizeHint() const;

protected:
    /**
    * The event handler function that is called when the widget is clicked on by the mouse.
    *
    * @param event The mouse event structure, with information about the mouse press.
    */
    virtual void mousePressEvent(QMouseEvent *event);

    /**
    * The event handler function that is called when the widget is released by the mouse.
    *
    * @param event The mouse event structure, with information about the mouse release.
    */
    virtual void mouseReleaseEvent(QMouseEvent *event);

    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    int indWidth, indHeight;
    QGridLayout *layout;
    std::string profileName;
    bool activeProfile;
    ProfilesDisplayer *owner;

signals:
    void emitChangeProfile(QString);
};

#endif // PROFILEDISPLAYER_H
