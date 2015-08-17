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

#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QScrollArea.h>
#include <QLayout.h>
#include "ProfileWriter.h"
#include <QSignalMapper>
#include <QGroupBox.h>
#include <QLabel.h>
#include <QListWidget.h>
#include <QPushButton.h>

struct sequenceCommand {
    QLabel* commandTitle;
    QListWidget* actions;
};

struct sequenceWidgets {
    QGroupBox* grouper;
    QLabel* stateTitle;
    QListWidget* sequences;
    std::vector<sequenceCommand> commands;
};

struct holdWidgets {
    QGroupBox* grouper;
    QVBoxLayout* holdLayout;
    QPushButton* editHoldButton;
};

class ProfileWidget : public QScrollArea
{
    Q_OBJECT

public:
    ProfileWidget(Profile profile, QWidget *parent = 0);
    ~ProfileWidget();

    Profile getProfile();

private:
    void drawProfile(Profile profile);
    void drawHold(Hold hold, int ind, bool insertBefore = false);
    void drawSequence(Sequence sequence, int ind, bool insertBefore = false);
    void modifySequence(int ind, Sequence seq);
    void modifyHold(int ind, Hold hold);
    Profile prof;
    QVBoxLayout* vlayout;
    QSignalMapper* mapper;
    QSignalMapper* holdMapper;
    QSignalMapper* deleteMapper;
    QSignalMapper* holdDeleteMapper;
    std::vector<sequenceWidgets> seqWidgetList;
    std::vector<holdWidgets> holdWidgetList;

public slots:
    void editButtonClicked(int id);
    void holdEditButtonClicked(int id);
    void deleteButtonClicked(int id);
    void holdDeleteButtonClicked(int id);
    void addSequenceButtonClicked();
    void addHoldButtonClicked();

};

#endif // PROFILEWIDGET_H
