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

#include "ProfileIcon.h"

#include <qlabel.h>
#include <qlayout.h>

ProfileIcon::ProfileIcon(int widgetWidth, int widgetHeight, bool active, QPixmap imgActive, QPixmap imgInactive, QWidget *parent)
	: QWidget(parent), indWidth(widgetWidth), indHeight(widgetHeight), imgActive(imgActive), imgInactive(imgInactive)
{
	setContextMenuPolicy(Qt::ActionsContextMenu);
	setToolTip(tr("Drag the profile icon with the left mouse button.\n"
		"Use the right mouse button to open a context menu."));
	setWindowTitle(tr("Profile Icon"));

	setWindowOpacity(GUI_OPACITY);
	QPalette pal;
	pal.setColor(QPalette::Background, MIDAS_GREY);
	setAutoFillBackground(true);
	setPalette(pal);
	setWindowFlags(Qt::WindowStaysOnTopHint);

	layout = new QHBoxLayout;
	setLayout(layout);

	// init inactive
	iconLabel = new QLabel();
	if (active)
	{
        iconLabel->setPixmap(imgActive.scaled(indWidth, indHeight, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation));
	}
	else
	{
        iconLabel->setPixmap(imgInactive.scaled(indWidth, indHeight, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation));
	}

	iconLabel->setFrameShape(QFrame::Box);
	iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	iconLabel->setBackgroundRole(QPalette::Base);
	iconLabel->setAutoFillBackground(true);
	iconLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	iconLabel->setScaledContents(true);

	layout->addWidget(iconLabel);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setFixedSize(indWidth, indHeight);
}

void ProfileIcon::resizeEvent(QResizeEvent *event)
{
	QRegion maskedRegion(0, 0, width(), height(), QRegion::Rectangle);
	setMask(maskedRegion);
}

QSize ProfileIcon::sizeHint() const
{
	return QSize(indWidth, indHeight);
}

void ProfileIcon::setImgActiveSel(bool active)
{
	if (active)
	{
        iconLabel->setPixmap(imgActive.scaled(indWidth, indHeight, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation));
	}
	else
	{
        iconLabel->setPixmap(imgInactive.scaled(indWidth, indHeight, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation));
	}
}
