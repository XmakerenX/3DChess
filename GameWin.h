#ifndef  _GAMEWIN_H
#define  _GAMEWIN_H

#ifdef _WIN32
    #include "Windows/WindowsGameWin.h"
    typedef WindowsGameWin GameWin;
#else
    #include "Linux/LinuxX11Window.h"
    #include "Linux/LinuxWaylandWindow.h"
    typedef LinuxWaylandWindow GameWin;
    typedef LinuxX11Window GameWinX11;
    typedef LinuxWaylandWindow GameWinWayland;
#endif

#endif  //_GAMEWIN_H
