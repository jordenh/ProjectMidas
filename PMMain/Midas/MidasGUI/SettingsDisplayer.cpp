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

#include "SettingsDisplayer.h"
#include <QEvent.h>
#include <qgridlayout.h>
#include <qslider.h>
#include <QFrame.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>

SettingsDisplayer::SettingsDisplayer(int widgetWidth, int widgetHeight, QWidget *parent)
    : QWidget(parent), indWidth(widgetWidth), indHeight(widgetHeight), currentBuzzModeCount(buzzFeedbackMode::MINIMAL)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    setToolTip(tr("Drag the Settings Displayer with the left mouse button.\n"
        "Use the right mouse button to open a context menu."));
    setWindowTitle(tr("Settings Displayer"));

    setWindowOpacity(GUI_OPACITY);
    QPalette pal;
    pal.setColor(QPalette::Background, QColor(205, 205, 193));
    setAutoFillBackground(true);
    setPalette(pal);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(WIDGET_BUFFER);
    setLayout(mainLayout);

    yawSlider = new QSlider(Qt::Orientation::Horizontal, this);
    yawSlider->setTracking(true);
    yawSlider->setMinimum(MIN_SLIDER_ANGLE);
    yawSlider->setMaximum(MAX_SLIDER_ANGLE);
    yawSlider->setValue((int)INIT_YAW_ANGLE);
    pitchSlider = new QSlider(Qt::Orientation::Horizontal, this);
    pitchSlider->setTracking(true);
    pitchSlider->setMinimum(MIN_SLIDER_ANGLE);
    pitchSlider->setMaximum(MAX_SLIDER_ANGLE);
    pitchSlider->setValue((int)INIT_PITCH_ANGLE);

    connect(yawSlider, SIGNAL(sliderReleased()), this, SLOT(handleSliders()));
    connect(pitchSlider, SIGNAL(sliderReleased()), this, SLOT(handleSliders()));

    // TODO - fix so that it updates even when slider moves from click on slider bar.
    connect(yawSlider, SIGNAL(sliderMoved(int)), this, SLOT(handleSlidersChange(int)));
    connect(pitchSlider, SIGNAL(sliderMoved(int)), this, SLOT(handleSlidersChange(int)));

    buzzFeedbackButton = new QPushButton(buzzFeedbackModeToString((buzzFeedbackMode)(currentBuzzModeCount)).c_str(), this);
    connect(buzzFeedbackButton, SIGNAL(clicked(bool)), this, SLOT(handleClicked(bool)));
    mainLayout->addWidget(buzzFeedbackButton);

    gyroPowerSpinBox = new QSpinBox(this);
    gyroPowerSpinBox->setMinimum(MIN_GYRO_POW);
    gyroPowerSpinBox->setMaximum(MAX_GYRO_POW);
    gyroPowerSpinBox->setValue(DEFAULT_GYRO_POW);
    gyroScaleDownSpinBox = new QDoubleSpinBox(this);
    gyroScaleDownSpinBox->setMinimum(MIN_GYRO_SCALE_DOWN);
    gyroScaleDownSpinBox->setMaximum(MAX_GYRO_SCALE_DOWN);
    gyroScaleDownSpinBox->setValue(DEFAULT_GYRO_SCALE_DOWN);
    gyroScaleDownSpinBox->setSingleStep(50);

    connect(gyroPowerSpinBox, SIGNAL(valueChanged(int)), this, SLOT(gyroPowerValueChanged(int)));
    connect(gyroScaleDownSpinBox, SIGNAL(valueChanged(double)), this, SLOT(gyroScaledDownValueChanged(double)));

    QHBoxLayout* hlayout1 = new QHBoxLayout;
    hlayout1->addWidget(new QLabel("Gyro Exponent: "));
    hlayout1->addWidget(gyroPowerSpinBox);
    hlayout1->addWidget(new QLabel("Gyro Scale Down: "));
    hlayout1->addWidget(gyroScaleDownSpinBox);
    
    mainLayout->addLayout(hlayout1);

    QHBoxLayout* hlayout2 = new QHBoxLayout;

    hlayout2->addWidget(new QLabel("Yaw: "));
    hlayout2->addWidget(yawSlider);
    yawValue = new QLabel(QString::number(yawSlider->sliderPosition()));
    hlayout2->addWidget(yawValue);
    hlayout2->addWidget(new QLabel("Pitch: "));
    hlayout2->addWidget(pitchSlider);
    pitchValue = new QLabel(QString::number(pitchSlider->sliderPosition()));
    hlayout2->addWidget(pitchValue);

    mainLayout->addLayout(hlayout2);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(indWidth, indHeight);
}

SettingsDisplayer::~SettingsDisplayer()
{
}

QSize SettingsDisplayer::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void SettingsDisplayer::resizeEvent(QResizeEvent *event)
{
    QRegion maskedRegion(0, 0, width(), height(), QRegion::Rectangle);
    setMask(maskedRegion);
}

void SettingsDisplayer::handleSliders()
{
    emitSliderValues((unsigned int)yawSlider->sliderPosition(), (unsigned int)pitchSlider->sliderPosition());
    int yawInt = yawSlider->sliderPosition();
    yawValue->setText(QString::number(yawInt));
    int pitchInt = pitchSlider->sliderPosition();
    pitchValue->setText(QString::number(pitchInt));
}

void SettingsDisplayer::handleSlidersChange(int newPos)
{
    emitSliderValues((unsigned int)yawSlider->sliderPosition(), (unsigned int)pitchSlider->sliderPosition());
    int yawInt = yawSlider->sliderPosition();
    yawValue->setText(QString::number(yawInt));
    int pitchInt = pitchSlider->sliderPosition();
    pitchValue->setText(QString::number(pitchInt));
}

void SettingsDisplayer::handleClicked(bool checked)
{
    // disregard checked - not caring.
    currentBuzzModeCount++;
    currentBuzzModeCount %= NUM_BUZZ_MODES;
    emitBuzzFeedbackChange(currentBuzzModeCount);
    buzzFeedbackButton->setText(buzzFeedbackModeToString((buzzFeedbackMode)(currentBuzzModeCount)).c_str());
}

void SettingsDisplayer::gyroPowerValueChanged(int val)
{
    emitGyroPowerValue(val);
}

void SettingsDisplayer::gyroScaledDownValueChanged(double val)
{
    emitGyroScaleDownValue(val);
}
