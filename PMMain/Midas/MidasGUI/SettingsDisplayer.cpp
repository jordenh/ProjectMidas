#include "SettingsDisplayer.h"
#include <QEvent.h>
#include <QGridLayout.h>
#include <qslider.h>
#include <QFrame.h>
#include <qlabel.h>

SettingsDisplayer::SettingsDisplayer(int widgetWidth, int widgetHeight, QWidget *parent)
    : QWidget(parent), indWidth(widgetWidth), indHeight(widgetHeight)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    setToolTip(tr("Drag the Settings Displayer with the left mouse button.\n"
        "Use the right mouse button to open a context menu."));
    setWindowTitle(tr("Settings Displayer"));

    setWindowOpacity(0.75);
    QPalette pal;
    pal.setColor(QPalette::Background, QColor(205, 205, 193));
    setAutoFillBackground(true);
    setPalette(pal);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    layout = new QHBoxLayout;
    layout->setSpacing(5);
    setLayout(layout);

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

    // this slot is not working. TODO - fix so that it updates even when slider moves from click on slider bar.
    connect(yawSlider, SIGNAL(sliderMoved(int)), this, SLOT(handleSlidersChange(int)));
    connect(pitchSlider, SIGNAL(sliderMoved(int)), this, SLOT(handleSlidersChange(int)));

    layout->addWidget(new QLabel("Yaw: "));
    layout->addWidget(yawSlider);
    yawValue = new QLabel(QString::number(yawSlider->sliderPosition()));
    layout->addWidget(yawValue);
    layout->addWidget(new QLabel("Pitch: "));
    layout->addWidget(pitchSlider);
    pitchValue = new QLabel(QString::number(pitchSlider->sliderPosition()));
    layout->addWidget(pitchValue);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(indWidth, indHeight);
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