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
#include "MyoDevice.h"
#include <QEvent.h>
#include <qgridlayout.h>
#include <qslider.h>
#include <qcombobox.h>
#include <QFrame.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <QCheckBox.h>
#include <qaction.h>
#include <qicon.h>

SettingsDisplayer::SettingsDisplayer(int widgetWidth, int widgetHeight, QWidget *parent)
    : DraggableWidget(parent, Qt::WindowStaysOnTopHint),
    indWidth(widgetWidth), indHeight(widgetHeight), currentBuzzModeCount(buzzFeedbackMode::MINIMAL)
{
    setToolTip(tr("Drag the Settings Displayer with the left mouse button."));
    setWindowTitle(tr("Settings"));

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
    yawSlider->setValue((int)INIT_ACCEL_YAW_ANGLE);
    yawSlider->setEnabled((DEFAULT_OVERRIDE_ANG_SENSE == false));
    pitchSlider = new QSlider(Qt::Orientation::Horizontal, this);
    pitchSlider->setTracking(true);
    pitchSlider->setMinimum(MIN_SLIDER_ANGLE);
    pitchSlider->setMaximum(MAX_SLIDER_ANGLE);
    pitchSlider->setValue((int)INIT_ACCEL_PITCH_ANGLE);
    pitchSlider->setEnabled((DEFAULT_OVERRIDE_ANG_SENSE == false));

    connect(yawSlider, SIGNAL(sliderReleased()), this, SLOT(handleSliders()));
    connect(pitchSlider, SIGNAL(sliderReleased()), this, SLOT(handleSliders()));

    // TODO - fix so that it updates even when slider moves from click on slider bar.
    connect(yawSlider, SIGNAL(sliderMoved(int)), this, SLOT(handleSlidersChange(int)));
    connect(pitchSlider, SIGNAL(sliderMoved(int)), this, SLOT(handleSlidersChange(int)));

    buzzFeedbackButton = new QPushButton(buzzFeedbackModeToString((buzzFeedbackMode)(currentBuzzModeCount)).c_str(), this);
    connect(buzzFeedbackButton, SIGNAL(clicked(bool)), this, SLOT(handleClicked(bool)));
    
    helpLevelComboBox = new QComboBox(this);
    helpLevelComboBox->addItem(ALL_LVL);
    helpLevelComboBox->addItem(COMPLEX_LVL);
    helpLevelComboBox->addItem(SIMPLE_LVL);
    helpLevelComboBox->addItem(LOCKS_LVL);
    connect(helpLevelComboBox, SIGNAL(activated(QString)), this, SLOT(handleHelpLevelChanged(QString)));

    useGyroForCursorAccelButton = new QCheckBox("Apply acceleration to cursor", this);
    useGyroForCursorAccelButton->setChecked(DEFAULT_USE_ACCEL);
    connect(useGyroForCursorAccelButton, SIGNAL(clicked()), this, SLOT(handleUseGyroForCursorAccelButton()));

    removeGyroOnHoldMouseButton = new QCheckBox("Remove acceleration when holding cursor", this);
    removeGyroOnHoldMouseButton->setChecked(DEFUALT_REMOVE_ACCEL_ON_HOLD);
    connect(removeGyroOnHoldMouseButton, SIGNAL(clicked()), this, SLOT(handleRemoveGyroOnHoldMouseButton()));

    overrideAngularSensitivityButton = new QCheckBox("Override angular sensitivity with dynamic defaults");
    overrideAngularSensitivityButton->setChecked(DEFAULT_OVERRIDE_ANG_SENSE);
    connect(overrideAngularSensitivityButton, SIGNAL(clicked()), this, SLOT(handleOverrideAngularSensitivityButton()));
    
    gyroPowerSpinBox = new QSpinBox(this);
    gyroPowerSpinBox->setMinimum(MIN_GYRO_POW);
    gyroPowerSpinBox->setMaximum(MAX_GYRO_POW);
    gyroPowerSpinBox->setValue(DEFAULT_GYRO_POW);
    if (DEFAULT_USE_ACCEL)
    {
        gyroPowerSpinBox->setEnabled(true);
    }
    else
    {
        gyroPowerSpinBox->setEnabled(false);
    }
    gyroScaleDownSpinBox = new QDoubleSpinBox(this);
    gyroScaleDownSpinBox->setMinimum(MIN_GYRO_SCALE_DOWN);
    gyroScaleDownSpinBox->setMaximum(MAX_GYRO_SCALE_DOWN);
    gyroScaleDownSpinBox->setValue(DEFAULT_GYRO_SCALE_DOWN);
    gyroScaleDownSpinBox->setSingleStep(50);
    if (DEFAULT_USE_ACCEL)
    {
        gyroScaleDownSpinBox->setEnabled(true);
    }
    else
    {
        gyroScaleDownSpinBox->setEnabled(false);
    }

    connect(gyroPowerSpinBox, SIGNAL(valueChanged(int)), this, SLOT(gyroPowerValueChanged(int)));
    connect(gyroScaleDownSpinBox, SIGNAL(valueChanged(double)), this, SLOT(gyroScaledDownValueChanged(double)));

    desiredXRotationSpinBox = new QDoubleSpinBox(this);
    desiredXRotationSpinBox->setSingleStep(0.1);
    desiredXRotationSpinBox->setMaximum(2 * M_PI);
    desiredXRotationSpinBox->setMinimum(0);
    desiredXRotationSpinBox->setDecimals(5);
    desiredXRotationSpinBox->setValue(DEFAULT_DESIRED_X_ROTATION);
    connect(desiredXRotationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(handleDesiredXRotationChanged(double)));

    holdLengthSpinBox = new QSpinBox(this);
    holdLengthSpinBox->setMinimum(400);
    holdLengthSpinBox->setMaximum(DEFAULT_PROG_MAX_DELTA); // dont allow a hold time to be longer than the timeout between actions
    holdLengthSpinBox->setValue(DEFAULT_REQ_HOLD_TIME);
    holdLengthSpinBox->setSingleStep(100);
    connect(holdLengthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handleHoldLengthChanged(int)));

    QHBoxLayout* hlayout0 = new QHBoxLayout;
    hlayout0->addWidget(new QLabel("Myo Vibration Level: "));
    hlayout0->addWidget(buzzFeedbackButton);
    mainLayout->addLayout(hlayout0);
    
    QHBoxLayout* hlayout0b = new QHBoxLayout;
    hlayout0b->addWidget(new QLabel("Help Level (when active): "));
    hlayout0b->addWidget(helpLevelComboBox);
    mainLayout->addLayout(hlayout0b);

    mainLayout->addWidget(useGyroForCursorAccelButton);
    mainLayout->addWidget(removeGyroOnHoldMouseButton);
    mainLayout->addWidget(overrideAngularSensitivityButton);

    QHBoxLayout* hlayout1 = new QHBoxLayout;
    hlayout1->addWidget(new QLabel("Acc=Gyro\^exp/scale. "), 1);
    hlayout1->addWidget(new QLabel("Exponent: "));
    hlayout1->addWidget(gyroPowerSpinBox);
    hlayout1->addWidget(new QLabel("Scale Down: "));
    hlayout1->addWidget(gyroScaleDownSpinBox);
    mainLayout->addLayout(hlayout1);

    QHBoxLayout* hlayout1b = new QHBoxLayout;
    hlayout1b->addWidget(new QLabel("X Rotation Offset (Advanced Setting): "));
    hlayout1b->addWidget(desiredXRotationSpinBox);
    mainLayout->addLayout(hlayout1b);

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

    useEmgImpulseButton = new QCheckBox("Stop Motion on EMG Impulse (WIP)", this);
    connect(useEmgImpulseButton, SIGNAL(clicked()), this, SLOT(handleUseEmgImpulseButton()));
    mainLayout->addWidget(useEmgImpulseButton);

    QHBoxLayout* hlayout3 = new QHBoxLayout;
    hlayout3->addWidget(new QLabel("Hold Length (ms): "));
    hlayout3->addWidget(holdLengthSpinBox);
    mainLayout->addLayout(hlayout3);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMinimumSize(indWidth, indHeight);

    QIcon *icon = new QIcon(TASK_BAR_ICON_PATH);
    setWindowIcon(*icon);
}

SettingsDisplayer::~SettingsDisplayer()
{
}

QSize SettingsDisplayer::sizeHint() const
{
    return QSize(indWidth, indHeight);
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
        removeGyroOnHoldMouseButton->setEnabled(true);
        overrideAngularSensitivityButton->setEnabled(true);

        handleOverrideAngularSensitivityButton(); // ensure the values set in this func are last to stick.
//
//        // assign defaults for pitch/yaw limitations
//        yawSlider->setValue(INIT_ACCEL_YAW_ANGLE);
//        pitchSlider->setValue(INIT_ACCEL_PITCH_ANGLE);
//        handleSlidersChange(0);
    }
    else
    {
        gyroPowerSpinBox->setEnabled(false);
        gyroScaleDownSpinBox->setEnabled(false);
        removeGyroOnHoldMouseButton->setEnabled(false);
        overrideAngularSensitivityButton->setEnabled(false);
//        yawSlider->setValue(INIT_NO_ACCEL_YAW_ANGLE);
//        pitchSlider->setValue(INIT_NO_ACCEL_PITCH_ANGLE);
//        handleSlidersChange(0);
    }
}

void SettingsDisplayer::handleRemoveGyroOnHoldMouseButton()
{
    emitRemoveGyroOnHoldMouseButton(removeGyroOnHoldMouseButton->isChecked());
}

void SettingsDisplayer::handleOverrideAngularSensitivityButton()
{
    emitOverrideAngularSensitivityButton(overrideAngularSensitivityButton->isChecked());

    if (overrideAngularSensitivityButton->isChecked())
    {
        yawSlider->setEnabled(false);
        pitchSlider->setEnabled(false);
    }
    else
    {
        yawSlider->setEnabled(true);
        pitchSlider->setEnabled(true);
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

void SettingsDisplayer::handleHelpLevelChanged(QString val)
{
    if (val.compare(ALL_LVL) == 0)
    {
        emitHelpLevelChanged(int(helpLevel::ALL));
    }
    else if (val.compare(COMPLEX_LVL) == 0)
    {
        emitHelpLevelChanged(int(helpLevel::COMPLEX));
    }
    else if (val.compare(SIMPLE_LVL) == 0)
    {
        emitHelpLevelChanged(int(helpLevel::SIMPLE));
    }
    else if (val.compare(LOCKS_LVL) == 0)
    {
        emitHelpLevelChanged(int(helpLevel::MINIMAL));
    }
}

void SettingsDisplayer::handleDesiredXRotationChanged(double val)
{
    emitDesiredXRotationChanged(val);
}

void SettingsDisplayer::handleHoldLengthChanged(int val)
{
    emitHoldLengthChanged(val);
}