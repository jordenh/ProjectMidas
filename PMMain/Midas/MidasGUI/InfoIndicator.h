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

#ifndef INFO_INDICATOR_H
#define INFO_INDICATOR_H

#include <QtWidgets/QWidget.h>

#include "MidasCommon.h"

class QLabel;
class QHBoxLayout;
class QPushButton;

class InfoIndicator : public QWidget
{
    Q_OBJECT

public:
    InfoIndicator(int widgetWidth = INFO_INDICATOR_WIDTH, int widgetHeight = INFO_INDICATOR_HEIGHT, QWidget *parent = 0);
    QSize sizeHint() const;
    

protected:
    void resizeEvent(QResizeEvent *event);

    void paintEvent(QPaintEvent *e);
    
private:
    QString getShowAllString();

    QPoint position;
    int indWidth, indHeight;
    QHBoxLayout *layout;
    QLabel *stateLabel;

    QPushButton *button;
    bool showAll;

    signals:
    void emitShowAllToggle(bool);

    private slots:
    void handleButton();

    public slots:
    void handleUpdateState(QString stateLabel);
};

#endif MOUSE_INDICATOR_H