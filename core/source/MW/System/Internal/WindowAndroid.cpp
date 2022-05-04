/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Internal.WindowAndroid;
import Microwave.System.Application;
import Microwave.Graphics.Internal.HWSurfaceEGL;
import <android/native_window.h>;
import <stdexcept>;
import <vector>;
import <unistd.h>;

namespace mw {
inline namespace system {

extern android_app* androidApp;

WindowAndroid* WindowAndroid::instance = nullptr;

sptr<Window> Window::New(const std::string title, const IVec2& pos, const IVec2& size) {
    return spnew<WindowAndroid>();
}

WindowAndroid::WindowAndroid()
{
    assert(instance == nullptr);
    instance = this;
    androidApp->onInputEvent = OnInput;
    size.x = ANativeWindow_getWidth(androidApp->window);
    size.y = ANativeWindow_getHeight(androidApp->window);
    this->dispatcher = Dispatcher::GetCurrent();
}

WindowAndroid::~WindowAndroid()
{
    Close();
    androidApp->onInputEvent = nullptr;
    instance = nullptr;
}

void WindowAndroid::SetTitle(const std::string& title) {
    this->title = title;
}

std::string WindowAndroid::GetTitle() const {
    return title;
}

void WindowAndroid::SetPos(const IVec2& pos) {
    this->pos = pos;
}

IVec2 WindowAndroid::GetPos() const {
    return pos;
}

void WindowAndroid::SetSize(const IVec2& size)
{
    // Android main window can only be resized by the OS.
}

void WindowAndroid::SetSizeInternal(const IVec2& size)
{
    this->size = size;
}

IVec2 WindowAndroid::GetSize() const {
    return size;
}

bool WindowAndroid::IsVisible() const {
    return visible;
}

void WindowAndroid::SetResizeable(bool resizeable)
{
    this->resizeable = resizeable;
}

bool WindowAndroid::IsResizeable() const {
    return resizeable;
}

void WindowAndroid::Show()
{
    // not supported on Android
    visible = true;
}

void WindowAndroid::Hide()
{
    JNIEnv* env = nullptr;
    androidApp->activity->vm->AttachCurrentThread(&env, nullptr);

    jclass android_content_Context = env->GetObjectClass(androidApp->activity->clazz);
    jmethodID midMoveTaskToBack = env->GetMethodID(android_content_Context, "moveTaskToBack", "(Z)Z");
    jboolean wasMoved = env->CallBooleanMethod(androidApp->activity->clazz, midMoveTaskToBack, jboolean(true));

    visible = false;
}

void WindowAndroid::Close()
{
    // not supported on Android
}

uintptr_t WindowAndroid::GetHandle() const {
    return (uintptr_t)0;
}

int32_t WindowAndroid::OnInput(struct android_app* app, AInputEvent* event)
{
    auto type = AInputEvent_getType(event);

    if(type == AINPUT_EVENT_TYPE_KEY)
    {
        auto action = AKeyEvent_getAction(event);
        int keycode = AKeyEvent_getKeyCode(event);

        switch (action)
        {
            case AKEY_EVENT_ACTION_DOWN:
                instance->OnKeyDown(TranslateKey(keycode));
                return 1;

            case AKEY_EVENT_ACTION_UP:
                instance->OnKeyUp(TranslateKey(keycode));
                return 1;

            case AKEY_EVENT_ACTION_MULTIPLE:
                return 1;
        }
    }
    else if(type == AINPUT_EVENT_TYPE_MOTION)
    {
        int action = AMotionEvent_getAction(event);
        int actionCode = action & AMOTION_EVENT_ACTION_MASK;
        int pointerIndex = action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        int pointerId = AMotionEvent_getPointerId(event, pointerIndex);
        float x = AMotionEvent_getX(event, pointerIndex);
        float y = AMotionEvent_getY(event, pointerIndex);

        switch (actionCode)
        {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                instance->OnPointerDown(IVec2(math::RoundToInt(x), math::RoundToInt(y)), pointerId);
                return 1;

            case AMOTION_EVENT_ACTION_MOVE:
                instance->OnPointerMove(IVec2(math::RoundToInt(x), math::RoundToInt(y)), pointerId);
                return 1;

            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
            case AMOTION_EVENT_ACTION_CANCEL:
                instance->OnPointerUp(IVec2(math::RoundToInt(x), math::RoundToInt(y)), pointerId);
                return 1;
        }
    }

    return 0;
}

Keycode WindowAndroid::TranslateKey(int keycode)
{
    switch (keycode)
    {
        case AKEYCODE_A: return Keycode::A;
        case AKEYCODE_B: return Keycode::B;
        case AKEYCODE_C: return Keycode::C;
        case AKEYCODE_D: return Keycode::D;
        case AKEYCODE_E: return Keycode::E;
        case AKEYCODE_F: return Keycode::F;
        case AKEYCODE_G: return Keycode::G;
        case AKEYCODE_H: return Keycode::H;
        case AKEYCODE_I: return Keycode::I;
        case AKEYCODE_J: return Keycode::J;
        case AKEYCODE_K: return Keycode::K;
        case AKEYCODE_L: return Keycode::L;
        case AKEYCODE_M: return Keycode::M;
        case AKEYCODE_N: return Keycode::N;
        case AKEYCODE_O: return Keycode::O;
        case AKEYCODE_P: return Keycode::P;
        case AKEYCODE_Q: return Keycode::Q;
        case AKEYCODE_R: return Keycode::R;
        case AKEYCODE_S: return Keycode::S;
        case AKEYCODE_T: return Keycode::T;
        case AKEYCODE_U: return Keycode::U;
        case AKEYCODE_V: return Keycode::V;
        case AKEYCODE_W: return Keycode::W;
        case AKEYCODE_X: return Keycode::X;
        case AKEYCODE_Y: return Keycode::Y;
        case AKEYCODE_Z: return Keycode::Z;
        case AKEYCODE_0: return Keycode::Num0;
        case AKEYCODE_1: return Keycode::Num1;
        case AKEYCODE_2: return Keycode::Num2;
        case AKEYCODE_3: return Keycode::Num3;
        case AKEYCODE_4: return Keycode::Num4;
        case AKEYCODE_5: return Keycode::Num5;
        case AKEYCODE_6: return Keycode::Num6;
        case AKEYCODE_7: return Keycode::Num7;
        case AKEYCODE_8: return Keycode::Num8;
        case AKEYCODE_9: return Keycode::Num9;
        case AKEYCODE_F1: return Keycode::F1;
        case AKEYCODE_F2: return Keycode::F2;
        case AKEYCODE_F3: return Keycode::F3;
        case AKEYCODE_F4: return Keycode::F4;
        case AKEYCODE_F5: return Keycode::F5;
        case AKEYCODE_F6: return Keycode::F6;
        case AKEYCODE_F7: return Keycode::F7;
        case AKEYCODE_F8: return Keycode::F8;
        case AKEYCODE_F9: return Keycode::F9;
        case AKEYCODE_F10: return Keycode::F10;
        case AKEYCODE_F11: return Keycode::F11;
        case AKEYCODE_F12: return Keycode::F12;
        case AKEYCODE_BACK: return Keycode::Back;
        case AKEYCODE_FORWARD: return Keycode::Forward;
        case AKEYCODE_VOLUME_UP: return Keycode::VolumeUp;
        case AKEYCODE_VOLUME_DOWN: return Keycode::VolumeDown;
        case AKEYCODE_VOLUME_MUTE: return Keycode::VolumeMute;
        case AKEYCODE_SPACE: return Keycode::Space;
        case AKEYCODE_DEL: return Keycode::Backspace;
        case AKEYCODE_FORWARD_DEL: return Keycode::Del;
        case AKEYCODE_DPAD_LEFT: return Keycode::LeftArrow;
        case AKEYCODE_DPAD_RIGHT: return Keycode::RightArrow;
        case AKEYCODE_DPAD_UP: return Keycode::UpArrow;
        case AKEYCODE_DPAD_DOWN: return Keycode::DownArrow;
        case AKEYCODE_ALT_LEFT: return Keycode::Alt;
        case AKEYCODE_ALT_RIGHT: return Keycode::Alt;
        case AKEYCODE_CTRL_LEFT: return Keycode::Ctrl;
        case AKEYCODE_CTRL_RIGHT: return Keycode::Ctrl;
        case AKEYCODE_SHIFT_LEFT: return Keycode::Shift;
        case AKEYCODE_SHIFT_RIGHT: return Keycode::Shift;
        case AKEYCODE_ENTER: return Keycode::Enter;
        case AKEYCODE_ESCAPE: return Keycode::Escape;
        case AKEYCODE_PLUS: return Keycode::Equals;
        case AKEYCODE_MINUS: return Keycode::Minus;
        case AKEYCODE_LEFT_BRACKET: return Keycode::LeftBracket;
        case AKEYCODE_RIGHT_BRACKET: return Keycode::RightBracket;
        case AKEYCODE_APOSTROPHE: return Keycode::Quote;
        case AKEYCODE_SEMICOLON: return Keycode::Semicolon;
        case AKEYCODE_SLASH: return Keycode::Slash;
        case AKEYCODE_BACKSLASH: return Keycode::Backslash;
        case AKEYCODE_COMMA: return Keycode::Comma;
        case AKEYCODE_PERIOD: return Keycode::Period;
        case AKEYCODE_GRAVE: return Keycode::Grave;
        case AKEYCODE_TAB: return Keycode::Tab;
    }

    return (Keycode)((int)Keycode::Unknown + keycode);
}

sptr<HWRenderTarget> WindowAndroid::GetHWRenderTarget() {
    return surface;
}

} // system
} // mw
