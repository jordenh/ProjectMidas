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

#ifndef DISTANCE_WIDGET_H
#define DISTANCE_WIDGET_H

#include <qwidget.h>

class distanceBar;
class MidasThread;

class DistanceWidget : public QWidget
{
    Q_OBJECT
public:
    DistanceWidget(MidasThread *mainThread, int width, int height,
        QWidget *parent = 0);
    
public slots:
    /* iOS immediate < 0.5m, near = a little more, far = a few metres 
     * http://www.devfright.com/ibeacons-tutorial-ios-7-clbeaconregion-clbeacon/
     */
    void updateDistanceLabel(float db);

    void signalLoss(bool isConnected);

private:
    distanceBar    *bar;
    float           distance;
    int             dispWidth;
    int             dispHeight;

    /**
     * Calculate the distance from the give db value returned from myo rssi
     * request. The formula used is based off of measured values.
     *
     * @param db Float value corresponding to the db received from the myo
     * @return Float that represents an approximate distance.
     */
    inline float dbToDist(float db);

};

#endif /* DISTANCE_WIDGET_H */
