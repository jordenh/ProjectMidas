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

#ifndef KEYBOARD_WIDGET_H
#define KEYBOARD_WIDGET_H

#include "DraggableWidget.h"

#include "MyoCommon.h"
#include "RingData.h"

class MidasThread;
class ringData;

#define KEYBOARD_RADIUS 200
#define RING_WIDTH 50

class KeyboardWidget : public DraggableWidget
{
    Q_OBJECT

public:
    KeyboardWidget(MidasThread *mainThread, int radius = KEYBOARD_RADIUS, int ringWidth = RING_WIDTH, QWidget *parent = 0);
    QSize sizeHint() const;

    void addWheel(ringData wheel);
    void addWheels(std::vector<ringData> *kybrdRingData);
    void clearWheels();

public slots:
    void updateKeyboard(int, double, bool, bool);

    void handleDebugInfo(int, int);

protected:
    /**
    * The event that is called when the GUI needs to paint itself.
    *
    * @param event The information about the paint event.
    */
    void paintEvent(QPaintEvent *event);

    /**
    * The event that is called when the widget is resized.
    *
    * @param event The information about the resize event.
    */
    void resizeEvent(QResizeEvent *event);

private:
    int getSelectedKeyFromAngle(double angle);
    void drawRing(QPainter &painter, std::vector<ringData::keyboardValue> *ring, int ringInnerRad, bool isSelected);
    void drawKey(QPainter &painter, int ringInnerRad, qreal currAngle, qreal deltaAngle,
        QRectF& outerRect, QRectF& innerRect, ringData::keyboardValue keyDat, QColor& lineColour, int distBetween);

    QPoint position, cursorPos;
    std::vector<ringData> wheels;
    int keyboardRadius, ringWidth;
    int selectedWheel;
    int selectedKey;
    bool outerSelected, centerSelected;

    QFont holdFont;
    QFont timesFont;

    QString tempDebugText1;
    QString tempDebugText2;
    QString tempDebugText3;
    QString tempDebugText4;
};

#endif