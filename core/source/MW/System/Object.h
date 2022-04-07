/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

#pragma once
#include <MW/System/Json.h>
#include <MW/System/Pointers.h>
#include <MW/System/Task.h>
#include <MW/System/UUID.h>
#include <unordered_map>
#include <string>
#include <stack>

namespace mw {
inline namespace system {

class Executor;

struct TypeInfo
{
    const char* name = nullptr;

    bool operator==(const TypeInfo& t) const { return &t == this; }
    bool operator!=(const TypeInfo& t) const { return !(t == *this); }
};

class Object;
class ObjectLinker;

struct ILink
{
    virtual ~ILink() = default;
    virtual Task<void> LinkAsync(
        ObjectLinker* linker,
        const sptr<Executor>& executor) = 0;
};

class ObjectLinker
{
    std::unordered_map<UUID, sptr<Object>> objects;
    std::vector<uptr<ILink>> links;

    static bool AddObject(ObjectLinker* linker, const sptr<Object>& object);

    friend class Object;
    friend struct ILink;
public:

    const std::unordered_map<UUID, sptr<Object>>& GetObjects() const {
        return objects;
    }

    template<class T> requires std::is_base_of_v<Object, T>
    static void SaveLink(
        json& obj,
        const std::string& keyForUUID,
        const sptr<T>& ptr);

    template<class T> requires std::is_base_of_v<Object, T>
    static void SaveAsset(
        json& val,
        const sptr<T>& ptr);

    template<class T> requires std::is_base_of_v<Object, T>
        static void SaveAsset(
            json& obj,
            const std::string& keyForUUID,
            const sptr<T>& ptr);

    template<class T> //requires std::is_base_of_v<Object, T>
    static bool RestoreLink(
        ObjectLinker* linker,
        const sptr<Object>& hostObject,
        wptr<T>& assignee,
        const UUID& targetObjectID);

    template<class T> //requires std::is_base_of_v<Object, T>
    static bool RestoreLink(
        ObjectLinker* linker,
        const sptr<Object>& hostObject,
        wptr<T>& assignee,
        const json& obj,
        const std::string& keyOfUUID);

    template<class T> //requires std::is_base_of_v<Object, T>
    static bool RestoreLink(
        ObjectLinker* linker,
        const sptr<Object>& hostObject,
        sptr<T>& assignee,
        const UUID& targetObjectID);

    template<class T>
    static bool RestoreLink(
        ObjectLinker* linker,
        const sptr<Object>& hostObject,
        sptr<T>& assignee,
        const json& obj,
        const std::string& keyOfUUID);

    template<class T> //requires std::is_base_of_v<Object, T>
    static bool RestoreAsset(
        ObjectLinker* linker,
        const sptr<Object>& hostObject,
        sptr<T>& assignee,
        const UUID& targetAssetID);

    template<class T>
    static bool RestoreAsset(
        ObjectLinker* linker,
        const sptr<Object>& hostObject,
        sptr<T>& assignee,
        const json& obj,
        const std::string& keyOfUUID);

    static Task<void> LinkAsync(
        ObjectLinker* linker,
        const sptr<Executor>& executor);

    static void Link(ObjectLinker* linker);
};

class Object : public sp_from_this<Object>
{
protected:
    UUID uuid = UUID::New();
    std::string name;
public:
    Object() = default;
    virtual ~Object() = default;

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    Object(Object&&) = default;
    Object& operator=(Object&&) = default;

    void SetUUID(const UUID& uuid) {
        this->uuid = uuid;
    }

    const UUID& GetUUID() const {
        return uuid;
    }

    void SetName(const std::string& name) {
        this->name = name;
    }

    const std::string& GetName() const {
        return name;
    }

    static const TypeInfo& GetClassType() {
        static TypeInfo type{ "mw::system::Object" };
        return type;
    }

    virtual const TypeInfo& GetObjectType() const {
        return GetClassType();
    }

    virtual void ToJson(json& obj) const {
        obj["objectType"] = GetObjectType().name;
        obj["uuid"] = uuid;
        obj["name"] = name;
    }

    virtual void FromJson(const json& obj, ObjectLinker* linker)
    {
        uuid = obj["uuid"];
        name = obj["name"];
        ObjectLinker::AddObject(linker, This());
    }

    template<class T = Object>
    sptr<T> This() {
        return std::dynamic_pointer_cast<T>(shared_from_this());
    }

    template<class T = Object>
    sptr<const T> This() const {
        return std::dynamic_pointer_cast<const T>(shared_from_this());
    }
};

inline bool ObjectLinker::AddObject(ObjectLinker* linker, const sptr<Object>& object)
{
    if (!linker)
        return false;

    linker->objects[object->GetUUID()] = object;
    return true;
}

template<class T, class A>
struct ObjectLink : ILink
{
    sptr<Object> hostObject;
    A& assignee;
    UUID targetObjectID;

    ObjectLink(
        const sptr<Object>& hostObject,
        A& assignee,
        const UUID& targetObjectID)
            : hostObject(hostObject),
            assignee(assignee),
            targetObjectID(targetObjectID)
    {
        assert(hostObject);
    }

    virtual Task<void> LinkAsync(
        ObjectLinker* linker,
        const sptr<Executor>& executor) override
    {
        assert(hostObject);

        auto it = linker->GetObjects().find(targetObjectID);
        if (it != linker->GetObjects().end())
        {
            assignee = std::dynamic_pointer_cast<T>(it->second);
        }

        co_return;
    }
};

namespace detail
{

Task<sptr<Object>> GetAssetAsync(
    const UUID& uuid,
    const sptr<Executor>& executor);

}

template<class T, class A>
struct AssetLink : ILink
{
    sptr<Object> hostObject;
    A& assignee;
    UUID targetAssetID;

    AssetLink(
        const sptr<Object>& hostObject,
        A& assignee,
        const UUID& targetAssetID)
        : hostObject(hostObject),
          assignee(assignee),
          targetAssetID(targetAssetID)
    {
        assert(hostObject);
    }

    virtual Task<void> LinkAsync(
        ObjectLinker* linker,
        const sptr<Executor>& executor) override
    {
        assert(hostObject);
        auto asset = co_await detail::GetAssetAsync(targetAssetID, executor);
        assignee = std::dynamic_pointer_cast<T>(asset);
        co_return;
    }
};

template<class T> requires std::is_base_of_v<Object, T>
inline void ObjectLinker::SaveLink(
    json& obj,
    const std::string& keyForUUID,
    const sptr<T>& ptr)
{
    obj[keyForUUID] = ptr ? json(ptr->GetUUID()) : json(nullptr);
}

template<class T> requires std::is_base_of_v<Object, T>
inline void ObjectLinker::SaveAsset(
    json& val,
    const sptr<T>& ptr)
{
    val = ptr ? json(ptr->GetUUID()) : json(nullptr);
}

template<class T> requires std::is_base_of_v<Object, T>
inline void ObjectLinker::SaveAsset(
    json& obj,
    const std::string& keyForUUID,
    const sptr<T>& ptr)
{
    obj[keyForUUID] = ptr ? json(ptr->GetUUID()) : json(nullptr);
}

template<class T>
inline bool ObjectLinker::RestoreLink(
    ObjectLinker* linker,
    const sptr<Object>& hostObject,
    wptr<T>& assignee,
    const UUID& targetObjectID)
{
    if (!linker)
        return false;

    linker->links.emplace_back(
        upnew<ObjectLink<T, wptr<T>>>(hostObject, assignee, targetObjectID)
    );

    return true;
}

template<class T>
inline bool ObjectLinker::RestoreLink(
    ObjectLinker* linker,
    const sptr<Object>& hostObject,
    wptr<T>& assignee,
    const json& obj,
    const std::string& keyOfUUID)
{
    if (!linker)
        return false;

    if (auto it = obj.find(keyOfUUID); it != obj.end() && !it->is_null())
    {
        UUID linkID = *it;
        return RestoreLink(linker, hostObject, assignee, linkID);
    }

    return false;
}

template<class T>
inline bool ObjectLinker::RestoreLink(
    ObjectLinker* linker,
    const sptr<Object>& hostObject,
    sptr<T>& assignee,
    const UUID& targetObjectID)
{
    if (!linker)
        return false;

    linker->links.emplace_back(
        upnew<ObjectLink<T, sptr<T>>>(hostObject, assignee, targetObjectID)
    );

    return true;
}

template<class T>
inline bool ObjectLinker::RestoreLink(
    ObjectLinker* linker,
    const sptr<Object>& hostObject,
    sptr<T>& assignee,
    const json& obj,
    const std::string& keyOfUUID)
{
    if (!linker)
        return false;

    if (auto it = obj.find(keyOfUUID); it != obj.end() && !it->is_null())
    {
        UUID linkID = *it;
        return RestoreLink(linker, hostObject, assignee, linkID);
    }

    return false;
}

template<class T>
inline bool ObjectLinker::RestoreAsset(
    ObjectLinker* linker,
    const sptr<Object>& hostObject,
    sptr<T>& assignee,
    const UUID& targetAssetID)
{
    if (!linker)
        return false;

    linker->links.emplace_back(
        upnew<AssetLink<T, sptr<T>>>(hostObject, assignee, targetAssetID)
    );

    return true;
}

template<class T>
inline bool ObjectLinker::RestoreAsset(
    ObjectLinker* linker,
    const sptr<Object>& hostObject,
    sptr<T>& assignee,
    const json& obj,
    const std::string& keyOfUUID)
{
    if (!linker)
        return false;

    if (auto it = obj.find(keyOfUUID); it != obj.end() && !it->is_null())
    {
        UUID assetID = *it;
        return RestoreAsset(linker, hostObject, assignee, assetID);
    }

    return false;
}

class ObjectFactory
{
public:
    typedef sptr<mw::system::Object>(*ConstructFunc)();

    static void RegisterConstructor(const std::string& name, ConstructFunc func)
    {
        constructors[name] = func;
    }

    static void ToJson(const sptr<Object>& target, json& obj)
    {
        std::string objectType = target->GetObjectType().name;

        auto it = constructors.find(objectType);
        if (it == constructors.end())
            throw std::runtime_error("no serializer found for type '" + std::string(objectType) + "'");

        target->ToJson(obj);
    }

    static sptr<Object> FromJson(const json& obj, ObjectLinker* linker)
    {
        std::string objectType = obj.value("objectType", std::string());

        auto it = constructors.find(objectType);
        if (it == constructors.end())
            throw std::runtime_error("no serializer found for type '" + objectType + "'");

        auto target = it->second();
        target->FromJson(obj, linker);
        return target;
    }

    template<class T>
    static sptr<T> FromJson(const json& obj, ObjectLinker* linker) {
        return std::dynamic_pointer_cast<T>(FromJson(obj, linker));
    }

private:
    inline static std::unordered_map<std::string, ConstructFunc> constructors;
};

template<class T = Object> requires std::is_base_of_v<Object, T>
inline sptr<T> Instantiate(const sptr<Object>& object)
{
    json obj;
    object->ToJson(obj);

    ObjectLinker linker;
    sptr<T> ret = ObjectFactory::FromJson<T>(obj, &linker);
    // update IDs of all objects to new unique ones
    if (ret) ObjectLinker::Link(&linker);

    return ret;
}

#define REGISTER_OBJECT(ObjectSubclass) \
public: \
static const TypeInfo& GetClassType() { \
    static TypeInfo type { #ObjectSubclass }; \
    return type; \
} \
virtual const TypeInfo& GetObjectType() const override { \
    return GetClassType(); \
} \
private: \
inline static struct ObjectConstructor_##ObjectSubclass \
{ \
    ObjectConstructor_##ObjectSubclass() { \
        mw::system::ObjectFactory::RegisterConstructor(#ObjectSubclass, &Construct); \
    } \
private: \
    static mw::sptr<mw::system::Object> Construct() { \
        return mw::system::spnew<ObjectSubclass>(); \
    } \
} _objectConstructor_##ObjectSubclass;

} // system
} // mw
