/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Internal.WindowIOS;
import Microwave.Graphics.Internal.HWSurfaceEAGL;
import <stdexcept>;
import <vector>;

@interface MainView : UIView
{
    
}
@end

@interface ViewController : UIViewController
{
    @public MainWindow* mainWindow;
}
@end

namespace mw {
inline namespace system {

sptr<Window> Window::New(const std::string title, const IVec2& pos, const IVec2& size) {
    return spnew<WindowIOS>();
}

WindowIOS::WindowIOS()
{
    auto screen = [UIScreen mainScreen];
    auto scale = screen.nativeScale;
    CGSize screenSize = screen.bounds.size;
    size.x = math::RoundToInt(screenSize.width * scale);
    size.y = math::RoundToInt(screenSize.height * scale);
    
    this->dispatcher = Dispatcher::GetCurrent();
}

WindowIOS::~WindowIOS() {
    Close();
}

void WindowIOS::SetTitle(const std::string& title) {
    this->title = title;
//    if (mainWindow) {
//        mainWindow.title = [NSString stringWithUTF8String:title.c_str()];
//    }
}

std::string WindowIOS::GetTitle() const {
    return title;
}

void WindowIOS::SetPos(const IVec2& pos)
{
    this->pos = pos;
    
    if(mainWindow) {
//        auto screenFrame = [[mainWindow screen] frame];
//        auto rect = NSMakeRect(pos.x, screenFrame.size.height - pos.y - size.y, size.x, size.y);
//        rect = [mainWindow frameRectForContentRect:rect];
//        [mainWindow setFrame:rect display:YES];
    }
}

IVec2 WindowIOS::GetPos() const {
    return pos;
}

void WindowIOS::SetSize(const IVec2& size)
{
    this->size = size;
    
    if(surface)
    {
        auto nativeSurface = mw::sptr_cast<mw::gfx::WindowSurfaceIOSOpenGL>(surface);
        nativeSurface->UpdateSize();
    }

    RaiseEvent(&WindowDelegate::OnResize, size);
}

IVec2 WindowIOS::GetSize() const {
    return size;
}

bool WindowIOS::IsVisible() const {
    return visible;
}

void WindowIOS::SetResizeable(bool resizeable)
{
    this->resizeable = resizeable;

    if(mainWindow)
    {
//        if(resizeable)
//            mainWindow.styleMask |= NSWindowStyleMaskResizable;
//        else
//            mainWindow.styleMask &= ~NSWindowStyleMaskResizable;
    }
}

bool WindowIOS::IsResizeable() const {
    return resizeable;
}

void WindowIOS::Show()
{
    if (!mainWindow) {
        mainWindow = CreateNativeWindow();
    }

    //[mainWindow setIsVisible:YES];
    visible = true;
}

void WindowIOS::Hide() {
    //[mainWindow setIsVisible:NO];
    //visible = false;
}

void WindowIOS::Close()
{
//    if(mainWindow) {
//        mainWindow.releasedWhenClosed = true;
//        [mainWindow close];
//        mainWindow = nil;
//    }
}

uintptr_t WindowIOS::GetHandle() const {
    return (uintptr_t)mainWindow;
}

MainWindow* WindowIOS::CreateNativeWindow()
{
    auto screen = [UIScreen mainScreen];
    
    MainWindow* window = [[MainWindow alloc] initWithFrame:screen.bounds];
    window->window = std::dynamic_pointer_cast<WindowIOS>(shared_from_this());
    window.contentScaleFactor = screen.nativeScale;
    
    window->viewController = [[ViewController alloc] init];
    window->viewController->mainWindow = window;
    [window setRootViewController:window->viewController];
    
    window->eaglLayer = (CAEAGLLayer*)window->viewController.view.layer;
    window->eaglLayer.frame = screen.bounds;
    window->eaglLayer.contentsScale = screen.nativeScale;
    
    [window makeKeyAndVisible];
    
    return window;
}

MainView* WindowIOS::CreateNativeView()
{
    auto screen = [UIScreen mainScreen];
    
    auto view = [[MainView alloc] initWithFrame:screen.bounds];
    view.contentScaleFactor = screen.nativeScale;
    
    return view;
}

Keycode WindowIOS::TranslateKey(int keycode) {
    return Keycode::Space;
}

sptr<HWRenderTarget> WindowIOS::GetHWRenderTarget() {
    return surface;
}

} // system
} // mw

@implementation MainView
+(Class)layerClass {
    return [CAEAGLLayer class];
}
@end


@implementation ViewController
- (void)loadView
{
    auto window = self->mainWindow->window.lock();
    self.view = window->CreateNativeView();
}
- (void)viewDidLoad {
    mainWindow->orientation = [UIApplication sharedApplication].statusBarOrientation;
    [[UIApplication sharedApplication] setStatusBarHidden:YES];
}
- (void)viewDidAppear:(BOOL)animated {
    // ...
}
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return false;
}
- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator
{
    [coordinator animateAlongsideTransition:nil
                                 completion:^(id<UIViewControllerTransitionCoordinatorContext> context)
    {
        UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
        
        auto window = mainWindow->window.lock();
        if(window)
        {
            CGRect frame = [[UIScreen mainScreen] nativeBounds];
            window->SetSize(mw::IVec2(frame.size.width, frame.size.height));
        }
    }];
}
@end


@implementation MainWindow
@end
