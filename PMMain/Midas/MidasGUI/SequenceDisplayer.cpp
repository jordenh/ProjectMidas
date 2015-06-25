#include "SequenceDisplayer.h"

#include "MidasCommon.h"
#include "DraggableWidget.h"
#include <QImage.h>
#include <qevent.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <QGridLayout.h>

#define LABEL_NUM_COLS      3
#define LABEL_NUM_ROWS      1
#define SEQ_NUMBER_NUM_COLS 1
#define SEQ_NUMBER_NUM_ROWS 1
#define NUM_SEQUENCE_STEPS  3
#define NUM_COLS (LABEL_NUM_COLS + SEQ_NUMBER_NUM_COLS + NUM_SEQUENCE_STEPS)
#define GUI_WIDTH_BUFFER 1

SequenceDisplayer::SequenceDisplayer(QWidget *parent)
    : QWidget(parent)
{
    gridLayout = new QGridLayout;
    gridLayout->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    gridLayout->setSpacing(5);
    setLayout(gridLayout);
    maxNumSequences = 10;

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);

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
    newSequence.seqLabel->setFont(timesFont);
    newSequence.seqLabel->setWordWrap(true);
    formBoxLabel(newSequence.seqLabel);
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
        int currCol = 0;
        seq.seqLabel->setHidden(false);
        seq.seqPosLabel->setHidden(false);
        gridLayout->addWidget(seq.seqLabel, currRow, currCol, LABEL_NUM_ROWS, LABEL_NUM_COLS);
        currCol += LABEL_NUM_COLS;
        gridLayout->addWidget(seq.seqPosLabel, currRow, currCol, SEQ_NUMBER_NUM_ROWS, SEQ_NUMBER_NUM_COLS);
        currCol += SEQ_NUMBER_NUM_COLS;
        std::vector<sequenceImageSet>::iterator sequenceIt;
        for (sequenceIt = seq.sequenceImages.begin() + seq.imageOffset;
            sequenceIt != seq.sequenceImages.end() && currCol < NUM_COLS; sequenceIt++)
        {
            QPixmap pixmap = sequenceIt->laterImage;
            if (currCol == LABEL_NUM_COLS + SEQ_NUMBER_NUM_COLS) pixmap = sequenceIt->nextImage;

            sequenceIt->currentImgLabel->setPixmap(pixmap);
            sequenceIt->currentImgLabel->setEnabled(!pixmap.isNull());
            sequenceIt->currentImgLabel->setHidden(false);
            gridLayout->addWidget(sequenceIt->currentImgLabel, currRow, currCol);
            currCol++;
        }

        currRow++;
    }
}