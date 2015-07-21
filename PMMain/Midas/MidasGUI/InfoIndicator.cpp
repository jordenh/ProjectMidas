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

#include "InfoIndicator.h"

#include <QGridLayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <QTimer.h>
#include <QEvent.h>
#include <QPainter.h>
#include <qstyle.h>
#include <qframe.h>
#include <qdesktopwidget.h>
#include <math.h>
#include <qmessagebox.h>

InfoIndicator::InfoIndicator(int widgetWidth, int widgetHeight, QWidget *parent)
    : QWidget(parent), indWidth(widgetWidth), indHeight(widgetHeight), showAll(false)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    setToolTip(tr("Drag the info indicator with the left mouse button.\n"
        "Use the right mouse button to open the Settings and Profile Widgets."));
    setWindowTitle(tr("Info Indicator"));

    setWindowOpacity(GUI_OPACITY);
    QPalette pal;
    pal.setColor(QPalette::Background, MIDAS_GREY);
    setAutoFillBackground(true);
    setPalette(pal);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    layout = new QHBoxLayout;
    layout->setSpacing(WIDGET_BUFFER);
    setLayout(layout);
    
    // attempt 2
    //setStyleSheet("border: 1px solid black");
    //
    // attempt 1
    //QFrame *frame = new QFrame;
    //frame->setFrameShape(QFrame::Shape::Box);
    //layout->addWidget(frame);

    QFont timesFont("Times", 9, QFont::Bold);
    stateLabel = new QLabel();
    stateLabel->setFont(timesFont);
    layout->addWidget(stateLabel, 1, Qt::AlignLeft);

    button = new QPushButton("?", this);
    button->setText(getShowAllString());
	button->setFixedWidth(20); // Remove this line if getShowAllString has more than just a "?"
    QFont timesSmall("Times", 8, QFont::DemiBold);
    button->setFont(timesSmall);
    connect(button, SIGNAL(released()), this, SLOT(handleButton()));
    layout->addWidget(button, 0, Qt::AlignRight);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(indWidth, indHeight);
}

void InfoIndicator::paintEvent(QPaintEvent *e)
{
    // Give the GUI a single pixel black border.
    QPainter painter(this);
    painter.drawRoundedRect(QRect(0, 0, this->width() - 1, this->height() - 1), 0, 0);
    QWidget::paintEvent(e);
}

void InfoIndicator::resizeEvent(QResizeEvent *event)
{
    QRegion maskedRegion(0, 0 , width(), height(), QRegion::Rectangle);
    setMask(maskedRegion);
}

void InfoIndicator::handleButton()
{
    showAll = !showAll;
    button->setText(getShowAllString());
    emitShowAllToggle(showAll);
}

QSize InfoIndicator::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void InfoIndicator::handleUpdateState(QString stateString)
{
    stateLabel->setText(stateString);
}

QString InfoIndicator::getShowAllString()
{
    if (showAll)
    {
        return tr("?");
    }
    else
    {
        return tr("?");
    }

}