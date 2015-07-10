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

            int currentWidth = currentImageSet.nextImage.width();
            int currentHeight = currentImageSet.nextImage.height();

            QPixmap nextModified(currentWidth, currentHeight);
            nextModified.fill(Qt::transparent); // force alpha channel
            QPainter painter(&nextModified);
            painter.drawPixmap(0, 0, currentImageSet.nextImage);

            QPixmap laterModified(currentWidth, currentHeight);
            laterModified.fill(Qt::transparent); // force alpha channel
            QPainter painter2(&laterModified);
            QImage holdOverlayImage;
            QPixmap pic;
            painter2.drawPixmap(0, 0, currentImageSet.laterImage);
            switch (lengths[idx])
            {
            case PoseLength::HOLD:
                holdOverlayImage = QImage(QString(POSE_LENGTH_HOLD_PATH));
                pic = QPixmap::fromImage(holdOverlayImage);
                pic = pic.scaled(currentWidth / POSE_LEN_SCALEDOWN_ICON, currentHeight / POSE_LEN_SCALEDOWN_ICON, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);

                painter.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                painter2.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                currentImageSet.nextImage = nextModified;
                currentImageSet.laterImage = laterModified;
                break;
            case PoseLength::IMMEDIATE:
                holdOverlayImage = QImage(QString(POSE_LENGTH_IMMEDIATE_PATH));
                pic = QPixmap::fromImage(holdOverlayImage);
                pic = pic.scaled(currentWidth / POSE_LEN_SCALEDOWN_ICON, currentHeight / POSE_LEN_SCALEDOWN_ICON, Qt::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation);

                painter.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                painter2.drawPixmap(currentWidth - (currentWidth / POSE_LEN_SCALEDOWN_ICON), 0, pic);
                currentImageSet.nextImage = nextModified;
                currentImageSet.laterImage = laterModified;
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
    QImage fingerSpreadLater(QString(FINGER_SPREAD_LATER_PATH));
    QImage fingerSpreadNext(QString(FINGER_SPREAD_NEXT_PATH));
    QImage fistLater(QString(FIST_LATER_PATH));
    QImage fistNext(QString(FIST_NEXT_PATH));
    QImage doubleTapLater(QString(DOUBLE_TAP_LATER_PATH));
    QImage doubleTapNext(QString(DOUBLE_TAP_NEXT_PATH));
    QImage waveInLater(QString(WAVE_IN_LATER_PATH));
    QImage waveInNext(QString(WAVE_IN_NEXT_PATH));
    QImage waveOutLater(QString(WAVE_OUT_LATER_PATH));
    QImage waveOutNext(QString(WAVE_OUT_NEXT_PATH));
    QImage noHandLater(QString(NO_HAND_LATER_PATH));
    QImage noHandNext(QString(NO_HAND_NEXT_PATH));
    
    sequenceImageSet fingerSpread;
    fingerSpread.actionTag = Pose::fingersSpread;
    fingerSpread.nextImage = QPixmap::fromImage(fingerSpreadNext);
    fingerSpread.laterImage = QPixmap::fromImage(fingerSpreadLater);
    idToImageMap[fingerSpread.actionTag] = fingerSpread;

    sequenceImageSet fist;
    fist.actionTag = Pose::fist;
    fist.nextImage = QPixmap::fromImage(fistNext);
    fist.laterImage = QPixmap::fromImage(fistLater);
    idToImageMap[fist.actionTag] = fist;

    sequenceImageSet doubleTap;
    doubleTap.actionTag = Pose::doubleTap;
    doubleTap.nextImage = QPixmap::fromImage(doubleTapNext);
    doubleTap.laterImage = QPixmap::fromImage(doubleTapLater);
    idToImageMap[doubleTap.actionTag] = doubleTap;

    sequenceImageSet waveIn;
    waveIn.actionTag = Pose::waveIn;
    waveIn.nextImage = QPixmap::fromImage(waveInNext);
    waveIn.laterImage = QPixmap::fromImage(waveInLater);
    idToImageMap[waveIn.actionTag] = waveIn;

    sequenceImageSet waveOut;
    waveOut.actionTag = Pose::waveOut;
    waveOut.nextImage = QPixmap::fromImage(waveOutNext);
    waveOut.laterImage = QPixmap::fromImage(waveOutLater);
    idToImageMap[waveOut.actionTag] = waveOut;

    sequenceImageSet rest;
    rest.actionTag = Pose::rest;
    rest.nextImage = QPixmap::fromImage(noHandNext);
    rest.laterImage = QPixmap::fromImage(noHandLater);
    idToImageMap[rest.actionTag] = rest;

}
