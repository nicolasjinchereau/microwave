/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/Math/Math.h>
#include <MW/Graphics/Font.h>
#include <MW/Scene/Components/AudioSource.h>
#include <MW/Scene/Components/Canvas.h>
#include <MW/Scene/Components/DirectionalLight.h>
#include <MW/Scene/Components/ImageView.h>
#include <MW/Scene/Components/MeshRenderer.h>
#include <MW/Scene/Components/RigidBody.h>
#include <MW/Scene/Components/Script.h>
#include <MW/Scene/Components/TextView.h>
#include <MW/Scene/Node.h>
#include <MW/Scene/PhysicsWorld.h>
#include <MW/Scene/Scene.h>
#include "CameraController.h"
#include "Coin.h"
#include "Player.h"
#include "SpinningGear.h"
#include "BatteryMeter.h"
#include "BigDoors.h"
#include "ExitDoorTrigger.h"
#include "CoinCounter.h"
#include "WinScreen.h"
#include "LossScreen.h"

using namespace mw;

class Game : public Script
           , public IUserEvents
           , public ISceneInputEvents
{
public:
    sptr<Camera> camera;
    sptr<Camera> uiCamera;
    
    sptr<Node> level;
    std::vector<sptr<Node>> coins;
    sptr<Player> player;
    sptr<Node> playerStart;
    sptr<BigDoors> exitDoor;

    sptr<Canvas> canvas;
    sptr<View> gamePanel;
    sptr<TextView> loadingText;
    sptr<CoinCounter> coinCounter;
    sptr<BatteryMeter> batteryMeter;
    sptr<WinScreen> winScreen;
    sptr<LossScreen> lossScreen;
    sptr<AudioSource> music;
    sptr<AudioSource> levelPassSound;
    sptr<AudioSource> levelFailSound;

    float startTime = 0;
    bool playing = false;

    virtual void Start() override
    {
        LoadAllAsync();
    }

    Task<void> LoadAllAsync()
    {
        using namespace std::literals::chrono_literals;

        co_await InitUI();
        co_await Task<void>::Delay(20ms); // let UI update
        GetNode()->GetScene()->SetUpdateEnabled(false);
        
        co_await InitLevel();
        co_await InitGameUI();
        co_await InitAudio();

        uiCamera->GetNode()->SetActive(false);
        loadingText->GetNode()->SetActive(false);
        gamePanel->GetNode()->SetActive(true);
        GetNode()->GetScene()->SetUpdateEnabled(true);
        co_await Task<void>::Delay(20ms); // let UI update

        camera->GetNode()->SetActive(true);
        uiCamera->GetNode()->SetActive(true);

        startTime = GetClock()->GetTime();
        playing = true;
    }

    Task<void> InitUI()
    {
        auto scene = GetNode()->GetScene();
        auto assetLibrary = App::Get()->GetAssetLibrary();

        // set up UI camera
        uiCamera = scene->GetRootNode()->AddChild()->AddComponent<Camera>();
        uiCamera->SetMode(CameraViewMode::OrthoFixedHeight);
        uiCamera->SetCullingMask(LayerMask::UI);
        uiCamera->SetRenderOrder(1);

        canvas = scene->GetRootNode()->AddChild()->AddComponent<Canvas>();
        canvas->SetReferenceSize(Vec2(2048, 1536));
        canvas->SetCamera(uiCamera);

        auto font = co_await assetLibrary->GetAssetAsync<Font>("Fonts/comic_bold.ttf");
        loadingText = canvas->GetNode()->AddChild()->AddComponent<TextView>();
        loadingText->SetFont(font);
        loadingText->SetText("Loading...");
        loadingText->SetFontSize(80);
        loadingText->SetAnchor(Box(0, 1, 1, 0)); // middle left
        loadingText->SetAnchorOffset(Box(0, 0, 0, 0));
        loadingText->SetAlignment(TextAlign::MiddleCenter);

        loadingText->GetNode()->SetLayerMask(LayerMask::UI, true);
    }

    Task<void> InitLevel()
    {
        auto scene = GetNode()->GetScene();
        auto assetLibrary = App::Get()->GetAssetLibrary();

        auto levelModel = co_await assetLibrary->GetAssetAsync<Node>("Models/Level01.fbx");
        auto bigDoorsModel = co_await assetLibrary->GetAssetAsync<Node>("Models/BigDoors.fbx");

        level = Instantiate<Node>(levelModel);
        scene->GetRootNode()->AddChild(level);

        auto exitDoorTransform = level->GetChild("ExitDoorTransform");
        
        auto exitDoorNode = Instantiate<Node>(bigDoorsModel);
        exitDoorNode->SetParent(exitDoorTransform, false);
        exitDoor = exitDoorNode->AddComponent<BigDoors>();
        
        auto exitDoorTriggerNode = level->GetChild("ExitDoorTrigger");
        auto exitDoorTrigger = exitDoorTriggerNode->AddComponent<ExitDoorTrigger>();
        exitDoorTrigger->exitDoor = exitDoor;
        exitDoorTrigger->game = This<Game>();

        coins = level->FindChildren("Coin", true);
        for (auto& coin : coins) {
            auto c = coin->AddComponent<Coin>();
            c->game = This<Game>();
        }

        auto gears = level->FindChildren("SpinningGear", true);
        for (auto& gear : gears) {
            gear->AddComponent<SpinningGear>();
        }

        // set up lighting
        scene->SetAmbientColor({ 0.8392157f, 0.7098039f, 0.7921569f, 0.0f });
        auto lightNode = scene->GetRootNode()->AddChild();
        lightNode->SetRotation(50.0f, -30.0f, 0);
        auto light = lightNode->AddComponent<DirectionalLight>();
        light->SetColor({ 0.7686275f, 0.5490196f, 0.5490196f, 0.0f });
        light->SetIntensity(1.0f);
        
        // set up main camera
        auto camNode = spnew<Node>();
        camNode->SetLocalPosition(2, 4, -8);
        camNode->LookAt(Vec3(2, 1, 0));
        camera = camNode->AddComponent<Camera>();
        camera->SetMode(CameraViewMode::Perspective);
        camera->SetCullingMask(LayerMask::Default);
        camera->SetRenderOrder(0);
        auto camController = camNode->AddComponent<CameraController>();
        scene->GetRootNode()->AddChild(camNode);
        camera->GetNode()->SetActive(false);

        // set up player
        auto playerNode = scene->GetRootNode()->AddChild(spnew<Node>());
        player = playerNode->AddComponent<Player>();
        co_await player->InitAsync();

        camController->player = playerNode;
        playerStart = level->GetChild("PlayerStart");
        playerNode->SetPosition(playerStart->GetPosition());
        playerNode->SetRotation(Quat::LookRotation(Vec3::Right(), Vec3::Up()));

        // enable gravity
        scene->GetPhysics()->SetGravity(mw::Vec3(0, -30.0f, 0));
    }

    Task<void> InitGameUI()
    {
        auto scene = GetNode()->GetScene();
        auto assetLibrary = App::Get()->GetAssetLibrary();
        auto canvasNode = canvas->GetNode();

        gamePanel = canvas->GetNode()->AddChild()->AddComponent<View>();
        gamePanel->SetAnchor(Box(0, 1, 1, 0));
        gamePanel->SetAnchorOffset(Box(0, 0, 0, 0));
        gamePanel->GetNode()->SetActive(false);

        coinCounter = gamePanel->GetNode()->AddChild()->AddComponent<CoinCounter>();
        co_await coinCounter->InitAsync();

        batteryMeter = gamePanel->GetNode()->AddChild()->AddComponent<BatteryMeter>();
        batteryMeter->player = player;
        co_await batteryMeter->InitAsync();

        winScreen = gamePanel->GetNode()->AddChild()->AddComponent<WinScreen>();
        winScreen->game = This<Game>();
        co_await winScreen->InitAsync();
        winScreen->GetNode()->SetActive(false);
        
        lossScreen = gamePanel->GetNode()->AddChild()->AddComponent<LossScreen>();
        lossScreen->game = This<Game>();
        co_await lossScreen->InitAsync();
        lossScreen->GetNode()->SetActive(false);

        gamePanel->GetNode()->SetLayerMask(LayerMask::UI, true);
    }

    Task<void> InitAudio()
    {
        auto assetLibrary = App::Get()->GetAssetLibrary();
        auto scene = GetNode()->GetScene();

        auto song = co_await assetLibrary->GetAssetAsync<AudioClip>("Audio/Music/Gameplay.mp3");
        auto levelPassClip = co_await assetLibrary->GetAssetAsync<AudioClip>("Audio/Sounds/LevelPass.wav");
        auto levelFailClip = co_await assetLibrary->GetAssetAsync<AudioClip>("Audio/Sounds/LevelFail.wav");

        // set up audio
        auto musicNode = spnew<Node>();
        music = musicNode->AddComponent<AudioSource>();
        scene->GetRootNode()->AddChild(musicNode);
        music->SetClip(song);
        music->SetLooping(true);
        music->Play();

        levelPassSound = GetNode()->AddComponent<AudioSource>();
        levelPassSound->SetClip(levelPassClip);

        levelFailSound = GetNode()->AddComponent<AudioSource>();
        levelFailSound->SetClip(levelFailClip);
    }

    void ResetGame()
    {
        auto scene = GetNode()->GetScene();
        scene->GetRootNode()->ClearChildren();
        scene->GetRootNode()->AddChild()->AddComponent<Game>();
    }

    virtual void Update() override
    {
        if (!playing)
            return;

        if(!player->alive)
        {
            playing = false;
            StartCoroutine(EndGameAsLoss());
        }
    }

    virtual void OnKeyDown(Keycode key) override
    {
        if (!playing)
            return;

        if(key == Keycode::G)
        {
            GetScene()->SetGizmosEnabled(!GetScene()->GetGizmosEnabled());
        }
        else if(key == Keycode::R)
        {
            ResetGame();
        }
        else if (key == Keycode::K)
        {
            player->batteryLevel = 0;
        }
    }

    void OnPlayerExit()
    {
        if(playing)
        {
            playing = false;
            StartCoroutine(EndGameAsWin());
        }
    }

    Coroutine EndGameAsWin()
    {
        float endTime = GetClock()->GetTime();
        float completionTime = endTime - startTime;

        player->SetEnabled(false);
        player->anim->CrossFade("Idle");
        exitDoor->CloseDoors();

        music->Stop();

        co_yield Wait::Seconds(1.0f);
        levelPassSound->Play();
        co_yield Wait::Seconds(0.5f);

        winScreen->SetCoinCount(coinCounter->GetCoinsCollected());
        winScreen->SetCompletionTime(completionTime);
        
        auto node = winScreen->GetNode();
        node->SetActive(true);

        float now = GetClock()->GetTime();
        float scaleStartTime = now;
        float scaleEndTime = scaleStartTime + 0.5f;

        while(now < scaleEndTime)
        {
            now = GetClock()->GetTime();

            float t = (now - scaleStartTime) / (scaleEndTime - scaleStartTime);
            t = math::Clamp01(t);
            t = 1.0f - (1.0f - t) * (1.0f - t);
            node->SetLocalScale(Vec3::One() * t);

            co_yield Wait::Next();
        }
        
        node->SetLocalScale(Vec3::One());
    }

    Coroutine EndGameAsLoss()
    {
        music->Stop();

        co_yield Wait::Seconds(1.0f);
        levelFailSound->Play();
        co_yield Wait::Seconds(0.5f);

        auto node = lossScreen->GetNode();
        node->SetActive(true);

        float now = GetClock()->GetTime();
        float scaleStartTime = now;
        float scaleEndTime = scaleStartTime + 0.5f;

        while (now < scaleEndTime)
        {
            now = GetClock()->GetTime();

            float t = (now - scaleStartTime) / (scaleEndTime - scaleStartTime);
            t = math::Clamp01(t);
            t = 1.0f - (1.0f - t) * (1.0f - t);
            node->SetLocalScale(Vec3::One() * t);

            co_yield Wait::Next();
        }

        node->SetLocalScale(Vec3::One());
    }

    void OnCoinCollected(Coin* coin)
    {
        auto coinNode = coin->GetNode();

        Vec2 canvasPos = canvas->WorldToCanvasPos(coinNode->GetPosition(), camera);
        coinCounter->AnimateCollectedCoin(canvasPos);

        coinNode->SetParent(nullptr);
    }
};
