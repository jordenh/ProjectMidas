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

    connect(ui.RollPositiveTypeComboBox, SIGNAL(activated(const QString &)),  this, SLOT(handleActivatePosRollCommandBox(const QString &)));
    connect(ui.RollNegativeTypeComboBox, SIGNAL(activated(const QString &)),  this, SLOT(handleActivateNegRollCommandBox(const QString &)));
    connect(ui.PitchPositiveTypeComboBox, SIGNAL(activated(const QString &)), this, SLOT(handleActivatePosPitchCommandBox(const QString &)));
    connect(ui.PitchNegativeTypeComboBox, SIGNAL(activated(const QString &)), this, SLOT(handleActivateNegPitchCommandBox(const QString &)));
    connect(ui.YawPositiveTypeComboBox, SIGNAL(activated(const QString &)),   this, SLOT(handleActivatePosYawCommandBox(const QString &)));
    connect(ui.YawNegativeTypeComboBox, SIGNAL(activated(const QString &)),   this, SLOT(handleActivateNegYawCommandBox(const QString &)));

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
    returnHold.id = ui.gestureComboBox->currentText().toUInt();
    
    returnHold.holdModeActionType = ui.holdModeActionType->currentText().toStdString();
    returnHold.intervalLen = ui.intervalLength->value();
    returnHold.intervalExecMultiplier = ui.intervalExecMultiplier->value();
    returnHold.intervalMaxExecs = ui.intervalMaxExecs->value();

    if (isAngleSet(ui.RollPositiveTypeComboBox, ui.RollNegativeTypeComboBox))
    {
        AngleAction roll;
        roll.type = "roll";
        roll.anglePositive.type = ui.RollPositiveTypeComboBox->currentText().toStdString();
        if (ui.RollPositiveActionComboBox->currentText() == "inputVector")
        {
              roll.anglePositive.actions.push_back(ui.RollPositiveActionComboBox->currentText().toStdString() + "," +
                  ui.RollPositiveKeyInput->text().toStdString());
        }
        else
        {
            roll.anglePositive.actions.push_back(ui.RollPositiveActionComboBox->currentText().toStdString());
        }
        roll.angleNegative.type = ui.RollNegativeTypeComboBox->currentText().toStdString();
        if (ui.RollNegativeActionComboBox->currentText() == "inputVector")
        {
            roll.angleNegative.actions.push_back(ui.RollNegativeActionComboBox->currentText().toStdString() + "," +
                ui.RollNegativeKeyInput->text().toStdString());
        }
        else
        {
            roll.angleNegative.actions.push_back(ui.RollNegativeActionComboBox->currentText().toStdString());
        }
        roll.sensitivity = ui.rollSensitivity->value();

        returnHold.angles.push_back(roll);
    }

    if (isAngleSet(ui.PitchPositiveTypeComboBox, ui.PitchNegativeTypeComboBox))
    {
        AngleAction pitch;
        pitch.type = "pitch";
        pitch.anglePositive.type = ui.PitchPositiveTypeComboBox->currentText().toStdString();
        if (ui.PitchPositiveActionComboBox->currentText() == "inputVector")
        {
              pitch.anglePositive.actions.push_back(ui.PitchPositiveActionComboBox->currentText().toStdString() + "," +
                  ui.PitchPositiveKeyInput->text().toStdString());
        }
        else
        {
            pitch.anglePositive.actions.push_back(ui.PitchPositiveActionComboBox->currentText().toStdString());
        }
        pitch.angleNegative.type = ui.PitchNegativeTypeComboBox->currentText().toStdString();
        if (ui.PitchNegativeActionComboBox->currentText() == "inputVector")
        {
              pitch.angleNegative.actions.push_back(ui.PitchNegativeActionComboBox->currentText().toStdString() + "," +
                  ui.PitchNegativeKeyInput->text().toStdString());
        }
        else
        {
            pitch.angleNegative.actions.push_back(ui.PitchNegativeActionComboBox->currentText().toStdString());
        }
        pitch.sensitivity = ui.pitchSensitivity->value();

        returnHold.angles.push_back(pitch);
    }

    if (isAngleSet(ui.YawPositiveTypeComboBox, ui.YawNegativeTypeComboBox))
    {
        AngleAction yaw;
        yaw.type = "yaw";
        yaw.anglePositive.type = ui.YawPositiveTypeComboBox->currentText().toStdString();
        if (ui.YawPositiveActionComboBox->currentText() == "inputVector")
        {
              yaw.anglePositive.actions.push_back(ui.YawPositiveActionComboBox->currentText().toStdString() + "," +
                  ui.YawPositiveKeyInput->text().toStdString());
        }
        else
        {
            yaw.anglePositive.actions.push_back(ui.YawPositiveActionComboBox->currentText().toStdString());
        }
        yaw.angleNegative.type = ui.YawNegativeTypeComboBox->currentText().toStdString();
        if (ui.YawNegativeActionComboBox->currentText() == "inputVector")
        {
              yaw.angleNegative.actions.push_back(ui.YawNegativeActionComboBox->currentText().toStdString() + "," +
                  ui.YawNegativeKeyInput->text().toStdString());
        }
        else
        {
            yaw.angleNegative.actions.push_back(ui.YawNegativeActionComboBox->currentText().toStdString());
        }
        yaw.sensitivity = ui.yawSensitivity->value();

        returnHold.angles.push_back(yaw);
    }

    this->accept();
}

void HoldEditor::handleActivatePosRollCommandBox(const QString & text)
{
    if (text == "keyboard")
    {
        formKybdActions(ui.RollPositiveActionComboBox);
    }
    else if (text == "mouse")
    {
        formMouseActions(ui.RollPositiveActionComboBox);
    }
}

void HoldEditor::handleActivateNegRollCommandBox(const QString & text)
{
    if (text == "keyboard")
    {
        formKybdActions(ui.RollNegativeActionComboBox);
    }
    else if (text == "mouse")
    {
        formMouseActions(ui.RollNegativeActionComboBox);
    }
}

void HoldEditor::handleActivatePosPitchCommandBox(const QString & text)
{
    if (text == "keyboard")
    {
        formKybdActions(ui.PitchPositiveActionComboBox);
    }
    else if (text == "mouse")
    {
        formMouseActions(ui.PitchPositiveActionComboBox);
    }
}

void HoldEditor::handleActivateNegPitchCommandBox(const QString & text)
{
    if (text == "keyboard")
    {
        formKybdActions(ui.PitchNegativeActionComboBox);
    }
    else if (text == "mouse")
    {
        formMouseActions(ui.PitchNegativeActionComboBox);
    }
}

void HoldEditor::handleActivatePosYawCommandBox(const QString & text)
{
    if (text == "keyboard")
    {
        formKybdActions(ui.YawPositiveActionComboBox);
    }
    else if (text == "mouse")
    {
        formMouseActions(ui.YawPositiveActionComboBox);
    }
}

void HoldEditor::handleActivateNegYawCommandBox(const QString & text)
{
    if (text == "keyboard")
    {
        formKybdActions(ui.YawNegativeActionComboBox);
    }
    else if (text == "mouse")
    {
        formMouseActions(ui.YawNegativeActionComboBox);
    }
}


void HoldEditor::formMouseActions(QComboBox *comboBox)
{
    comboBox->clear();

    comboBox->addItem(QString("leftClick"));
    comboBox->addItem(QString("rightClick"));
    comboBox->addItem(QString("middleClick"));
    comboBox->addItem(QString("leftHold"));
    comboBox->addItem(QString("rightHold"));
    comboBox->addItem(QString("middleHold"));
    comboBox->addItem(QString("moveLeft"));
    comboBox->addItem(QString("moveRight"));
    comboBox->addItem(QString("moveUp"));
    comboBox->addItem(QString("moveDown"));
    comboBox->addItem(QString("moveHor"));
    comboBox->addItem(QString("moveVert"));
    comboBox->addItem(QString("scrollLeft"));
    comboBox->addItem(QString("scrollRight"));
    comboBox->addItem(QString("scrollUp"));
    comboBox->addItem(QString("scrollDown"));
    comboBox->addItem(QString("shiftScrollUp"));
    comboBox->addItem(QString("shiftScrollDown"));
    comboBox->addItem(QString("leftRelease"));
    comboBox->addItem(QString("rightRelease"));
    comboBox->addItem(QString("middleRelease"));
    comboBox->addItem(QString("releaseLrmButs"));
}

void HoldEditor::formKybdActions(QComboBox *comboBox)
{
    comboBox->clear();

    comboBox->addItem(QString("undo"));
    comboBox->addItem(QString("redo"));
    comboBox->addItem(QString("zoomIn"));
    comboBox->addItem(QString("zoomOut"));
    comboBox->addItem(QString("zoom100"));
    comboBox->addItem(QString("escape"));
    comboBox->addItem(QString("enter"));
    comboBox->addItem(QString("tab"));
    comboBox->addItem(QString("switchWinForward"));
    comboBox->addItem(QString("switchWinReverse"));
    comboBox->addItem(QString("copy"));
    comboBox->addItem(QString("paste"));
    comboBox->addItem(QString("cut"));
    comboBox->addItem(QString("fileMenu"));
    comboBox->addItem(QString("newBrowser"));
    comboBox->addItem(QString("gotoAddrBar"));
    comboBox->addItem(QString("lockDesktop"));
    comboBox->addItem(QString("editMenu"));
    comboBox->addItem(QString("viewMenu"));
    comboBox->addItem(QString("winHome"));
    comboBox->addItem(QString("hideApps"));
    comboBox->addItem(QString("control"));
    comboBox->addItem(QString("volumeUp"));
    comboBox->addItem(QString("volumeDown"));
    comboBox->addItem(QString("backspace"));
    comboBox->addItem(QString("upArrow"));
    comboBox->addItem(QString("downArrow"));
    comboBox->addItem(QString("rightArrow"));
    comboBox->addItem(QString("leftArrow"));
    comboBox->addItem(QString("inputVector"));
    comboBox->addItem(QString("none"));
}



