#ifndef SETTINGS_DISPLAYER_H
#define SETTINGS_DISPLAYER_H

#include <QWidget>

#include "MidasCommon.h"

class QLabel;
class QHBoxLayout;
class QSlider;

#define MIN_SLIDER_ANGLE 0
#define MAX_SLIDER_ANGLE 50

class SettingsDisplayer : public QWidget
{
    Q_OBJECT

public:
    SettingsDisplayer(int widgetWidth = INFO_INDICATOR_WIDTH, int widgetHeight = INFO_INDICATOR_HEIGHT, QWidget *parent = 0);
    QSize sizeHint() const;

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QPoint position;
    int indWidth, indHeight;
    QHBoxLayout *layout;
    QLabel *stateLabel;

    QSlider *yawSlider;
    QSlider *pitchSlider;
    QLabel *yawValue;
    QLabel *pitchValue;

signals:
    void emitSliderValues(unsigned int, unsigned int);

private slots:
    void handleSliders();
    void handleSlidersChange(int);
};

#endif SETTINGS_DISPLAYER_H