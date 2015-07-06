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

#include "DistanceWidget.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include "qaction.h"
#include "qapplication.h"
#include "qboxlayout.h"
#include "distanceBar.h"
#include "MidasThread.h"
#include "qlabel.h"


DistanceWidget::DistanceWidget(MidasThread* mainThread, int width, int height, QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint), distance(0), dispWidth(width), dispHeight(height)
{
    bar = new distanceBar(0, 0, this, width, height);
    QVBoxLayout *layout = new QVBoxLayout;

    // Temporarily allow a Quit
    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl-Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(quitAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    setToolTip(tr("Strong signal means the device is 0.5m or closer.\n"
        "Medium signal means the device is 4m or closer.\n"
        "Low signal means the device is beyond 4m.\n"
        "Out of Range means the device is not found."));
    setWindowTitle(tr("Info Indicator"));

    layout->addWidget(bar);

    setWindowOpacity(0.75);
    QPalette pal;
    setPalette(pal);

    setFixedSize(dispWidth, dispHeight);

    setLayout(layout);

    connect(mainThread, SIGNAL(emitRssi(float)), this, SLOT(updateDistanceLabel(float)));
    connect(mainThread, SIGNAL(emitDisconnect(bool)), this, SLOT(signalLoss(bool)));
}

void DistanceWidget::updateDistanceLabel(float db)
{
    distance = dbToDist(db);

    if (distance < 0.5)
    {
        bar->setValue((int)distanceBar::bins::SIG_IMMEDIATE);
    }
    else if (distance < 4)
    {
        bar->setValue((int)distanceBar::bins::SIG_NEAR);
    }
    else
    {
        bar->setValue((int)distanceBar::bins::SIG_FAR);
    }
}

void DistanceWidget::signalLoss(bool isConnected)
{
    if (!isConnected)
    {
        bar->setValue((int)distanceBar::bins::SIG_OOR);
    }
}

inline float DistanceWidget::dbToDist(float db)
{
    return 3.8429*std::pow(10, -6)*std::pow(db, 4) +
        7.55606*std::pow(10, -4)*std::pow(db, 3) +
        5.34042218*std::pow(10, -2)*std::pow(db, 2) +
        1.5151387757*db +
        13.8505553761;
}
