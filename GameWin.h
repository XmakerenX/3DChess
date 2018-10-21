#ifndef  _GAMEWIN_H
#define  _GAMEWIN_H

#ifdef _WIN32
    #include "Windows/WindowsGameWin.h"
    typedef WindowsGameWin GameWin;
#else
    #include "Linux/LinuxGameWin.h"
    typedef LinuxGameWin GameWin;
#endif

#endif  //_GAMEWIN_H
