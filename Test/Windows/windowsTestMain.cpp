#include <iostream>
#include <string>
#include "../TestingWin.h"
#include "../../WindowsConsole.h"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	TestingWin gamewin;
	std::string fontName = "NotoMono";
	std::string path;

	CreateConsoleWindow();

	std::cout << "--------------------------------\n";
	std::cout << "starting init window\n";
	std::cout << "--------------------------------\n";

	gamewin.setHINSTANCE(hInstance);
	gamewin.initWindow();
	std::cout << "starting init OpenGL\n";
	gamewin.initOpenGL(1024, 768);
	std::cout << "--------------------------------\n";

	std::cout << "Beginning game\n";
	int retCode = gamewin.BeginGame();

	if (!gamewin.Shutdown())
		return 1;

	return retCode;
}
