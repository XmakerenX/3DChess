#ifndef  _PCH_H
#define  _PCH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <functional>
#include <future>

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL // needed for gtx extensions
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include<png.h>
#include<jpeglib.h>

#ifdef FBX
    #include <fbxsdk.h>
#endif

#ifdef _WIN32
    #include <windows.h>
    #undef max
    #undef min
#else
    #include <sys/time.h>
    
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/extensions/Xrandr.h>
    #include <GL/glx.h>
    
    #include <wayland-client.h>
    #include <wayland-client-protocol.h>
    #include <wayland-egl.h>
    #include <wayland-cursor.h>
    #include <wayland-server.h>
    #include <xdg-shell-unstable-v6-protocol.h>
    #include <xkbcommon/xkbcommon.h>
    
    #include <EGL/egl.h>
    #include <EGL/eglext.h>
#endif

#include <cinttypes>
#include <cassert>
#include <cstring>

#include <boost/signals2/signal.hpp>
#include <boost/signals2.hpp>
#include <boost/bind/bind.hpp>

#include<cstdio>

#endif  //_PCH_H
