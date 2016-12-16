#include <iostream>
#include "GameWin.h"

int main(int argc, char* argv[])
{
    GameWin gamewin;

    std::cout << "starting init window\n";

    gamewin.initWindow();
    std::cout << "starting init OpenGL\n";
    gamewin.initOpenGL(800,600);

    std::cout << "Beginning game\n";
    int retCode = gamewin.BeginGame();

    if (!gamewin.Shutdown())
        return 1;

    return retCode;
}

