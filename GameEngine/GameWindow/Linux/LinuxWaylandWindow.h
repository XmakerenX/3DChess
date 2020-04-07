#ifndef  _LINUXWAYLANDGAMEWIN_H
#define  _LINUXWAYLANDGAMEWIN_H

#include "../BaseWindow.h"

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <wayland-cursor.h>
#include <wayland-server.h>
#include <xdg-shell-unstable-v6-protocol.h>
#include <xkbcommon/xkbcommon.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Input/Linux/virtualKeysWayland.h"

typedef EGLDisplay (EGLAPIENTRYP eglGetPlatformDisplayEXTProc) (EGLenum platform, void *native_display, const EGLint *attrib_list);
typedef EGLBoolean (EGLAPIENTRYP eglSwapBuffersWithDamageEXTProc) (EGLDisplay dpy, EGLSurface surface, EGLint *rects, EGLint n_rects);
typedef EGLSurface (EGLAPIENTRYP eglCreatePlatformWindowSurfaceEXTProc) (EGLDisplay dpy, EGLConfig config, void *native_window, const EGLint *attrib_list);

// struct MonitorInfo
// {
//     struct Mode
//     {
//         Mode(RRMode _ID, GLuint _width, GLuint _height, GLuint _frequency)
//         : ID(_ID), width(_width), height(_height), frequency(_frequency)
//         {}
//         RRMode ID;
//         GLuint width;
//         GLuint height;
//         GLuint frequency;
//     };
//     
//     MonitorInfo(int _index , const Rect& _positionRect, const std::vector<Mode>& _modes, const std::vector<RROutput>& _outputs)
//     :modes(_modes), outputs(_outputs)
//     {
//         index = _index;
//         positionRect = _positionRect;
//     }
//     
//     MonitorInfo(int _index , const Rect _positionRect, std::vector<Mode>&& _modes, std::vector<RROutput>&& _outputs)
//     :modes(_modes), outputs(_outputs)
//     {
//         index = _index;
//         positionRect = _positionRect;
//     }
//     
//     int index;
//     Rect positionRect;
//     std::vector<Mode> modes;
//     std::vector<RROutput> outputs;
// };

class LinuxWaylandWindow : public BaseWindow
{
public:
    LinuxWaylandWindow();
    virtual ~LinuxWaylandWindow();
    
    void pumpMessages      ();
    bool closeWindow       ();
    
    void setFullScreenMode(bool fullscreen);
    bool setMonitorResolution(int monitorIndex, Resolution newResolution);
    void setWindowPosition(int x, int y);
    void moveWindowToMonitor(int monitorIndex);

    void setCursorPos(Point newPos);
    Point getCursorPos();

    Point getWindowPosition();
    GLuint getWindowCurrentMonitor();
    
    void copyToClipboard(const std::string& text);
    std::string PasteClipboard();
    
    virtual std::function<void (const std::string&)> getCopyToClipboardFunc();
    virtual std::function<std::string (void)> getPasteClipboardFunc();
    
    std::vector<std::vector<Mode1>> getMonitorsModes() const;

private:
    static bool isExtensionSupported(const char *extList, const char *extension);
    static bool checkEglExtension(const char *extensions, const char *extension);

    void xdg_shell_ping(zxdg_shell_v6 *shell, uint32_t serial);
    
    void pointer_handle_enter(wl_pointer *pointer, uint32_t serial, wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy);
    void pointer_handle_leave(wl_pointer *pointer, uint32_t serial, wl_surface *surface);
    void pointer_handle_motion(wl_pointer *pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy);
    void pointer_handle_button(wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
    void pointer_handle_axis(wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
    
    void keyboard_handle_keymap(wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size);
    void keyboard_handle_enter(wl_keyboard *keyboard, uint32_t serial, wl_surface *surface, wl_array *keys);
    void keyboard_handle_leave(wl_keyboard *keyboard, uint32_t serial, wl_surface *surface);
    void keyboard_handle_key(wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
    void keyboard_handle_modifiers(wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched,
                                          uint32_t mods_locked, uint32_t group);
    void keyboard_handle_repeat_info(wl_keyboard *wl_keyboard, int32_t rate, int32_t delay);
    
    void key_repeat(char key, uint32_t keyCode);
        
    void seat_handle_capabilities(wl_seat *seat, unsigned int caps);
    void seat_handle_name(wl_seat *wl_seat, const char *name);
    void registry_handle_global(wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
    static void registry_handle_global_remove(wl_registry *registry, uint32_t name);

    void handle_surface_configure(zxdg_surface_v6 *surface, uint32_t serial);
    void handle_toplevel_configure(zxdg_toplevel_v6 *toplevel, int32_t width, int32_t height, wl_array *states);
    void handle_toplevel_close(zxdg_toplevel_v6 *xdg_toplevel);
    
    static void data_source_target(void *data, wl_data_source *source, const char *mime_type);
    static void data_source_send(void *data, wl_data_source *source, const char *mime_type, int32_t fd);
    static void data_source_cancelled(void *data, wl_data_source *source);
    static std::vector<std::string> s_offerMimeTypes;
    static uint32_t s_sourceActions;
    
    static void data_offer_offer(void *data, wl_data_offer *wl_data_offer, const char *type);
    static void data_offer_source_actions(void *data, wl_data_offer *wl_data_offer, uint32_t source_actions);
    static void data_offer_action(void *data, wl_data_offer *wl_data_offer, uint32_t dnd_action);
    static std::vector<wl_data_offer*> s_offers;
    
    static void data_device_data_offer(void *data, wl_data_device *data_device, struct wl_data_offer *_offer);
    static void data_device_enter(void *data, wl_data_device *data_device, uint32_t serial, wl_surface *surface, wl_fixed_t x_w, wl_fixed_t y_w, wl_data_offer *offer);
    static void data_device_leave(void *data, wl_data_device *data_device);
    static void data_device_motion(void *data, wl_data_device *data_device, uint32_t time, wl_fixed_t x_w, wl_fixed_t y_w);
    static void data_device_drop(void *data, wl_data_device *data_device);
    static void data_device_selection(void *data, wl_data_device *wl_data_device, wl_data_offer *offer);
    static wl_data_offer * selectionOffer;
    
    static bool ctxErrorOccurred;
    static const double s_doubleClickTime;
    
    static std::string s_clipboardString;
    
    bool platformInit   (int width, int height);
    bool initDisplay();
    
    void create_xdg_surface();
    
    bool createWindow(int width, int height);
    bool createEmptyCursorPixmap();
    bool createOpenGLContext();
    
    void glSwapBuffers();
    void getMonitorsInfo();
    
    static const wl_registry_listener registry_listener;
    
    static const zxdg_surface_v6_listener xdg_surface_listener;
    static const zxdg_toplevel_v6_listener xdg_toplevel_listener;
    
    static const wl_pointer_listener pointer_listener;
    static const wl_keyboard_listener keyboard_listener;
    
    static const zxdg_shell_v6_listener xdg_shell_listener;
    static const wl_seat_listener seat_listener;
    
    static const wl_data_source_listener data_source_listener;
    static const wl_data_offer_listener data_offer_listener;
    static const wl_data_device_listener data_device_listener;
    
    static uint32_t s_serial;
    
    double lastLeftClickTime;
    double lastRightClickTime; 
    
    Point m_lastCursorLocation;
    
    wl_display * m_display;
    static wl_display * s_display;
    xkb_context * m_xkbContext;
    xkb_keymap * m_xkbkeymap;
    xkb_state * m_state;
    xkb_mod_mask_t m_controlMask;
    xkb_mod_mask_t m_shiftMask;
    xkb_mod_mask_t m_altMask;
    ModifierKeysStates m_modifiersStates;

    bool m_maximized;
    
    wl_data_device_manager * m_dataDeviceManager;
    static wl_data_device * s_dataDevice;
    static wl_data_device_manager * m_dataDeviceMan;
    static wl_data_source * s_dataSource;
    
    zxdg_toplevel_v6 * m_xdg_toplevel;
    
    wl_registry * m_registry;
    wl_compositor * m_compositor;
    zxdg_shell_v6 * m_shell;
    zxdg_surface_v6 * m_xdg_surface;
    wl_seat * m_seat;
    wl_shm * m_shm;
    wl_cursor_theme * m_cursor_theme;
    wl_cursor * m_default_cursor;
    wl_pointer * m_pointer;
    wl_keyboard * m_keyboard;
    
    wl_surface * m_cursor_surface;
    
    eglSwapBuffersWithDamageEXTProc m_swapbuffers;
    
    EGLDisplay m_eglDpy;
    EGLContext m_eglCtx;
    EGLConfig m_eglConf;
    
    wl_surface *  m_surface;
    wl_egl_window * m_native;
    EGLSurface m_egl_surface;
    
    bool wait_for_configure;
    
    //std::vector<MonitorInfo> m_monitors;
    GLuint m_primaryMonitorIndex;    
    
    Timer::AlarmIterator m_keyAlarmIterator;
    bool m_alaramToRemove;
};

#endif  //_LINUXWAYLANDGAMEWIN_H
