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
    gvector<gptr<Node>>          _children;
    gvector<gptr<Component>>     _components;
    gptr<Node>                   _parent;
    gptr<Scene>                  _scene;
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

    gptr<Node> AddChild();
    gptr<Node> AddChild(gptr<Node>&& child);
    void AddChild(const gptr<Node>& child);
    void RemoveChild(const gptr<Node>& child);
    void ClearChildren();

    void SetParent(const gptr<Node>& newParent, bool preserveWorldTransform = true);
    void SetParent(nullptr_t, bool preserveWorldTransform = true) { SetParent(gptr<Node>(), preserveWorldTransform); }
    gptr<Node> GetParent() const;

    gptr<const Scene> GetScene() const;
    gptr<Scene> GetScene();

    const gvector<gptr<Node>>& GetChildren() const;
    const gvector<gptr<Component>>& GetComponents() const;

    size_t GetChildCount() const;
    gptr<Node> GetChild(int index) const;
    gptr<Node> GetChild(const UUID& uuid) const;
    gptr<Node> GetChild(const path& fullPath);
    path GetFullPath() const;

    gptr<Node> FindChild(std::string_view name, bool allowPartialMatch = false);
    gptr<Node> FindChild(LayerMask mask);
    gptr<Node> FindChild(std::predicate<const gptr<Node>&> auto const& pred);
    gvector<gptr<Node>> FindChildren(std::string_view name, bool allowPartialMatch = false);
    gvector<gptr<Node>> FindChildren(LayerMask mask);
    gvector<gptr<Node>> FindChildren(std::predicate<const gptr<Node>&> auto const& pred);
    void FindChildren(gvector<gptr<Node>>& out, std::string_view name, bool allowPartialMatch = false);
    void FindChildren(gvector<gptr<Node>>& out, LayerMask mask);
    void FindChildren(gvector<gptr<Node>>& out, std::predicate<const gptr<Node>&> auto const& pred);

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

    void RemoveComponent(const gptr<Component>& comp);

    template<class T> requires std::is_base_of_v<Component, T>
    gptr<T> AddComponent(const gptr<T>& comp);

    template<class T> requires std::is_base_of_v<Component, T>
    gptr<T> AddComponent();

    template<class T>
    gptr<T> GetComponent();

    template<class T>
    gvector<gptr<T>> GetComponents();

    template<class T>
    void GetComponents(gvector<gptr<T>>& out);

    template<class T>
    gptr<T> FindComponentDownward();

    template<class T>
    gvector<gptr<T>> FindComponentsDownward();

    template<class T>
    void FindComponentsDownward(gvector<gptr<T>>& out);

    template<class T, class F>
    void FindComponentsDownward(F&& fun);

    template<class T>
    gptr<T> FindComponentUpward();

    template<class T>
    gvector<gptr<T>> FindComponentsUpward();

    template<class T>
    void FindComponentsUpward(gvector<gptr<T>>& out);

    template<class T, class F>
    void FindComponentsUpward(F&& fun);

    int GetObjectCount() const
    {
        int totalObjects = 1;
        totalObjects += (int)_components.size();
        
        for(auto& c : _children)
            totalObjects += c->GetObjectCount();
        
        return totalObjects;
    }

private:
    gptr<Component> AddComponentImpl(const gptr<Component>& comp);

    void SetScene(const gptr<Scene>& scene);
    void AttachToScene(const gptr<Scene>& scene);
    void DetachFromScene();
    void SignalStructureChanged();
};

gptr<Node> Node::FindChild(std::predicate<const gptr<Node>&> auto const& pred)
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

gvector<gptr<Node>> Node::FindChildren(std::predicate<const gptr<Node>&> auto const& pred)
{
    gvector<gptr<Node>> ret;
    FindChildren(ret, pred);
    return ret;
}

void Node::FindChildren(gvector<gptr<Node>>& out, std::predicate<const gptr<Node>&> auto const& pred)
{
    for (auto& c : _children)
    {
        if (pred(c))
            out.push_back(c);
        
        c->FindChildren(out, pred);
    }
}

template<class T> requires std::is_base_of_v<Component, T>
gptr<T> Node::AddComponent(const gptr<T>& comp) {
    AddComponentImpl(comp);
    return comp;
}

template<class T> requires std::is_base_of_v<Component, T>
gptr<T> Node::AddComponent() {
    auto comp = gpnew<T>();
    AddComponent(comp);
    return comp;
}

template<class T>
gptr<T> Node::GetComponent()
{
    gptr<T> ret;

    for (auto& c : _components)
    {
        if (ret = gpcast<T>(c))
            break;
    }

    return ret;
}

template<class T>
gvector<gptr<T>> Node::GetComponents()
{
    gvector<gptr<T>> ret;
    GetComponents(ret);
    return ret;
}

template<class T>
void Node::GetComponents(gvector<gptr<T>>& out)
{
    for (auto& c : _components)
    {
        if (gptr<T> t = gpcast<T>(c)) {
            out.push_back(std::move(t));
        }
    }
}

template<class T>
gptr<T> Node::FindComponentDownward()
{
    struct R
    {
        static gptr<T> FindComponent(const gptr<Node>& node)
        {
            for (auto& c : node->_components)
            {
                if (gptr<T> t = gpcast<T>(c))
                    return std::move(t);
            }

            for (auto& n : node->_children)
            {
                if (gptr<T> t = FindComponent(n))
                    return std::move(t);
            }

            return nullptr;
        }
    };

    return R::FindComponent(self(this));
}

template<class T>
gvector<gptr<T>> Node::FindComponentsDownward()
{
    gvector<gptr<T>> ret;
    FindComponentsDownward(ret);
    return ret;
}

template<class T>
void Node::FindComponentsDownward(gvector<gptr<T>>& out)
{
    struct R
    {
        static void FindComponents(
            const gptr<Node>& node, gvector<gptr<T>>& out)
        {
            for (auto& c : node->_components)
            {
                if (gptr<T> t = gpcast<T>(c))
                    out.push_back(std::move(t));
            }

            for (auto& n : node->_children)
                FindComponents(n, out);
        }
    };

    R::FindComponents(self(this), out);
}

template<class T, class F>
void Node::FindComponentsDownward(F&& fun)
{
    struct R
    {
        static void FindComponents(
            const gptr<Node>& node, F& fun)
        {
            for (auto& c : node->_components)
            {
                if (gptr<T> t = gpcast<T>(c))
                    fun(t);
            }

            for (auto& n : node->_children)
                FindComponents(n, fun);
        }
    };

    R::FindComponents(self(this), fun);
}

template<class T>
gptr<T> Node::FindComponentUpward()
{
    struct R
    {
        static gptr<T> FindComponent(const gptr<Node>& node)
        {
            for (auto& c : node->_components)
            {
                if (gptr<T> t = gpcast<T>(c))
                    return std::move(t);
            }

            if (auto parent = node->GetParent())
            {
                if (gptr<T> t = FindComponent(parent))
                    return std::move(t);
            }

            return nullptr;
        }
    };

    return R::FindComponent(self(this));
}

template<class T>
gvector<gptr<T>> Node::FindComponentsUpward()
{
    gvector<gptr<T>> ret;
    FindComponentsUpward(ret);
    return ret;
}

template<class T>
void Node::FindComponentsUpward(gvector<gptr<T>>& out)
{
    struct R
    {
        static void FindComponents(const gptr<Node>& node, gvector<gptr<T>>& out)
        {
            for (auto& c : node->_components)
            {
                if (gptr<T> t = gpcast<T>(c))
                    out.push_back(std::move(t));
            }

            if (auto parent = node->GetParent())
                FindComponents(parent, out);
        }
    };

    R::FindComponents(self(this), out);
}

template<class T, class F>
void Node::FindComponentsUpward(F&& fun)
{
    struct R
    {
        static void FindComponents(const gptr<Node>& node, F& fun)
        {
            for (auto& c : node->_components)
            {
                if (gptr<T> t = gpcast<T>(c))
                    fun(t);
            }

            if (auto parent = node->GetParent())
                FindComponents(parent, fun);
        }
    };

    R::FindComponents(self(this), fun);
}

} // scene
} // mw
