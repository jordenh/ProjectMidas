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

#include "SequenceDisplayer.h"

#include "MidasCommon.h"
#include "DraggableWidget.h"
#include "MainGUI.h"
#include <QImage.h>
#include <qevent.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <QGridLayout.h>
#include <qlabel.h>

#define LABEL_NUM_COLS      2
#define LABEL_NUM_ROWS      1
#define SEQ_NUMBER_NUM_COLS 1
#define SEQ_NUMBER_NUM_ROWS 1
#define NUM_SEQUENCE_STEPS  4 // Max number of steps to show to a user at a time
#define NUM_COLS (LABEL_NUM_COLS + SEQ_NUMBER_NUM_COLS + NUM_SEQUENCE_STEPS)
#define GUI_WIDTH_BUFFER 1
#define MAX_NUM_SEQUENCES_DISPLAYED 15

SequenceDisplayer::SequenceDisplayer(QWidget *parent)
    : QWidget(parent), isRightHand(true), midasHelpLevel(helpLevel::ALL)
{
    gridLayout = new QGridLayout;
    gridLayout->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    gridLayout->setSpacing(WIDGET_BUFFER);
    gridLayout->setMargin(0);
    setLayout(gridLayout);
    maxNumSequences = MAX_NUM_SEQUENCES_DISPLAYED;

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(GUI_OPACITY);

    maxHeight = GRID_ELEMENT_SIZE * maxNumSequences;
    maxWidth = GRID_ELEMENT_SIZE * (NUM_COLS + GUI_WIDTH_BUFFER);

    setFixedSize(maxWidth, maxHeight);
}

SequenceDisplayer::~SequenceDisplayer()
{
    cleanup();
}

void SequenceDisplayer::cleanup()
{
    std::map<int, sequenceData>::iterator it;

    for (it = sequenceIdToDataMap.begin(); it != sequenceIdToDataMap.end(); ++it)
    {
        delete it->second.seqLabel;
        it->second.seqLabel = NULL;
        delete it->second.seqPosLabel;
        it->second.seqPosLabel = NULL;

        std::vector<sequenceImageSet>::iterator image_it;
        for (image_it = it->second.sequenceImages.begin(); image_it != it->second.sequenceImages.end(); image_it++)
        {
            delete image_it->currentImgLabel;
        }
    }

    delete gridLayout;
}

QSize SequenceDisplayer::sizeHint() const
{
    return QSize(maxWidth, maxHeight);
}

void SequenceDisplayer::formBoxLabel(QLabel *label)
{
    label->setEnabled(false);
    label->setFrameShape(QFrame::Box);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label->setBackgroundRole(QPalette::Base);
    label->setAlignment(Qt::AlignCenter);
    label->setAutoFillBackground(true);
    label->setFixedSize(GRID_ELEMENT_SIZE, GRID_ELEMENT_SIZE);
    label->setScaledContents(true);
}

void SequenceDisplayer::registerSequenceImages(int seqId, QString sequenceName, std::vector<sequenceImageSet> sequenceImages)
{
    sequenceData newSequence;
    newSequence.sequenceImages = sequenceImages;
    QFont timesFont("Times", 9, QFont::Bold);
    newSequence.seqLabel = new QLabel(tr("%1").arg(sequenceName));
    newSequence.seqLabel->setWordWrap(true);
    formBoxLabel(newSequence.seqLabel);  
    newSequence.seqLabel->setFont(timesFont);
    newSequence.seqLabel->setFixedSize(GRID_ELEMENT_SIZE * LABEL_NUM_COLS, GRID_ELEMENT_SIZE);
    newSequence.seqPosLabel = new QLabel;
    newSequence.seqPosLabel->setFont(timesFont);
    formBoxLabel(newSequence.seqPosLabel);

    std::vector<sequenceImageSet>::iterator it;
    for (it = newSequence.sequenceImages.begin(); it != newSequence.sequenceImages.end(); it++)
    {
        it->currentImgLabel = new QLabel;
        formBoxLabel(it->currentImgLabel);
    }

    sequenceIdToDataMap[seqId] = newSequence;
}

void SequenceDisplayer::showSequences(std::vector<sequenceProgressData> progressPairs)
{
    // Clear the current active widgets.
    clearWidgets();
    activeSequencesIdToDataMap.clear();

    std::vector<sequenceProgressData>::iterator it;

    for (it = progressPairs.begin(); it != progressPairs.end(); ++it)
    {
        sequenceData seqData = sequenceIdToDataMap[it->seqId];
        seqData.seqPosLabel->setText(tr("%1 / %2").arg(QString::number(it->progress),
            QString::number(seqData.sequenceImages.size())));
        seqData.imageOffset = it->progress;
        activeSequencesIdToDataMap[it->seqId] = seqData;
    }

    updateSequences();
}

void SequenceDisplayer::clearRow(sequenceData seq, bool deleteLabels)
{
    seq.seqLabel->setHidden(true);
    gridLayout->removeWidget(seq.seqLabel);
    seq.seqPosLabel->setHidden(true);
    gridLayout->removeWidget(seq.seqPosLabel);

    if (deleteLabels)
    {
        delete seq.seqLabel;
        delete seq.seqPosLabel;
        seq.seqLabel = seq.seqPosLabel = NULL;
    }

    std::vector<sequenceImageSet> sequenceImages = seq.sequenceImages;
    std::vector<sequenceImageSet>::iterator sequenceIt;
    for (sequenceIt = sequenceImages.begin();
        sequenceIt != sequenceImages.end(); sequenceIt++)
    {
        sequenceIt->currentImgLabel->setHidden(true);
        gridLayout->removeWidget(sequenceIt->currentImgLabel);
        if (deleteLabels)
        {
            delete sequenceIt->currentImgLabel;
            sequenceIt->currentImgLabel = NULL;
        }
    }
}

void SequenceDisplayer::clearWidgets(bool deleteLabels)
{
    std::map<int, sequenceData>::iterator it;
    for (it = activeSequencesIdToDataMap.begin(); it != activeSequencesIdToDataMap.end(); it++)
    {
        sequenceData seq = it->second;
        clearRow(seq, deleteLabels);
    }
}

void SequenceDisplayer::updateSequences()
{
    std::map<int, sequenceData>::iterator it;

    int currRow = 0;
    for (it = activeSequencesIdToDataMap.begin(); it != activeSequencesIdToDataMap.end(); it++)
    {
        sequenceData seq = it->second;

        switch (midasHelpLevel)
        {
        case helpLevel::MINIMAL:
            if (seq.seqLabel->text().compare("Lock", Qt::CaseInsensitive) == 0 ||
                seq.seqLabel->text().compare("Unlock", Qt::CaseInsensitive) == 0)
            {
                break;
            }
            else
            {
                // go to displaying next sequence
                continue;
            }
        case helpLevel::SIMPLE:
            if (seq.seqLabel->text().compare("Lock", Qt::CaseInsensitive) == 0 ||
                seq.seqLabel->text().compare("Unlock", Qt::CaseInsensitive) == 0 ||
                seq.sequenceImages.size() == 1)
            {
                break;
            }
            else
            {
                // go to displaying next sequence
                continue;
            }
        case helpLevel::COMPLEX:
            if (seq.sequenceImages.size() >= 2)
            {
                break;
            }
            else
            {
                // go to displaying next sequence
                continue;
            }
        case helpLevel::ALL:
            break;
        default:
            break;
        }

        int currCol = 0;
        seq.seqLabel->setHidden(false);
        gridLayout->addWidget(seq.seqLabel, currRow, currCol, LABEL_NUM_ROWS, LABEL_NUM_COLS);
        currCol += LABEL_NUM_COLS;
#ifdef SHOW_SEQUENCE_COUNT
        seq.seqPosLabel->setHidden(false);
        gridLayout->addWidget(seq.seqPosLabel, currRow, currCol, SEQ_NUMBER_NUM_ROWS, SEQ_NUMBER_NUM_COLS);
        currCol += SEQ_NUMBER_NUM_COLS;
#endif
        std::vector<sequenceImageSet>::iterator sequenceIt;
        for (sequenceIt = seq.sequenceImages.begin();
            sequenceIt != seq.sequenceImages.end() && currCol < NUM_COLS; sequenceIt++)
        {
            QPixmap pixmap;
#ifdef SHOW_SEQUENCE_COUNT
            if (currCol < (LABEL_NUM_COLS + SEQ_NUMBER_NUM_COLS + seq.imageOffset))
#else
            if (currCol < (LABEL_NUM_COLS + seq.imageOffset))
#endif
            {
                // show completed sequenceImages (for legacy reasons called laterImage)
                pixmap = isRightHand ? sequenceIt->laterImageRight : sequenceIt->laterImageLeft;
            }
            else
            {
                // show upcoming sequenceImages, which are a different more vibrant colour (called nextImage)
                pixmap = isRightHand ? sequenceIt->nextImageRight : sequenceIt->nextImageLeft;
            }

            sequenceIt->currentImgLabel->setPixmap(pixmap);
            sequenceIt->currentImgLabel->setEnabled(!pixmap.isNull());
            sequenceIt->currentImgLabel->setHidden(false);
            gridLayout->addWidget(sequenceIt->currentImgLabel, currRow, currCol);
            currCol++;
        }

        currRow++;
    }
}

void SequenceDisplayer::handleIsRightHand(bool isRightHand)
{
    this->isRightHand = isRightHand;
    updateSequences();
}

void SequenceDisplayer::handleHelpLevelChanged(int lvl)
{
    midasHelpLevel = helpLevel(lvl);
    clearWidgets();
    updateSequences();
}