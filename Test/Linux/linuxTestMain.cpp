#include <iostream>
#include <string>
#include "../../Linux/LinuxX11Window.h"
#include "../TestingGame.h"

int main(int argc, char* argv[])
{
    TestingGame gamewin;
    BaseWindow* window = new LinuxX11Window();
    std::string fontName = "NotoMono";
    std::string path;

    std::cout << "--------------------------------\n";
    std::cout << "starting init window\n";
    std::cout << "--------------------------------\n";

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
