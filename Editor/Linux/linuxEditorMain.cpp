#include <iostream>
#include <string>
#include "../GUIEditorWin.h"

int main(int argc, char* argv[])
{
    GUIEditorWin gamewin;
    std::string fontName = "NotoMono";
    std::string path;

    std::cout << "--------------------------------\n";
    std::cout << "starting init window\n";
    std::cout << "--------------------------------\n";

    gamewin.initWindow();
    std::cout << "starting init OpenGL\n";
    gamewin.initOpenGL(1024,768, 0);
    std::cout << "--------------------------------\n";

    std::cout << "Beginning game\n";
    int retCode = gamewin.BeginGame();

    if (!gamewin.Shutdown())
        return 1;

    return retCode;
}

