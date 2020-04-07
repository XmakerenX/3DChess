#include <iostream>
#include <string>
#include <cstdlib>
#include "Chess.h"
#include <GameWindow/Linux/LinuxX11Window.h>
#include <GameWindow/Linux/LinuxWaylandWindow.h>

int main(int argc, char* argv[])
{
    Chess gamewin;
    BaseWindow * window = nullptr;
    std::string fontName = "NotoMono";
    std::string path;
    

    char* waylandDisplay;
    waylandDisplay = std::getenv("WAYLAND_DISPLAY");
    if (waylandDisplay != nullptr)
    {
        std::cout << waylandDisplay << "\n";
        window = new LinuxWaylandWindow();
    }
    else
    {
        char* display = std::getenv("DISPLAY");
        if (display != nullptr)
        {
            window = new LinuxX11Window();
            std::cout << display << "\n";
        }
    }
    
    std::cout << "--------------------------------\n";
    std::cout << "starting init window\n";
    std::cout << "--------------------------------\n";

    std::cout << "starting init OpenGL\n";
    if (!gamewin.initGame(window, 1024, 768))
    {
        std::cout << "Error occured, Quiting..\n";
        return 1;
    }
    
    // give x11 a sec to respond to our init
    sleep(1);
    
    std::cout << "--------------------------------\n";
    std::cout << "Beginning game\n";
    int retCode = gamewin.BeginGame();

    if (!gamewin.Shutdown())
        return 1;

    return retCode;
}
