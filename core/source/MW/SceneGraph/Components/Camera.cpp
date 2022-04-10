/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.SceneGraph.Components.Camera;
import Microwave.Graphics.RenderTarget;
import Microwave.Graphics.GraphicsContext;
import Microwave.SceneGraph.LayerMask;
import Microwave.SceneGraph.Node;
import Microwave.System.App;
import <array>;
import <cstddef>;
import <cstdint>;
import <ranges>;
import <span>;

namespace mw {
inline namespace scene {

wptr<Camera> Camera::_currentCamera;

//Camera::Camera()
//{
//    SetEventBit(EventBit::Camera);
//}

void Camera::SetMode(CameraViewMode mode)
{
    _mode = mode;
    SetDirty();
}

CameraViewMode Camera::GetMode() const {
    return _mode;
}

void Camera::SetFixedWidth(float width) {
    _fixedWidth = width;
    SetDirty();
}

float Camera::GetFixedWidth() const {
    return _fixedWidth;
}

void Camera::SetFixedHeight(float height) {
    _fixedHeight = height;
    SetDirty();
}

float Camera::GetFixedHeight() const {
    return _fixedHeight;
}

void Camera::SetFOV(float fovY) {
    _fov = fovY;
    SetDirty();
}

float Camera::GetFOV() const {
    return _fov;
}

void Camera::SetNearPlane(float znear) {
    _nearPlane = znear;
    SetDirty();
}

float Camera::GetNearPlane() const {
    return _nearPlane;
}

void Camera::SetFarPlane(float zfar)
{
    _farPlane = zfar;
    SetDirty();
}

float Camera::GetFarPlane() const {
    return _farPlane;
}

void Camera::SetCullingMask(LayerMask mask) {
    _cullingMask = mask;
}

LayerMask Camera::GetCullingMask() const {
    return _cullingMask;
}

void Camera::SetRenderOrder(uint32_t order) {
    _renderOrder = order;
}

uint32_t Camera::GetRenderOrder() const {
    return _renderOrder;
}

const Mat4& Camera::GetViewProjectionMatrix() const {
    UpdateView();
    return _mtxVP;
}

const Mat4& Camera::GetProjectionMatrix() const {
    UpdateView();
    return _mtxProjection;
}

const Plane& Camera::GetFrustumPlane(int p) const {
    UpdateView();
    return _frustumPlanes[p];
}

bool Camera::CanSee(const std::span<Vec3>& vertices) const
{
    UpdateView();

    for (int p = 0; p < 6; p++)
    {
        bool inside = false;

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            if (_frustumPlanes[p].InFront(vertices[i]))
            {
                inside = true;
                break;
            }
        }

        if (!inside)
            return false;
    }

    return true;
}

bool Camera::CanSee(const Sphere& sphere) const
{
    UpdateView();

    for (int p = 0; p < 6; p++)
    {
        if (_frustumPlanes[p].InBack(sphere))
            return false;
    }

    return true;
}

bool Camera::CanSee(const AABox& bbox) const
{
    UpdateView();

    std::array<Vec3, 8> corners;
    bbox.GetCorners(corners);
    
    using std::ranges::all_of;

    for (auto& plane : _frustumPlanes)
    {
        if(all_of(corners, [&](auto& c){ return !plane.InFront(c); }))
            return false;
    }

    Vec3 vmin = bbox.GetMin();
    Vec3 vmax = bbox.GetMax();
    
    if(all_of(_frustumCorners, [&](auto& c){ return c.x < vmin.x; }))
        return false;

    if(all_of(_frustumCorners, [&](auto& c){ return c.x > vmax.x; }))
        return false;

    if(all_of(_frustumCorners, [&](auto& c){ return c.y < vmin.y; }))
        return false;

    if(all_of(_frustumCorners, [&](auto& c){ return c.y > vmax.y; }))
        return false;

    if(all_of(_frustumCorners, [&](auto& c){ return c.z < vmin.z; }))
        return false;

    if(all_of(_frustumCorners, [&](auto& c){ return c.z > vmax.z; }))
        return false;

    return true;
}

Ray Camera::ScreenPointToRay(const Vec3& point) const {
    return ScreenPointToRay(point.x, point.y);
}

sptr<GraphicsContext> GetGraphics()
{
    sptr<GraphicsContext> graphics;

    auto app = App::Get();
    if (app)
        graphics = app->GetGraphics();

    if (!graphics)
        throw std::runtime_error("App::GetGraphics returned null.");

    return graphics;
}

Ray Camera::ScreenPointToRay(float x, float y) const
{
    auto graphics = GetGraphics();

    Ray ray;

    UpdateView();

    Mat4 mtxInv = _mtxVP.Inverse();
    IntRect vp = graphics->GetViewport();
    Vec2 depthRange = graphics->GetDepthRangeNDC();

    if (_mode == CameraViewMode::Perspective)
    {
        Vec4 pt(
            (x - vp.x) / vp.w * 2.0f - 1.0f,
            (y - vp.y) / vp.h * 2.0f - 1.0f,
            depthRange.x,
            depthRange.y);

        pt *= mtxInv;

        pt.w = 1.0f / pt.w;

        pt.x *= pt.w;
        pt.y *= pt.w;
        pt.z *= pt.w;

        ray.origin = GetNode()->GetPosition();
        ray.direction = Vec3(pt.x, pt.y, pt.z) - ray.origin;
        ray.direction.Normalize();
    }
    else
    {
        Vec4 pt1(
            (x - vp.x) / vp.w * 2.0f - 1.0f,
            (y - vp.y) / vp.h * 2.0f - 1.0f,
            depthRange.x,
            depthRange.y);

        Vec4 pt2(
            (x - vp.x) / vp.w * 2.0f - 1.0f,
            (y - vp.y) / vp.h * 2.0f - 1.0f,
            1.0f,
            1.0f);

        pt1 *= mtxInv;

        pt1.w = 1.0f / pt1.w;

        pt1.x *= pt1.w;
        pt1.y *= pt1.w;
        pt1.z *= pt1.w;

        pt2 *= mtxInv;

        pt2.w = 1.0f / pt2.w;

        pt2.x *= pt2.w;
        pt2.y *= pt2.w;
        pt2.z *= pt2.w;

        ray.origin = Vec3(pt1);
        ray.direction = Vec3(pt2) - ray.origin;
        ray.direction.Normalize();
    }

    return ray;
}

Vec3 Camera::WorldToScreen(const Vec3& position) const
{
    auto graphics = GetGraphics();

    UpdateView();
    Vec4 ndc = Vec4(position, 1.0f) * _mtxVP;

    ndc /= ndc.w;

    auto viewport = graphics->GetViewport();

    Vec3 ret;
    ret.x = viewport.x + (ndc.x + 1.0f) * 0.5f * viewport.w;
    ret.y = viewport.y + (ndc.y + 1.0f) * 0.5f * viewport.h;
    ret.z = 0;
    return ret;
}

Vec3 Camera::ScreenToWorld(const Vec3& point) const
{
    auto graphics = GetGraphics();

    UpdateView();
    Vec3 camPos = GetNode()->GetPosition();
    Vec3 ret = Vec3::Zero();

    auto viewport = graphics->GetViewport();
    auto aspect = (float)viewport.w / viewport.h;

    switch (_mode)
    {
    case CameraViewMode::OrthoFixedWidth:
        ret.x = camPos.x + point.x * (_fixedWidth / viewport.w);
        ret.y = camPos.y + point.y * (_fixedWidth / aspect / viewport.h);
        break;

    case CameraViewMode::OrthoFixedHeight:
        ret.x = camPos.x + point.x * (_fixedHeight * aspect / viewport.w);
        ret.y = camPos.y + point.y * (_fixedHeight / viewport.h);
        break;

    case CameraViewMode::Perspective:
        throw("Invalid CameraViewMode 'Perspective'. Use ScreenPointToRay() instead.");
    }

    return ret;
}

float Camera::GetDepth(const Vec3& pos) const
{
    auto p = Vec4(pos, 1.0f) * GetNode()->GetWorldToLocalMatrix();
    return math::Clamp01((p.z - _nearPlane) / (_farPlane - _nearPlane));
}

void Camera::OnTransformChanged() {
    SetDirty();
}

void Camera::SetDirty() const {
    _dirty = true;
}

void Camera::UpdateView() const
{
    auto graphics = GetGraphics();
    auto viewport = graphics->GetViewport();

    if (_vp != viewport) {
        _dirty = true;
        _vp = viewport;
    }

    if (_dirty)
    {
        float aspect = (float)viewport.w / viewport.h;
        float nearHalfWidth = 0;
        float nearHalfHeight = 0;
        float farHalfWidth = 0;
        float farHalfHeight = 0;

        if(_mode == CameraViewMode::OrthoFixedWidth)
        {
            float halfWidth = _fixedWidth * 0.5f;
            float halfHeight = _fixedWidth / aspect * 0.5f;
            nearHalfWidth = halfWidth;
            nearHalfHeight = halfHeight;
            farHalfWidth = halfWidth;
            farHalfHeight = halfHeight;
            _mtxProjection = graphics->GetOrthoMatrix(-halfWidth, halfWidth, -halfHeight, halfHeight, _nearPlane, _farPlane);
        }
        else if (_mode == CameraViewMode::OrthoFixedHeight)
        {
            float halfWidth = _fixedHeight * aspect * 0.5f;
            float halfHeight = _fixedHeight * 0.5f;
            nearHalfWidth = halfWidth;
            nearHalfHeight = halfHeight;
            farHalfWidth = halfWidth;
            farHalfHeight = halfHeight;
            _mtxProjection = graphics->GetOrthoMatrix(-halfWidth, halfWidth, -halfHeight, halfHeight, _nearPlane, _farPlane);
        }
        else if (_mode == CameraViewMode::Perspective)
        {
            float f = tan(math::DegToRad * _fov / 2.0f);
            float sx = f * aspect;
            float sy = f;
            nearHalfWidth = sx * _nearPlane;
            nearHalfHeight = sy * _nearPlane;
            farHalfWidth = sx * _farPlane;
            farHalfHeight = sy * _farPlane;
            _mtxProjection = graphics->GetPerspectiveMatrix(_fov, aspect, _nearPlane, _farPlane);
        }

        Mat4 mtx = GetNode()->GetLocalToWorldMatrix();
        _mtxVP = mtx.Inverse() * _mtxProjection;

        Vec3 nearCenter = Vec3(0, 0, _nearPlane);
        Vec3 farCenter = Vec3(0, 0, _farPlane);

        Vec3 nearTL = Vec4(nearCenter + Vec3::Up() * nearHalfHeight + Vec3::Left() * nearHalfWidth, 1) * mtx;
        Vec3 nearTR = Vec4(nearCenter + Vec3::Up() * nearHalfHeight + Vec3::Right() * nearHalfWidth, 1) * mtx;
        Vec3 nearBL = Vec4(nearCenter + Vec3::Down() * nearHalfHeight + Vec3::Left() * nearHalfWidth, 1) * mtx;
        Vec3 nearBR = Vec4(nearCenter + Vec3::Down() * nearHalfHeight + Vec3::Right() * nearHalfWidth, 1) * mtx;
        Vec3 farTL = Vec4(farCenter + Vec3::Up() * farHalfHeight + Vec3::Left() * farHalfWidth, 1) * mtx;
        Vec3 farTR = Vec4(farCenter + Vec3::Up() * farHalfHeight + Vec3::Right() * farHalfWidth, 1) * mtx;
        Vec3 farBL = Vec4(farCenter + Vec3::Down() * farHalfHeight + Vec3::Left() * farHalfWidth, 1) * mtx;
        Vec3 farBR = Vec4(farCenter + Vec3::Down() * farHalfHeight + Vec3::Right() * farHalfWidth, 1) * mtx;

        _frustumCorners[FrustumCorner::NearTopLeft] = nearTL;
        _frustumCorners[FrustumCorner::NearTopRight] = nearTR;
        _frustumCorners[FrustumCorner::NearBottomLeft] = nearBL;
        _frustumCorners[FrustumCorner::NearBottomRight] = nearBR;
        _frustumCorners[FrustumCorner::FarTopLeft] = farTL;
        _frustumCorners[FrustumCorner::FarTopRight] = farTR;
        _frustumCorners[FrustumCorner::FarBottomLeft] = farBL;
        _frustumCorners[FrustumCorner::FarBottomRight] = farBR;

        _frustumPlanes[FrustumPlane::Left] = Plane(farTL, farBL, nearBL);
        _frustumPlanes[FrustumPlane::Right] = Plane(nearTR, nearBR, farBR);
        _frustumPlanes[FrustumPlane::Top] = Plane(farTL, nearTL, nearTR);
        _frustumPlanes[FrustumPlane::Bottom] = Plane(farBR, nearBR, nearBL);
        _frustumPlanes[FrustumPlane::Near] = Plane(nearTL, nearBL, nearBR);
        _frustumPlanes[FrustumPlane::Far] = Plane(farTR, farBR, farBL);
    }

    _dirty = false;
}

sptr<Camera> Camera::GetCurrentCamera() {
    return _currentCamera.lock();
}

void Camera::SetCurrentCamera(const wptr<Camera>& cam) {
    _currentCamera = cam;
}

void Camera::SetCurrentCamera(std::nullptr_t) {
    SetCurrentCamera(wptr<Camera>());
}

}
}
