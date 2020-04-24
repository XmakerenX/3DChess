//
// 3D Chess - A cross platform Chess game for 2 players made using OpenGL and c++
// Copyright (C) 2016-2020 Matan Keren <xmakerenx@gmail.com>
//
// This file is part of 3D Chess.
//
// 3D Chess is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// 3D Chess is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with 3D Chess.  If not, see <http://www.gnu.org/licenses/>.
//

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#undef max
#undef min
#include <iostream>
#include <string>
#include "Chess.h"
#include <GameWindow/Windows/WindowsGameWin.h>
#include <GameWindow/Windows/WindowsConsole.h>

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Chess gamewin;
	WindowsGameWin* window = new WindowsGameWin();
	
    std::string fontName = "NotoMono";
    std::string path;

    CreateConsoleWindow();

    SetProcessDPIAware();

    std::cout << "--------------------------------\n";
    std::cout << "starting init window\n";
    std::cout << "--------------------------------\n";

	window->setHINSTANCE(hInstance);
    std::cout << "starting init OpenGL\n";
    if (!gamewin.initGame(window,1024, 768))
    {
        std::cout << "Error occured, Quiting..\n";
        return 1;
    }
    
    std::cout << "--------------------------------\n";
    std::cout << "Beginning game\n";
    int retCode = gamewin.BeginGame();

    if (!gamewin.Shutdown())
        return 1;

    return retCode;
}
