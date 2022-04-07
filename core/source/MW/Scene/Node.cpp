/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#include <MW/System/Json.h>
#include <MW/Scene/Node.h>
#include <MW/Scene/Components/Component.h>
#include <MW/Scene/Scene.h>
#include <MW/Utilities/Util.h>

namespace mw {
inline namespace scene {

void Node::ToJson(json& obj) const
{
    Object::ToJson(obj);

    obj["localPos"] = _localPos;
    obj["localRot"] = _localRot;
    obj["localScale"] = _localScale;

    obj["active"] = _active;
    obj["layerMask"] = _layerMask;

    json childrenObj = json::array();
    for (auto& child : _children) {
        json val;
        if(child) child->ToJson(val);
        childrenObj.push_back(val);
    }
    obj["children"] = std::move(childrenObj);

    json componentsObj = json::array();
    for (auto& comp : _components) {
        json val;
        if (comp) comp->ToJson(val);
        componentsObj.push_back(val);
    }
    obj["components"] = std::move(componentsObj);
}

void Node::FromJson(const json& obj, ObjectLinker* linker)
{
    Object::FromJson(obj, linker);

    _localPos = obj.value("localPos", _localPos);
    _localRot = obj.value("localRot", _localRot);
    _localScale = obj.value("localScale", _localScale);

    _active = obj.value("active", _active);
    _layerMask = obj.value("layerMask", _layerMask);

    auto sthis = This<Node>();

    const json& childrenObj = obj["children"];
    for (auto& childObj : childrenObj)
    {
        auto child = ObjectFactory::FromJson<Node>(childObj, linker);
        child->_parent = sthis;
        _children.push_back(std::move(child));
    }

    const json& componentsObj = obj["components"];
    for (auto& compObj : componentsObj)
    {
        auto comp = ObjectFactory::FromJson<Component>(compObj, linker);
        comp->node = sthis;
        _components.push_back(std::move(comp));
    }

    SetDirty();
}

void Node::SetActive(bool active)
{
    if(_active != active)
    {
        _active = active;

        if(active)
        {
            for(auto& c : _components)
                c->OnNodeActivated();
        }
        else
        {
            for (auto& c : _components)
                c->OnNodeDeactivated();
        }
    }
}

bool Node::IsActive() const {
    return _active;
}

bool Node::IsBranchActive() const
{
    sptr<Node> parent = _parent.lock();
    return _active && (!parent || parent->IsBranchActive());
}

void Node::SetDirty()
{
    if (!_dirty)
    {
        _dirty = true;

        for (auto& c : _components)
            c->OnTransformChanged();

        for (auto& c : _children)
            c->SetDirty();
    }
}

void Node::UpdateWorldTransform() const
{
    if (_dirty)
    {
        if (auto par = _parent.lock())
        {
            par->UpdateWorldTransform();

            Vec3 worldPos = _localPos;
            Quat worldRot = _localRot;
            Vec3 worldScale = _localScale;

            Vec3 parentScale = par->_worldScale;
            Quat parentRot = par->_worldRot;
            Vec3 parentPos = par->_worldPos;

            worldPos = worldPos.Multiply(parentScale);
            worldPos *= parentRot;
            worldPos += parentPos;

            worldRot *= parentRot;

            worldScale = worldScale.Multiply(parentScale);

            _worldPos = worldPos;
            _worldRot = worldRot;
            _worldScale = worldScale;
        }
        else
        {
            _worldPos = _localPos;
            _worldRot = _localRot;
            _worldScale = _localScale;
        }

        _dirty = false;
    }
}

void Node::SetLocalTransform(const Transform& transform)
{
    SetLocalTransform(
        transform.position,
        transform.rotation,
        transform.scale
    );
}

void Node::SetLocalTransform(const Vec3& pos, const Quat& rot, const Vec3& scale)
{
    _localPos = pos;
    _localRot = rot;
    _localScale = scale;
    SetDirty();
}

Transform Node::GetLocalTransform() const
{
    return {
        _localPos,
        _localRot,
        _localScale
    };
}

void Node::SetGlobalTransform(const Transform& transform)
{
    SetGlobalTransform(
        transform.position,
        transform.rotation,
        transform.scale
    );
}

void Node::SetGlobalTransform(const Vec3& pos, const Quat& rot, const Vec3& scale)
{
    auto par = _parent.lock();
    if (par)
    {
        Vec3 parentInvScale = Vec3::One().Divide(par->GetScale());
        Quat parentInvRot = par->GetRotation().Inverse();

        // position
        auto localPos = pos;
        localPos -= par->GetPosition();
        localPos *= parentInvRot;
        localPos = localPos.Multiply(parentInvScale);
        _localPos = localPos;

        // rotation
        _localRot = rot * parentInvRot;

        // scale
        _localScale = scale.Multiply(parentInvScale);
    }
    else
    {
        _localPos = pos;
        _localRot = rot;
        _localScale = scale;
    }

    SetDirty();
}

Transform Node::GetGlobalTransform() const
{
    UpdateWorldTransform();
    return {
        _worldPos,
        _worldRot,
        _worldScale
    };
}

void Node::SetPosition(const Vec3& pos) {
    _localPos = WorldToLocalPos(pos);
    SetDirty();
}

void Node::SetPosition(float x, float y, float z) {
    SetPosition(Vec3(x, y, z));
}

Vec3 Node::GetPosition() const {
    UpdateWorldTransform();
    return _worldPos;
}

//Vec3 Node::LocalToWorldPos(const Vec3& pos) const
//{
//    Vec3 worldPos = _localPos;
//
//    auto par = _parent.lock();
//    if (par) {
//        worldPos = worldPos.Multiply(par->GetScale());
//        worldPos *= par->GetRotation();
//        worldPos += par->GetPosition();
//    }
//
//    return worldPos;
//}

Vec3 Node::WorldToLocalPos(const Vec3& pos) const
{
    Vec3 localPos = pos;

    auto par = _parent.lock();
    if (par) {
        localPos -= par->GetPosition();
        localPos *= par->GetRotation().Inverse();
        localPos = localPos.Divide(par->GetScale());
    }

    return localPos;
}

void Node::SetLocalPosition(const Vec3& pos) {
    _localPos = pos;
    SetDirty();
}

void Node::SetLocalPosition(float x, float y, float z) {
    SetLocalPosition(Vec3(x, y, z));
}

Vec3 Node::GetLocalPosition() const {
    return _localPos;
}

void Node::SetRotation(const Quat& rot) {
    auto par = _parent.lock();
    _localRot = par ? rot * par->GetRotation().Inverse() : rot;
    SetDirty();
}

void Node::SetRotation(const Vec3& angles) {
    SetRotation(Quat(angles.x, angles.y, angles.z));
}

void Node::SetRotation(float x, float y, float z) {
    SetRotation(Quat(x, y, z));
}

void Node::SetRotation(float angle, const Vec3& axis) {
    SetRotation(Quat::AngleAxis(angle, axis));
}

Quat Node::GetRotation() const {
    UpdateWorldTransform();
    return _worldRot;
}

void Node::SetLocalRotation(const Quat& rot) {
    _localRot = rot;
    SetDirty();
}

Quat Node::GetLocalRotation() const {
    return _localRot;
}

void Node::SetScale(const Vec3& scale) {
    auto par = _parent.lock();
    _localScale = par ? scale.Divide(par->GetScale()) : scale;
    SetDirty();
}

void Node::SetScale(float x, float y, float z) {
    SetScale(Vec3(x, y, z));
}

Vec3 Node::GetScale() const {
    UpdateWorldTransform();
    return _worldScale;
}

void Node::SetLocalScale(const Vec3& scale) {
    _localScale = scale;
    SetDirty();
}

void Node::SetLocalScale(float x, float y, float z) {
    SetLocalScale(Vec3(x, y, z));
}

Vec3 Node::GetLocalScale() const {
    return _localScale;
}

Vec3 Node::Right() const {
    return Vec3::Right() * GetRotation();
}

Vec3 Node::Up() const {
    return Vec3::Up() * GetRotation();
}

Vec3 Node::Forward() const {
    return Vec3::Forward() * GetRotation();
}

Mat4 Node::GetLocalToWorldMatrix() const
{
    Vec3 pos = GetPosition();
    Quat rot = GetRotation();
    Vec3 scale = GetScale();

    Mat4 mtx = rot.ToMatrix();

    mtx.m11 *= scale.x;
    mtx.m12 *= scale.x;
    mtx.m13 *= scale.x;

    mtx.m21 *= scale.y;
    mtx.m22 *= scale.y;
    mtx.m23 *= scale.y;

    mtx.m31 *= scale.z;
    mtx.m32 *= scale.z;
    mtx.m33 *= scale.z;

    mtx.m41 = pos.x;
    mtx.m42 = pos.y;
    mtx.m43 = pos.z;

    return mtx;
}

Mat4 Node::GetWorldToLocalMatrix() const
{
    Vec3 pos = -GetPosition();
    Quat rot = GetRotation().Inverse();
    Vec3 invScale = Vec3::One().Divide(GetScale());

    Mat4 mtxInv = rot.ToMatrix();

    mtxInv.m11 *= invScale.x;
    mtxInv.m12 *= invScale.x;
    mtxInv.m13 *= invScale.x;

    mtxInv.m21 *= invScale.y;
    mtxInv.m22 *= invScale.y;
    mtxInv.m23 *= invScale.y;

    mtxInv.m31 *= invScale.z;
    mtxInv.m32 *= invScale.z;
    mtxInv.m33 *= invScale.z;

    mtxInv.m41 = (pos.x * mtxInv.m11) + (pos.y * mtxInv.m21) + (pos.z * mtxInv.m31);
    mtxInv.m42 = (pos.x * mtxInv.m12) + (pos.y * mtxInv.m22) + (pos.z * mtxInv.m32);
    mtxInv.m43 = (pos.x * mtxInv.m13) + (pos.y * mtxInv.m23) + (pos.z * mtxInv.m33);

    return mtxInv;
}

void Node::SignalStructureChanged()
{
    auto node = This<Node>();
    while (node)
    {
        for (auto& comp : node->_components)
            comp->OnStructureChanged();

        node = node->GetParent();
    }
}

sptr<Node> Node::AddChild() {
    return AddChild(spnew<Node>());
}

sptr<Node> Node::AddChild(sptr<Node>&& child) {
    AddChild(child);
    return std::move(child);
}

void Node::AddChild(const sptr<Node>& child) {
    child->SetParent(This<Node>());
}

void Node::RemoveChild(const sptr<Node>& child)
{
    auto childsParent = child->GetParent();
    if (childsParent == This<Node>())
        childsParent->SetParent(nullptr);
}

void Node::ClearChildren()
{
    while (!_children.empty()) {
        auto c = _children.back();
        c->SetParent(nullptr);
    }
}

void Node::SetParent(const wptr<Node>& parent, bool preserveWorldTransform)
{
    auto oldParent = _parent.lock();
    auto newParent = parent.lock();

    if (newParent == oldParent)
        return;

    if (auto scene = GetScene())
        DetachFromScene();

    if (oldParent)
    {
        if (preserveWorldTransform)
        {
            Vec3 parentScale = oldParent->GetScale();
            Quat parentRot = oldParent->GetRotation();

            _localPos = _localPos.Multiply(parentScale);
            _localPos *= parentRot;
            _localPos += oldParent->GetPosition();

            _localRot = _localRot * parentRot;

            _localScale = _localScale.Multiply(parentScale);
        }

        auto it = std::find(
            oldParent->_children.begin(),
            oldParent->_children.end(),
            This<Node>());

        oldParent->_children.erase(it);
        oldParent->SignalStructureChanged();
    }

    _parent = newParent;

    if (newParent)
    {
        if (preserveWorldTransform)
        {
            Quat invParentRot = newParent->GetRotation().Inverse();
            Vec3 invParentScale = Vec3::One().Divide(newParent->GetScale());

            _localPos -= newParent->GetPosition();
            _localPos *= invParentRot;
            _localPos = _localPos.Multiply(invParentScale);

            _localRot = _localRot * invParentRot;

            _localScale = _localScale.Multiply(invParentScale);
        }

        newParent->_children.push_back(This<Node>());

        if (auto scene = newParent->GetScene())
            AttachToScene(scene);
    }

    SetDirty();
    SignalStructureChanged();
}

sptr<Node> Node::GetParent() const {
    return _parent.lock();
}

sptr<const Scene> Node::GetScene() const {
    return _scene.lock();
}

sptr<Scene> Node::GetScene() {
    return _scene.lock();
}

void Node::SetScene(const sptr<Scene>& scene)
{
    auto curr = _scene.lock();

    if (curr != scene)
    {
        if (curr)
            DetachFromScene();

        if (scene)
            AttachToScene(scene);
    }
}

void Node::AttachToScene(const sptr<Scene>& scene)
{
    _scene = scene;
    if (scene)
    {
        for (auto& c : _components) {
            scene->RegisterComponent(c);
            c->OnAttachedToScene();
        }
    }

    for (auto& c : _children)
        c->AttachToScene(scene);
}

void Node::DetachFromScene()
{
    if (auto scene = _scene.lock())
    {
        for (auto& c : _components) {
            c->OnDetachFromScene();
            scene->UnregisterComponent(c);
        }
    }

    _scene.reset();

    for (auto& c : _children)
        c->DetachFromScene();
}

size_t Node::GetChildCount() const {
    return _children.size();
}

sptr<Node> Node::GetChild(int index) const {
    return _children[index];
}

sptr<Node> Node::GetChild(const UUID& uuid) const
{
    struct R
    {
        static sptr<Node> GetChild(const sptr<Node>& node, const UUID& uuid)
        {
            if (node->GetUUID() == uuid)
                return node;

            for (auto& child : node->GetChildren())
            {
                if (sptr<Node> c = GetChild(child, uuid))
                    return std::move(c);
            }

            return nullptr;
        }
    };
    
    sptr<Node> node = const_cast<Node*>(this)->This<Node>();
    return R::GetChild(node, uuid);
}

const std::vector<sptr<Node>>& Node::GetChildren() const {
    return _children;
}

sptr<Component> Node::AddComponent(const sptr<Component>& comp)
{
    auto oldOwner = comp->node.lock();
    auto newOwner = This<Node>();

    if (oldOwner != newOwner)
    {
        if (oldOwner)
            oldOwner->RemoveComponent(comp);

        comp->node = newOwner;
        _components.push_back(comp);

        if (auto scene = _scene.lock()) {
            scene->RegisterComponent(comp);
            comp->OnAttachedToScene();
        }

        SignalStructureChanged();
    }

    return comp;
}

void Node::RemoveComponent(const sptr<Component>& comp)
{
    auto sthis = This<Node>();
    auto owner = comp->node.lock();

    if (owner == sthis)
    {
        if (auto scene = _scene.lock()) {
            comp->OnDetachFromScene();
            scene->UnregisterComponent(comp);
        }
        
        Erase(_components, comp);
        comp->node.reset();

        SignalStructureChanged();
    }
}

const std::vector<sptr<Component>>& Node::GetComponents() const {
    return _components;
}

namespace {
    void getFullPathHelper(
        const sptr<const Node>& node,
        std::string& fullPath,
        int reserve = 0)
    {
        sptr<Node> parent = node->GetParent();
        if (parent)
        {
            int res = reserve;
            if (res) res += 1;
            res += node->GetName().size();

            getFullPathHelper(parent, fullPath, res);

            if(!fullPath.empty())
                fullPath += "/";

            fullPath += node->GetName();
        }
        else
        {
            fullPath.reserve(reserve);
        }
    }
}

path Node::GetFullPath() const
{
    std::string fullPath;
    getFullPathHelper(This<const Node>(), fullPath);
    return path(std::move(fullPath));
}

sptr<Node> Node::GetChild(const path& fullPath)
{
    sptr<Node> node = This<Node>();
    auto it = fullPath.begin();

    while (node && it != fullPath.end())
    {
        const std::string& part = it->string();

        auto itChild = std::find_if(
            node->_children.begin(), node->_children.end(),
            [&](const sptr<Node>& n) { return n->GetName() == part; }
        );

        if (itChild != node->_children.end())
        {
            node = *itChild;
            ++it;
        }
        else
        {
            node = nullptr;
        }
    }

    return node;
}

sptr<Node> Node::FindChild(std::string_view name, bool allowPartialMatch)
{
    return FindChild(
        [=](const sptr<Node>& n)
        {
            if(allowPartialMatch)
                return n->GetName().find(name) != std::string::npos;
            else
                return n->GetName() == name;
        }
    );
}

sptr<Node> Node::FindChild(LayerMask mask)
{
    return FindChild(
        [=](const sptr<Node>& n) {
            return (n->GetLayerMask() & mask) != 0;
        }
    );
}

std::vector<sptr<Node>> Node::FindChildren(std::string_view name, bool allowPartialMatch)
{
    std::vector<sptr<Node>> ret;
    FindChildren(ret, name, allowPartialMatch);
    return ret;
}

std::vector<sptr<Node>> Node::FindChildren(LayerMask mask)
{
    std::vector<sptr<Node>> ret;
    FindChildren(ret, mask);
    return ret;
}

void Node::FindChildren(std::vector<sptr<Node>>& out, std::string_view name, bool allowPartialMatch)
{
    FindChildren(out,
        [=](const sptr<Node>& n)
        {
            if (allowPartialMatch)
                return n->GetName().find(name) != std::string::npos;
            else
                return n->GetName() == name;
        }
    );
}

void Node::FindChildren(std::vector<sptr<Node>>& out, LayerMask mask)
{
    FindChildren(out,
        [=](const sptr<Node>& n) {
            return (n->GetLayerMask() & mask) != 0;
        }
    );
}

void Node::LookAt(const Vec3& target, const Vec3& up) {
    SetRotation(Quat::LookRotation(target - GetPosition(), up));
}

void Node::Translate(const Vec3& offset) {
    SetPosition(GetPosition() + offset);
}

void Node::Translate(float x, float y, float z) {
    Translate(Vec3(x, y, z));
}

Vec3 Node::TransformPoint(const Vec3& point) const {
    return Vec4(point, 1.0f) * GetLocalToWorldMatrix();
}

Vec3 Node::TransformVector(const Vec3& vector) const {
    return Vec4(vector, 0.0f) * GetLocalToWorldMatrix();
}

Vec3 Node::TransformDirection(const Vec3& direction) const {
    return direction * GetRotation();
}

Vec3 Node::InverseTransformPoint(const Vec3& point) const {
    return Vec4(point, 1.0f) * GetWorldToLocalMatrix();
}

Vec3 Node::InverseTransformVector(const Vec3& vector) const {
    return Vec4(vector, 0.0f) * GetWorldToLocalMatrix();
}

Vec3 Node::InverseTransformDirection(const Vec3& direction) const {
    return direction * (GetRotation().Inverse());
}

void Node::SetLayerMask(LayerMask mask, bool includeChildren)
{
    _layerMask = mask;

    if (includeChildren)
    {
        for (auto& c : _children)
            c->SetLayerMask(mask, true);
    }
}

LayerMask Node::GetLayerMask() const {
    return _layerMask;
}

}
}
