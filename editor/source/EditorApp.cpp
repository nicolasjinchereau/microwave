/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

import Microwave;
import Microwave.Editor.Toolbar;
import Microwave.Tools.SceneEditor;
import <cstdlib>;

namespace mw {
inline namespace editor {

class EditorApp : public App
                , public IDispatchHandler
{
public:
    gptr<GraphicsContext> graphics;
    gptr<AudioContext> audio;
    gptr<AssetLibrary> assetLibrary;
    gptr<SceneRenderer> sceneRenderer;

    // window scene
    gptr<Scene> windowScene;
    gptr<Camera> windowCamera;
    gptr<Toolbar> toolbar;
    gptr<ImageView> viewportImage;
    gptr<Texture> viewportTex;
    gptr<Canvas> windowCanvas;
    
    gptr<SceneEditorLib> editorLib;
    IEditor* editor = nullptr;

    // root of microwave repository path
    path microwavePath = "D:/Repositories/microwave";
    path projectPath = "D:/Repositories/microwave/app-proj";
    std::string projectName = "TestApp";

    virtual void OnInitialize(WindowConfig& config) override
    {
        config.title = "Microwave Editor";
        config.pos = IVec2(300, 300);
        config.size = IVec2(1024, 786);
        config.resizable = true;
    }

    virtual void OnStart() override
    {
        try
        {
            //audio = AudioContext::New();
            //AudioContext::SetCurrent(audio);
            
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

            // compile user code with microwave into editor.dll
            //editorLib = gpnew<SceneEditorLib>("editor.dll");
            //editor = editorLib->GetSceneEditor();

            // create scene representing window UI
            CreateWindowScene();
            
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

    void CreateWindowScene()
    {
        windowScene = gpnew<Scene>();
        auto rootNode = windowScene->GetRootNode();

        windowCamera = rootNode->AddChild()->AddComponent<Camera>();
        windowCamera->SetMode(CameraViewMode::OrthoFixedHeight);
        windowCamera->SetCullingMask(LayerMask::UI);
        windowCamera->SetRenderOrder(1);

        auto dpi = GetMainWindow()->GetDPI();
        auto unitsPerInch = 200;
        auto scaleFactor = (float)dpi / unitsPerInch;

        windowCanvas = rootNode->AddChild()->AddComponent<Canvas>();
        windowCanvas->SetCamera(windowCamera);
        windowCanvas->SetFitMode(FitMode::AdjustBoth);
        windowCanvas->SetScaleFactor(scaleFactor);

        toolbar = windowCanvas->GetNode()->AddChild()->AddComponent<Toolbar>();
        toolbar->AddButton("Textures/UI/LoadIcon.png", []{ writeln("Load"); });
        toolbar->AddButton("Textures/UI/SaveIcon.png", []{ writeln("Save"); });
        toolbar->AddButton("Textures/UI/CompileIcon.png", [this]{ CompileUserCode(); });

        viewportImage = windowCanvas->GetNode()->AddChild()->AddComponent<ImageView>();
        viewportImage->SetAnchor(Box(0, 1, 1, 0)); // left, top, right, bottom
        viewportImage->SetAnchorOffset(Box(0, -100, 0, 0));
        viewportImage->GetNode()->SetLayerMask(LayerMask::UI, true);

        windowScene->Update();

        auto sz = viewportImage->GetSize();
        viewportTex = gpnew<Texture>(PixelDataFormat::RGBA32, IVec2{ (int)sz.x, (int)sz.y }, true);
        viewportImage->SetTexture(viewportTex);
    }

    void CompileUserCode()
    {
        path sourcePath = projectPath / "source";
        path dataPath = projectPath / "data";
        path projPath = projectPath / "projects" / "windows";

        // set up project folder with 'source', 'data', 'project' folders
        if (!path::exists(projectPath)) path::create_directories(projectPath);
        if (!path::exists(sourcePath))  path::create_directories(sourcePath);
        if (!path::exists(dataPath))    path::create_directories(dataPath);
        if (!path::exists(projPath))    path::create_directories(projPath);

        // generate project file for DLL using premake as 'projectPath'
        path templatesPath = microwavePath / "templates";
        
        std::string premakeFile = File::ReadAllText(templatesPath / "premake5.lua");
        premakeFile = ReplaceAll(premakeFile, "${PROJECT_NAME}", projectName);
        premakeFile = ReplaceAll(premakeFile, "${PROJECT_PATH}", projectPath.string());
        premakeFile = ReplaceAll(premakeFile, "${MICROWAVE_PATH}", microwavePath.string());
        File::WriteAllText(projectPath / "premake5.lua", premakeFile);
        
        path buildDylibScriptPath = projectPath / "build-dylib.cmd";
        std::string buildDylibFile = File::ReadAllText(templatesPath / "build-dylib.cmd");
        buildDylibFile = ReplaceAll(buildDylibFile, "${PROJECT_NAME}", projectName);
        buildDylibFile = ReplaceAll(buildDylibFile, "${MICROWAVE_PATH}", microwavePath.string());
        File::WriteAllText(buildDylibScriptPath, buildDylibFile);

        std::string cmd = "cd " + projectPath.string() + " && " + buildDylibScriptPath.string();
        ::system(cmd.c_str());

        // LOAD DLL
        path dylibPath = projectPath / "bin" / "Win32" / "Debug" / (projectName + ".dll");
        editorLib = gpnew<SceneEditorLib>(dylibPath.string());
        editor = editorLib->GetSceneEditor();
    }

    virtual void OnDispatch() override
    {
        auto windowSize = GetMainWindow()->GetSize();

        windowScene->Update();

        if(!viewportTex)
        {
            auto sz = viewportImage->GetSize();
            viewportTex = gpnew<Texture>(PixelDataFormat::RGBA32, IVec2{ (int)sz.x, (int)sz.y }, true);
            viewportImage->SetTexture(viewportTex);
            graphics->SetViewport(windowSize);
        }

        if (editor)
        {
            editor->SetViewportSize(windowSize.x, windowSize.y);
            editor->Render();
            //editor->GetPixels(/* viewport buffer */);
            // viewportTex->SetPixels(/* viewport buffer */);
            // upload pixels to viewport texture...
        }

        sceneRenderer->Render(windowScene);
    }

    virtual void OnActivate() override {
        
    }
    
    virtual void OnDeactivate() override {
        
    }
    
    virtual void OnQuit() override
    {
        editorLib.reset();
        editor = nullptr;
        windowScene.reset();
        assetLibrary.reset();
        SetAssetLibrary(nullptr);
        GraphicsContext::SetCurrent(nullptr);
        AudioContext::SetCurrent(nullptr);
        audio.reset();
        graphics.reset();
    }

    // WindowEventHandler overrides
    virtual void OnResize(Window* window, IVec2 size) override
    {
        viewportTex = nullptr;
    }

    virtual void OnClose(Window* window) override {
        Dispatcher::GetCurrent()->Quit();
    }

    virtual void OnKeyDown(Window* window, Keycode key) override {
        windowScene->SendKeyDown(window, key);
    }

    virtual void OnKeyUp(Window* window, Keycode key) override {
        windowScene->SendKeyUp(window, key);
    }

    virtual void OnPointerDown(Window* window, IVec2 pos, int id) override {
        windowScene->SendPointerDown(window, pos, id);
    }

    virtual void OnPointerMove(Window* window, IVec2 pos, int id) override {
        windowScene->SendPointerMove(window, pos, id);
    }

    virtual void OnPointerUp(Window* window, IVec2 pos, int id) override {
        windowScene->SendPointerUp(window, pos, id);
    }
};

gptr<EditorApp> app = gpnew<EditorApp>();

} // editor
} // mw
