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

#ifndef PROFILE_ICON_H
#define PROFILE_ICON_H

#include "DraggableWidget.h"

#include "MidasCommon.h"
#include <qpixmap.h>

class QLabel;
class QHBoxLayout;

class ProfileIcon : public QWidget
{
	Q_OBJECT

public:
	ProfileIcon(int widgetWidth = SPECIFIC_PROFILE_ICON_SIZE, int widgetHeight = SPECIFIC_PROFILE_ICON_SIZE, bool active = false,
		QPixmap imgActive = QPixmap(), QPixmap imgInactive = QPixmap(), QWidget *parent = 0);
	QSize sizeHint() const;

	void setImgActiveSel(bool active);
		
protected:
	void resizeEvent(QResizeEvent *event);

private:
	QPoint position;
	int indWidth, indHeight;
	QHBoxLayout *layout;
	QLabel *iconLabel;
	QPixmap imgActive;
	QPixmap imgInactive;

	bool imgActiveSel;
};

#endif PROFILE_ICON_H