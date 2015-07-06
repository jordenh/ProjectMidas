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

#ifndef PROFILE_CREATOR_GUI_H
#define PROFILE_CREATOR_GUI_H

#include <QMainWindow.h>
#include <QMenuBar.h>
#include <QApplication.h>
#include <QToolButton.h>
#include <QTabWidget.h>
#include <QToolBar.h>
#include <QAction.h>
#include <QMenu.h>
#include "ProfileWriter.h"
#include "ProfileWidget.h"

class ProfileCreatorGUI : public QMainWindow
{
    Q_OBJECT

public:
    ProfileCreatorGUI();
    ~ProfileCreatorGUI();

    QSize sizeHint() const;

private slots:
    void newFile();
    void open();
    void save();
    void newProfile();

private:
    void createMainGUI();
    void createActions();
    void createMenu();
    void createToolBar();
    void loadProfiles(std::string filename);

    QTabWidget* tabWidget;
    QToolBar* toolBar;
    QToolButton* addTabButton;
    QMenu* fileMenu;
    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    std::vector<ProfileWidget*> profileWidgets;
};

#endif /* PROFILE_CREATOR_GUI_H */