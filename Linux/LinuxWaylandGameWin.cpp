#include "LinuxWaylandGameWin.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <GL/glut.h>
#include <unistd.h>
#include <linux/input-event-codes.h>
#include <X11/Xatom.h>
#include "../input.h"
#include "virtualKeysLinux.h"
#include <sys/mman.h>
#include <algorithm>
#include <fcntl.h>    
#include <unistd.h>
#include <poll.h>

const double LinuxWaylandGameWin::s_doubleClickTime = 0.5;
const wl_registry_listener LinuxWaylandGameWin::registry_listener = 
{
    reinterpret_cast<void (*)(void*, wl_registry*, uint32_t, const char*, uint32_t)>(&LinuxWaylandGameWin::registry_handle_global),
    reinterpret_cast<void (*)(void*, wl_registry*, uint32_t)>(&LinuxWaylandGameWin::registry_handle_global_remove)
};
const zxdg_surface_v6_listener LinuxWaylandGameWin::xdg_surface_listener = 
{
    reinterpret_cast<void (*)(void*, zxdg_surface_v6*, uint32_t)>(&LinuxWaylandGameWin::handle_surface_configure)
};
       
const zxdg_toplevel_v6_listener LinuxWaylandGameWin::xdg_toplevel_listener = 
{
    reinterpret_cast<void (*)(void*, zxdg_toplevel_v6*, int32_t, int32_t, wl_array*)>(&LinuxWaylandGameWin::handle_toplevel_configure),
    reinterpret_cast<void (*)(void*, zxdg_toplevel_v6*)>(&LinuxWaylandGameWin::handle_toplevel_close)
};
    
const wl_pointer_listener LinuxWaylandGameWin::pointer_listener = 
{
    reinterpret_cast<void (*)(void*, wl_pointer*, uint32_t, wl_surface*, wl_fixed_t, wl_fixed_t)>(&LinuxWaylandGameWin::pointer_handle_enter),
    reinterpret_cast<void (*)(void*, wl_pointer*, uint32_t, wl_surface*)>(&LinuxWaylandGameWin::pointer_handle_leave),
    reinterpret_cast<void (*)(void*, wl_pointer*, uint32_t, wl_fixed_t, wl_fixed_t)>(&LinuxWaylandGameWin::pointer_handle_motion),
    reinterpret_cast<void (*)(void*, wl_pointer*, uint32_t, uint32_t, uint32_t, uint32_t)>(&LinuxWaylandGameWin::pointer_handle_button),
    reinterpret_cast<void (*)(void*, wl_pointer*, uint32_t, uint32_t, wl_fixed_t)>(&LinuxWaylandGameWin::pointer_handle_axis)
};
const wl_keyboard_listener LinuxWaylandGameWin::keyboard_listener = 
{
    reinterpret_cast<void (*)(void*, wl_keyboard*, uint32_t, int ,uint32_t)>(&LinuxWaylandGameWin::keyboard_handle_keymap),
    reinterpret_cast<void (*)(void*, wl_keyboard*, uint32_t, wl_surface*, wl_array*)>(&LinuxWaylandGameWin::keyboard_handle_enter),
    reinterpret_cast<void (*)(void*, wl_keyboard*, uint32_t, wl_surface*)>(&LinuxWaylandGameWin::keyboard_handle_leave),
    reinterpret_cast<void (*)(void*, wl_keyboard*, uint32_t, uint32_t, uint32_t, uint32_t)>(&LinuxWaylandGameWin::keyboard_handle_key),
    reinterpret_cast<void (*)(void*, wl_keyboard*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)>(&LinuxWaylandGameWin::keyboard_handle_modifiers),
    reinterpret_cast<void (*)(void*, wl_keyboard*, int32_t, int32_t)>(&LinuxWaylandGameWin::keyboard_handle_repeat_info)
};
const zxdg_shell_v6_listener LinuxWaylandGameWin::xdg_shell_listener = 
{
    reinterpret_cast<void (*)(void*, zxdg_shell_v6*, uint32_t)>(&LinuxWaylandGameWin::xdg_shell_ping)
};
const wl_seat_listener LinuxWaylandGameWin::seat_listener = 
{
    reinterpret_cast<void (*)(void*, wl_seat*, unsigned int)>(&LinuxWaylandGameWin::seat_handle_capabilities),
    reinterpret_cast<void (*)(void*, wl_seat*, const char *)>(&LinuxWaylandGameWin::seat_handle_name)
};

const wl_data_source_listener LinuxWaylandGameWin::data_source_listener = {
    data_source_target,
    data_source_send,
    data_source_cancelled
};

const wl_data_offer_listener LinuxWaylandGameWin::data_offer_listener = {
    data_offer_offer,
    data_offer_source_actions,
    data_offer_action
};


const wl_data_device_listener LinuxWaylandGameWin::data_device_listener = {
    data_device_data_offer,
    data_device_enter,
    data_device_leave,
    data_device_motion,
    data_device_drop,
    data_device_selection
};

uint32_t LinuxWaylandGameWin::s_sourceActions = 0;
wl_data_offer * LinuxWaylandGameWin::selectionOffer = nullptr;

wl_data_device * LinuxWaylandGameWin::s_dataDevice = nullptr;
wl_data_device_manager * LinuxWaylandGameWin::m_dataDeviceMan = nullptr;
wl_data_source * LinuxWaylandGameWin::s_dataSource = nullptr;

std::string LinuxWaylandGameWin::s_clipboardString;
std::vector<wl_data_offer*> LinuxWaylandGameWin::s_offers;
uint32_t LinuxWaylandGameWin::s_serial = 0;
std::vector<std::string> LinuxWaylandGameWin::s_offerMimeTypes;

wl_display * LinuxWaylandGameWin::s_display = nullptr;
// double calculateXRefreshRate(const XRRModeInfo *info)
// {
//     return (info->hTotal && info->vTotal) ?
//         round(((double)info->dotClock / (double)(info->hTotal * info->vTotal))) : 0;
// }

//-----------------------------------------------------------------------------
// Name : LinuxGameWin (constructor)
//-----------------------------------------------------------------------------
LinuxWaylandGameWin::LinuxWaylandGameWin()
    :m_modifiersStates(false, false, false)
{
    m_display = nullptr;
    
    bool m_fullscreen = false;
    bool m_maximized = false;
    
    m_xdg_toplevel = nullptr;
    m_registry = nullptr;
    m_compositor = nullptr;
    m_shell = nullptr;
    m_xdg_surface = nullptr;
    m_seat = nullptr;
    m_shm = nullptr;
    m_cursor_theme = nullptr;
    m_default_cursor = nullptr;
    m_pointer = nullptr;
    m_keyboard = nullptr;
    
    m_cursor_surface = nullptr;
    
    m_swapbuffers = nullptr;
    
    m_eglDpy = nullptr;
    m_eglCtx = nullptr;
    m_eglConf = nullptr;
    
    m_surface = nullptr;
    m_native = nullptr;
    m_egl_surface = nullptr;
    
    wait_for_configure = false;
    
    m_xkbContext = nullptr;
    m_xkbkeymap = nullptr;
    m_state = nullptr;
    
    m_lastCursorLocation = Point(0, 0);
    
    m_alaramToRemove = false;
    
    m_dataDeviceManager = nullptr;
}

//-----------------------------------------------------------------------------
// Name : LinuxGameWin (destructor)
//-----------------------------------------------------------------------------
LinuxWaylandGameWin::~LinuxWaylandGameWin()
{
}

//-----------------------------------------------------------------------------
// Name : platformInit ()
//-----------------------------------------------------------------------------
bool LinuxWaylandGameWin::platformInit(int width, int height)
{
    if (!initDisplay())
        return false;
    
    if (!createOpenGLContext())
        return false;
    
    if (!createWindow(width, height))
        return false;
        
    return true;
}

//-----------------------------------------------------------------------------
// Name : initDisplay ()
//-----------------------------------------------------------------------------
bool LinuxWaylandGameWin::initDisplay()
{    
    m_display = wl_display_connect(NULL);
    s_display = m_display;
    if (!m_display)
        return false;
    
    m_registry = wl_display_get_registry(m_display);
        
    wl_registry_add_listener(m_registry, &registry_listener, this);
    
    m_xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (!m_xkbContext)
    {
        std::cout << "Failed to create xkbContext\n";
        return false;
    }
    
    int ret = wl_display_roundtrip(m_display);
    std::cout << ret << "\n";
    
    return true;
}

//-----------------------------------------------------------------------------
// Name : create_xdg_surface ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::create_xdg_surface()
{    
    m_xdg_surface = zxdg_shell_v6_get_xdg_surface(m_shell, m_surface);
    zxdg_surface_v6_add_listener(m_xdg_surface, &xdg_surface_listener, this);
        
    m_xdg_toplevel = zxdg_surface_v6_get_toplevel(m_xdg_surface);
    zxdg_toplevel_v6_add_listener(m_xdg_toplevel, &xdg_toplevel_listener, this);
    
    zxdg_toplevel_v6_set_title(m_xdg_toplevel, "simple-egl");
    wait_for_configure = true;
    wl_surface_commit(m_surface);
}

//-----------------------------------------------------------------------------
// Name : getMonitorsInfo ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::getMonitorsInfo()
{
}

//-----------------------------------------------------------------------------
// Name : getMonitorsModes ()
//-----------------------------------------------------------------------------
std::vector<std::vector<Mode1>> LinuxWaylandGameWin::getMonitorsModes() const
{
    std::vector<std::vector<Mode1>> monitorsModes;
//     for (MonitorInfo monitor : m_monitors)
//     {
//         std::vector<Mode1> monitorModes;
//         for (MonitorInfo::Mode mode : monitor.modes)
//             monitorModes.emplace_back(mode.width, mode.height);
//                 
//         monitorsModes.push_back(std::move(monitorModes));
//     }
        
    return monitorsModes;
}

//-----------------------------------------------------------------------------
// Name : createWindow ()
//-----------------------------------------------------------------------------
bool LinuxWaylandGameWin::createWindow(int width, int height)
{
    m_surface = wl_compositor_create_surface(m_compositor);
    m_native = wl_egl_window_create(m_surface, width, height);
    
    eglCreatePlatformWindowSurfaceEXTProc eglCreatePlatformWindowSurface = nullptr;
    eglCreatePlatformWindowSurface = (eglCreatePlatformWindowSurfaceEXTProc)eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");
    if (!eglCreatePlatformWindowSurface)
        return false;
    
    m_egl_surface = eglCreatePlatformWindowSurface(m_eglDpy, m_eglConf, m_native, nullptr);
    
    if (m_shell)
        create_xdg_surface();
    else
        return false;
        
    EGLBoolean ret = eglMakeCurrent(m_eglDpy, m_egl_surface, m_egl_surface, m_eglCtx);
    
    if (ret != EGL_TRUE)
        return false;
        
    // VSync , 0 off , 1 on
    eglSwapInterval(m_eglDpy, 1);
        
//     if (m_shell && m_fullscreen)
//         zxdg_toplevel_v6_set_fullscreen(m_xdg_toplevel, nullptr);
        
    m_cursor_surface = wl_compositor_create_surface(m_compositor);
    
    std::cout << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << "\n";
    
    return true;
}

//-----------------------------------------------------------------------------
// Name : createOpenGLContext ()
//-----------------------------------------------------------------------------
bool LinuxWaylandGameWin::createOpenGLContext()
{   
    const EGLint context_attribs[] = 
    {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 3,
        EGL_NONE
    };
    
    EGLint config_attribs[] = 
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };
    
    eglGetPlatformDisplayEXTProc eglGetPlatformDisplayEXT = nullptr;
    const char * extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    
    if (!extensions)
        return false;
    
    if (!checkEglExtension(extensions, "EGL_EXT_platform_wayland") && 
        !checkEglExtension(extensions, "EGL_KHR_platform_wayland"))
        return false;
        
    eglGetPlatformDisplayEXT = (eglGetPlatformDisplayEXTProc)eglGetProcAddress("eglGetPlatformDisplayEXT");
    
    if (!eglGetPlatformDisplayEXT)
        return false;

    m_eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_WAYLAND_KHR, m_display, nullptr);
    
    EGLint major = 3, minor = 3;
    int count, n;
    EGLBoolean ret = eglInitialize(m_eglDpy, &major, &minor);
    if (ret != EGL_TRUE)
        return false;
    
    ret = eglBindAPI(EGL_OPENGL_API);
    if (ret != EGL_TRUE)
        return false;
    
    if (!eglGetConfigs(m_eglDpy, nullptr, 9, &count ) || count < 1)
        return false;
    
    EGLConfig *configs = new EGLConfig[count];
    ret = eglChooseConfig(m_eglDpy, config_attribs, configs, count, &n);
    
    if (ret != EGL_TRUE || n < 1)
        return false;
    
    for (int i = 0; i < n; i++)
    {
        int size;
        int samples, samplesBuffers;
        eglGetConfigAttrib(m_eglDpy, configs[i], EGL_BUFFER_SIZE, &size);
        eglGetConfigAttrib(m_eglDpy, configs[i], EGL_SAMPLES, &samples);
        eglGetConfigAttrib(m_eglDpy, configs[i], EGL_SAMPLES, &samplesBuffers);
                
        if (32 == size && samples == 4)
        {
            m_eglConf = configs[i];
            break;
        }
    }
    delete[] configs;
    
    if (m_eglConf == nullptr)
    {
        std::cout << "didn't find config with buffer size 32\n";
        return false;
    }
    
    m_eglCtx = eglCreateContext(m_eglDpy, m_eglConf, EGL_NO_CONTEXT, context_attribs);
    if (!m_eglCtx)
        return false;
        
    return true;
}

//-----------------------------------------------------------------------------
// Name : createEmptyCursorPixmap ()
//-----------------------------------------------------------------------------
bool LinuxWaylandGameWin::createEmptyCursorPixmap()
{    
    return true;
}

//-----------------------------------------------------------------------------
// Name : glSwapBuffers ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::glSwapBuffers()
{
    glColorMask(FALSE, FALSE, FALSE, TRUE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT); 
    glColorMask(TRUE, TRUE, TRUE, TRUE);
    
    eglSwapBuffers(m_eglDpy, m_egl_surface);
}

//-----------------------------------------------------------------------------
// Name : setFullScreenMode ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::setFullScreenMode(bool fullscreen)
{

}

//-----------------------------------------------------------------------------
// Name : setMonitorResolution ()
//-----------------------------------------------------------------------------
bool LinuxWaylandGameWin::setMonitorResolution(int monitorIndex, Resolution newResolution)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : setWindowPosition ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::setWindowPosition(int x, int y)
{

}

//-----------------------------------------------------------------------------
// Name : moveWindowToMonitor ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::moveWindowToMonitor(int monitorIndex)
{

}
    
//-----------------------------------------------------------------------------
// Name : isExtensionSupported ()
//-----------------------------------------------------------------------------
bool LinuxWaylandGameWin::isExtensionSupported(const char *extList, const char *extension)
{
    return false;
}

//-----------------------------------------------------------------------------
// Name : checkEglExtension ()
//-----------------------------------------------------------------------------
bool LinuxWaylandGameWin::checkEglExtension(const char *extensions, const char *extension)
{
    size_t extlen = strlen(extension);
    const char *end = extensions + strlen(extensions);

    while (extensions < end) 
    {
        size_t n = 0;

        /* Skip whitespaces, if any */
        if (*extensions == ' ') 
        {
            extensions++;
            continue;
        }

        n = strcspn(extensions, " ");

        /* Compare strings */
        if (n == extlen && strncmp(extension, extensions, n) == 0)
            return true; /* Found */

            extensions += n;
    }

    /* Not found */
    return false;
}

//-----------------------------------------------------------------------------
// Name : xdg_shell_ping ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::xdg_shell_ping(zxdg_shell_v6 *shell, uint32_t serial)
{
    zxdg_shell_v6_pong(shell, serial);
}

//-----------------------------------------------------------------------------
// Name : pointer_handle_enter ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::pointer_handle_enter(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy)
{    
    m_lastCursorLocation = Point(wl_fixed_to_int(sx), wl_fixed_to_int(sy));
    
    wl_buffer * buffer;
    wl_cursor_image * image;
        
    image = m_default_cursor->images[0];
    buffer = wl_cursor_image_get_buffer(image);
    if (!buffer)
        return;
        
    wl_pointer_set_cursor(pointer,
                          serial,
                          m_cursor_surface,
                          image->hotspot_x,
                          image->hotspot_y);
    wl_surface_attach(m_cursor_surface, buffer, 0, 0);
    wl_surface_damage(m_cursor_surface, 0, 0, image->width, image->height);
    wl_surface_commit(m_cursor_surface);
    
//     if (m_fullscreen)
//         wl_pointer_set_cursor(pointer, serial, nullptr, 0,0 );
//     else if (m_default_cursor)
    
    s_serial = serial;
}

//-----------------------------------------------------------------------------
// Name : pointer_handle_leave ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::pointer_handle_leave(wl_pointer *pointer, uint32_t serial, struct wl_surface *surface)
{
    s_serial = serial;
}

//-----------------------------------------------------------------------------
// Name : pointer_handle_motion ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::pointer_handle_motion(wl_pointer *pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    m_lastCursorLocation = Point(wl_fixed_to_int(sx), wl_fixed_to_int(sy));
    sendMouseEvent(MouseEvent(MouseEventType::MouseMoved, m_lastCursorLocation, false, m_timer.getCurrentTime(), 0), m_modifiersStates);
}

//-----------------------------------------------------------------------------
// Name : pointer_handle_button ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::pointer_handle_button(wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{    
    if (!m_xdg_toplevel)
        return;
        
    if (state == WL_POINTER_BUTTON_STATE_PRESSED)
    {
        if (button == BTN_LEFT)
        {
            std::cout << "left button pressed\n";
            m_oldCursorLoc = m_lastCursorLocation;
            //m_mouseDrag = true;
            
            double curTime = m_timer.getCurrentTime();
            if (curTime - lastLeftClickTime < s_doubleClickTime)
            {
                std::cout << "left button was double clicked\n";
                sendMouseEvent(MouseEvent(MouseEventType::DoubleLeftButton, m_lastCursorLocation, true, m_timer.getCurrentTime(), 0), m_modifiersStates);
            }
            else
                sendMouseEvent(MouseEvent(MouseEventType::LeftButton, m_lastCursorLocation, true, m_timer.getCurrentTime(), 0), m_modifiersStates);
            lastLeftClickTime = curTime;
        }
        
        if (button == BTN_RIGHT)
        {
            std::cout << "right button pressed\n";

            double curTime = m_timer.getCurrentTime();
            if (curTime - lastRightClickTime < s_doubleClickTime)
            {
                std::cout << "right button was double clicked\n";
                sendMouseEvent(MouseEvent(MouseEventType::DoubleRightButton, m_lastCursorLocation, true, m_timer.getCurrentTime(), 0), m_modifiersStates);
            }
            else
                sendMouseEvent(MouseEvent(MouseEventType::RightButton, m_lastCursorLocation,true, m_timer.getCurrentTime(), 0), m_modifiersStates);
            lastRightClickTime = curTime;
        }
    }
    else
    {
        if (button == BTN_LEFT)
        {
            std::cout << "left button released\n";
            m_mouseDrag = false;
            sendMouseEvent(MouseEvent(MouseEventType::LeftButton, m_lastCursorLocation, false, m_timer.getCurrentTime(), 0), m_modifiersStates);
        }
        
        if (button == BTN_RIGHT)
        {
            std::cout << "right button released\n";
                    sendMouseEvent(MouseEvent(MouseEventType::RightButton, m_lastCursorLocation, false, m_timer.getCurrentTime(), 0), m_modifiersStates);
        }
    }
    
    s_serial = serial;
    //zxdg_toplevel_v6_move(m_xdg_toplevel, m_seat, serial);
}

//-----------------------------------------------------------------------------
// Name : pointer_handle_axis ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::pointer_handle_axis(wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
}

//-----------------------------------------------------------------------------
// Name : keyboard_handle_keymap ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::keyboard_handle_keymap(wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size)
{
    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) 
    {
        close(fd);
        return;
    }

    char * map_str = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (map_str == MAP_FAILED) 
    {
        close(fd);
        return;
    }

    m_xkbkeymap = xkb_keymap_new_from_string(m_xkbContext, 
                                             map_str,
                                             XKB_KEYMAP_FORMAT_TEXT_V1,
                                             XKB_KEYMAP_COMPILE_NO_FLAGS);
    munmap(map_str, size);
    close(fd);
    
    if (!m_xkbkeymap) 
    {
        std::cout << "Failed to compile keymap\n";
        return;
    }
    
    m_state = xkb_state_new(m_xkbkeymap);
    if (!m_state) 
    {
        std::cout << "Failed to create xkb state\n";
        return;
    }
    
    m_controlMask = 1 << xkb_keymap_mod_get_index(m_xkbkeymap, "Control");
    m_shiftMask = 1 << xkb_keymap_mod_get_index(m_xkbkeymap, "Shift");
    m_altMask = 1 << xkb_keymap_mod_get_index(m_xkbkeymap, "Alt");
}

//-----------------------------------------------------------------------------
// Name : keyboard_handle_enter ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::keyboard_handle_enter(wl_keyboard *keyboard, uint32_t serial, wl_surface *surface, struct wl_array *keys)
{
    s_serial = serial;
}

//-----------------------------------------------------------------------------
// Name : keyboard_handle_leave ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::keyboard_handle_leave(wl_keyboard *keyboard, uint32_t serial, wl_surface *surface)
{
    s_serial = serial;
}

//-----------------------------------------------------------------------------
// Name : keyboard_handle_key ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::keyboard_handle_key(wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{   
    s_serial = serial;
    if (!m_shell)
        return;
    
    if (key >= 0)
        m_keysStatus[key] = state;
        
    char buffer[20];
    const xkb_keysym_t *syms;
    xkb_state_key_get_syms(m_state, key + 8, &syms);
    int bytesCopied = xkb_state_key_get_utf8(m_state, key + 8, buffer, 20);
    
    if (bytesCopied == 0)
    {
        buffer[0] = '\0';
        //GK_VirtualKey vKey = waylandVirtualKeysTable[*syms];
        GK_VirtualKey vKey = waylandVirtualKeysTable[key];
        if (vKey != GK_VirtualKey::GK_UNKNOWN)
            m_keysStatus[static_cast<int>(vKey)] = state;
        
        std::cout << "Virtual key was " << (int)vKey << "\n";
        
        sendVirtualKeyEvent(vKey, state, m_modifiersStates);
    }
    else
    {
        sendKeyEvent(buffer[0], state);
    }
    
    std::cout << "key " << buffer[0] << " "<< (int)buffer[0] << "\n";
    
    if (state)
    {
        std::cout << "//-----------------------------------------\n";
        std::cout << "key pressed " << key << "\n";
        std::cout << "//-----------------------------------------\n";
        if (m_alaramToRemove)
            m_timer.removeAlarm(m_keyAlarmIterator);
        m_keyAlarmIterator = m_timer.addAlram(m_timer.convertTime(0, 600000), std::bind(&LinuxWaylandGameWin::key_repeat, this, buffer[0], key), m_timer.convertTime(0, 40000));
        m_alaramToRemove = true;
    }
    else
    {
        std::cout << "//-----------------------------------------\n";
        std::cout << "key released " << key << "\n";
        std::cout << "//-----------------------------------------\n";
        if (m_alaramToRemove)
            m_timer.removeAlarm(m_keyAlarmIterator);
        m_alaramToRemove = false;
    }
    
    
//     if (key == KEY_F11 && state)
//     {
//         if (m_fullscreen)
//         {
//             std::cout << "unset fullscreen\n";
//             m_fullscreen = ~m_fullscreen;
//             zxdg_toplevel_v6_unset_fullscreen(m_xdg_toplevel);
//         }
//         else
//         {
//             std::cout << "set fullscreen\n";
//             m_fullscreen = ~m_fullscreen;
//             zxdg_toplevel_v6_set_fullscreen(m_xdg_toplevel, nullptr);
//         }
//     }
//     else if (key == KEY_ESC && state)
//         // set running to false
//         m_gameRunning = false;
}

//-----------------------------------------------------------------------------
// Name : keyboard_handle_modifiers ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::keyboard_handle_modifiers(wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched,
                               uint32_t mods_locked, uint32_t group)
{
    xkb_state_update_mask(m_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
    
    xkb_mod_mask_t mask  = xkb_state_serialize_mods(m_state, static_cast<xkb_state_component>(XKB_STATE_MODS_DEPRESSED | XKB_STATE_MODS_LATCHED));
    m_modifiersStates = ModifierKeysStates(mask & m_shiftMask,
                                           mask & m_controlMask,
                                           mask & m_altMask);
}

//-----------------------------------------------------------------------------
// Name : keyboard_handle_repeat_info ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::keyboard_handle_repeat_info(wl_keyboard *wl_keyboard, int32_t rate, int32_t delay)
{
    std::cout << "Rate = " << rate << " delay = " << delay << "\n";
}

//-----------------------------------------------------------------------------
// Name : key_repeat ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::key_repeat(char key, uint32_t keyCode)
{
    std::cout << "//-----------------------------------------\n";
    std::cout << "key repeated " << key     << "\n";
    std::cout << "//-----------------------------------------\n";
    if (key != '\0')
        sendKeyEvent(key, true);
    else
    {
        GK_VirtualKey vKey = waylandVirtualKeysTable[keyCode];
        sendVirtualKeyEvent(vKey, true, m_modifiersStates);
    }
}

//-----------------------------------------------------------------------------
// Name : seat_handle_capabilities ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::seat_handle_capabilities(wl_seat *seat, unsigned int caps)
{        
    if ( (caps & WL_SEAT_CAPABILITY_POINTER) && !m_pointer)
    {
        m_pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(m_pointer, &pointer_listener, this);
    }
    else if ( !(caps & WL_SEAT_CAPABILITY_POINTER) && m_pointer)
    {
        wl_pointer_destroy(m_pointer);
        m_pointer = nullptr;
    }
    
    if ( (caps & WL_SEAT_CAPABILITY_KEYBOARD) && !m_keyboard)
    {
        m_keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(m_keyboard, &keyboard_listener, this);
    }
    else if ( !(caps & WL_SEAT_CAPABILITY_KEYBOARD) && m_keyboard )
    {
        wl_keyboard_destroy(m_keyboard);
        m_keyboard = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Name : seat_handle_name ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::seat_handle_name(wl_seat *wl_seat, const char *name)
{
}

//-----------------------------------------------------------------------------
// Name : registry_handle_global ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::registry_handle_global(wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
    uint32_t minVersion = 4;
    
    if (strcmp(interface, "wl_compositor") == 0 )
    {
        m_compositor = static_cast<wl_compositor*>(wl_registry_bind(registry,
                                                                    name,
                                                                    &wl_compositor_interface,
                                                                    std::min(version, minVersion)));
    }
    else if (strcmp(interface, "zxdg_shell_v6") == 0)
    {
        m_shell = static_cast<zxdg_shell_v6*>(wl_registry_bind(registry,
                                                               name,
                                                               &zxdg_shell_v6_interface,
                                                               1));
        zxdg_shell_v6_add_listener(m_shell, &xdg_shell_listener, this);
    }
    else if (strcmp(interface, "wl_seat") == 0)
    {        
        m_seat = static_cast<wl_seat*>(wl_registry_bind(registry,
                                                        name,
                                                        &wl_seat_interface,
                                                        1));
        wl_seat_add_listener(m_seat, &seat_listener, this);
        
        if (m_dataDeviceMan) 
        {
            s_dataDevice = wl_data_device_manager_get_data_device(m_dataDeviceManager, m_seat);
            wl_data_device_add_listener(s_dataDevice, &data_device_listener, nullptr);
        }
        
    }
    else if (strcmp(interface, "wl_shm") == 0)
    {
        m_shm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, 1));
        
        m_cursor_theme = wl_cursor_theme_load(nullptr, 32, m_shm);
        if (!m_cursor_theme)
        {
            std::cout << "unable to load default theme\n";
            return;
        }
        
        m_default_cursor = wl_cursor_theme_get_cursor(m_cursor_theme, "left_ptr");
        if (!m_default_cursor)
        {
            std::cout << "unable to find default left pointer\n";
            return;
        }
    }
    else if (strcmp(interface, "wl_data_device_manager") == 0)
    {
        m_dataDeviceMan = static_cast<wl_data_device_manager*>(wl_registry_bind(registry,
                                                                                    name,
                                                                                    &wl_data_device_manager_interface,
                                                                                    std::min(version, (unsigned int)3)));
        
        if (m_dataDeviceMan) 
        {
            s_dataDevice = wl_data_device_manager_get_data_device(m_dataDeviceMan, m_seat);
            wl_data_device_add_listener(s_dataDevice, &data_device_listener, nullptr);
        }
    }
}

//-----------------------------------------------------------------------------
// Name : registry_handle_global_remove ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::registry_handle_global_remove(wl_registry *registry, uint32_t name)
{
}

//-----------------------------------------------------------------------------
// Name : handle_surface_configure ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::handle_surface_configure(zxdg_surface_v6 *surface, uint32_t serial)
{
    zxdg_surface_v6_ack_configure(surface, serial);
    wait_for_configure = false;
}

//-----------------------------------------------------------------------------
// Name : handle_toplevel_configure ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::handle_toplevel_configure(zxdg_toplevel_v6 *toplevel, int32_t width, int32_t height, wl_array *states)
{
    m_fullscreen = false;
    m_maximized = false;
    
    for (int i = 0; i < states->size; i++)
    {
        switch (((uint32_t*)states->data)[i])
        {
            case ZXDG_TOPLEVEL_V6_STATE_FULLSCREEN:
                m_fullscreen = true;
                break;
                
            case ZXDG_TOPLEVEL_V6_STATE_MAXIMIZED:
                m_maximized = true;
                break;
        }
    }
    
    if (width != 0 || height != 0)
        reshape(width, height);
    
    if (m_native)
        wl_egl_window_resize(m_native, m_winWidth, m_winHeight, 0, 0);
    
}

//-----------------------------------------------------------------------------
// Name : handle_toplevel_close ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::handle_toplevel_close(zxdg_toplevel_v6 *xdg_toplevel)
{
    m_gameRunning = false;
}

//-----------------------------------------------------------------------------
// Name : data_source_target ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_source_target(void *data, wl_data_source *source, const char *mime_type)
{
}

//-----------------------------------------------------------------------------
// Name : data_source_send ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_source_send(void *data, wl_data_source *source, const char *mime_type, int32_t fd)
{
    FILE * fp = fdopen(fd, "w");
    if (fp == nullptr)
    {
        close(fd);
        return;
    }
    
    fwrite(s_clipboardString.c_str(), sizeof(std::string::value_type), s_clipboardString.size(), fp);
    fclose(fp);
}

//-----------------------------------------------------------------------------
// Name : data_source_cancelled ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_source_cancelled(void *data, wl_data_source *source)
{
    wl_data_source_destroy(source);
}

//-----------------------------------------------------------------------------
// Name : data_offer_offer ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_offer_offer(void *data, wl_data_offer *wl_data_offer, const char *type)
{
}

//-----------------------------------------------------------------------------
// Name : data_offer_source_actions ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_offer_source_actions(void *data, wl_data_offer *wl_data_offer, uint32_t source_actions)
{
}

//-----------------------------------------------------------------------------
// Name : data_offer_action ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_offer_action(void *data, wl_data_offer *wl_data_offer, uint32_t dnd_action)
{
}

//-----------------------------------------------------------------------------
// Name : data_device_data_offer ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_device_data_offer(void *data, wl_data_device *data_device, struct wl_data_offer *_offer)
{
     wl_data_offer_add_listener(_offer, &data_offer_listener, _offer);
}

//-----------------------------------------------------------------------------
// Name : data_device_enter ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_device_enter(void *data, wl_data_device *data_device, uint32_t serial, wl_surface *surface, wl_fixed_t x_w, wl_fixed_t y_w, wl_data_offer *offer)
{    
}

//-----------------------------------------------------------------------------
// Name : data_device_leave ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_device_leave(void *data, wl_data_device *data_device)
{
}

//-----------------------------------------------------------------------------
// Name : data_device_motion ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_device_motion(void *data, wl_data_device *data_device, uint32_t time, wl_fixed_t x_w, wl_fixed_t y_w)
{
}

//-----------------------------------------------------------------------------
// Name : data_device_drop ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_device_drop(void *data, wl_data_device *data_device)
{
    
}

//-----------------------------------------------------------------------------
// Name : data_device_selection ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::data_device_selection(void *data, wl_data_device *wl_data_device, wl_data_offer *offer)
{
    if (selectionOffer)
        wl_data_offer_destroy(selectionOffer);
    
    if (offer)
    {
//         s_offer++;
//         if (s_offer == 1)
//             return;
        selectionOffer = (wl_data_offer*)wl_data_offer_get_user_data(offer);
        std::cout << "offer = " << offer << "  selection = " << selectionOffer << "\n";
        std::cout << "source = " << s_dataSource << "\n";
    }
    else
        selectionOffer = nullptr;
}

//-----------------------------------------------------------------------------
// Name : copyToClipboard ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::copyToClipboard(const std::string &text)
{
    s_clipboardString = text;
    
    if (!m_dataDeviceMan)
        return;
    
    s_dataSource = wl_data_device_manager_create_data_source(m_dataDeviceMan);
    if (!s_dataSource)
        return;
    
    wl_data_source_offer(s_dataSource, "text/plain;charset=utf-8");
    wl_data_source_add_listener(s_dataSource, &data_source_listener, nullptr);
    wl_data_device_set_selection(s_dataDevice, s_dataSource, s_serial);
}

//-----------------------------------------------------------------------------
// Name : PasteClipboard ()
//-----------------------------------------------------------------------------
std::string LinuxWaylandGameWin::PasteClipboard()
{
    if (!selectionOffer)
        return std::string();
        
    int pipefd[2];
    
    if (pipe2(pipefd, O_CLOEXEC | O_NONBLOCK) == -1)
    {
        std::cout << "Couldn't create pipe\n";
        return std::string();
    }
    
    wl_data_offer_receive(selectionOffer, "text/plain;charset=utf-8", pipefd[1]);
    
    wl_display_flush(s_display);
    
    close(pipefd[1]);
    
    // pump messages as we might have send the message to our selfs....
    // as it is possible our program holds the clipboard so it needs to answer itself...
    wl_display_dispatch(s_display);
    
    int result = 0;
    do
    {
        pollfd info;
        info.fd = pipefd[0];
        info.events = POLLIN | POLLPRI;
        result = poll(&info, 1, 1000);
    }while(result < 0 && errno == EINTR);
    
    if (result == 0 || result < 0)
    {
        std::cout << "Pipe error\n";
        return std::string();
    }
        
    char buffer[256];
    ssize_t bytesToRead = read(pipefd[0], buffer, 256);
    
    close(pipefd[0]);
    
    if (bytesToRead != 0)
        return std::string(buffer, bytesToRead);
    else
        return std::string();
}

//-----------------------------------------------------------------------------
// Name : setCursorPos ()
//-----------------------------------------------------------------------------
void LinuxWaylandGameWin::setCursorPos(Point newPos)
{

}

//-----------------------------------------------------------------------------
// Name : getCursorPos ()
//-----------------------------------------------------------------------------
Point LinuxWaylandGameWin::getCursorPos()
{
    return Point(0, 0);
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
//-----------------------------------------------------------------------------
int LinuxWaylandGameWin::BeginGame()
{
    while (m_gameRunning)
    {
        if (wait_for_configure)
            wl_display_dispatch(m_display);
        else
        {
            wl_display_dispatch_pending(m_display);
            
            m_timer.frameAdvanced();

            int err = glGetError();
            if (err != GL_NO_ERROR)
                std::cout <<"MsgLoop: ERROR bitches\n";

            if (!m_timer.isCap())
            {
                drawing();
            }
        }
    }
    
    return 0;
}

//-----------------------------------------------------------------------------
// Name : Shutdown ()
//-----------------------------------------------------------------------------
bool LinuxWaylandGameWin::Shutdown()
{        
    eglMakeCurrent(m_eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(m_display, m_egl_surface);
    wl_egl_window_destroy(m_native);
    
    if (m_xdg_toplevel)
        zxdg_toplevel_v6_destroy(m_xdg_toplevel);
    
    if (m_xdg_surface)
        zxdg_surface_v6_destroy(m_xdg_surface);
    
    wl_surface_destroy(m_surface);
    
    return true;
    
}

//-----------------------------------------------------------------------------
// Name : getWindowPosition ()
//-----------------------------------------------------------------------------
Point LinuxWaylandGameWin::getWindowPosition()
{    
    return Point(0, 0);
}

//-----------------------------------------------------------------------------
// Name : getWindowCurrentMonitor ()
//-----------------------------------------------------------------------------
GLuint LinuxWaylandGameWin::getWindowCurrentMonitor()
{        
    return 0;
}
