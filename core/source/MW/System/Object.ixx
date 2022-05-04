/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.System.Object;
import Microwave.System.Console;
import Microwave.System.Json;
import Microwave.System.Pointers;
import Microwave.System.Task;
import Microwave.System.UUID;
import Microwave.System.Spinlock;
import <boost/type_index.hpp>;
import <cassert>;
import <memory>;
import <stack>;
import <string>;
import <typeinfo>;
import <type_traits>;
import <unordered_map>;
import <vector>;
import <functional>;
import <optional>;
import <typeindex>;
import <mutex>;
import <unordered_set>;
import <set>;

export namespace mw {
inline namespace system {

class Object;
class ObjectLinker;
class Executor;
class Type;

struct TypeIndexHash
{
    size_t operator()(const std::type_index& ti) const noexcept {
        return ti.hash_code();
    }
};

class ITypeHandle
{
public:
    static auto& handlesByName() {
        static std::unordered_map<std::string, const ITypeHandle*> container;
        return container;
    }

    static auto& handlesByIndex()
    {
        static std::unordered_map<std::type_index, const ITypeHandle*, TypeIndexHash> container;
        return container;
    }

    friend class Type;

    virtual ~ITypeHandle() = default;
    virtual const std::type_info& info() const = 0;
    virtual const std::string& name() const = 0;
    virtual sptr<Object> create() const = 0;
};

template<class T>
class TypeHandle : public ITypeHandle
{
    std::string typeName;
public:
    static const ITypeHandle* GetHandle() {
        static const TypeHandle<T> handle;
        return &handle;
    }

    TypeHandle() {
        typeName =  boost::typeindex::type_id<T>().pretty_name();
        if(typeName.starts_with("struct ")) typeName.erase(0, 7);
        if(typeName.starts_with("class ")) typeName.erase(0, 6);

        handlesByName()[typeName] = this;
        handlesByIndex()[std::type_index(typeid(T))] = this;

        //Console::WriteLine("Type: %", typeName);
    }

    virtual const std::type_info& info() const override {
        return typeid(T);
    }

    virtual const std::string& name() const override {
        return typeName;
    }

    virtual sptr<Object> create() const override
    {
        if constexpr (std::is_default_constructible_v<T>) {
            return spnew<T>();
        }
        else {
            return sptr<Object>();
        }
    }
};

class Type
{
protected:
    const ITypeHandle* handle{};
    Type(const ITypeHandle* handle) : handle(handle){}
public:

    const std::type_info& info() const { return handle->info(); }
    const std::string& name() const { return handle->name(); }
    sptr<Object> create() const { return handle->create(); }

    bool operator==(const Type& other) const { return handle == other.handle; }
    bool operator!=(const Type& other) const { return handle == other.handle; }

    template<class T>
    static Type Get();

    static std::optional<Type> Find(const std::string& name);
    static std::optional<Type> Find(const sptr<Object>& obj);
    static std::optional<Type> Find(const Object* obj);

    // instantiate this to enable Type::Find functions
    template<class T>
    class Pin
    {
        const ITypeHandle* handle{};
    public:
        Pin() : handle(TypeHandle<T>::GetHandle()){}
        Type GetType() const { return { handle }; }
    };
};

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

    const auto& GetObjects() const {
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
    inline static Type::Pin<Object> pin;
protected:
    UUID uuid = UUID::New();
    std::string name;
public:
    struct Registry
    {
        Spinlock lock;
        std::unordered_set<Object*> objects;

        void Add(Object* obj) {
            std::lock_guard<Spinlock> lk(lock);
            objects.insert(obj);
        }

        void Remove(Object* obj) {
            std::lock_guard<Spinlock> lk(lock);
            objects.erase(obj);
        }

        void DumpLiveObjects()
        {
            std::set<std::string_view> typeNames;

            {
                std::lock_guard<Spinlock> lk(lock);

                for(auto& p : objects)
                    typeNames.insert(typeid(*p).name());
            }

            for(auto name : typeNames)
                Console::WriteLine("%", name);
        }
    };

    static Registry& GetRegistry() {
        static Registry registry;
        return registry;
    }

    Object() {
        //GetRegistry().Add(this);
    }

    virtual ~Object() {
        //GetRegistry().Remove(this);
    }

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

    virtual void ToJson(json& obj) const
    {
        auto type = Type::Find(this);
        obj["objectType"] = type ? type->name() : "unknown";
        obj["uuid"] = uuid;
        obj["name"] = name;
    }

    virtual void FromJson(const json& obj, ObjectLinker* linker)
    {
        uuid = obj["uuid"];
        name = obj["name"];
        ObjectLinker::AddObject(linker, SharedFrom(this));
    }

    template<class T> requires std::is_base_of_v<Object, T>
    sptr<T> SharedFrom(T* thisPtr) {
        assert(static_cast<Object*>(thisPtr) == this);
        return sptr<T>(shared_from_this(), thisPtr);
    }

    template<class T> requires std::is_base_of_v<Object, T>
    sptr<const T> SharedFrom(const T* thisPtr) const {
        assert(static_cast<const Object*>(thisPtr) == this);
        return sptr<const T>(shared_from_this(), thisPtr);
    }

    //static void SaveToJson(const sptr<Object>& target, json& obj)
    //{
    //    auto type = Type::Find(target);
    //    if (!type)
    //        throw std::runtime_error("Type is not pinned.");

    //    target->ToJson(obj);
    //}

    static sptr<Object> CreateFromJson(const json& obj, ObjectLinker* linker)
    {
        std::string typeName = obj.value("objectType", std::string());

        if (typeName.empty() || typeName == "unknown")
            throw std::runtime_error("unknown type");

        auto type = Type::Find(typeName);
        if (!type)
            throw std::runtime_error("type not found");

        auto target = type->create();
        target->FromJson(obj, linker);
        return target;
    }

    template<class T>
    static sptr<T> CreateFromJson(const json& obj, ObjectLinker* linker) {
        return spcast<T>(CreateFromJson(obj, linker));
    }
};

template<class T>
Type Type::Get() {
    return Type { TypeHandle<T>::GetHandle() };
}

std::optional<Type> Type::Find(const std::string& name)
{
    std::optional<Type> ret;

    auto it = ITypeHandle::handlesByName().find(name);
    if(it != ITypeHandle::handlesByName().end()) {
        ret = Type { it->second };
    }

    return ret;
}

std::optional<Type> Type::Find(const sptr<Object>& obj) {
    return Find(obj.get());
}

std::optional<Type> Type::Find(const Object* obj)
{
    assert(obj);
    std::optional<Type> ret;

    auto idx = std::type_index(typeid(*obj));

    auto it = ITypeHandle::handlesByIndex().find(idx);
    if(it != ITypeHandle::handlesByIndex().end()) {
        ret = Type { it->second };
    }

    return ret;
}

bool ObjectLinker::AddObject(ObjectLinker* linker, const sptr<Object>& object)
{
    if (!linker)
        return false;

    auto uuid = object->GetUUID();
    linker->objects[uuid] = object;
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
            assignee = spcast<T>(it->second);
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
        assignee = spcast<T>(asset);
        co_return;
    }
};

template<class T> requires std::is_base_of_v<Object, T>
void ObjectLinker::SaveLink(
    json& obj,
    const std::string& keyForUUID,
    const sptr<T>& ptr)
{
    obj[keyForUUID] = ptr ? json(ptr->GetUUID()) : json(nullptr);
}

template<class T> requires std::is_base_of_v<Object, T>
void ObjectLinker::SaveAsset(
    json& val,
    const sptr<T>& ptr)
{
    val = ptr ? json(ptr->GetUUID()) : json(nullptr);
}

template<class T> requires std::is_base_of_v<Object, T>
void ObjectLinker::SaveAsset(
    json& obj,
    const std::string& keyForUUID,
    const sptr<T>& ptr)
{
    obj[keyForUUID] = ptr ? json(ptr->GetUUID()) : json(nullptr);
}

template<class T>
bool ObjectLinker::RestoreLink(
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
bool ObjectLinker::RestoreLink(
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
bool ObjectLinker::RestoreLink(
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
bool ObjectLinker::RestoreLink(
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
bool ObjectLinker::RestoreAsset(
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
bool ObjectLinker::RestoreAsset(
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

template<class T = Object> requires std::is_base_of_v<Object, T>
inline sptr<T> Instantiate(const sptr<Object>& object)
{
    json obj;
    object->ToJson(obj);

    ObjectLinker linker;
    sptr<T> ret = Object::CreateFromJson<T>(obj, &linker);
    // update IDs of all objects to new unique ones
    if (ret) ObjectLinker::Link(&linker);

    return ret;
}

} // system
} // mw
