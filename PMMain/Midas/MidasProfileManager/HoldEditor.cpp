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

#include "HoldEditor.h"

HoldEditor::HoldEditor(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.doneButton, SIGNAL(released()), this, SLOT(handleDone()));
}

HoldEditor::~HoldEditor()
{

}

Hold HoldEditor::getReturnHold()
{
    return returnHold;
}

bool HoldEditor::isAngleSet(QComboBox* positive, QComboBox* negative)
{
    return !(positive->currentText() == "none" && negative->currentText() == "none");
}

void HoldEditor::handleDone()
{
    returnHold.gesture = ui.gestureComboBox->currentText().toStdString();
    
    if (isAngleSet(ui.rollPositiveComboBox, ui.rollNegativeComboBox))
    {
        AngleAction roll;
        roll.type = "roll";
        roll.anglePositive = ui.rollPositiveComboBox->currentText().toStdString();
        roll.angleNegative = ui.rollNegativeComboBox->currentText().toStdString();

        returnHold.angles.push_back(roll);
    }

    if (isAngleSet(ui.pitchPositiveComboBox, ui.pitchNegativeComboBox))
    {
        AngleAction pitch;
        pitch.type = "pitch";
        pitch.anglePositive = ui.pitchPositiveComboBox->currentText().toStdString();
        pitch.angleNegative = ui.pitchNegativeComboBox->currentText().toStdString();

        returnHold.angles.push_back(pitch);
    }

    if (isAngleSet(ui.yawPositiveComboBox, ui.yawNegativeComboBox))
    {
        AngleAction yaw;
        yaw.type = "yaw";
        yaw.anglePositive = ui.yawPositiveComboBox->currentText().toStdString();
        yaw.angleNegative = ui.yawNegativeComboBox->currentText().toStdString();

        returnHold.angles.push_back(yaw);
    }

    this->accept();
}