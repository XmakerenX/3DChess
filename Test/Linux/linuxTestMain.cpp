#include <iostream>
#include <string>
#include "../TestingWin.h"

int main(int argc, char* argv[])
{
	TestingWin gamewin;
	std::string fontName = "NotoMono";
	std::string path;

	std::cout << "--------------------------------\n";
	std::cout << "starting init window\n";
	std::cout << "--------------------------------\n";

	gamewin.initWindow();
	std::cout << "starting init OpenGL\n";
	//gamewin.initOpenGL(800,600);
	gamewin.initOpenGL(1024, 768);
	std::cout << "--------------------------------\n";

	std::cout << "Beginning game\n";
	int retCode = gamewin.BeginGame();

	if (!gamewin.Shutdown())
		return 1;

	return retCode;
}
