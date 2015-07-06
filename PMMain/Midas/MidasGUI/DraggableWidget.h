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

#ifndef DRAGGABLE_WIDGET_H
#define DRAGGABLE_WIDGET_H

#include <QtWidgets/QWidget.h>

#define GRID_ELEMENT_SIZE   48

/**
 * This class represents a Widget that can be dragged using the mouse, by left-clicking the widget
 * and dragging.
 */
class DraggableWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructs a DraggableWidget with a given parent (or no parent, if NULL), and the
     * specified flags f.
     *
     * @param parent The parent widget.
     * @param f The window flags.
     */
    DraggableWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~DraggableWidget();

protected:
    /**
     * The event handler function that is called when the widget is clicked on by the mouse.
     *
     * @param event The mouse event structure, with information about the mouse press.
     */
    virtual void mousePressEvent(QMouseEvent *event);

    /**
     * The event handler function that is called when the mouse moves over the widget.
     */
    virtual void mouseMoveEvent(QMouseEvent *event);
    
private:
    QPoint position;
};

#endif