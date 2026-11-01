
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

#include <game-activity/GameActivity.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <game-text-input/gametextinput.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/log.h>
#include <android/native_window.h>

#include <cassert>
#include <string>

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
        // TODO (temporary diagnostic, remove once text delivery is confirmed working) :
        __android_log_print(ANDROID_LOG_INFO, "gvk-text-input", "on_app_cmd(): %s, window=%dx%d",
            cmd == APP_CMD_WINDOW_RESIZED ? "APP_CMD_WINDOW_RESIZED" : "APP_CMD_CONTENT_RECT_CHANGED",
            pAndroidApp->window ? ANativeWindow_getWidth(pAndroidApp->window) : -1,
            pAndroidApp->window ? ANativeWindow_getHeight(pAndroidApp->window) : -1);
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

// NOTE : Touch is a current-frame snapshot (matching Mouse/Keyboard), not an event
//  history -- see gvk-system/touch.hpp.  Finds an existing slot tracking this id, or
//  claims the first free one for a pointer appearing for the first time.  Only
//  searches .staged, since that's what this frame's translation has built up so far
//  (an inactive slot's .staged was already left false by whatever released it,
//  whether that was earlier this same frame or last frame -- nothing to reset here).
static Touch::Pointer* find_or_claim_touch_pointer(Touch& touch, int32_t id)
{
    for (auto& pointer : touch.pointers) {
        if (pointer.active.staged && pointer.id == id) {
            return &pointer;
        }
    }
    for (auto& pointer : touch.pointers) {
        if (!pointer.active.staged) {
            pointer.id = id;
            return &pointer;
        }
    }
    return nullptr; // NOTE : More than Touch::MaxPointerCount simultaneous pointers; silently dropped.
}

static Touch::Pointer* find_touch_pointer(Touch& touch, int32_t id)
{
    for (auto& pointer : touch.pointers) {
        if (pointer.active.staged && pointer.id == id) {
            return &pointer;
        }
    }
    return nullptr;
}

// NOTE : Mirrors glfw_to_gvk_key() in glfw-surface.cpp -- a practical subset (letters,
//  digits, common punctuation, navigation, function keys, modifiers), not exhaustive;
//  extend as real gaps are found, same as the GLFW side already does (eg. GLFW_KEY_
//  WORLD_1/2 map to Unknown there too).
static Key android_keycode_to_gvk_key(int32_t keyCode)
{
    switch (keyCode) {
    case AKEYCODE_SPACE        : return Key::SpaceBar;
    case AKEYCODE_APOSTROPHE   : return Key::OEM_Quote;
    case AKEYCODE_COMMA        : return Key::OEM_Comma;
    case AKEYCODE_MINUS        : return Key::OEM_Minus;
    case AKEYCODE_PERIOD       : return Key::OEM_Period;
    case AKEYCODE_SLASH        : return Key::OEM_ForwardSlash;
    case AKEYCODE_0            : return Key::Zero;
    case AKEYCODE_1            : return Key::One;
    case AKEYCODE_2            : return Key::Two;
    case AKEYCODE_3            : return Key::Three;
    case AKEYCODE_4            : return Key::Four;
    case AKEYCODE_5            : return Key::Five;
    case AKEYCODE_6            : return Key::Six;
    case AKEYCODE_7            : return Key::Seven;
    case AKEYCODE_8            : return Key::Eight;
    case AKEYCODE_9            : return Key::Nine;
    case AKEYCODE_SEMICOLON    : return Key::OEM_SemiColon;
    case AKEYCODE_EQUALS       : return Key::OEM_Plus;
    case AKEYCODE_A            : return Key::A;
    case AKEYCODE_B            : return Key::B;
    case AKEYCODE_C            : return Key::C;
    case AKEYCODE_D            : return Key::D;
    case AKEYCODE_E            : return Key::E;
    case AKEYCODE_F            : return Key::F;
    case AKEYCODE_G            : return Key::G;
    case AKEYCODE_H            : return Key::H;
    case AKEYCODE_I            : return Key::I;
    case AKEYCODE_J            : return Key::J;
    case AKEYCODE_K            : return Key::K;
    case AKEYCODE_L            : return Key::L;
    case AKEYCODE_M            : return Key::M;
    case AKEYCODE_N            : return Key::N;
    case AKEYCODE_O            : return Key::O;
    case AKEYCODE_P            : return Key::P;
    case AKEYCODE_Q            : return Key::Q;
    case AKEYCODE_R            : return Key::R;
    case AKEYCODE_S            : return Key::S;
    case AKEYCODE_T            : return Key::T;
    case AKEYCODE_U            : return Key::U;
    case AKEYCODE_V            : return Key::V;
    case AKEYCODE_W            : return Key::W;
    case AKEYCODE_X            : return Key::X;
    case AKEYCODE_Y            : return Key::Y;
    case AKEYCODE_Z            : return Key::Z;
    case AKEYCODE_LEFT_BRACKET : return Key::OEM_OpenBracket;
    case AKEYCODE_BACKSLASH    : return Key::OEM_BackSlash;
    case AKEYCODE_RIGHT_BRACKET: return Key::OEM_CloseBracket;
    case AKEYCODE_GRAVE        : return Key::OEM_Tilde;
    case AKEYCODE_ESCAPE       : return Key::Escape;
    case AKEYCODE_ENTER        : return Key::Enter;
    case AKEYCODE_TAB          : return Key::Tab;
    case AKEYCODE_DEL          : return Key::Backspace; // NOTE : Android's "DEL" is backspace.
    case AKEYCODE_FORWARD_DEL  : return Key::Delete;
    case AKEYCODE_INSERT       : return Key::Insert;
    case AKEYCODE_DPAD_RIGHT   : return Key::RightArrow;
    case AKEYCODE_DPAD_LEFT    : return Key::LeftArrow;
    case AKEYCODE_DPAD_DOWN    : return Key::DownArrow;
    case AKEYCODE_DPAD_UP      : return Key::UpArrow;
    case AKEYCODE_PAGE_UP      : return Key::PageUp;
    case AKEYCODE_PAGE_DOWN    : return Key::PageDown;
    case AKEYCODE_MOVE_HOME    : return Key::Home;
    case AKEYCODE_MOVE_END     : return Key::End;
    case AKEYCODE_CAPS_LOCK    : return Key::CapsLock;
    case AKEYCODE_SCROLL_LOCK  : return Key::ScrollLock;
    case AKEYCODE_NUM_LOCK     : return Key::NumLock;
    case AKEYCODE_SYSRQ        : return Key::PrintScreen;
    case AKEYCODE_BREAK        : return Key::Pause;
    case AKEYCODE_F1           : return Key::F1;
    case AKEYCODE_F2           : return Key::F2;
    case AKEYCODE_F3           : return Key::F3;
    case AKEYCODE_F4           : return Key::F4;
    case AKEYCODE_F5           : return Key::F5;
    case AKEYCODE_F6           : return Key::F6;
    case AKEYCODE_F7           : return Key::F7;
    case AKEYCODE_F8           : return Key::F8;
    case AKEYCODE_F9           : return Key::F9;
    case AKEYCODE_F10          : return Key::F10;
    case AKEYCODE_F11          : return Key::F11;
    case AKEYCODE_F12          : return Key::F12;
    case AKEYCODE_NUMPAD_0     : return Key::NumPad0;
    case AKEYCODE_NUMPAD_1     : return Key::NumPad1;
    case AKEYCODE_NUMPAD_2     : return Key::NumPad2;
    case AKEYCODE_NUMPAD_3     : return Key::NumPad3;
    case AKEYCODE_NUMPAD_4     : return Key::NumPad4;
    case AKEYCODE_NUMPAD_5     : return Key::NumPad5;
    case AKEYCODE_NUMPAD_6     : return Key::NumPad6;
    case AKEYCODE_NUMPAD_7     : return Key::NumPad7;
    case AKEYCODE_NUMPAD_8     : return Key::NumPad8;
    case AKEYCODE_NUMPAD_9     : return Key::NumPad9;
    case AKEYCODE_NUMPAD_DOT        : return Key::Decimal;
    case AKEYCODE_NUMPAD_DIVIDE     : return Key::Divide;
    case AKEYCODE_NUMPAD_MULTIPLY   : return Key::Multiply;
    case AKEYCODE_NUMPAD_SUBTRACT   : return Key::Subtract;
    case AKEYCODE_NUMPAD_ADD        : return Key::Add;
    case AKEYCODE_NUMPAD_ENTER      : return Key::Enter;
    case AKEYCODE_NUMPAD_EQUALS     : return Key::Enter;
    case AKEYCODE_SHIFT_LEFT   : return Key::LeftShift;
    case AKEYCODE_CTRL_LEFT    : return Key::LeftControl;
    case AKEYCODE_ALT_LEFT     : return Key::LeftAlt;
    case AKEYCODE_META_LEFT    : return Key::LeftWindow;
    case AKEYCODE_SHIFT_RIGHT  : return Key::RightShift;
    case AKEYCODE_CTRL_RIGHT   : return Key::RightControl;
    case AKEYCODE_ALT_RIGHT    : return Key::RightAlt;
    case AKEYCODE_META_RIGHT   : return Key::RightWindow;
    case AKEYCODE_MENU         : return Key::Menu;
    default                    : return Key::Unknown;
    }
}

// NOTE : Decodes one UTF-8 code point starting at bytes[*pIndex], advancing *pIndex
//  past it.  GameTextInputState::text_UTF8 is "modified UTF-8" per gametextinput.h,
//  which only differs from standard UTF-8 in how it encodes U+0000 and codepoints
//  outside the BMP -- neither comes up for ordinary typed text, so a plain decoder is
//  fine here.  A truncated trailing sequence (cut off mid-character) just advances to
//  the end and returns whatever was decoded so far; not expected for committed IME
//  text, not worth more care than that.
static uint32_t decode_utf8_codepoint(const char* pBytes, size_t size, size_t* pIndex)
{
    auto& i = *pIndex;
    auto byte0 = (uint8_t)pBytes[i];
    uint32_t codepoint = 0;
    size_t extraBytes = 0;
    if ((byte0 & 0x80) == 0x00) { codepoint = byte0; extraBytes = 0; }
    else if ((byte0 & 0xe0) == 0xc0) { codepoint = byte0 & 0x1f; extraBytes = 1; }
    else if ((byte0 & 0xf0) == 0xe0) { codepoint = byte0 & 0x0f; extraBytes = 2; }
    else if ((byte0 & 0xf8) == 0xf0) { codepoint = byte0 & 0x07; extraBytes = 3; }
    else { ++i; return 0xfffd; } // Invalid leading byte
    ++i;
    for (size_t j = 0; j < extraBytes && i < size; ++j, ++i) {
        auto byte = (uint8_t)pBytes[i];
        if ((byte & 0xc0) != 0x80) { return 0xfffd; } // Invalid continuation byte
        codepoint = (codepoint << 6) | (byte & 0x3f);
    }
    return codepoint;
}

// NOTE : Tracks the last-seen GameTextInput committed text so only *new* characters
//  get pushed to Surface::TextStream each frame (TextStream is a per-frame stream of
//  newly typed codepoints, same contract as GLFW's char callback -- not "the field's
//  current full value").  Only one gvk::system::Surface ever exists on Android, same
//  justification as spAndroidApp above, so a plain static is enough.
static std::string sLastGameTextInputText;

// NOTE : Callback for GameActivity_getTextInputState(), called synchronously (not
//  deferred) with state valid only for the duration of the call -- pContext is the
//  Surface::TextStream to append into.  Simple suffix-diff against
//  sLastGameTextInputText: correct for ordinary forward typing (the actual use case --
//  typing a value into a field), not a general edit-distance/cursor-aware diff.  A
//  non-append edit (backspace mid-string, cursor reposition + insert, select-and-
//  replace) just resyncs silently without emitting anything that frame -- a known
//  limitation, not silently wrong (nothing gets corrupted, a complex edit just doesn't
//  show up immediately).  Backspace/Delete at the end of the field are handled by the
//  ordinary Keyboard path (AKEYCODE_DEL/FORWARD_DEL) below, not here.
static void on_game_text_input_state(void* pContext, const GameTextInputState* pState)
{
    // TODO (temporary diagnostic, remove once text delivery is confirmed working) :
    __android_log_print(ANDROID_LOG_INFO, "gvk-text-input", "on_game_text_input_state() called, pState=%p, text_UTF8=\"%s\", text_length=%d",
        (void*)pState, pState && pState->text_UTF8 ? pState->text_UTF8 : "(null)", pState ? pState->text_length : -1);
    auto& textStream = *(Surface::TextStream*)pContext;
    if (!pState || !pState->text_UTF8) {
        return;
    }
    std::string currentText(pState->text_UTF8, (size_t)pState->text_length);
    if (currentText.size() > sLastGameTextInputText.size() &&
        currentText.compare(0, sLastGameTextInputText.size(), sLastGameTextInputText) == 0) {
        size_t i = sLastGameTextInputText.size();
        while (i < currentText.size()) {
            textStream.push_back(decode_utf8_codepoint(currentText.data(), currentText.size(), &i));
        }
    }
    sLastGameTextInputText = std::move(currentText);
}

// NOTE : Drains GameActivity's per-frame motion/key event batch into
//  gvk::system::Touch/Keyboard, and reads committed text out of GameTextInput into
//  Surface::TextStream.  Draining has to happen every frame regardless of whether
//  anything is listening -- unswapped/uncleared buffers are how new events get room
//  to arrive.
//  NOTE on text, corrected after real on-device testing (see also the NOTE on
//  on_game_text_input_state() above): GameActivityKeyEvent::unicodeChar is a resolved
//  Unicode codepoint per keystroke (kept below, still fed to TextStream -- harmless,
//  and real for a physical/Bluetooth keyboard, which bypasses the IME entirely), but
//  it is NOT how this tablet's on-screen keyboard delivers ordinary typed characters,
//  even plain digits -- confirmed by typing "7" into a field and it never arriving.
//  The soft keyboard commits text through GameTextInput's InputConnection-backed
//  buffer instead.  Two earlier attempts at reading that got this wrong: both reached
//  past GameActivity into the raw GameTextInput object (GameActivity_getTextInput() +
//  GameTextInput_getState()/setState()) -- one hung the app on-device
//  (GameTextInput_setState() called before GameActivity_showSoftInput(), likely
//  before GameActivity's Java-side InputConnection was actually live), the other just
//  stopped the keyboard from appearing.  The documented, intended path (confirmed
//  against the actual games-activity-4.0.0 headers vendored for this build, not just
//  version-ambiguous web docs) stays entirely at the GameActivity level instead:
//  android_app::textInputState is a flag GameActivity's own glue code sets when new
//  text state is available ("Use GameActivity_getTextInputState to get information
//  about the text entered by the user" -- the flag's own doc comment in
//  android_native_app_glue.h); the caller polls it, calls
//  GameActivity_getTextInputState() (not the raw GameTextInput_getState()) when set,
//  and clears it.  No setState call of any kind before showSoftInput().
static void translate_input_events(android_app* pAndroidApp, Surface::ControlBlock* pSurfaceControlBlock)
{
    auto pInputBuffer = android_app_swap_input_buffers(pAndroidApp);
    if (!pInputBuffer) {
        return;
    }
    if (pSurfaceControlBlock) {
        auto& touch = pSurfaceControlBlock->mInput.touch;
        for (uint64_t event_i = 0; event_i < pInputBuffer->motionEventsCount; ++event_i) {
            const auto& motionEvent = pInputBuffer->motionEvents[event_i];
            auto actionMasked = motionEvent.action & AMOTION_EVENT_ACTION_MASK;

            // Every pointer this event reports is down and at this position right
            // now -- including, per the Android MotionEvent contract, the specific
            // pointer that's being released in a POINTER_UP/UP event (its slot gets
            // explicitly cleared below instead).
            for (uint32_t pointer_i = 0; pointer_i < motionEvent.pointerCount && pointer_i < Touch::MaxPointerCount; ++pointer_i) {
                const auto& pointerAxes = motionEvent.pointers[pointer_i];
                auto pTouchPointer = find_or_claim_touch_pointer(touch, pointerAxes.id);
                if (pTouchPointer) {
                    std::array<float, 2> position{ GameActivityPointerAxes_getX(&pointerAxes), GameActivityPointerAxes_getY(&pointerAxes) };
                    if (!pTouchPointer->active.current && !pTouchPointer->active.staged) {
                        // NOTE : Seed current/previous directly for a newly-claimed pointer so
                        //  its first down() frame reports delta() == 0, not a jump from
                        //  whatever this slot's position happened to be (zero, or a stale
                        //  leftover from whichever earlier, now-released pointer last used
                        //  this slot) to its real first touch position -- confirmed live
                        //  on-device as the cause of the camera swinging wildly right on
                        //  touch-down, worst with two fingers since a human two-finger
                        //  touch-down is rarely perfectly simultaneous at the event level, so
                        //  the transient one-finger state (driving look) hit this too.
                        pTouchPointer->position.current = position;
                        pTouchPointer->position.previous = position;
                    }
                    pTouchPointer->active.staged = true;
                    pTouchPointer->position.staged = position;
                }
            }

            if (actionMasked == AMOTION_EVENT_ACTION_UP) {
                // The last pointer just lifted.
                if (motionEvent.pointerCount) {
                    if (auto pTouchPointer = find_touch_pointer(touch, motionEvent.pointers[0].id)) {
                        pTouchPointer->active.staged = false;
                    }
                }
            } else if (actionMasked == AMOTION_EVENT_ACTION_POINTER_UP) {
                // One of several pointers lifted; which one is encoded in the action.
                auto actionPointerIndex = (uint32_t)((motionEvent.action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
                if (actionPointerIndex < motionEvent.pointerCount) {
                    if (auto pTouchPointer = find_touch_pointer(touch, motionEvent.pointers[actionPointerIndex].id)) {
                        pTouchPointer->active.staged = false;
                    }
                }
            } else if (actionMasked == AMOTION_EVENT_ACTION_CANCEL) {
                // Tracking lost (eg. an system gesture took over) -- nothing reported
                // as down is trustworthy anymore.
                for (auto& pointer : touch.pointers) {
                    pointer.active.staged = false;
                }
            }
        }

        for (uint64_t event_i = 0; event_i < pInputBuffer->keyEventsCount; ++event_i) {
            const auto& keyEvent = pInputBuffer->keyEvents[event_i];
            auto down = keyEvent.action == AKEY_EVENT_ACTION_DOWN;
            pSurfaceControlBlock->mInput.keyboard.staged[(size_t)android_keycode_to_gvk_key(keyEvent.keyCode)] = down;
            if (down && keyEvent.unicodeChar) {
                pSurfaceControlBlock->mTextStream.push_back((uint32_t)keyEvent.unicodeChar);
            }
        }

        if (pAndroidApp->textInputState) {
            // TODO (temporary diagnostic, remove once text delivery is confirmed working) :
            __android_log_print(ANDROID_LOG_INFO, "gvk-text-input", "pAndroidApp->textInputState is set, calling GameActivity_getTextInputState()");
            GameActivity_getTextInputState(pAndroidApp->activity, on_game_text_input_state, &pSurfaceControlBlock->mTextStream);
            pAndroidApp->textInputState = 0;
        }
    }
    if (pInputBuffer->motionEventsCount) {
        android_app_clear_motion_events(pInputBuffer);
    }
    if (pInputBuffer->keyEventsCount) {
        android_app_clear_key_events(pInputBuffer);
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
    translate_input_events(pAndroidApp, pSurfaceControlBlock);
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

void Surface::show_soft_input()
{
    assert(mReference);
    auto pAndroidApp = detail::get_android_app();
    if (pAndroidApp) {
        // TODO : Tried GameActivity_setImeEditorInfo(TYPE_CLASS_NUMBER | SIGNED |
        //  DECIMAL, IME_ACTION_DONE, 0) here, on the theory that the default plain-text
        //  IME's autocorrect/predictive text was holding typed characters in a
        //  "composing" (not yet committed) state until a word boundary, explaining why
        //  typed text wasn't appearing until the field lost focus.  Reverted --
        //  confirmed live on-device this made things worse (the keyboard visibly
        //  switched from full keyboard to numpad mid-session) without fixing the
        //  original symptom, and the committed text showed two decimal points
        //  ("5.5.966k"), meaning the numeric restriction wasn't even being honored.
        //  Diagnostic logging in on_game_text_input_state()/translate_input_events()
        //  below shows GameActivity_getTextInputState() *does* report growing text
        //  mid-session, well before the field loses focus, so the capture side (and
        //  presumably AddInputCharacter()/TextStream) seem to be working -- the "only
        //  updates after closing" report may be about whether the on-screen edit box
        //  is visibly, noticeably updating while the keyboard covers half the screen,
        //  not about whether new text is actually arriving.  Needs confirming on-
        //  device which it is before trying another fix.
        GameActivity_showSoftInput(pAndroidApp->activity, GAMEACTIVITY_SHOW_SOFT_INPUT_IMPLICIT);
    }
}

void Surface::hide_soft_input()
{
    assert(mReference);
    auto pAndroidApp = detail::get_android_app();
    if (pAndroidApp) {
        GameActivity_hideSoftInput(pAndroidApp->activity, GAMEACTIVITY_HIDE_SOFT_INPUT_IMPLICIT_ONLY);
    }
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
