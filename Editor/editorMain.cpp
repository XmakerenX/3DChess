#include <iostream>
#include <string>
#include <fontconfig/fontconfig.h>
//#include "GameWin.h"
//#include "TestWin.h"
#include "GUIEditorWin.h"

int main(int argc, char* argv[])
{
    //GameWin gamewin;
    //TestWin gamewin;
    GUIEditorWin gamewin;
    std::string fontName = "NotoMono";
    std::string path;

    std::cout << "--------------------------------\n";
    std::cout << "starting init window\n";
    std::cout << "--------------------------------\n";

    gamewin.initWindow();
    std::cout << "starting init OpenGL\n";
    //gamewin.initOpenGL(800,600);
    gamewin.initOpenGL(1024,768);
    std::cout << "--------------------------------\n";

    std::cout << "Beginning game\n";
    int retCode = gamewin.BeginGame();

    if (!gamewin.Shutdown())
        return 1;

    return retCode;
}

