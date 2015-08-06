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

#include "SequenceImageManager.h"
#include <QImage.h>
#include <qpainter.h>
#include <qstring.h>

#include "myo\myo.hpp"

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
using namespace myo;
#endif

SequenceImageManager::SequenceImageManager()
{
}


SequenceImageManager::~SequenceImageManager()
{
}

std::vector<sequenceImageSet> SequenceImageManager::formSequenceSetFromIds(std::vector<int> ids, std::vector<PoseLength> lengths)
{
    std::vector<int>::iterator it;
    std::vector<sequenceImageSet> sequenceImages;

    int idx = 0;
    for (it = ids.begin(); it != ids.end(); ++it)
    {
        if (idToImageMap.find(*it) != idToImageMap.end())
        {
            sequenceImageSet currentImageSet = idToImageMap[*it];

            int currentWidth = currentImageSet.nextImageRight.width();
            int currentHeight = currentImageSet.nextImageRight.height();

            QPixmap nextRightModified(currentWidth, currentHeight);
            nextRightModified.fill(Qt::transparent); // force alpha channel
            QPainter painter(&nextRightModified);
            painter.drawPixmap(0, 0, currentImageSet.nextImageRight);
            
            QPixmap nextLeftModified(currentWidth, currentHeight);
            nextLeftModified.fill(Qt::transparent); // force alpha channel
            QPainter painter1b(&nextLeftModified);
            painter1b.drawPixmap(0, 0, currentImageSet.nextImageLeft);

            QPixmap laterRightModified(currentWidth, currentHeight);
            laterRightModified.fill(Qt::transparent); // force alpha channel
            QPainter painter2(&laterRightModified);
            painter2.drawPixmap(0, 0, currentImageSet.laterImageRight);

            QPixmap laterLeftModified(currentWidth, currentHeight);
            laterLeftModified.fill(Qt::transparent); // force alpha channel
            QPainter painter2b(&laterLeftModified);
            painter2b.drawPixmap(0, 0, currentImageSet.laterImageLeft);

            QImage overlayImage;
            QPixmap pic;
            switch (lengths[idx])
            {
            case PoseLength::HOLD:
                overlayImage = QImage(QString(POSE_LENGTH_HOLD_PATH));
                pic = QPixmap::fromImage(overlayImage);
                pic = pic.scaled(currentWidth / POSE_LEN_SCALEDOWN_ICON, currentHeight / POSE_LEN_SCALEDOWN_ICON, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);

                painter.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                painter1b.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                painter2.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                painter2b.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                currentImageSet.nextImageRight = nextRightModified;
                currentImageSet.nextImageLeft = nextLeftModified;
                currentImageSet.laterImageRight = laterRightModified;
                currentImageSet.laterImageLeft = laterLeftModified;
                break;
            case PoseLength::IMMEDIATE:
                overlayImage = QImage(QString(POSE_LENGTH_IMMEDIATE_PATH));
                pic = QPixmap::fromImage(overlayImage);
                pic = pic.scaled(currentWidth / POSE_LEN_SCALEDOWN_ICON, currentHeight / POSE_LEN_SCALEDOWN_ICON, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);

                painter.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                painter1b.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                painter2.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                painter2b.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                currentImageSet.nextImageRight = nextRightModified;
                currentImageSet.nextImageLeft = nextLeftModified;
                currentImageSet.laterImageRight = laterRightModified;
                currentImageSet.laterImageLeft = laterLeftModified;
                break;
            case PoseLength::TAP:
                break;
            default:
                break;
            }

            sequenceImages.push_back(currentImageSet);
            sequenceImages.at(idx).poseLen = lengths[idx];
            idx++;
        }
        else
        {
            return std::vector<sequenceImageSet>();
        }
    }

    return sequenceImages;
}

void SequenceImageManager::loadImages()
{
    QImage rightFingerSpreadLater(QString(RIGHT_FINGER_SPREAD_LATER_PATH));
    QImage rightFingerSpreadNext(QString(RIGHT_FINGER_SPREAD_NEXT_PATH));
    QImage rightFistLater(QString(RIGHT_FIST_LATER_PATH));
    QImage rightFistNext(QString(RIGHT_FIST_NEXT_PATH));
    QImage rightDoubleTapLater(QString(RIGHT_DOUBLE_TAP_LATER_PATH));
    QImage rightDoubleTapNext(QString(RIGHT_DOUBLE_TAP_NEXT_PATH));
    QImage rightWaveInLater(QString(RIGHT_WAVE_IN_LATER_PATH));
    QImage rightWaveInNext(QString(RIGHT_WAVE_IN_NEXT_PATH));
    QImage rightWaveOutLater(QString(RIGHT_WAVE_OUT_LATER_PATH));
    QImage rightWaveOutNext(QString(RIGHT_WAVE_OUT_NEXT_PATH));
    QImage leftFingerSpreadLater(QString(LEFT_FINGER_SPREAD_LATER_PATH));
    QImage leftFingerSpreadNext(QString(LEFT_FINGER_SPREAD_NEXT_PATH));
    QImage leftFistLater(QString(LEFT_FIST_LATER_PATH));
    QImage leftFistNext(QString(LEFT_FIST_NEXT_PATH));
    QImage leftDoubleTapLater(QString(LEFT_DOUBLE_TAP_LATER_PATH));
    QImage leftDoubleTapNext(QString(LEFT_DOUBLE_TAP_NEXT_PATH));
    QImage leftWaveInLater(QString(LEFT_WAVE_IN_LATER_PATH));
    QImage leftWaveInNext(QString(LEFT_WAVE_IN_NEXT_PATH));
    QImage leftWaveOutLater(QString(LEFT_WAVE_OUT_LATER_PATH));
    QImage leftWaveOutNext(QString(LEFT_WAVE_OUT_NEXT_PATH));


    QImage noHandLater(QString(NO_HAND_LATER_PATH));
    QImage noHandNext(QString(NO_HAND_NEXT_PATH));
    
    sequenceImageSet fingerSpread;
    fingerSpread.actionTag = Pose::fingersSpread;
    fingerSpread.nextImageRight = QPixmap::fromImage(rightFingerSpreadNext);
    fingerSpread.laterImageRight = QPixmap::fromImage(rightFingerSpreadLater);
    fingerSpread.nextImageLeft = QPixmap::fromImage(leftFingerSpreadNext);
    fingerSpread.laterImageLeft = QPixmap::fromImage(leftFingerSpreadLater);
    idToImageMap[fingerSpread.actionTag] = fingerSpread;

    sequenceImageSet fist;
    fist.actionTag = Pose::fist;
    fist.nextImageRight = QPixmap::fromImage(rightFistNext);
    fist.laterImageRight = QPixmap::fromImage(rightFistLater);
    fist.nextImageLeft = QPixmap::fromImage(leftFistNext);
    fist.laterImageLeft = QPixmap::fromImage(leftFistLater);
    idToImageMap[fist.actionTag] = fist;

    sequenceImageSet doubleTap;
    doubleTap.actionTag = Pose::doubleTap;
    doubleTap.nextImageRight = QPixmap::fromImage(rightDoubleTapNext);
    doubleTap.laterImageRight = QPixmap::fromImage(rightDoubleTapLater);
    doubleTap.nextImageLeft = QPixmap::fromImage(leftDoubleTapNext);
    doubleTap.laterImageLeft = QPixmap::fromImage(leftDoubleTapLater);
    idToImageMap[doubleTap.actionTag] = doubleTap;

    sequenceImageSet waveIn;
    waveIn.actionTag = Pose::waveIn;
    waveIn.nextImageRight = QPixmap::fromImage(rightWaveInNext);
    waveIn.laterImageRight = QPixmap::fromImage(rightWaveInLater);
    waveIn.nextImageLeft = QPixmap::fromImage(leftWaveInNext);
    waveIn.laterImageLeft = QPixmap::fromImage(leftWaveInLater);
    idToImageMap[waveIn.actionTag] = waveIn;

    sequenceImageSet waveOut;
    waveOut.actionTag = Pose::waveOut;
    waveOut.nextImageRight = QPixmap::fromImage(rightWaveOutNext);
    waveOut.laterImageRight = QPixmap::fromImage(rightWaveOutLater);
    waveOut.nextImageLeft = QPixmap::fromImage(leftWaveOutNext);
    waveOut.laterImageLeft = QPixmap::fromImage(leftWaveOutLater);
    idToImageMap[waveOut.actionTag] = waveOut;

    sequenceImageSet rest;
    rest.actionTag = Pose::rest;
    rest.nextImageRight = QPixmap::fromImage(noHandNext);
    rest.laterImageRight = QPixmap::fromImage(noHandLater);
    rest.nextImageLeft = QPixmap::fromImage(noHandNext);
    rest.laterImageLeft = QPixmap::fromImage(noHandLater);
    idToImageMap[rest.actionTag] = rest;

}
