/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

import Microwave;
import Test.Game;
import <iostream>;
import <memory>;
import <array>;
import <fstream>;

using namespace mw;

namespace Test {

class TestApplication : public App
                      , public IDispatchHandler
{
public:
    sptr<GraphicsContext> graphics;
    sptr<AudioContext> audio;
    sptr<Window> mainWindow;
    sptr<WindowSurface> surface;
    sptr<AssetLibrary> assetLibrary;
    sptr<Scene> scene;
    sptr<SceneRenderer> sceneRenderer;
    sptr<Mesh> dummy;

    virtual void OnInitialize(AppConfig& config) override
    {
        config.windowTitle = "Microwave";
        config.windowPos = IVec2(300, 300);
        config.windowSize = IVec2(1024, 576);
    }

    virtual void OnStart() override
    {
        try
        {
            // set up graphics
            mainWindow = GetMainWindow();
            auto windowSize = mainWindow->GetSize();
            IntRect rc(0, 0, windowSize.x, windowSize.y);

            audio = AudioContext::New();
            AudioContext::SetCurrent(audio);

            //graphics = GraphicsContext::New(GraphicsDriverType::Direct3D11);
            graphics = GraphicsContext::New(GraphicsDriverType::OpenGL);
            graphics->CreateWindowSurface(mainWindow);
            graphics->SetSwapInterval(1);
            graphics->SetRenderTarget(mainWindow->GetSurface());
            graphics->SetViewport(rc);
            graphics->SetBlendingEnabled(true);
            SetGraphics(graphics);

            // set up asset library
            auto rootDir = io::File::GetDefaultDataPath();
            assetLibrary = spnew<AssetLibrary>(rootDir);
            SetAssetLibrary(assetLibrary);

            bool forceImport = false;
            auto assetDatabase = spnew<AssetDatabase>(assetLibrary);
            assetDatabase->Refresh(forceImport);
            assetDatabase.reset();

            // set up scene
            scene = spnew<Scene>();
            scene->GetRootNode()->AddChild()->AddComponent<Game>();

            sceneRenderer = spnew<SceneRenderer>();

            Dispatcher::GetCurrent()->AddHandler(This<IDispatchHandler>());
            Dispatcher::GetCurrent()->SetContinuousDispatchRate(1000);
        }
        catch (std::exception& ex)
        {
            std::string msg = ex.what();
            Console::WriteLine("initialization failed: %", msg);
        }
    }
    
    virtual void OnDispatch() override
    {
        scene->Update();
        sceneRenderer->Render(scene);
    }

    virtual void OnActivate() override {
        
    }
    
    virtual void OnDeactivate() override {
        
    }
    
    virtual void OnQuit() override {
        scene.reset();
    }

    // WindowEventHandler overrides
    virtual void OnClose(Window* window) override {
        Dispatcher::GetCurrent()->Quit();
    }

    virtual void OnKeyDown(Window* window, Keycode key) override {
        scene->SendKeyDown(window, key);
    }

    virtual void OnKeyUp(Window* window, Keycode key) override {
        scene->SendKeyUp(window, key);
    }

    virtual void OnPointerDown(Window* window, IVec2 pos, int id) override {
        scene->SendPointerDown(window, pos, id);
    }

    virtual void OnPointerMove(Window* window, IVec2 pos, int id) override {
        scene->SendPointerMove(window, pos, id);
    }

    virtual void OnPointerUp(Window* window, IVec2 pos, int id) override {
        scene->SendPointerUp(window, pos, id);
    }
};

sptr<TestApplication> app = spnew<TestApplication>();

} // Test
