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

#ifndef HOLDEDITOR_H
#define HOLDEDITOR_H

#include <QDialog>
#include "ProfileWriter.h"
#include "ui_HoldEditor.h"

class HoldEditor : public QDialog
{
    Q_OBJECT

public:
    HoldEditor(QWidget *parent = 0);
    ~HoldEditor();

    Hold getReturnHold();
    void populateGUIWithHold(Hold hold);

private:
    void formMouseActions(QComboBox * comboBox);
    void formKybdActions(QComboBox * comboBox);
    void updateGUIWithHold(Hold hold);

    Ui::HoldEditor ui;

    Hold returnHold;
    bool isAngleSet(QComboBox* positive, QComboBox* negative);

public slots:
    void handleDone();
    void handleActivatePosRollCommandBox(const QString & text);
    void handleActivateNegRollCommandBox(const QString & text);
    void handleActivatePosPitchCommandBox(const QString & text);
    void handleActivateNegPitchCommandBox(const QString & text);
    void handleActivatePosYawCommandBox(const QString & text);
    void handleActivateNegYawCommandBox(const QString & text);
};

#endif // HOLDEDITOR_H
