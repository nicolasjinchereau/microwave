/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.SceneGraph.Node;
import Microwave.Math;
import Microwave.SceneGraph.LayerMask;
import Microwave.System.Json;
import Microwave.System.Object;
import Microwave.System.Path;
import Microwave.System.Pointers;
import Microwave.System.UUID;
import <cstdint>;
import <memory>;
import <string>;
import <vector>;
import <tuple>;
import <type_traits>;
import <utility>;

export namespace mw {
inline namespace scene {

class Scene;
class Component;

class Node : public Object
{
    inline static Type::Pin<Node> pin;

    Vec3                         _localPos = Vec3::Zero();
    Quat                         _localRot = Quat::Identity();
    Vec3                         _localScale = Vec3::One();
    mutable Vec3                 _worldPos = Vec3::Zero();
    mutable Quat                 _worldRot = Quat::Identity();
    mutable Vec3                 _worldScale = Vec3::One();
    mutable bool                 _dirty = true;
    bool                         _active = true;
    std::vector<sptr<Node>>      _children;
    std::vector<sptr<Component>> _components;
    wptr<Node>                   _parent;
    wptr<Scene>                  _scene;
    LayerMask                    _layerMask = LayerMask::Default;

public:
    friend Component;
    friend Scene;

    Node(){}
    virtual ~Node(){};

    virtual void ToJson(json& obj) const override;
    virtual void FromJson(const json& obj, ObjectLinker* linker) override;

    // mark this node as active
    void SetActive(bool active);

    // true if this node is marked active
    bool IsActive() const;

    // true if all nodes upto the root node are active
    bool IsBranchActive() const;

    void SetDirty();
    void UpdateWorldTransform() const;

    // position, rotation, scale
    Transform GetLocalTransform() const;
    void SetLocalTransform(const Transform& transform);
    void SetLocalTransform(const Vec3& pos, const Quat& rot, const Vec3& scale);

    // position, rotation, scale
    Transform GetGlobalTransform() const;
    void SetGlobalTransform(const Transform& transform);
    void SetGlobalTransform(const Vec3& pos, const Quat& rot, const Vec3& scale);

    void SetPosition(const Vec3& pos);
    void SetPosition(float x, float y, float z);
    Vec3 GetPosition() const;

    void SetLocalPosition(const Vec3& pos);
    void SetLocalPosition(float x, float y, float z);
    Vec3 GetLocalPosition() const;

    //Vec3 LocalToWorldPos(const Vec3& pos) const;
    Vec3 WorldToLocalPos(const Vec3& pos) const;

    void SetRotation(const Quat& rotation);
    void SetRotation(const Vec3& angles);
    void SetRotation(float x, float y, float z);
    void SetRotation(float angle, const Vec3& axis);
    Quat GetRotation() const;

    void SetLocalRotation(const Quat& rot);
    Quat GetLocalRotation() const;

    void SetScale(const Vec3& scale);
    void SetScale(float x, float y, float z);
    Vec3 GetScale() const;

    void SetLocalScale(const Vec3& scale);
    void SetLocalScale(float x, float y, float z);
    Vec3 GetLocalScale() const;

    Vec3 Right() const;
    Vec3 Up() const;
    Vec3 Forward() const;

    Mat4 GetLocalToWorldMatrix() const;
    Mat4 GetWorldToLocalMatrix() const;

    sptr<Node> AddChild();
    sptr<Node> AddChild(sptr<Node>&& child);
    void AddChild(const sptr<Node>& child);
    void RemoveChild(const sptr<Node>& child);
    void ClearChildren();

    void SetParent(const wptr<Node>& parent, bool preserveWorldTransform = true);
    void SetParent(nullptr_t, bool preserveWorldTransform = true) { SetParent(wptr<Node>(), preserveWorldTransform); }
    sptr<Node> GetParent() const;

    sptr<const Scene> GetScene() const;
    sptr<Scene> GetScene();

    const std::vector<sptr<Node>>& GetChildren() const;
    const std::vector<sptr<Component>>& GetComponents() const;

    size_t GetChildCount() const;
    sptr<Node> GetChild(int index) const;
    sptr<Node> GetChild(const UUID& uuid) const;
    sptr<Node> GetChild(const path& fullPath);
    path GetFullPath() const;

    sptr<Node> FindChild(std::string_view name, bool allowPartialMatch = false);
    sptr<Node> FindChild(LayerMask mask);
    sptr<Node> FindChild(std::predicate<const sptr<Node>&> auto const& pred);
    std::vector<sptr<Node>> FindChildren(std::string_view name, bool allowPartialMatch = false);
    std::vector<sptr<Node>> FindChildren(LayerMask mask);
    std::vector<sptr<Node>> FindChildren(std::predicate<const sptr<Node>&> auto const& pred);
    void FindChildren(std::vector<sptr<Node>>& out, std::string_view name, bool allowPartialMatch = false);
    void FindChildren(std::vector<sptr<Node>>& out, LayerMask mask);
    void FindChildren(std::vector<sptr<Node>>& out, std::predicate<const sptr<Node>&> auto const& pred);

    void LookAt(const Vec3& target, const Vec3& up = Vec3::Up());
    void Translate(const Vec3& offset);
    void Translate(float x, float y, float z);

    // transform by Scale*Rotation*Translation
    Vec3 TransformPoint(const Vec3& point) const;

    // transform by Scale*Rotation
    Vec3 TransformVector(const Vec3& vector) const;

    // transform by Rotation
    Vec3 TransformDirection(const Vec3& direction) const;

    // inverse transform by Scale*Rotation*Translation
    Vec3 InverseTransformPoint(const Vec3& point) const;

    // inverse transform by Scale*Rotation
    Vec3 InverseTransformVector(const Vec3& vector) const;

    // inverse transform by Rotation
    Vec3 InverseTransformDirection(const Vec3& direction) const;
    
    void SetLayerMask(LayerMask mask, bool includeChildren = false);
    LayerMask GetLayerMask() const;

    void RemoveComponent(const sptr<Component>& comp);

    template<class T> requires std::is_base_of_v<Component, T>
    sptr<T> AddComponent(const sptr<T>& comp);

    template<class T> requires std::is_base_of_v<Component, T>
    sptr<T> AddComponent();

    template<class T>
    sptr<T> GetComponent();

    template<class T>
    std::vector<sptr<T>> GetComponents();

    template<class T>
    void GetComponents(std::vector<sptr<T>>& out);

    template<class T>
    sptr<T> FindComponentDownward();

    template<class T>
    std::vector<sptr<T>> FindComponentsDownward();

    template<class T>
    void FindComponentsDownward(std::vector<sptr<T>>& out);

    template<class T, class F>
    void FindComponentsDownward(F&& fun);

    template<class T>
    sptr<T> FindComponentUpward();

    template<class T>
    std::vector<sptr<T>> FindComponentsUpward();

    template<class T>
    void FindComponentsUpward(std::vector<sptr<T>>& out);

    template<class T, class F>
    void FindComponentsUpward(F&& fun);

private:
    sptr<Component> AddComponentImpl(const sptr<Component>& comp);

    void SetScene(const sptr<Scene>& scene);
    void AttachToScene(const sptr<Scene>& scene);
    void DetachFromScene();
    void SignalStructureChanged();
};

sptr<Node> Node::FindChild(std::predicate<const sptr<Node>&> auto const& pred)
{
    for (auto& c : _children)
    {
        if(pred(c))
            return c;

        if (auto n = c->FindChild(pred))
            return n;
    }

    return nullptr;
}

std::vector<sptr<Node>> Node::FindChildren(std::predicate<const sptr<Node>&> auto const& pred)
{
    std::vector<sptr<Node>> ret;
    FindChildren(ret, pred);
    return ret;
}

void Node::FindChildren(std::vector<sptr<Node>>& out, std::predicate<const sptr<Node>&> auto const& pred)
{
    for (auto& c : _children)
    {
        if (pred(c))
            out.push_back(c);
        
        c->FindChildren(out, pred);
    }
}

template<class T> requires std::is_base_of_v<Component, T>
sptr<T> Node::AddComponent(const sptr<T>& comp) {
    AddComponentImpl(comp);
    return comp;
}

template<class T> requires std::is_base_of_v<Component, T>
sptr<T> Node::AddComponent() {
    return AddComponent(spnew<T>());
}

template<class T>
sptr<T> Node::GetComponent()
{
    sptr<T> ret;

    for (auto& c : _components)
    {
        if (ret = spcast<T>(c))
            break;
    }

    return ret;
}

template<class T>
std::vector<sptr<T>> Node::GetComponents()
{
    std::vector<sptr<T>> ret;
    GetComponents(ret);
    return ret;
}

template<class T>
void Node::GetComponents(std::vector<sptr<T>>& out)
{
    for (auto& c : _components)
    {
        if (sptr<T> t = spcast<T>(c)) {
            out.push_back(std::move(t));
        }
    }
}

template<class T>
sptr<T> Node::FindComponentDownward()
{
    struct R
    {
        static sptr<T> FindComponent(const sptr<Node>& node)
        {
            for (auto& c : node->_components)
            {
                if (sptr<T> t = spcast<T>(c))
                    return std::move(t);
            }

            for (auto& n : node->_children)
            {
                if (sptr<T> t = FindComponent(n))
                    return std::move(t);
            }

            return nullptr;
        }
    };

    return R::FindComponent(SharedFrom(this));
}

template<class T>
std::vector<sptr<T>> Node::FindComponentsDownward()
{
    std::vector<sptr<T>> ret;
    FindComponentsDownward(ret);
    return ret;
}

template<class T>
void Node::FindComponentsDownward(std::vector<sptr<T>>& out)
{
    struct R
    {
        static void FindComponents(
            const sptr<Node>& node, std::vector<sptr<T>>& out)
        {
            for (auto& c : node->_components)
            {
                if (sptr<T> t = spcast<T>(c))
                    out.push_back(std::move(t));
            }

            for (auto& n : node->_children)
                FindComponents(n, out);
        }
    };

    R::FindComponents(SharedFrom(this), out);
}

template<class T, class F>
void Node::FindComponentsDownward(F&& fun)
{
    struct R
    {
        static void FindComponents(
            const sptr<Node>& node, F& fun)
        {
            for (auto& c : node->_components)
            {
                if (sptr<T> t = spcast<T>(c))
                    fun(t);
            }

            for (auto& n : node->_children)
                FindComponents(n, fun);
        }
    };

    R::FindComponents(SharedFrom(this), fun);
}

template<class T>
sptr<T> Node::FindComponentUpward()
{
    struct R
    {
        static sptr<T> FindComponent(const sptr<Node>& node)
        {
            for (auto& c : node->_components)
            {
                if (sptr<T> t = spcast<T>(c))
                    return std::move(t);
            }

            if (auto parent = node->GetParent())
            {
                if (sptr<T> t = FindComponent(parent))
                    return std::move(t);
            }

            return nullptr;
        }
    };

    return R::FindComponent(SharedFrom(this));
}

template<class T>
std::vector<sptr<T>> Node::FindComponentsUpward()
{
    std::vector<sptr<T>> ret;
    FindComponentsUpward(ret);
    return ret;
}

template<class T>
void Node::FindComponentsUpward(std::vector<sptr<T>>& out)
{
    struct R
    {
        static void FindComponents(const sptr<Node>& node, std::vector<sptr<T>>& out)
        {
            for (auto& c : node->_components)
            {
                if (sptr<T> t = spcast<T>(c))
                    out.push_back(std::move(t));
            }

            if (auto parent = node->GetParent())
                FindComponents(parent, out);
        }
    };

    R::FindComponents(SharedFrom(this), out);
}

template<class T, class F>
void Node::FindComponentsUpward(F&& fun)
{
    struct R
    {
        static void FindComponents(const sptr<Node>& node, F& fun)
        {
            for (auto& c : node->_components)
            {
                if (sptr<T> t = spcast<T>(c))
                    fun(t);
            }

            if (auto parent = node->GetParent())
                FindComponents(parent, fun);
        }
    };

    R::FindComponents(SharedFrom(this), fun);
}

} // scene
} // mw
