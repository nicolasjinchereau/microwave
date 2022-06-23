/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Internal.WindowMacOS;
import Microwave.Math;
import <stdexcept>;
import <vector>;

namespace mw {
inline namespace system {

gptr<Window> Window::New(const std::string title, const IVec2& pos, const IVec2& size) {
    return gpnew<WindowMacOS>(title, pos, size);
}

WindowMacOS::WindowMacOS()
    : WindowMacOS("", IVec2::Zero(), IVec2::Zero())
{
}

WindowMacOS::WindowMacOS(
    const std::string title,
    const IVec2& pos,
    const IVec2& size)
{
    this->title = title;
    this->pos = pos;
    this->size = size;
    this->dispatcher = Dispatcher::GetCurrent();
}

WindowMacOS::~WindowMacOS() {
    Close();
}

void WindowMacOS::SetTitle(const std::string& title) {
    this->title = title;
    if (mainWindow) {
        mainWindow.title = [NSString stringWithUTF8String:title.c_str()];
    }
}

std::string WindowMacOS::GetTitle() const {
    return title;
}

void WindowMacOS::SetPos(const IVec2& pos)
{
    this->pos = pos;
    
    if(mainWindow) {
        auto screenFrame = [[mainWindow screen] frame];
        auto rect = NSMakeRect(pos.x, screenFrame.size.height - pos.y - size.y, size.x, size.y);
        rect = [mainWindow frameRectForContentRect:rect];
        [mainWindow setFrame:rect display:YES];
    }
}

IVec2 WindowMacOS::GetPos() const {
    return pos;
}

void WindowMacOS::SetSize(const IVec2& size) {
    this->size = size;
    if(this->mainWindow) {
        [this->mainWindow setContentSize:NSMakeSize(size.x, size.y)];
    }
}

IVec2 WindowMacOS::GetSize() const {
    return size;
}

bool WindowMacOS::IsVisible() const {
    return visible;
}

void WindowMacOS::SetResizeable(bool resizeable)
{
    this->resizeable = resizeable;

    if(mainWindow)
    {
        if(resizeable)
            mainWindow.styleMask |= NSWindowStyleMaskResizable;
        else
            mainWindow.styleMask &= ~NSWindowStyleMaskResizable;
    }
}

bool WindowMacOS::IsResizeable() const {
    return resizeable;
}

void WindowMacOS::Show()
{
    if (!mainWindow) {
        mainWindow = CreateNativeWindow(title, pos, size, resizeable);
    }

    [mainWindow setIsVisible:YES];
    visible = true;
}

void WindowMacOS::Hide() {
    [mainWindow setIsVisible:NO];
    visible = false;
}

void WindowMacOS::Close()
{
    if(mainWindow) {
        mainWindow.releasedWhenClosed = true;
        [mainWindow close];
        mainWindow = nil;
    }
}

uintptr_t WindowMacOS::GetHandle() const {
    return (uintptr_t)mainWindow;
}

MainWindow* WindowMacOS::CreateNativeWindow(
    const std::string title,
    const IVec2& pos,
    const IVec2& size,
    bool resizeable)
{
    NSRect rc = NSMakeRect(pos.x, pos.y, size.x, size.y);
    int style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable;
    if(resizeable) style |= NSWindowStyleMaskResizable;
    
    MainWindow* window = [[MainWindow alloc] initWithContentRect:rc];
    window->window = std::dynamic_pointer_cast<WindowMacOS>(shared_from_this());
    [window setTitle:[[NSString alloc] initWithUTF8String:title.c_str()]];
    [window makeKeyAndOrderFront:window];
    return window;
}

Keycode WindowMacOS::TranslateKey(int keycode)
{
    switch(keycode)
    {
    case kVK_ANSI_A: return Keycode::A;
    case kVK_ANSI_B: return Keycode::B;
    case kVK_ANSI_C: return Keycode::C;
    case kVK_ANSI_D: return Keycode::D;
    case kVK_ANSI_E: return Keycode::E;
    case kVK_ANSI_F: return Keycode::F;
    case kVK_ANSI_G: return Keycode::G;
    case kVK_ANSI_H: return Keycode::H;
    case kVK_ANSI_I: return Keycode::I;
    case kVK_ANSI_J: return Keycode::J;
    case kVK_ANSI_K: return Keycode::K;
    case kVK_ANSI_L: return Keycode::L;
    case kVK_ANSI_M: return Keycode::M;
    case kVK_ANSI_N: return Keycode::N;
    case kVK_ANSI_O: return Keycode::O;
    case kVK_ANSI_P: return Keycode::P;
    case kVK_ANSI_Q: return Keycode::Q;
    case kVK_ANSI_R: return Keycode::R;
    case kVK_ANSI_S: return Keycode::S;
    case kVK_ANSI_T: return Keycode::T;
    case kVK_ANSI_U: return Keycode::U;
    case kVK_ANSI_V: return Keycode::V;
    case kVK_ANSI_W: return Keycode::W;
    case kVK_ANSI_X: return Keycode::X;
    case kVK_ANSI_Y: return Keycode::Y;
    case kVK_ANSI_Z: return Keycode::Z;
    case kVK_ANSI_1: return Keycode::Num1;
    case kVK_ANSI_2: return Keycode::Num2;
    case kVK_ANSI_3: return Keycode::Num3;
    case kVK_ANSI_4: return Keycode::Num4;
    case kVK_ANSI_5: return Keycode::Num5;
    case kVK_ANSI_6: return Keycode::Num6;
    case kVK_ANSI_7: return Keycode::Num7;
    case kVK_ANSI_8: return Keycode::Num8;
    case kVK_ANSI_9: return Keycode::Num9;
    case kVK_ANSI_0: return Keycode::Num0;
    case 0xA6: return Keycode::Back;
    case 0xA7: return Keycode::Forward;
    case kVK_VolumeUp: return Keycode::VolumeUp;
    case kVK_VolumeDown: return Keycode::VolumeDown;
    case kVK_Mute: return Keycode::VolumeMute;
    case kVK_Space: return Keycode::Space;
    case kVK_Delete: return Keycode::Backspace;
    case kVK_ForwardDelete: return Keycode::Del;
    case kVK_LeftArrow: return Keycode::LeftArrow;
    case kVK_RightArrow: return Keycode::RightArrow;
    case kVK_UpArrow: return Keycode::UpArrow;
    case kVK_DownArrow: return Keycode::DownArrow;
    case kVK_Option: return Keycode::Alt;
    case kVK_Control: return Keycode::Ctrl;
    case kVK_Shift: return Keycode::Shift;
    case kVK_Return: return Keycode::Enter;
    case kVK_Escape: return Keycode::Escape;
    case kVK_ANSI_Equal: return Keycode::Equals;
    case kVK_ANSI_Minus: return Keycode::Minus;
    case kVK_ANSI_LeftBracket: return Keycode::LeftBracket;
    case kVK_ANSI_RightBracket: return Keycode::RightBracket;
    case kVK_ANSI_Quote: return Keycode::Quote;
    case kVK_ANSI_Semicolon: return Keycode::Semicolon;
    case kVK_ANSI_Backslash: return Keycode::Backslash;
    case kVK_ANSI_Comma: return Keycode::Comma;
    case kVK_ANSI_Slash: return Keycode::Slash;
    case kVK_ANSI_Period: return Keycode::Period;
    case kVK_ANSI_Grave: return Keycode::Grave;
    case kVK_Tab: return Keycode::Tab;
    }

    return (Keycode)((int)Keycode::Unknown + keycode);
}

gptr<HWRenderTarget> WindowMacOS::GetHWRenderTarget() {
    return surface;
}

} // system
} // mw

@implementation MainWindow
-(MainWindow*)initWithContentRect:(NSRect)contentRect
{
    auto screenFrame = [[NSScreen mainScreen] frame];
    contentRect.origin.y = screenFrame.size.height - contentRect.origin.y - contentRect.size.height;
    
    int style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable;
    self = [super initWithContentRect:contentRect styleMask:style backing:NSBackingStoreBuffered defer:NO];
    
    if(self) {
        [self setDelegate:self];
        [self setAcceptsMouseMovedEvents:YES];
    }
    
    return self;
}

-(BOOL)windowShouldClose:(NSWindow*)sender {
    auto sp = window.lock();
    if(sp) sp->OnClose();
    return NO;
}
-(void)windowDidResize:(NSNotification*)notification {
    NSWindow* nsWindow = notification.object;
    CGSize size = nsWindow.contentView.frame.size;
    auto sp = window.lock();
    if(sp) sp->OnResize(mw::IVec2(size.width, size.height));
}
-(void)mouseDown:(NSEvent*)theEvent {
    NSPoint pt = theEvent.locationInWindow;
    auto sp = window.lock();
    if(sp) sp->OnPointerDown(mw::IVec2(pt.x, pt.y), 0);
}
-(void)mouseMoved:(NSEvent*)theEvent {
    NSPoint pt = theEvent.locationInWindow;
    auto sp = window.lock();
    if(sp) sp->OnPointerMove(mw::IVec2(pt.x, pt.y), 0);
}
-(void)mouseDragged:(NSEvent*)theEvent {
    NSPoint pt = theEvent.locationInWindow;
    auto sp = window.lock();
    if(sp) sp->OnPointerMove(mw::IVec2(pt.x, pt.y), 0);
}
-(void)mouseUp:(NSEvent*)theEvent {
    NSPoint pt = theEvent.locationInWindow;
    auto sp = window.lock();
    if(sp) sp->OnPointerUp(mw::IVec2(pt.x, pt.y), 0);
}
-(void)rightMouseDown:(NSEvent*)theEvent {
    NSPoint pt = theEvent.locationInWindow;
    auto sp = window.lock();
    if(sp) sp->OnPointerDown(mw::IVec2(pt.x, pt.y), 1);
}
-(void)rightMouseUp:(NSEvent*)theEvent {
    NSPoint pt = theEvent.locationInWindow;
    auto sp = window.lock();
    if(sp) sp->OnPointerUp(mw::IVec2(pt.x, pt.y), 1);
}
-(void)keyDown:(NSEvent*)theEvent {
	auto code = mw::WindowMacOS::TranslateKey(theEvent.keyCode);
    auto sp = window.lock();
    if(sp) sp->OnKeyDown(code);
}
-(void)keyUp:(NSEvent*)theEvent {
	auto code = mw::WindowMacOS::TranslateKey(theEvent.keyCode);
    auto sp = window.lock();
    if(sp) sp->OnKeyUp(code);
}
-(void)flagsChanged:(NSEvent*)theEvent
{
    auto sp = window.lock();
    if(sp)
    {
        if([theEvent modifierFlags] & NSShiftKeyMask)
        {
            if(!sp->shiftPressed) {
                sp->shiftPressed = true;
                sp->OnKeyDown(mw::Keycode::Shift);
            }
        }
        else
        {
            if(sp->shiftPressed) {
                sp->shiftPressed = false;
                sp->OnKeyUp(mw::Keycode::Shift);
            }
        }
    }
}
@end
