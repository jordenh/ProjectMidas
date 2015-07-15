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

#ifndef DISTANCE_BAR_H
#define DISTANCE_BAR_H

#include <qwidget.h>

#include <MidasCommon.h>
#include <QGraphicsRectItem>
#include <qpainter.h>
#include <QPaintEvent>

class QRect;
class QLabel;

class distanceBar : public QWidget
{
    Q_OBJECT
public:
    distanceBar(qreal x, qreal y, QWidget *parent = 0, qreal width = DISTANCE_DISPLAY_WIDTH,
        qreal height = DISTANCE_DISPLAY_HEIGHT);
 
    enum class bins{
        SIG_OOR = 0,
        SIG_FAR = 1,
        SIG_NEAR = 2,
        SIG_IMMEDIATE = 3
    };

public slots:
    void setValue(int bin);

protected:
    void paintEvent(QPaintEvent *e);
        
private:
    QWidget *parent;

    QRect *long_bar;
    QLabel  *dist_text;
    QColor  colour;
    QString current_bin_text;
    int     current_bin;

    const qreal bar_width = 10;
    const qreal long_bar_height = 15;
    const qreal med_bar_height = 10;
    const qreal short_bar_height = 5;
    const qreal bar_margin = 2;
    const QString  *sOutOfRange = new QString("Out of Range");
    const QString  *sFar = new QString("Low Signal");
    const QString  *sNear = new QString("Medium Signal");
    const QString  *sImmediate = new QString("Strong Signal");
    const QColor OOS_colour = Qt::red;
    const QColor Far_colour = Qt::yellow;
    const QColor Near_colour = QColor(0, 128, 255);
    const QColor Immediate_colour = Qt::green;



};
#endif /* DISTANCE_BAR_H */
