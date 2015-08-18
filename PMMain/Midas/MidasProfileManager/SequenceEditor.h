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

#ifndef SEQUENCEEDITOR_H
#define SEQUENCEEDITOR_H

#include <QDialog>
#include "ui_SequenceEditor.h"
#include "ProfileWriter.h"

class SequenceEditor : public QDialog
{
    Q_OBJECT

public:
    SequenceEditor(QWidget *parent = 0);
    ~SequenceEditor();

    Sequence getSequence();
    void setOtherSequences(std::vector<Sequence>* otherSequences);

    void populateGUIWithSeq(Sequence seq);

private:
    void formCommandComboBox();
	void formKeyLineInput();
    void formStateChangeActions();
    void formMouseActions();
    void formKybdActions();
    void formKybdGUIActions();
	void formProfileChangeActions();
    bool checkPrefixConstraint(std::string &errorMessage);
    void updateGUIWithSeq(Sequence seq);

    Ui::SequenceEditor ui;
    // returnSeuqnece is the sequence that this editor is creating.
    Sequence returnSequence;
    // otherSequences used to compare against when adding sequences to verify sanity.
    std::vector<Sequence>* otherSequences;

public slots:
    void handleAddGesture();
    void handleAddAction();
    void handleDone();
    void handleActivateCommandBox(const QString & text);
    void handleClearGestures();
    void handleClearActions();
};

#endif // SEQUENCEEDITOR_H
