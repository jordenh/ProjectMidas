#include "ProfileSignaller.h"
#include "ControlState.h"

ProfileSignaller::ProfileSignaller(QObject *parent)
    : QObject(parent)
{
}

ProfileSignaller::~ProfileSignaller()
{

}

void ProfileSignaller::handleProfilePress(QString name)
{
	controlStateHandle->setProfile(name.toStdString());
}
