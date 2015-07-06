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

#include "distanceBar.h"

#include <qlabel.h>
#include <qrect.h>

distanceBar::distanceBar(qreal x, qreal y, QWidget *parent, qreal width,
    qreal height) : parent(parent)
{
    //long_bar = new QRect(x, y, bar_width, long_bar_height);
    long_bar = new QRect(x, y, width, height);
    colour = OOS_colour;
    current_bin_text = *sOutOfRange;
}

void distanceBar::setValue(int bin)
{
    if (current_bin != bin)
    {
        switch (bin)
        {
        case (int)bins::SIG_FAR:
            colour = Far_colour;
            current_bin_text = *sFar;
            break;
        case (int)bins::SIG_NEAR:
            colour = Near_colour;
            current_bin_text = *sNear;
            break;
        case (int)bins::SIG_IMMEDIATE:
            colour = Immediate_colour;
            current_bin_text = *sImmediate;
            break;
        default:
            colour = OOS_colour;
            current_bin_text = *sOutOfRange;
            break;
        }
        parent->update();
        current_bin = bin;
    }
}

void distanceBar::paintEvent(QPaintEvent *e)
{
    QRect rect = e->rect();
    QPainter painter(this);
    QBrush brush(colour, Qt::SolidPattern);
    QFont font("Times", 9, QFont::Bold);
    
    painter.setBrush(brush);
    painter.setPen(Qt::black);
    painter.setFont(font);
    painter.drawRect(rect);
    painter.drawText(rect, Qt::AlignCenter, current_bin_text);
}