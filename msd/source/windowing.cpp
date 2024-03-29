#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <timeapi.h>
#include <cstdint>
#include <assert.h>
#include <stdio.h>
#include <glad/glad.h>
#include <glad/glad_wgl.h>
#include "common.h"
#include "initializations.h"

#if _DEBUG
void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* userParam);
#endif

LRESULT CALLBACK WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

static PIXELFORMATDESCRIPTOR pixelFormatDescriptor;

void windowing_initialize()
{
    WNDCLASSW wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszClassName = Monosand;
    // | GCHandle of the managed Monosand.Window | HDC |
    wc.cbWndExtra = sizeof(void*) * 2;
    RegisterClassW(&wc);

    pixelFormatDescriptor = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                     // version number  
        PFD_DRAW_TO_WINDOW |   // support window  
        PFD_SUPPORT_OPENGL |   // support OpenGL  
        PFD_DOUBLEBUFFER,      // double buffered  
        PFD_TYPE_RGBA,         // RGBA type  
        24,                    // 24-bit color depth  
        0, 0, 0, 0, 0, 0,      // color bits ignored  
        0,                     // no alpha buffer  
        0,                     // shift bit ignored  
        0,                     // no accumulation buffer  
        0, 0, 0, 0,            // accum bits ignored  
        24,                    // 24-bit z-buffer      
        0,                     // no stencil buffer  
        0,                     // no auxiliary buffer  
        PFD_MAIN_PLANE,        // main layer  
        0,                     // reserved  
        0, 0, 0                // layer masks ignored  
    };

    timeBeginPeriod(1);
    ImmDisableIME(NULL); // TODO: imm support
}

// TODO: error handle
EXPORT HGLRC MsdCreateRenderContext()
{
    // this function can only be called once
    assert(glViewport == 0);

    wchar_t chr = L'\0';
    HWND dummyHwnd = CreateWindowExW(0, Monosand, &chr, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
    HDC hdc = GetDC(dummyHwnd);
    int pixelFormat = ChoosePixelFormat(hdc, &pixelFormatDescriptor);
    SetPixelFormat(hdc, pixelFormat, &pixelFormatDescriptor);

    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);
    gladLoadGL();
    gladLoadWGL(hdc);
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hglrc);

    GLint attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,

    #ifndef MSDG_COMPATIBILITY_GL
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    #else
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    #endif
    #if _DEBUG
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
    #endif
        0
    };
    hglrc = wglCreateContextAttribsARB(hdc, nullptr, attribs);
    wglMakeCurrent(hdc, hglrc);
    // TODO error handling
    assert(hglrc != nullptr);
    assert(GLAD_WGL_EXT_swap_control);
#if _DEBUG
    glDebugMessageCallbackARB(gl_debug_callback, hglrc);
#endif
    render_context_graphics_init();
    ReleaseDC(dummyHwnd, hdc);
    DestroyWindow(dummyHwnd);
    wglMakeCurrent(nullptr, nullptr);
    return hglrc;
}

EXPORT void MsdAttachRenderContext(HWND hwnd, HGLRC hglrc)
{
    wglMakeCurrent((HDC)GetWindowLongPtrW(hwnd, sizeof(void*)), hglrc);
}

// TODO: initial position
// TODO: window style
EXPORT HWND CALLCONV MsdCreateWindow(int width, int height, wchar_t* title, void* gc_handle)
{
    RECT rect{ 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowExW(0L, Monosand, title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, NULL, NULL
    );
    SetWindowLongPtrW(hwnd, 0, (LONG_PTR)gc_handle);
    ShowWindow(hwnd, SW_HIDE);
    UpdateWindow(hwnd);

    // TODO impl error handler
    HDC hdc = GetDC(hwnd);
    int pixelFormat = ChoosePixelFormat(hdc, &pixelFormatDescriptor);
    SetPixelFormat(hdc, pixelFormat, &pixelFormatDescriptor);
    SetWindowLongPtrW(hwnd, sizeof(void*), (LONG_PTR)hdc);

    return hwnd;
}

EXPORT void CALLCONV MsdShowWindow(HWND hwnd) { ShowWindow(hwnd, SW_NORMAL); }

EXPORT void CALLCONV MsdHideWindow(HWND hwnd) { ShowWindow(hwnd, SW_HIDE); }

EXPORT void CALLCONV MsdDestroyWindow(HWND hwnd)
{
    DestroyWindow(hwnd);
}

EXPORT RECT CALLCONV MsdGetWindowRect(HWND hwnd)
{
    RECT rect{};
    GetClientRect(hwnd, &rect);
    return rect;
}

EXPORT void CALLCONV MsdSetWindowSize(HWND hwnd, int width, int height)
{
    RECT rect{ 0, 0, width, height };
    AdjustWindowRect(&rect, GetWindowLongPtrW(hwnd, GWL_STYLE), FALSE);
    SetWindowPos(hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);
}

EXPORT void CALLCONV MsdSetWindowPos(HWND hwnd, int x, int y)
{
    RECT rect{ x, y, 0, 0 };
    AdjustWindowRect(&rect, GetWindowLongPtrW(hwnd, GWL_STYLE), FALSE);
    SetWindowPos(hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE);
}

EXPORT void CALLCONV MsdSetWindowTitle(HWND hwnd, wchar_t* title)
{
    SetWindowTextW(hwnd, title);
}

EXPORT int CALLCONV MsdGetWindowTitle(HWND hwnd, wchar_t* title)
{
    int len = GetWindowTextLengthW(hwnd);
    if (title != nullptr)
        GetWindowTextW(hwnd, title, len + 1);
    return len;
}