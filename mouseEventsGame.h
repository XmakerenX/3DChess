#ifndef  _MOUSEEVENTSGAME_H
#define  _MOUSEEVENTSGAME_H

#include "RenderTypes.h"

enum class MouseEventType{Unknown = 0,
		      LeftButton,
		      DoubleLeftButton,
		      MiddleButton,
		      DoubleMiddleButton,
		      RightButton,
		      DoubleRightButton,
		      ScrollHoriz,
		      ScrollVert,
		      MouseMoved};

class MouseEvent
{
public:

    MouseEvent() : cursorPos(0,0)
    {
        type = MouseEventType::Unknown;
        down = false;
        timeStamp = 0;
        nLinesToScroll = 0;
    }

    MouseEvent(MouseEventType newType, Point newCursorPos, bool newDown, double newTimeStamp, int newLinesToScroll)
        :cursorPos(newCursorPos)
    {
        type = newType;
        down = newDown;
        timeStamp = newTimeStamp;
        nLinesToScroll = newLinesToScroll;
    }

    MouseEventType type;
    Point cursorPos;
    bool down;
    double timeStamp;
    int nLinesToScroll;

};

#endif  //_MOUSEEVENTSGAME_H
