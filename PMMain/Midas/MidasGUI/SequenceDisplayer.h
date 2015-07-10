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

#ifndef SEQUENCE_DISPLAYER_H
#define SEQUENCE_DISPLAYER_H

#include <qwidget.h>

#include "MyoCommon.h"

#include <map>
#include <vector>

class QGridLayout;
class QLabel;

#define NUM_COLUMNS 5

Q_DECLARE_METATYPE(std::vector<sequenceImageSet>)
Q_DECLARE_METATYPE(std::vector<sequenceProgressData>)

/**
 * The SequenceDisplayer class represents a GUI that can display Midas gesture sequences. These gesture sequences
 * are made up of a series of images that should convey information to the user on what gesture they must perform
 * next in the sequence.
 */
class SequenceDisplayer : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor for the SequenceDisplayer.
     *
     * @param parent The parent widget.
     */
    SequenceDisplayer(QWidget *parent = 0);

    ~SequenceDisplayer();

    /**
     * Returns the recommended size of the widget.
     *
     * @return The recommended size of the widget.
     */
    QSize sizeHint() const;

public slots:
    /**
     * Register a sequence, with an ID, name, and set of images.
     *
     * @param id The id of the sequence.
     * @param name A human-readable name for the sequence.
     * @param images The images that make up the sequence. Each element of the vector contains two images.
     */
    void registerSequenceImages(int, QString, std::vector<sequenceImageSet>);

    /**
     * Show the provided sequences on the display. The caller supplied a vector of pairs; each pair has
     * a sequence ID and a number that represents the current position in the sequence.
     *
     * @param sequenceIdProgressPairs A vector containing the pairs of sequence IDs and sequence positions.
     */
    void showSequences(std::vector<sequenceProgressData>);
 
private:
    /**
     * Clear the widgets in the grid layout.
     *
     * @param deleteLabels If true, deletes the labels in the sequences to free memory. Otherwise, only hides the 
     *        labels in the layout.
     */
    void clearWidgets(bool deleteLabels = false);

    /**
     * Clears the widgets in a row in the grid layout.
     *
     * @param seq The row of the grid layout to clear.
     * @param deleteLabels If true, deletes the labels in the row to free memory. Otherwise, only hides the labels.
     */
    void clearRow(sequenceData seq, bool deleteLabels = false);

    /**
     * Sets the properties of the provided label to conform to the look of the members of the grid layout.
     *
     * @param label The label to set properties for.
     */
    void formBoxLabel(QLabel *label);

    /**
     * Updates the GUI with the new active sequence information.
     */

    void updateSequences();

    /**
     * Cleans up the memory allocated by the sequence displayer.
     */
    void cleanup();

    std::map<int, sequenceData> sequenceIdToDataMap;
    std::map<int, sequenceData> activeSequencesIdToDataMap;
    QGridLayout *gridLayout;
    int maxWidth, maxHeight;
    int maxNumSequences;
};

#endif SEQUENCE_DISPLAYER_H