/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Components.Camera;
import Microwave.SceneGraph.Components.Component;
import Microwave.SceneGraph.Events;
import Microwave.SceneGraph.LayerMask;
import Microwave.Math;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <array>;
import <cstddef>;
import <cstdint>;
import <span>;

export namespace mw {
inline namespace scene {

enum class CameraViewMode : int
{
    OrthoFixedWidth,
    OrthoFixedHeight,
    Perspective
};

struct FrustumCorner
{
    constexpr static int NearTopLeft = 0;
    constexpr static int NearTopRight = 1;
    constexpr static int NearBottomLeft = 2;
    constexpr static int NearBottomRight = 3;
    constexpr static int FarTopLeft = 4;
    constexpr static int FarTopRight = 5;
    constexpr static int FarBottomLeft = 6;
    constexpr static int FarBottomRight = 7;
};

struct FrustumPlane
{
    constexpr static int Left = 0;
    constexpr static int Right = 1;
    constexpr static int Top = 2;
    constexpr static int Bottom = 3;
    constexpr static int Near = 4;
    constexpr static int Far = 5;
};

class Camera : public Component
{
    inline static Type::Pin<Camera> pin;
public:
    Camera() {
        SetEventBit(EventBit::Camera);
    }

    void SetMode(CameraViewMode mode);
    CameraViewMode GetMode() const;

    void SetFixedWidth(float width);
    float GetFixedWidth() const;

    void SetFixedHeight(float height);
    float GetFixedHeight() const;

    void SetFOV(float fovY);
    float GetFOV() const;

    void SetNearPlane(float znear);
    float GetNearPlane() const;

    void SetFarPlane(float zfar);
    float GetFarPlane() const;

    void SetCullingMask(LayerMask mask);
    LayerMask GetCullingMask() const;

    void SetRenderOrder(uint32_t order);
    uint32_t GetRenderOrder() const;

    const Mat4& GetViewProjectionMatrix() const;
    const Mat4& GetProjectionMatrix() const;
    const Plane& GetFrustumPlane(int p) const;

    bool CanSee(const std::span<Vec3>& vertices) const;
    bool CanSee(const Sphere& sphere) const;
    bool CanSee(const AABox& bbox) const;

    Ray ScreenPointToRay(const Vec3& point) const;
    Ray ScreenPointToRay(float x, float y) const;

    Vec3 WorldToScreen(const Vec3& position) const;
    Vec3 ScreenToWorld(const Vec3& point) const;

    float GetDepth(const Vec3& pos) const;
    
    static sptr<Camera> GetCurrentCamera();
    static void SetCurrentCamera(const wptr<Camera>& cam);
    static void SetCurrentCamera(std::nullptr_t);

    // Component overrides
    virtual void OnTransformChanged() override;

protected:

    void SetDirty() const;
    void UpdateView() const;

    static wptr<Camera> _currentCamera;

    mutable bool _dirty = true;
    mutable Mat4 _mtxProjection;
    mutable Mat4 _mtxVP;
    mutable IntRect _vp;
    mutable std::array<Vec3, 8> _frustumCorners;
    mutable std::array<Plane, 6> _frustumPlanes;

    CameraViewMode _mode = CameraViewMode::OrthoFixedHeight;
    LayerMask _cullingMask = LayerMask::All;
    int _renderOrder = 0;

    float _fixedWidth = 100.0f;
    float _fixedHeight = 100.0f;
    float _fov = 60.0f;
    float _nearPlane = 0.1f;
    float _farPlane = 1000.0f;
};

}
}
