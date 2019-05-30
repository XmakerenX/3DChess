#include <iostream>
#include <string>
#include "../GUIEditor.h"
#include "../../Windows/WindowsGameWin.h"
#include "../../Windows/WindowsConsole.h"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GUIEditor gamewin;
	WindowsGameWin* window = new WindowsGameWin();
    std::string fontName = "NotoMono";
    std::string path;

    CreateConsoleWindow();

    std::cout << "--------------------------------\n";
    std::cout << "starting init window\n";
    std::cout << "--------------------------------\n";

	window->setHINSTANCE(hInstance);
	
    //gamewin.initWindow();
    std::cout << "starting init OpenGL\n";
    if (!gamewin.initGame(window, 1024, 768))
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

