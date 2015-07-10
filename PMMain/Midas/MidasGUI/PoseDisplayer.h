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

#ifndef POSE_DISPLAYER_H
#define POSE_DISPLAYER_H

#define DISCONNECTED_PATH   "Resources\\assets\\gesture_icons\\reConnect.png"
#define UNSYNCHED_PATH      "Resources\\assets\\gesture_icons\\reSync.png"
#define NORMAL_STATE_PATH   "Resources\\assets\\gesture_icons\\blue.png"

#include <qwidget.h>

#include "MyoCommon.h"

class QGridLayout;
class QLabel;

class PoseDisplayer : public QWidget
{
    Q_OBJECT

public:
    PoseDisplayer(int widgetWidth, int widgetHeight, QWidget *parent = 0);
    ~PoseDisplayer();
    QSize sizeHint() const;

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void initPoseImgMap();

    void updateDisplayerAlerts();

    QPoint position;
    int indWidth, indHeight;
    QGridLayout *layout;

    QLabel *poseImgLabel;

    bool connected;
    bool synched;

    QImage *disconnectedImage;
    QImage *unsynchedImage;
    QImage *normalImage ;

public slots:
    void handlePoseImages(std::vector<sequenceImageSet> poseImages);
    void handleDisconnect();
    void handleConnect();
    void handleUnsync();
    void handleSync();

    
};

#endif POSE_DISPLAYER_H