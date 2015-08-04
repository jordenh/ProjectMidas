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

#include "DraggableWidget.h"

#include "MidasCommon.h"
#include "MyoCommon.h"

class QLabel;
class QComboBox;
class QSlider;
class QPushButton;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

class SettingsDisplayer : public DraggableWidget
{
    Q_OBJECT

public:
    SettingsDisplayer(int widgetWidth = SETTINGS_WIDTH, int widgetHeight = SETTINGS_HEIGHT, QWidget *parent = 0);
    ~SettingsDisplayer();
    QSize sizeHint() const;

private:
    QPoint position;
    int indWidth, indHeight;

    QComboBox *helpLevelComboBox;

    // Sliders to update settings for how many degrees the arm has to move to 
    // cover an entire monitor.
    QSlider *yawSlider;
    QSlider *pitchSlider;
    QLabel *yawValue;
    QLabel *pitchValue;

    QCheckBox *useGyroForCursorAccelButton;
    // SpinBoxs to update Gyro cursor control variables
    QSpinBox *gyroPowerSpinBox;
    QDoubleSpinBox *gyroScaleDownSpinBox;

    // Button to change buzzFeedbackMode
    QPushButton *buzzFeedbackButton;
    unsigned int currentBuzzModeCount;

    QCheckBox *useEmgImpulseButton;

signals:
    void emitSliderValues(unsigned int, unsigned int);
    void emitBuzzFeedbackChange(unsigned int);
    void emitUseGyroForCursorAccelButton(bool);
    void emitGyroPowerValue(int);
    void emitGyroScaleDownValue(double);
    void emitUseEmgImpulseButton(bool);
    void emitHelpLevelChanged(int);

private slots:
    void handleSliders();
    void handleSlidersChange(int);
    void handleClicked(bool);
    void handleUseGyroForCursorAccelButton();
    void gyroPowerValueChanged(int);
    void gyroScaledDownValueChanged(double);
    void handleUseEmgImpulseButton();
    void handleHelpLevelChanged(QString);
};

#endif SETTINGS_DISPLAYER_H