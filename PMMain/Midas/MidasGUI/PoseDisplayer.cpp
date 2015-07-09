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

#include "PoseDisplayer.h"
#include "SequenceImageManager.h"
#include <QAction.h>
#include <QApplication.h>
#include <QPainter.h>
#include <qstyle.h>
#include <qdesktopwidget.h>
#include <math.h>
#include <qmessagebox.h>
#include <QGridLayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpixmap.h>

PoseDisplayer::PoseDisplayer(int widgetWidth, int widgetHeight, QWidget *parent)
    : QWidget(parent), indWidth(widgetWidth), indHeight(widgetHeight)
{
    // Temporarily allow a Quit
    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl-Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(quitAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    setToolTip(tr("Drag the Pose Displayer with the left mouse button.\n"
        "Use the right mouse button to open a context menu."));
    setWindowTitle(tr("Pose Displayer"));

    setWindowOpacity(GUI_OPACITY);
    setWindowFlags(Qt::WindowStaysOnTopHint);

    layout = new QGridLayout;
    layout->setMargin(0);
    setLayout(layout);

    poseImgLabel = new QLabel();
    poseImgLabel->setFrameShape(QFrame::Box);
    poseImgLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    poseImgLabel->setBackgroundRole(QPalette::Base);
    poseImgLabel->setAutoFillBackground(true);
    poseImgLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    poseImgLabel->setScaledContents(true);

    layout->addWidget(poseImgLabel, 0, 0, Qt::AlignLeft | Qt::AlignTop);
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFixedSize(indWidth, indHeight);
}

PoseDisplayer::~PoseDisplayer()
{
    delete poseImgLabel; poseImgLabel = NULL;
    delete layout; layout = NULL;
}

void PoseDisplayer::resizeEvent(QResizeEvent *event)
{
}

QSize PoseDisplayer::sizeHint() const
{
    return QSize(indWidth, indHeight);
}

void PoseDisplayer::handlePoseImages(std::vector<sequenceImageSet> poseImages)
{
    if (poseImages.size() == 1)
    {
        QPixmap scaledPic = poseImages[0].nextImage;
        scaledPic = scaledPic.scaled(indWidth, indHeight);        
        poseImgLabel->setPixmap(scaledPic);
    }
}
