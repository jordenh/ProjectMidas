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

#include "SequenceEditor.h"
#include <QListWidgetItem>
#include <QMessageBox.h>
#include <QMenu.h>

SequenceEditor::SequenceEditor(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    otherSequences = NULL;
    formCommandComboBox();
    formMouseActions();
	formKeyLineInput();

    connect(ui.addGestureButton, SIGNAL(released()), this, SLOT(handleAddGesture()));
    connect(ui.addActionButton, SIGNAL(released()), this, SLOT(handleAddAction()));
    connect(ui.doneButton, SIGNAL(released()), this, SLOT(handleDone()));
    connect(ui.commandComboBox, SIGNAL(activated(const QString &)), this, SLOT(handleActivateCommandBox(const QString &)));		
    connect(ui.clearGesturesButton, SIGNAL(released()), this, SLOT(handleClearGestures()));
    connect(ui.clearActionsButton, SIGNAL(released()), this, SLOT(handleClearActions()));
}

SequenceEditor::~SequenceEditor()
{

}

bool SequenceEditor::checkPrefixConstraint(std::string &errorMessage)
{
    if (otherSequences == NULL) 
    {
        errorMessage = "Did not receive other sequences to check against.";
        return false;
    }

    std::vector<Sequence>::iterator seqIt;
    std::vector<std::string> conflictingSequences;
    for (seqIt = otherSequences->begin(); seqIt != otherSequences->end(); seqIt++)
    {
        if (seqIt->state != returnSequence.state) continue;

        Sequence *shortSeq, *longSeq;
        if (returnSequence.gestures.size() <= seqIt->gestures.size())
        {
            shortSeq = &returnSequence;
            longSeq = &(*seqIt);
        }
        else
        {
            shortSeq = &(*seqIt);
            longSeq = &returnSequence;
        }

        bool conflict = true;
        for (int i = 0; i < shortSeq->gestures.size(); i++)
        {
            Gesture shortGest = shortSeq->gestures.at(i);
            Gesture longGest = longSeq->gestures.at(i);

            if (shortGest.type == "immediate" || longGest.type == "immediate")
            {
                if (shortGest.name == longGest.name) break;
            }

            if (shortGest.name != longGest.name || shortGest.type != longGest.type)
            {
                // Difference in prefixes, so no conflict.
                conflict = false;
                break;
            }
        }

        if (conflict)
        {
            conflictingSequences.push_back(seqIt->name);
        }
    }

    errorMessage = "";
    if (!conflictingSequences.empty())
    {
        errorMessage += "Sequence conflicts exist with: ";
        for (int i = 0; i < conflictingSequences.size(); i++)
        {
            errorMessage += conflictingSequences[i];

            if (i != conflictingSequences.size() - 1) errorMessage += ", ";
        }
        return false;
    }

    return true;
}

void SequenceEditor::setOtherSequences(std::vector<Sequence>* otherSequences)
{
    this->otherSequences = otherSequences;
}

void SequenceEditor::populateGUIWithSeq(Sequence seq) 
{ 
    updateGUIWithSeq(seq);
}

Sequence SequenceEditor::getSequence()
{
    return returnSequence;
}

void SequenceEditor::handleAddGesture()
{
    QString gestureType = ui.gestureTypeComboBox->currentText();
    QString gesture = ui.gestureComboBox->currentText();
    QString row = gestureType + " " + gesture;
    ui.gestureList->addItem(row);
}

void SequenceEditor::handleAddAction()
{
    QString command = ui.commandComboBox->currentText();
    ui.commandList->addItem(command);

    QString action = ui.actionComboBox->currentText();
	if (action == "inputVector")
	{
		action += ("," + ui.keyInput->text());
	}
    ui.actionList->addItem(action);
}

void SequenceEditor::handleDone()
{
    returnSequence.name = ui.lineEdit->text().toStdString();
    returnSequence.state = ui.stateComboBox->currentText().toStdString();

    std::vector<Gesture> gestures;
    for (int row = 0; row < ui.gestureList->count(); row++)
    {
        QListWidgetItem* item = ui.gestureList->item(row);
        Gesture gest;
        QStringList gestStr = item->text().split(" ");
        gest.name = gestStr[1].toStdString();
        gest.type = gestStr[0].toStdString();
        gestures.push_back(gest);
    }

    returnSequence.gestures = gestures;

    if (ui.actionList->count() == ui.commandList->count())
    {
        for (int row = 0; row < ui.commandList->count(); row++)
        {
            QListWidgetItem* item = ui.commandList->item(row);
            std::string command;
            command = item->text().toStdString();

            item = ui.actionList->item(row);
            std::string action;
            action = item->text().toStdString();

            Command cmd;
            cmd.type = command;
            std::vector<std::string> actionVec;
            actionVec.push_back(action); // Only support one action per command as of now. Makes more sense to have multiple commands.
            cmd.actions = actionVec;
            returnSequence.cmds.push_back(cmd);
        }
    }
    else
    {
        // default to legacy behaviour
        std::vector<std::string> actions;
        for (int row = 0; row < ui.actionList->count(); row++)
        {
            QListWidgetItem* item = ui.actionList->item(row);
            std::string action;
            action = item->text().toStdString();
            actions.push_back(action);
        }
        // TODO - in the future make this accept/write multiple commands. For now, not supported.
        Command cmd;
        cmd.type = ui.commandComboBox->currentText().toStdString();
        cmd.actions = actions;
        returnSequence.cmds.push_back(cmd);
    } 

    std::string errorMsg;
    if (checkPrefixConstraint(errorMsg))
    {
        this->accept();
    }
    else
    {
        QMessageBox::warning(this, tr("Invalid Sequence"), tr(errorMsg.c_str()));
    }
}

void SequenceEditor::handleClearGestures()
{
    ui.gestureList->clear();
}

void SequenceEditor::handleClearActions()
{
    ui.commandList->clear();
    ui.actionList->clear();
}

void SequenceEditor::handleActivateCommandBox(const QString & text)
{
    if (text == "keyboard")
    {
        formKybdActions();
    }
    else if (text == "keyboardGui")
    {
        formKybdGUIActions();
    }
    else if (text == "mouse")
    {
        formMouseActions();
    }
    else if (text == "stateChange")
    {
        formStateChangeActions();
    }
	else if (text == "profileChange")
	{
		formProfileChangeActions();
	}
}

void SequenceEditor::formCommandComboBox()
{
    ui.commandComboBox->clear();

	ui.commandComboBox->addItem(QString("mouse"));
    ui.commandComboBox->addItem(QString("keyboard"));
    ui.commandComboBox->addItem(QString("keyboardGui"));
    ui.commandComboBox->addItem(QString("stateChange"));
	ui.commandComboBox->addItem(QString("profileChange"));
}

void SequenceEditor::formKeyLineInput()
{
	// uncomment if we want ALNUM chars.
	//const QRegExp qregexp("[0-9a-zA-Z]*");
	//ui.keyInput->setValidator(new QRegExpValidator(qregexp));
}

void SequenceEditor::formStateChangeActions()
{
    ui.actionComboBox->clear();

    ui.actionComboBox->addItem(QString("lockMode"));
    ui.actionComboBox->addItem(QString("mouseMode"));
    ui.actionComboBox->addItem(QString("keyboardMode"));
    ui.actionComboBox->addItem(QString("gestureMode"));
    ui.actionComboBox->addItem(QString("gestureHoldOne"));
    ui.actionComboBox->addItem(QString("gestureHoldTwo"));
    ui.actionComboBox->addItem(QString("gestureHoldThree"));
    ui.actionComboBox->addItem(QString("gestureHoldFour"));
    ui.actionComboBox->addItem(QString("gestureHoldFive"));
}

void SequenceEditor::formMouseActions()
{
    ui.actionComboBox->clear();

    ui.actionComboBox->addItem(QString("leftClick"));
    ui.actionComboBox->addItem(QString("rightClick"));
    ui.actionComboBox->addItem(QString("middleClick"));
    ui.actionComboBox->addItem(QString("leftHold"));
    ui.actionComboBox->addItem(QString("rightHold"));
    ui.actionComboBox->addItem(QString("middleHold"));
    ui.actionComboBox->addItem(QString("moveLeft"));
    ui.actionComboBox->addItem(QString("moveRight"));
    ui.actionComboBox->addItem(QString("moveUp"));
    ui.actionComboBox->addItem(QString("moveDown"));
    ui.actionComboBox->addItem(QString("moveHor"));
    ui.actionComboBox->addItem(QString("moveVert"));
    ui.actionComboBox->addItem(QString("scrollLeft"));
    ui.actionComboBox->addItem(QString("scrollRight"));
    ui.actionComboBox->addItem(QString("scrollUp"));
    ui.actionComboBox->addItem(QString("scrollDown"));
    ui.actionComboBox->addItem(QString("leftRelease"));
    ui.actionComboBox->addItem(QString("rightRelease"));
    ui.actionComboBox->addItem(QString("middleRelease"));
    ui.actionComboBox->addItem(QString("releaseLrmButs"));
}

void SequenceEditor::formKybdActions()
{
    ui.actionComboBox->clear();

    ui.actionComboBox->addItem(QString("undo"));
    ui.actionComboBox->addItem(QString("redo"));
    ui.actionComboBox->addItem(QString("zoomIn"));
    ui.actionComboBox->addItem(QString("zoomOut"));
    ui.actionComboBox->addItem(QString("zoom100"));
    ui.actionComboBox->addItem(QString("escape"));
    ui.actionComboBox->addItem(QString("enter"));
    ui.actionComboBox->addItem(QString("tab"));
    ui.actionComboBox->addItem(QString("switchWinForward"));
    ui.actionComboBox->addItem(QString("switchWinReverse"));
    ui.actionComboBox->addItem(QString("copy"));
    ui.actionComboBox->addItem(QString("paste"));
    ui.actionComboBox->addItem(QString("cut"));
    ui.actionComboBox->addItem(QString("fileMenu"));
    ui.actionComboBox->addItem(QString("newBrowser"));
    ui.actionComboBox->addItem(QString("gotoAddrBar"));
    ui.actionComboBox->addItem(QString("lockDesktop"));
    ui.actionComboBox->addItem(QString("editMenu"));
    ui.actionComboBox->addItem(QString("viewMenu"));
    ui.actionComboBox->addItem(QString("winHome"));
    ui.actionComboBox->addItem(QString("hideApps"));
    ui.actionComboBox->addItem(QString("control"));
    ui.actionComboBox->addItem(QString("volumeUp"));
    ui.actionComboBox->addItem(QString("volumeDown"));
    ui.actionComboBox->addItem(QString("backspace"));
    ui.actionComboBox->addItem(QString("upArrow"));
    ui.actionComboBox->addItem(QString("downArrow"));
    ui.actionComboBox->addItem(QString("rightArrow"));
	ui.actionComboBox->addItem(QString("leftArrow")); 
	ui.actionComboBox->addItem(QString("inputVector"));
    ui.actionComboBox->addItem(QString("none"));
}

void SequenceEditor::formKybdGUIActions()
{
    ui.actionComboBox->clear();

    ui.actionComboBox->addItem(QString("swapRingFocus"));
    ui.actionComboBox->addItem(QString("changeWheels"));
    ui.actionComboBox->addItem(QString("select"));
    ui.actionComboBox->addItem(QString("holdSelect"));
    ui.actionComboBox->addItem(QString("undo"));
    ui.actionComboBox->addItem(QString("redo"));
    ui.actionComboBox->addItem(QString("zoomIn"));
    ui.actionComboBox->addItem(QString("zoomOut"));
    ui.actionComboBox->addItem(QString("zoom100"));
    ui.actionComboBox->addItem(QString("escape"));
    ui.actionComboBox->addItem(QString("enter"));
    ui.actionComboBox->addItem(QString("tab"));
    ui.actionComboBox->addItem(QString("switchWinForward"));
    ui.actionComboBox->addItem(QString("switchWinReverse"));
    ui.actionComboBox->addItem(QString("copy"));
    ui.actionComboBox->addItem(QString("paste"));
    ui.actionComboBox->addItem(QString("cut"));
    ui.actionComboBox->addItem(QString("fileMenu"));
    ui.actionComboBox->addItem(QString("newBrowser"));
    ui.actionComboBox->addItem(QString("gotoAddrBar"));
    ui.actionComboBox->addItem(QString("lockDesktop"));
    ui.actionComboBox->addItem(QString("editMenu"));
    ui.actionComboBox->addItem(QString("viewMenu"));
    ui.actionComboBox->addItem(QString("winHome"));
    ui.actionComboBox->addItem(QString("hideApps"));
    ui.actionComboBox->addItem(QString("control"));
    ui.actionComboBox->addItem(QString("volumeUp"));
    ui.actionComboBox->addItem(QString("volumeDown"));
    ui.actionComboBox->addItem(QString("backspace"));
    ui.actionComboBox->addItem(QString("upArrow"));
    ui.actionComboBox->addItem(QString("downArrow"));
    ui.actionComboBox->addItem(QString("rightArrow"));
    ui.actionComboBox->addItem(QString("leftArrow"));
    ui.actionComboBox->addItem(QString("none"));
}

void SequenceEditor::formProfileChangeActions()
{
	ui.actionComboBox->clear();

	ui.actionComboBox->addItem(QString("moveProfileForward"));
	ui.actionComboBox->addItem(QString("moveProfileBackward"));
}

void SequenceEditor::updateGUIWithSeq(Sequence seq)
{
    ui.lineEdit->setText(QString::fromStdString(seq.name));
    ui.stateComboBox->setCurrentText(QString::fromStdString(seq.state));

    for (int row = 0; row < seq.gestures.size(); row++)
    {
        Gesture gest = seq.gestures.at(row);
        QListWidgetItem* newItem = new QListWidgetItem();
        newItem->setText(QString::fromStdString(gest.type + " " + gest.name));
        ui.gestureList->addItem(newItem);
    }

    for (int row = 0; row < seq.cmds.size(); row++)
    {
        Command cmd = seq.cmds.at(row);
        QListWidgetItem* newCmd = new QListWidgetItem();
        QListWidgetItem* newAction = new QListWidgetItem();
        newCmd->setText(QString::fromStdString(cmd.type));
        newAction->setText(QString::fromStdString(cmd.actions.at(0)));
        ui.commandList->addItem(newCmd);
        ui.actionList->addItem(newAction);
    }
}