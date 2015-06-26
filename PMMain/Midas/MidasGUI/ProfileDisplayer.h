#ifndef PROFILEDISPLAYER_H
#define PROFILEDISPLAYER_H

#include <QFrame.h>
#include "MidasCommon.h"

class QGridLayout;


class ProfileDisplayer : public QFrame
{
    Q_OBJECT

public:
    ProfileDisplayer(std::string name, int widgetWidth = PROF_INDICATOR_WIDTH, 
        int widgetHeight = PROF_INDICATOR_HEIGHT, QWidget *parent = 0);
    ~ProfileDisplayer();

    QSize sizeHint() const;

protected:
    /**
    * The event handler function that is called when the widget is clicked on by the mouse.
    *
    * @param event The mouse event structure, with information about the mouse press.
    */
    virtual void mousePressEvent(QMouseEvent *event);

    /**
    * The event handler function that is called when the widget is released by the mouse.
    *
    * @param event The mouse event structure, with information about the mouse release.
    */
    virtual void mouseReleaseEvent(QMouseEvent *event);

    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    int indWidth, indHeight;
    QGridLayout *layout;
    std::string profileName;

signals:
    void emitChangeProfile(QString);
};

#endif // PROFILEDISPLAYER_H
