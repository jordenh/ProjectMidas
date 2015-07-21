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

#include "SCDDigester.h"

#include "ConnectionSignaller.h"
#include "SettingsSignaller.h"
#include "BaseMeasurements.h"
#include "CommandData.h"
#include "KeyboardContoller.h"
#include "SharedCommandData.h"
#include "MidasThread.h"
#include "ControlState.h"
#include "MyoState.h"
#include "MouseCtrl.h"
#include "ProfileManager.h"
#include "MainGUI.h"

SettingsSignaller SCDDigester::settingsSignaller;

SCDDigester::SCDDigester(SharedCommandData* scd, MidasThread *thread, ControlState *cntrlStateHandle, MyoState* myoStateHandle,
    MouseCtrl *mouseCtrl, KeyboardController *keyboardController, ProfileManager* profileManagerHandle,  MainGUI *mainGui, std::vector<ringData> *kybrdRingData)
{
    this->scdHandle = scd;
    this->threadHandle = thread;
    this->cntrlStateHandle = cntrlStateHandle;
	this->myoStateHandle = myoStateHandle;
    this->mouseCtrl = mouseCtrl;
	this->keyboardController = keyboardController;
    this->mainGUI = mainGui;

	this->pm = profileManagerHandle;
    this->count = 0;

	this->kybrdRingData = kybrdRingData;
    connSignaller = new ConnectionSignaller();
    connSignaller->setCurrentlyConnected(false);
    connSignaller->setCurrentlySynched(false);
    mainGUI->connectSignallerToPoseDisplayer(connSignaller);
    mainGUI->connectSignallerToSequenceDisplayer(connSignaller);

    mainGUI->connectSignallerToSettingsDisplayer(&settingsSignaller);
}


SCDDigester::~SCDDigester()
{
    delete connSignaller; connSignaller = NULL;
}

void SCDDigester::digest()
{
    CommandData nextCmd;
    static bool isConnected = true;
    static bool testConnected = true;

    bool consumed = scdHandle->consumeCommand(nextCmd);

    if (consumed)
    {
        switch (nextCmd.type)
        {
        case KYBRD_CMD:
            digestKybdCmd(nextCmd);
            break;
        case KYBRD_GUI_CMD:
            break;
        case MOUSE_CMD:
            mouseCtrl->sendCommand(nextCmd.action.mouse);
            break;
        case STATE_CHANGE:
            break;
        case PROFILE_CHANGE:
            digestProfileChange(nextCmd);
            break;
        case NONE:
            break;
        case UNKNOWN_COMMAND:
            break;
        default:
            break;
        }
    }

	vector2D mouseDelta = scdHandle->getDelta();
	if (cntrlStateHandle->getMode() == midasMode::MOUSE_MODE ||
        cntrlStateHandle->getMode() == midasMode::MOUSE_MODE2)
	{
        bool muscleImpulse = scdHandle->getImpulseStatus();
        if (settingsSignaller.getUseEMGImpulse() && muscleImpulse)
        {
            BaseMeasurements::getInstance().updateBaseCursor();
            BaseMeasurements::getInstance().setCurrentAnglesAsBase();
        }
        else 
        {
            mouseCtrl->sendCommand(mouseCmds::MOVE_ABSOLUTE, mouseDelta.x, -mouseDelta.y);
        }
	}

    handleConnectionData();

#ifdef JOYSTICK_CURSOR
    point unitVelocity = scdHandle->getVelocity();
	if (unitVelocity.x != 0)
	{
	    mouseCtrl->sendCommand(mouseCmds::MOVE_HOR, unitVelocity.x);
	}
	if (unitVelocity.y != 0)
	{
	    mouseCtrl->sendCommand(mouseCmds::MOVE_VERT, unitVelocity.y);
	}

    if (count % 1000 == 0)
    {
        threadHandle->emitVeloc(unitVelocity.x, unitVelocity.y);

        /* Only update GUI if the connection status changed*/
        testConnected = scdHandle->getIsConnected();
        if (isConnected != testConnected)
        {
            if (!testConnected)
            {
                threadHandle->emitDisconnect(testConnected);
            }
        }
        isConnected = testConnected;

        /* Only update RSSI if device connected */
        if (isConnected)
        {
            float rssi = scdHandle->getRssi();
            threadHandle->emitRssi(rssi);
        }
    }
#endif /* JOYSTICK_CURSOR */
    if (cntrlStateHandle->getMode() == midasMode::KEYBOARD_MODE)
    {
        unsigned int kybdGUISel = scdHandle->getKybdGuiSel();
        keyboardAngle currKeyAngle = scdHandle->getKeySelectAngle();
		
        double angleAsDouble = (double)currKeyAngle.angle;
        threadHandle->emitUpdateKeyboard(kybdGUISel, angleAsDouble, currKeyAngle.ringThreshReached, false); // todo kybrd fix test.
		
        // for debug only
        // int x = currKeyAngle.x;
        // int y = currKeyAngle.y;
        // threadHandle->emitDebugInfo(x, y);
		
        digestKeyboardGUIData(nextCmd);
    }

    count++;
}

// MAKE SURE THIS FUNCTION MATCHES THE SAME FUNCTION IN SCDDigester.
int SCDDigester::getSelectedKeyFromAngle(double angle, std::vector<ringData::keyboardValue> *ring)
{
	qreal deltaAngle = 360.0 / ring->size();
	int adjustedAngle = (int)(angle + deltaAngle / 2) % 360;
	// TODO: May have to change later, based on received angle
	return (int)(adjustedAngle / deltaAngle);
}

void SCDDigester::digestKeyboardGUIData(CommandData nextCommand)
{
    keyboardAngle currAngle;
    int ringKeySelIdx;
    char key;
    if (nextCommand.type == KYBRD_GUI_CMD)
    {
        unsigned int kybdGUISel = scdHandle->getKybdGuiSel();
        KeyboardVector kiVec;

        // handle special commands for keyboard gui updating.
        switch (nextCommand.action.kybdGUI)
        {
        case kybdGUICmds::SWAP_RING_FOCUS:
            // Swap which ring is focussed on (out/in) 
            // based on RingData structure: adding 1 will go from outer to inner ring and sub 1 will go from inner to outer
            if (kybdGUISel % 2 == 0)
            {
                kybdGUISel += 1;
            }
            else
            {
                kybdGUISel -= 1;
            }
            scdHandle->setKybdGuiSel(kybdGUISel);
            break;

        case kybdGUICmds::CHANGE_WHEELS:
            // go to next wheel
            kybdGUISel += 2;
            kybdGUISel %= (scdHandle->getKybdGuiSelMax());
            scdHandle->setKybdGuiSel(kybdGUISel);
            break;

        case kybdGUICmds::SELECT:
            currAngle = scdHandle->getKeySelectAngle();

            if (currAngle.ringThreshReached)
            {
                if (kybdGUISel % 2 == 0)
                {
                    ringKeySelIdx = getSelectedKeyFromAngle(currAngle.angle, (*kybrdRingData)[kybdGUISel / 2].getRingOutVectorHandle());
                    key = (*kybrdRingData)[kybdGUISel / 2].getRingOutVectorHandle()->at(ringKeySelIdx).main;
                }
                else
                {
                    ringKeySelIdx = getSelectedKeyFromAngle(currAngle.angle, (*kybrdRingData)[kybdGUISel / 2].getRingInVectorHandle());
                    key = (*kybrdRingData)[kybdGUISel / 2].getRingInVectorHandle()->at(ringKeySelIdx).main;
                }
            }
            else
            {
                key = CENTER_MAIN_KEY;
            }

            kiVec.inputCharDownUp(key);
            keyboardController->setKiVector(kiVec);
            keyboardController->sendData();

            //threadHandle->animateSelection(); // TODO
            
            break;
        case kybdGUICmds::HOLD_SELECT:
            currAngle = scdHandle->getKeySelectAngle();

            if (currAngle.ringThreshReached)
            {
                if (kybdGUISel % 2 == 0)
                {
                    ringKeySelIdx = getSelectedKeyFromAngle(currAngle.angle, (*kybrdRingData)[kybdGUISel / 2].getRingOutVectorHandle());
                    key = (*kybrdRingData)[kybdGUISel / 2].getRingOutVectorHandle()->at(ringKeySelIdx).hold;
                }
                else
                {
                    ringKeySelIdx = getSelectedKeyFromAngle(currAngle.angle, (*kybrdRingData)[kybdGUISel / 2].getRingInVectorHandle());
                    key = (*kybrdRingData)[kybdGUISel / 2].getRingInVectorHandle()->at(ringKeySelIdx).hold;
                }
            }
            else
            {
                key = CENTER_HOLD_KEY;
            }

            kiVec.inputCharDownUp(key);
            keyboardController->setKiVector(kiVec);
            keyboardController->sendData();

            //threadHandle->animateSelection(); // TODO

            break;
        default:
            break;
        }
    }
}

void SCDDigester::digestKybdCmd(CommandData nextCommand)
{
	if (nextCommand.action.kybd == kybdCmds::INPUT_VECTOR)
	{
        // handle INPUT_VECTOR seperately as KiVector needs to be populated
        // with each character from the vector.
		keyboardController->setKiVector(nextCommand.keyboardVector);
		keyboardController->sendData();
	}
	else
	{
		KeyboardVector vec = KeyboardVector::createFromCommand(nextCommand.action.kybd);
		keyboardController->setKiVector(vec);
		keyboardController->sendDataDelayed(2); // TODO - modify to try a lower value to see if lag can be reduced. Or change how 'sending delayed' works.
	}
}

void SCDDigester::digestProfileChange(CommandData nextCmd)
{
	std::vector<profile>* profiles = pm->getProfiles();
	if (profiles->size() == 0) { return; }

	std::string currProfileName = cntrlStateHandle->getProfile();
	std::string prevProfileName = "";
	std::string nextProfileName = "";
	for (int i = 0; i < profiles->size(); i++)
	{
		if (profiles->at(i).profileName == currProfileName)
		{
			if (i > 0)
			{
				prevProfileName = profiles->at(i - 1).profileName;
			}
			else
			{
				prevProfileName = profiles->at(profiles->size() - 1).profileName;
			}

			if (i < profiles->size() - 1)
			{
				nextProfileName = profiles->at(i + 1).profileName;
			}
			else
			{
				nextProfileName = profiles->at(0).profileName;
			}
			break;
		}
	}

	if (nextCmd.action.profile == MOVE_PROFILE_FORWARD)
	{
		cntrlStateHandle->setProfile(nextProfileName);
	}
	else if (nextCmd.action.profile == MOVE_PROFILE_BACKWARD)
	{
		cntrlStateHandle->setProfile(prevProfileName);
	}
}

void SCDDigester::handleConnectionData()
{
    // signall connection/sync
    if (scdHandle->getIsConnected() != connSignaller->getCurrentlyConnected())
    {
        if (scdHandle->getIsConnected())
        {
            connSignaller->emitConnect();
            connSignaller->setCurrentlyConnected(true);
        }
        else
        {
            connSignaller->emitDisconnect();
            connSignaller->setCurrentlyConnected(false);
        }
    }
    if (scdHandle->getIsSynched() != connSignaller->getCurrentlySynched())
    {
        if (scdHandle->getIsSynched())
        {
            connSignaller->emitSync();
            connSignaller->setCurrentlySynched(true);
        }
        else
        {
            connSignaller->emitUnsync();
            connSignaller->setCurrentlySynched(false);
        }
    }

    if (myoStateHandle->getArm() == Arm::armRight && !connSignaller->getIsRightHand())
    {
        connSignaller->emitIsRightHand(true);
        connSignaller->setIsRightHand(true);
    }
    if (myoStateHandle->getArm() == Arm::armLeft && connSignaller->getIsRightHand())
    {
        connSignaller->emitIsRightHand(false);
        connSignaller->setIsRightHand(false);
    }
}
