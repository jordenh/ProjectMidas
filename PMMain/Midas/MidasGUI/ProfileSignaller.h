#ifndef PROFILESIGNALLER_H
#define PROFILESIGNALLER_H

#include <QObject>

class ControlState;

class ProfileSignaller : public QObject
{
    Q_OBJECT

public:
    ProfileSignaller(QObject *parent = 0);
    ~ProfileSignaller();

	void setControlStateHandle(ControlState* handle) { controlStateHandle = handle; }

public slots:
    void handleProfilePress(QString);

private:
	ControlState* controlStateHandle; // not owned
};

#endif // PROFILESIGNALLER_H
