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

#include "MyoStatusWidget.h"
#include "qlayout.h"
#include "qicon.h"
#include "qprogressbar.h"
#include "qlabel.h"

#define BATTERY_ICON_WIDTH 240
#define SIGNAL_ICON_WIDTH 240


MyoStatusWidget::MyoStatusWidget(int widgetWidth, int widgetHeight, QWidget *parent)
{
    indHeight = widgetWidth; indHeight = widgetHeight;

    setToolTip(tr("Drag the Status Widget with the left mouse button."));
    setWindowTitle(tr("Myo Status Indicator"));

    setWindowOpacity(1);
    QPalette pal;
    pal.setColor(QPalette::Background, MIDAS_GREY);
    setAutoFillBackground(true);
    setPalette(pal);

    layout = new QVBoxLayout;
    setLayout(layout);

    this->batteryLevel = 0;
    this->signalStrength = 0;

    QFont timesFont("Times", 11, QFont::Bold, true);
    QLabel *title1 = new QLabel("Battery Level");
    title1->setFont(timesFont);
    layout->addWidget(title1, 0, Qt::AlignCenter); 
    batteryLevelBar = new QProgressBar(this);
    batteryLevelBar->setStyleSheet("color: blue");
    lowBatteryValueStyle = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 ); border-radius: 3px;}"
        "QProgressBar{"
        "border: 2px solid grey;"
        "border-radius: 5px;"
        "text-align: center;"
        "}";
    highBatteryValueStyle = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #78d,stop: 0.4999 #46a,stop: 0.5 #45a,stop: 1 #238 ); border-radius: 3px;}"
        "QProgressBar{"
        "border: 2px solid grey;"
        "border-radius: 5px;"
        "text-align: center;"
        "}";

    lowSignalStyle = "QProgressBar::chunk {background-color: #FF0350;"
        "width: 57px;"
        "margin: 1px;"
        "border-radius: 2px;"
        "}"
        "QProgressBar{"
        "border: 2px solid grey;"
        "border-radius: 5px;"
        "}";
    highSignalStyle = "QProgressBar::chunk {background-color: #05B8CC;"
        "width: 57px;"
        "margin: 1px;"
        "border-radius: 2px;"
        "}"
        "QProgressBar{"
        "border: 2px solid grey;"
        "border-radius: 5px;"
        "}";

    layout->addWidget(batteryLevelBar);

    QLabel *title2 = new QLabel("Signal Strength");
    title2->setFont(timesFont);
    layout->addWidget(title2, 0, Qt::AlignCenter);
    signalStrengthBar = new QProgressBar(this);
    signalStrengthBar->setMinimum(0);
    signalStrengthBar->setMaximum(100);
    signalStrengthBar->setTextVisible(false);
    layout->addWidget(signalStrengthBar);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMinimumSize(indWidth, indHeight);

    QIcon *icon = new QIcon(TASK_BAR_ICON_PATH);
    setWindowIcon(*icon);

    updateGUI();
}

MyoStatusWidget::~MyoStatusWidget()
{
}

QSize MyoStatusWidget::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void MyoStatusWidget::handleBatteryLevelChange(int newBatteryLevel)
{
    this->batteryLevel = newBatteryLevel;

    updateGUI();
}

void MyoStatusWidget::handleSignalStrengthChange(int newSignalStrength)
{
    this->signalStrength = newSignalStrength;

    updateGUI();
}

void MyoStatusWidget::updateGUI()
{
    batteryLevelBar->setMinimumWidth(BATTERY_ICON_WIDTH);
    batteryLevelBar->setValue(batteryLevel);
    if (batteryLevel < LOW_BATTERY_THRESHOLD)
    {
        batteryLevelBar->setStyleSheet(lowBatteryValueStyle);
    }
    else
    {
        batteryLevelBar->setStyleSheet(highBatteryValueStyle);
    }

    signalStrengthBar->setMinimumWidth(SIGNAL_ICON_WIDTH);
    signalStrengthBar->setValue(signalStrength);
    if (signalStrength < LOW_SIGNAL_THRESHOLD)
    {
        signalStrengthBar->setStyleSheet(lowSignalStyle);
    }
    else
    {
        signalStrengthBar->setStyleSheet(highSignalStyle);
    }
}