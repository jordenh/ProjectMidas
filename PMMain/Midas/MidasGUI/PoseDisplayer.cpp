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
    : QWidget(parent), indWidth(widgetWidth), indHeight(widgetHeight), connected(false), synched(false), isRightHand(true)
{
    setToolTip(tr("Drag the Pose Displayer with the left mouse button.\n"
        "Use the right mouse button to open the Settings and Profile Widgets."));
    setWindowTitle(tr("Pose Displayer"));

    setWindowOpacity(GUI_OPACITY);
    setWindowFlags(Qt::WindowStaysOnTopHint  | Qt::X11BypassWindowManagerHint);

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

    disconnectedImage = new QImage(QString(DISCONNECTED_PATH));
    unsynchedImage = new QImage(QString(UNSYNCHED_PATH));
    normalImage = new QImage(QString(NORMAL_STATE_PATH));
    updateDisplayerAlerts();
}

PoseDisplayer::~PoseDisplayer()
{
    delete poseImgLabel; poseImgLabel = NULL;
    delete layout; layout = NULL;
    delete disconnectedImage; disconnectedImage = NULL;
    delete unsynchedImage; unsynchedImage = NULL;
    delete normalImage; normalImage = NULL;
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
        QPixmap scaledPic;
        if (isRightHand)
        {
            scaledPic = poseImages[0].nextImageRight;
        }
        else
        {
            scaledPic = poseImages[0].nextImageLeft;
        }
        scaledPic = scaledPic.scaled(indWidth, indHeight, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);
        poseImgLabel->setPixmap(scaledPic);
    }
}

void PoseDisplayer::handleDisconnect()
{   
    connected = false;
    updateDisplayerAlerts();
}

void PoseDisplayer::handleConnect()
{
    connected = true;
    updateDisplayerAlerts();
}

void PoseDisplayer::handleUnsync()
{
    synched = false;
    updateDisplayerAlerts();
}

void PoseDisplayer::handleSync()
{
    synched = true;
    updateDisplayerAlerts();
}

void PoseDisplayer::handleIsRightHand(bool isRightHand)
{
    this->isRightHand = isRightHand;
}

void PoseDisplayer::updateDisplayerAlerts()
{
    // Disconnected takes priority over synched.
    if (!connected)
    {
        QPixmap scaledPic = QPixmap::fromImage(*disconnectedImage);
        scaledPic = scaledPic.scaled(indWidth, indHeight, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);
        poseImgLabel->setPixmap(scaledPic);
    }
    else if (!synched)
    {
        QPixmap scaledPic = QPixmap::fromImage(*unsynchedImage);
        scaledPic = scaledPic.scaled(indWidth, indHeight, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);
        poseImgLabel->setPixmap(scaledPic);
    }
    else
    {
        QPixmap scaledPic = QPixmap::fromImage(*normalImage);
        scaledPic = scaledPic.scaled(indWidth, indHeight, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);
        poseImgLabel->setPixmap(scaledPic);
    }
}