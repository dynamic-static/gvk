
/*******************************************************************************

MIT License

Copyright (c) Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "gvk-system/surface.hpp"
#include "android-game-activity-surface.hpp"

#ifdef __ANDROID__

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/native_window.h>

#include <cassert>

namespace gvk {
namespace system {

// NOTE : GameActivity/native_app_glue create exactly one android_app per process and it
//  lives for the process' whole lifetime, so a plain static is sufficient here -- no
//  lifetime management is needed the way there is for android_app::window (see
//  on_app_cmd() below).  Set by the application's own android_main() before calling
//  gvk_main() -- see entry-point.hpp.
static android_app* spAndroidApp = nullptr;

void set_android_app(android_app* pAndroidApp)
{
    spAndroidApp = pAndroidApp;
}

namespace detail {

android_app* get_android_app()
{
    return spAndroidApp;
}

} // namespace detail

static void on_app_cmd(android_app* pAndroidApp, int32_t cmd)
{
    auto pSurfaceControlBlock = (Surface::ControlBlock*)pAndroidApp->userData;
    if (!pSurfaceControlBlock) {
        return;
    }
    switch (cmd) {
    case APP_CMD_INIT_WINDOW: {
        pSurfaceControlBlock->mPlatformInfo.androidWindow = pAndroidApp->window;
    } break;
    case APP_CMD_TERM_WINDOW: {
        pSurfaceControlBlock->mPlatformInfo.androidWindow = nullptr;
    } break;
    case APP_CMD_WINDOW_RESIZED:
    case APP_CMD_CONTENT_RECT_CHANGED: {
        pSurfaceControlBlock->mStatus |= Surface::Resized;
    } break;
    case APP_CMD_GAINED_FOCUS: {
        pSurfaceControlBlock->mStatus |= Surface::GainedFocus;
    } break;
    case APP_CMD_LOST_FOCUS: {
        pSurfaceControlBlock->mStatus |= Surface::LostFocus;
    } break;
    case APP_CMD_DESTROY: {
        pSurfaceControlBlock->mStatus |= Surface::CloseRequested;
    } break;
    default: break;
    }
}

// NOTE : Drains and discards GameActivity's per-frame input batch.  This has to happen
//  every frame regardless of whether anything is listening (unswapped/uncleared buffers
//  are how new events get room to arrive), but nothing feeds gvk::system::Input yet --
//  Patrick: no sample needs touch/key input before gvk sample 04, so Touch is being
//  designed as its own focused piece of work rather than rushed in here.
static void drain_input_events(android_app* pAndroidApp)
{
    auto pInputBuffer = android_app_swap_input_buffers(pAndroidApp);
    if (pInputBuffer) {
        if (pInputBuffer->motionEventsCount) {
            android_app_clear_motion_events(pInputBuffer);
        }
        if (pInputBuffer->keyEventsCount) {
            android_app_clear_key_events(pInputBuffer);
        }
    }
}

int32_t Surface::create(const CreateInfo* pCreateInfo, Surface* pSurface)
{
    assert(pCreateInfo);
    assert(pSurface);
    auto pAndroidApp = detail::get_android_app();
    assert(pAndroidApp && "gvk::system::Surface::create() requires an android_app; is this being called from a gvk::system::run() entry point?");
    // NOTE : Creating new Reference here to match the GLFW backend's Surface::create();
    //  Android supports exactly one Surface per process (one android_app, one window), so
    //  there is no equivalent GLFW-window-set style registry to maintain here.
    Reference<Surface::ControlBlock> reference(newref);
    if (pCreateInfo->pTitle) {
        reference->mTitle = pCreateInfo->pTitle;
    }
    reference->mpWindowHandle = pAndroidApp;
    pAndroidApp->userData = &reference.get_obj();
    pAndroidApp->onAppCmd = on_app_cmd;
    // NOTE : GameActivity's window arrives asynchronously (APP_CMD_INIT_WINDOW); block
    //  here so Surface::create() returns a ready Surface, matching the desktop backend's
    //  contract.  The window going away again later (eg. backgrounding) is handled by
    //  gvk::wsi::Context's CreateInfo::pfnRecreateSurface, not here -- see session notes,
    //  plan step 5, decision A.
    while (!pAndroidApp->window && !pAndroidApp->destroyRequested) {
        int events = 0;
        android_poll_source* pSource = nullptr;
        if (ALooper_pollOnce(-1, nullptr, &events, (void**)&pSource) >= 0 && pSource) {
            pSource->process(pAndroidApp, pSource);
        }
    }
    if (!pAndroidApp->window) {
        return -3; // VK_ERROR_INITIALIZATION_FAILED; destroyRequested before a window ever arrived
    }
    reference->mPlatformInfo.androidWindow = pAndroidApp->window;
    pSurface->mReference = reference;
    return 0; // VK_SUCCESS
}

void Surface::update()
{
    auto pAndroidApp = detail::get_android_app();
    if (!pAndroidApp) {
        return;
    }
    auto pSurfaceControlBlock = (Surface::ControlBlock*)pAndroidApp->userData;
    if (pSurfaceControlBlock) {
        pSurfaceControlBlock->mStatus = 0;
        pSurfaceControlBlock->mInput.update();
        pSurfaceControlBlock->mTextStream.clear();
        pSurfaceControlBlock->mDroppedPaths.clear();
    }
    // NOTE : Non-blocking, unlike Surface::create()'s initial wait -- the caller's own
    //  frame loop is what's driving timing once rendering exists.
    int events = 0;
    android_poll_source* pSource = nullptr;
    while (ALooper_pollOnce(0, nullptr, &events, (void**)&pSource) >= 0) {
        if (pSource) {
            pSource->process(pAndroidApp, pSource);
        }
    }
    drain_input_events(pAndroidApp);
}

void Surface::get_window_position(int32_t* pX, int32_t* pY) const
{
    assert(mReference);
    // NOTE : GameActivity windows are always fullscreen; there is no OS-level position.
    if (pX) {
        *pX = 0;
    }
    if (pY) {
        *pY = 0;
    }
}

void Surface::get_window_extent(int32_t* pWidth, int32_t* pHeight) const
{
    assert(mReference);
    auto pAndroidWindow = mReference->mPlatformInfo.androidWindow;
    if (pWidth) {
        *pWidth = pAndroidWindow ? ANativeWindow_getWidth(pAndroidWindow) : 0;
    }
    if (pHeight) {
        *pHeight = pAndroidWindow ? ANativeWindow_getHeight(pAndroidWindow) : 0;
    }
}

void Surface::set_window_extent(const std::array<int32_t, 2>&)
{
    assert(mReference);
    // NOTE : No-op; window management on Android is the system's job, not the app's.
    //  Bare-minimum support for now -- Patrick: assume a simple fullscreen-always app
    //  until split-screen/resize gets designed properly.
}

Surface::ControlBlock::ControlBlock()
{
}

Surface::ControlBlock::~ControlBlock()
{
    auto pAndroidApp = detail::get_android_app();
    if (pAndroidApp && pAndroidApp->userData == this) {
        pAndroidApp->userData = nullptr;
        pAndroidApp->onAppCmd = nullptr;
    }
}

const std::string& Surface::get_title() const
{
    assert(mReference);
    return mReference->mTitle;
}

void Surface::set_title(const std::string& title)
{
    assert(mReference);
    // NOTE : No OS-visible effect; a GameActivity window has no titlebar.  Kept as plain
    //  storage so callers don't need a platform branch just to set a title.
    mReference->mTitle = title;
}

const Surface::CursorMode& Surface::get_cursor_mode() const
{
    assert(mReference);
    return mReference->mCursorMode;
}

void Surface::set_cursor_mode(CursorMode cursorMode)
{
    assert(mReference);
    // NOTE : No OS-visible effect; no mouse cursor concept on a touch-primary device.
    //  Kept as plain storage, same rationale as set_title() above.
    mReference->mCursorMode = cursorMode;
}

void Surface::set_cursor_type(CursorType)
{
    assert(mReference);
    // NOTE : No-op, same rationale as set_cursor_mode() above.
}

const Surface::PlatformInfo& Surface::get_platform_info() const
{
    assert(mReference);
    return mReference->mPlatformInfo;
}

} // namespace system
} // namespace gvk

#endif // __ANDROID__
