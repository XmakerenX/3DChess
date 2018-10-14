#include<Windows.h>
#include <io.h>
#include <fcntl.h>
#undef max
#undef min
#include <iostream>
#include <string>
#include "../ChessWindow.h"
#include "../../WindowsConsole.h"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChessWindow gamewin;
	std::string fontName = "NotoMono";
	std::string path;

	CreateConsoleWindow();

	std::cout << "--------------------------------\n";
	std::cout << "starting init window\n";
	std::cout << "--------------------------------\n";

	gamewin.initWindow();
	std::cout << "starting init OpenGL\n";
	if (!gamewin.initOpenGL(1024, 768, hInstance))
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