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
#include <QCheckBox.h>
#include <qline.h>

SettingsDisplayer::SettingsDisplayer(int widgetWidth, int widgetHeight, QWidget *parent)
    : DraggableWidget(parent, Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint),
    indWidth(widgetWidth), indHeight(widgetHeight), currentBuzzModeCount(buzzFeedbackMode::MINIMAL)
{
    setToolTip(tr("Drag the Settings Displayer with the left mouse button."));
    setWindowTitle(tr("Settings Displayer"));

    setWindowOpacity(1);
    QPalette pal;
    pal.setColor(QPalette::Background, MIDAS_GREY);
    setAutoFillBackground(true);
    setPalette(pal);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(WIDGET_BUFFER);
    setLayout(mainLayout);

    QFont timesFont("Times", 11, QFont::Bold, true);
    QLabel* title = new QLabel(QString("Settings"));
    title->setFont(timesFont);
    mainLayout->addWidget(title, 0, Qt::AlignCenter);

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

    

    useGyroForCursorAccelButton = new QCheckBox("Apply acceleration to cursor?", this);
    connect(useGyroForCursorAccelButton, SIGNAL(clicked()), this, SLOT(handleUseGyroForCursorAccelButton()));
    mainLayout->addWidget(useGyroForCursorAccelButton);

    QHBoxLayout* hlayout1 = new QHBoxLayout;
    gyroPowerSpinBox = new QSpinBox(this);
    gyroPowerSpinBox->setMinimum(MIN_GYRO_POW);
    gyroPowerSpinBox->setMaximum(MAX_GYRO_POW);
    gyroPowerSpinBox->setValue(DEFAULT_GYRO_POW);
    gyroPowerSpinBox->setEnabled(false);
    gyroScaleDownSpinBox = new QDoubleSpinBox(this);
    gyroScaleDownSpinBox->setMinimum(MIN_GYRO_SCALE_DOWN);
    gyroScaleDownSpinBox->setMaximum(MAX_GYRO_SCALE_DOWN);
    gyroScaleDownSpinBox->setValue(DEFAULT_GYRO_SCALE_DOWN);
    gyroScaleDownSpinBox->setSingleStep(50);
    gyroScaleDownSpinBox->setEnabled(false);

    connect(gyroPowerSpinBox, SIGNAL(valueChanged(int)), this, SLOT(gyroPowerValueChanged(int)));
    connect(gyroScaleDownSpinBox, SIGNAL(valueChanged(double)), this, SLOT(gyroScaledDownValueChanged(double)));

    hlayout1->addWidget(new QLabel("Accel = Gyro\^exp / scaleDown. "), 1);
    hlayout1->addWidget(new QLabel("Exponent: "));
    hlayout1->addWidget(gyroPowerSpinBox);
    hlayout1->addWidget(new QLabel("Scale Down: "));
    hlayout1->addWidget(gyroScaleDownSpinBox);
    
    mainLayout->addLayout(hlayout1);

    QHBoxLayout* hlayout2 = new QHBoxLayout;

    mainLayout->addWidget(new QLabel("Angular Sensitivity (Degree per Screen):"));
    hlayout2->addWidget(new QLabel("Horizontal: "));
    hlayout2->addWidget(yawSlider);
    yawValue = new QLabel(QString::number(yawSlider->sliderPosition()));
    hlayout2->addWidget(yawValue);
    hlayout2->addWidget(new QLabel("Vertical: "));
    hlayout2->addWidget(pitchSlider);
    pitchValue = new QLabel(QString::number(pitchSlider->sliderPosition()));
    hlayout2->addWidget(pitchValue);

    mainLayout->addLayout(hlayout2);

    QHBoxLayout* hlayout3 = new QHBoxLayout;

    useEmgImpulseButton = new QCheckBox("Stop Motion on EMG Impulse?", this);
    connect(useEmgImpulseButton, SIGNAL(clicked()), this, SLOT(handleUseEmgImpulseButton()));
    mainLayout->addWidget(useEmgImpulseButton);

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

void SettingsDisplayer::handleUseGyroForCursorAccelButton()
{
    emitUseGyroForCursorAccelButton(useGyroForCursorAccelButton->isChecked());
    if (useGyroForCursorAccelButton->isChecked())
    {
        gyroPowerSpinBox->setEnabled(true);
        gyroScaleDownSpinBox->setEnabled(true);

        // assign defaults for pitch/yaw limitations
        yawSlider->setValue(MAX_SLIDER_ANGLE);
        pitchSlider->setValue(MAX_SLIDER_ANGLE);
        handleSlidersChange(0);
    }
    else
    {
        gyroPowerSpinBox->setEnabled(false);
        gyroScaleDownSpinBox->setEnabled(false);
        yawSlider->setValue(INIT_YAW_ANGLE);
        pitchSlider->setValue(INIT_PITCH_ANGLE);
        handleSlidersChange(0);
    }
}

void SettingsDisplayer::gyroPowerValueChanged(int val)
{
    emitGyroPowerValue(val);
}

void SettingsDisplayer::gyroScaledDownValueChanged(double val)
{
    emitGyroScaleDownValue(val);
}

void SettingsDisplayer::handleUseEmgImpulseButton()
{
    emitUseEmgImpulseButton(useEmgImpulseButton->isChecked());
}