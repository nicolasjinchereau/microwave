/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Tools.SceneEditor;
import Microwave.Audio.AudioContext;
import Microwave.Data.Library.AssetLibrary;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.RenderTexture;
import Microwave.IO.File;
import Microwave.SceneGraph.Components.Camera;
import Microwave.SceneGraph.Node;
import Microwave.SceneGraph.Scene;
import Microwave.SceneGraph.SceneRenderer;
import Microwave.System.Pointers;
import Microwave.Tools.DynamicLibrary;
import Microwave.Tools.IEditor;
import <MW/System/Internal/PlatformHeaders.h>;

extern "C" {
    PLATFORM_EXPORT mw::IEditor* PLATFORM_CALL MWGetSceneEditor();
}

export namespace mw {
inline namespace tools {

class SceneEditor : public IEditor
{
    gptr<GraphicsContext> graphics;
    gptr<AudioContext> audio;
    gptr<AssetLibrary> assetLibrary;
    gptr<SceneRenderer> sceneRenderer;

    gptr<RenderTexture> worldTex;
    gptr<Scene> scene;
    gptr<Camera> worldCamera;
public:

    SceneEditor()
    {
        graphics = gpnew<GraphicsContext>(GraphicsDriverType::Direct3D11);
        GraphicsContext::SetCurrent(graphics);

        worldTex = gpnew<RenderTexture>(IVec2{ 800, 600 });
        graphics->SetRenderTarget(worldTex);

        // set up asset library
        auto rootDir = io::File::GetDefaultDataPath();
        assetLibrary = gpnew<AssetLibrary>(rootDir);

        //bool forceImport = false;
        //auto assetDatabase = gpnew<AssetDatabase>(assetLibrary);
        //assetDatabase->Refresh(forceImport);
        //assetDatabase.reset();

        // set up scene for content of 3D world
        scene = gpnew<Scene>();
        scene->SetAmbientColor({ 0.5f, 0.5f, 0.5f, 0.0f });

        worldCamera = scene->GetRootNode()->AddChild()->AddComponent<Camera>();
        worldCamera->SetMode(CameraViewMode::Perspective);
        worldCamera->SetCullingMask(LayerMask::Default);
        worldCamera->SetRenderOrder(0);
        worldCamera->GetNode()->SetPosition(Vec3(0, 5, -10));
        worldCamera->GetNode()->LookAt(Vec3(0, 3, 0));

        //gptr<Node> playerModelAsset = assetLibrary->GetAsset<Node>("Models/Player.fbx");
        //gptr<Node> playerModel = Instantiate<Node>(playerModelAsset);
        //scene->GetRootNode()->AddChild(playerModel);
        //playerModel->SetLocalPosition(Vec3::Zero());
        //playerModel->SetLocalRotation(Quat(0, 0, 0));

        sceneRenderer = gpnew<SceneRenderer>();
    }

    virtual ~SceneEditor() {
        graphics.reset();
    }

    virtual void SetViewportSize(int width, int height) override
    {
        // update size of render texture
        // call SetRenderTarget
    }

    virtual void Render() override
    {
        scene->Update();
        sceneRenderer->Render(scene);
    }

    virtual void GetPixels(std::byte* pBuffer, size_t size) override
    {
        // get pixels from render texture into pBuffer
    }

    static IEditor& Get() {
        static SceneEditor editor;
        return editor;
    }
};

class SceneEditorLib
{
    typedef decltype(&MWGetSceneEditor) MWGetSceneEditorFunc;

    MWGetSceneEditorFunc getFunc;
    mw::sptr<mw::DynamicLibrary> dylib;

public:
    SceneEditorLib(const std::string& libFilename) {
        dylib = mw::spnew<mw::DynamicLibrary>(libFilename);
        dylib->GetFunction("MWGetSceneEditor", getFunc);
    }

    mw::IEditor* GetSceneEditor() {
        return getFunc();
    }
};

} // tools
} // mw
