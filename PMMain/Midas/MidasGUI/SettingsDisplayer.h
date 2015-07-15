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

#ifndef SETTINGS_DISPLAYER_H
#define SETTINGS_DISPLAYER_H

#include <QWidget>

#include "MidasCommon.h"
#include "MyoCommon.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QSlider;
class QPushButton;

#define MIN_SLIDER_ANGLE 1
#define MAX_SLIDER_ANGLE 50

class SettingsDisplayer : public QWidget
{
    Q_OBJECT

public:
    SettingsDisplayer(int widgetWidth = INFO_INDICATOR_WIDTH, int widgetHeight = 2*INFO_INDICATOR_HEIGHT, QWidget *parent = 0);
    ~SettingsDisplayer();
    QSize sizeHint() const;

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QPoint position;
    int indWidth, indHeight;
    QVBoxLayout *mainLayout;
    QHBoxLayout *hlayout;
    QLabel *stateLabel;

    // Sliders to update settings for how many degrees the arm has to move to 
    // cover an entire monitor.
    QSlider *yawSlider;
    QSlider *pitchSlider;
    QLabel *yawValue;
    QLabel *pitchValue;

    // Button to change buzzFeedbackMode
    QPushButton *buzzFeedbackButton;
    unsigned int currentBuzzModeCount;

signals:
    void emitSliderValues(unsigned int, unsigned int);
    void emitBuzzFeedbackChange(unsigned int);

private slots:
    void handleSliders();
    void handleSlidersChange(int);
    void handleClicked(bool);
};

#endif SETTINGS_DISPLAYER_H