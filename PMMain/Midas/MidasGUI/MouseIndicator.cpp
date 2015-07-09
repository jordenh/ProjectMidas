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

#include "MouseIndicator.h"

#include "MidasThread.h"
#include <QTimer.h>
#include <QAction.h>
#include <QApplication.h>
#include <QEvent.h>
#include <QPainter.h>
#include <qdesktopwidget.h>
#include <math.h>
#include <qmessagebox.h>

MouseIndicator::MouseIndicator(MidasThread *mainThread, int deadZoneRad, int widgetWidth, int widgetHeight, QWidget *parent)
    : QWidget(parent), deadZoneRadius(deadZoneRad),
    indWidth(widgetWidth), indHeight(widgetHeight), cursorPos(0, 0)
{
    this->mainThread = mainThread;
    
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);

    // Temporarily allow a Quit
    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl-Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(quitAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    setToolTip(tr("Drag the mouse indicator with the left mouse button.\n"
                  "Use the right mouse button to open a context menu."));
    setWindowTitle(tr("Mouse Indicator"));

    setWindowOpacity(GUI_OPACITY);
    QPalette pal;
    pal.setColor(QPalette::Background, QColor(205, 205, 193));
    setAutoFillBackground(true);
    setPalette(pal);

    setFixedSize(indWidth, indHeight);

    connect(mainThread, SIGNAL(emitVeloc(int, int)), this, SLOT(handleUpdateCursorPos(int, int)));
}

void MouseIndicator::paintEvent(QPaintEvent *event)
{
    int side = qMin(width(), height());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);

    QColor deadZoneColour(139, 139, 131);
    QBrush brush(deadZoneColour);
    QPen pen(brush, 2, Qt::DashDotLine);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    int myWidth = width();
    int myHeight = height();
    int deadZoneWidth = width() * (deadZoneRadius / 100.0);
    int deadZoneHeight = width() * (deadZoneRadius / 100.0);
    int originX = -deadZoneWidth / 2;
    int originY = -deadZoneHeight / 2;
    painter.drawEllipse(originX, originY, deadZoneWidth, deadZoneHeight);

    QColor cursorIndicatorColor(200, 0, 0);
    QBrush cursorIndicatorBrush(cursorIndicatorColor);
    QPen cursorIndicatorPen(cursorIndicatorBrush, 5);
    painter.setPen(cursorIndicatorPen);
    painter.setBrush(Qt::NoBrush);

    painter.drawPoint(cursorPos);
}

void MouseIndicator::resizeEvent(QResizeEvent *event)
{
    int side = qMin(width(), height());
    QRegion maskedRegion(width() / 2 - side / 2, height() / 2 - side / 2, side, side, QRegion::Ellipse);
    setMask(maskedRegion);
}

QSize MouseIndicator::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void MouseIndicator::handleUpdateCursorPos(int percentX, int percentY)
{
    if (percentX > 100) percentX = 100;
    if (percentX < -100) percentX = -100;
    if (percentY > 100) percentY = 100;
    if (percentY < -100) percentY = -100;

    percentY *= -1;

    int rad = qMin(width(), height()) / 2;
    int newX = (percentX / 100.0) * rad;
    int newY = (percentY / 100.0) * rad;
    
    if (newX * newX + newY * newY > rad * rad)
    {
        // Constrain to the bounds of the circle.
        double angleInRads = atan2((double) newY, (double) newX);
        newX = rad * cos(angleInRads);
        newY = rad * sin(angleInRads);
    }

    cursorPos.setX(newX);
    cursorPos.setY(newY);
}