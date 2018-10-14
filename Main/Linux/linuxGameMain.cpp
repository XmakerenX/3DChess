#include <iostream>
#include <string>
#include "ChessWindow.h"

int main(int argc, char* argv[])
{
    ChessWindow gamewin;
    std::string fontName = "NotoMono";
    std::string path;

    std::cout << "--------------------------------\n";
    std::cout << "starting init window\n";
    std::cout << "--------------------------------\n";

    gamewin.initWindow();
    std::cout << "starting init OpenGL\n";
    if (!gamewin.initOpenGL(1024,768))
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