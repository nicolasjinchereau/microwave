/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

import Microwave;
import Test.Game;
import <iostream>;
import <memory>;
import <array>;
import <fstream>;
import <set>;

using namespace mw;

namespace Test {

class TestApplication : public App
                      , public IDispatchHandler
{
public:
    gptr<GraphicsContext> graphics;
    gptr<AudioContext> audio;
    gptr<AssetLibrary> assetLibrary;
    gptr<Scene> scene;
    gptr<SceneRenderer> sceneRenderer;

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
            audio = AudioContext::New();
            AudioContext::SetCurrent(audio);

            //graphics = gpnew<GraphicsContext>(GraphicsDriverType::Direct3D11);
            graphics = gpnew<GraphicsContext>(GraphicsDriverType::OpenGL);
            GraphicsContext::SetCurrent(graphics);

            graphics->SetRenderTarget(GetMainWindow());
            
            // set up asset library
            auto rootDir = io::File::GetDefaultDataPath();
            assetLibrary = gpnew<AssetLibrary>(rootDir);
            SetAssetLibrary(assetLibrary);

            bool forceImport = false;
            auto assetDatabase = gpnew<AssetDatabase>(assetLibrary);
            assetDatabase->Refresh(forceImport);
            assetDatabase.reset();

            // set up scene
            scene = gpnew<Scene>();
            scene->GetRootNode()->AddChild()->AddComponent<Game>();

            sceneRenderer = gpnew<SceneRenderer>();

            Dispatcher::GetCurrent()->AddHandler(self(this));
            Dispatcher::GetCurrent()->SetContinuousDispatchRate(1000);
        }
        catch (const Exception& ex)
        {
            std::string msg = ex.what();
            writeln("initialization failed: ", msg);
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
        assetLibrary.reset();
        SetAssetLibrary(nullptr);
        GraphicsContext::SetCurrent(nullptr);
        AudioContext::SetCurrent(nullptr);
        audio.reset();
        graphics.reset();
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

gptr<TestApplication> app = gpnew<TestApplication>();

} // Test
