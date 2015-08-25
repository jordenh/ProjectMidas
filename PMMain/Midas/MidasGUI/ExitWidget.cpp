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

#include "ExitWidget.h"
#include "qlayout.h"
#include "qicon.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qapplication.h"

ExitWidget::ExitWidget(int widgetWidth, int widgetHeight, QWidget *parent) : 
    DraggableWidget(parent, Qt::WindowStaysOnTopHint), indWidth(widgetWidth), indHeight(widgetHeight)
{
    setWindowTitle(tr("Exit Midas?"));

    setWindowOpacity(1);
    QPalette pal;
    pal.setColor(QPalette::Background, MIDAS_GREY);
    setAutoFillBackground(true);
    setPalette(pal);

    layout = new QVBoxLayout;
    setLayout(layout);

    QFont timesFont("Times", 11, QFont::Bold, true);
    QLabel *title1 = new QLabel("Are you sure you want to Exit?");
    title1->setFont(timesFont);

    layout->addWidget(title1, 0, Qt::AlignCenter);

    QHBoxLayout *hLayout = new QHBoxLayout;
    exitButton = new QPushButton("Exit", this);
    cancelButton = new QPushButton("Cancel", this);
    
    hLayout->addWidget(exitButton);
    hLayout->addWidget(cancelButton);

    connect(exitButton, SIGNAL(clicked()),
        qApp, SLOT(quit()));
    connect(cancelButton, SIGNAL(clicked()),
        this, SLOT(hide()));

    layout->addLayout(hLayout);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMinimumSize(indWidth, indHeight);

    QIcon *icon = new QIcon(TASK_BAR_ICON_PATH);
    setWindowIcon(*icon);
}

ExitWidget::~ExitWidget()
{
}

QSize ExitWidget::sizeHint() const
{
    return QSize(indWidth, indHeight);
}
