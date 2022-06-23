/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Data.Internal.FBXModelConverter;
import Microwave.Data.Internal.FBXUDPParser;
import Microwave.Graphics.RenderQueue;
import Microwave.IO.FileStream;
import Microwave.Math;
import Microwave.System.Exception;
import Microwave.System.Path;
import <MW/Data/Internal/FBX_SDK.h>;
import <MW/System/Debug.h>;
import <algorithm>;
import <cstddef>;
import <cstdint>;
import <iomanip>;
import <filesystem>;
import <map>;
import <set>;
import <span>;
import <stdexcept>;
import <string>;
import <sstream>;
import <vector>;
import <unordered_map>;
import <unordered_set>;

namespace mw {
inline namespace data {

class NamePool
{
    std::unordered_set<std::string> pool;
public:
    std::string GetUniqueName(std::string_view baseName)
    {
        constexpr int MaxInstances = 100000;

        std::string name { baseName };
        int nameInstance = 1;
        std::stringstream ss;

        while (pool.count(name) != 0 && nameInstance != MaxInstances)
        {
            ss.str("");
            ss << baseName;
            ss << std::setfill('0') << std::setw(3) << nameInstance++;
            name = ss.str();
        }

        if (nameInstance == MaxInstances)
            throw Exception("failed to generate unique name");

        pool.insert(name);
        return name;
    }

    void Clear() {
        pool.clear();
    }
};

class FbxMemoryStream : public fbxsdk::FbxStream
{
    gptr<Stream> stream;
    mutable std::span<std::byte>::iterator pos;
    int readerId;
    int writerId;
public:
    FbxMemoryStream(
        fbxsdk::FbxManager* pSdkManager,
        const gptr<Stream>& stream)
        : stream(stream)
    {
        readerId = pSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX (*.fbx)");
        writerId = -1;
    }

    ~FbxMemoryStream() {
        Close();
    }

    virtual EState GetState() {
        return stream->CanRead() ? fbxsdk::FbxStream::eOpen : fbxsdk::FbxStream::eClosed;
    }

    virtual bool Open(void* /*pStreamData*/) {
        stream->Seek(0, SeekOrigin::Begin);
        return stream->CanRead();
    }

    virtual bool Close() {
        stream->Seek(0, SeekOrigin::Begin);
        return true;
    }

    virtual bool Flush() {
        return true;
    }

    virtual size_t Write(const void* pData, fbxsdk::FbxUInt64 pSize) {
        Assert(0); // read only
        return 0;
    }

    virtual size_t Read(void* pData, fbxsdk::FbxUInt64 pSize) const
    {
        std::byte* pBuffer = (std::byte*)pData;
        return stream->Read(std::span<std::byte>(pBuffer, (size_t)pSize));
    }

    virtual int GetReaderID() const {
        return readerId;
    }

    virtual int GetWriterID() const {
        return writerId;
    }

    void Seek(const fbxsdk::FbxInt64& pOffset, const fbxsdk::FbxFile::ESeekPos& pSeekPos)
    {
        switch (pSeekPos)
        {
        case fbxsdk::FbxFile::eBegin:
            stream->Seek(pOffset, SeekOrigin::Begin);
            break;

        case fbxsdk::FbxFile::eCurrent:
            stream->Seek(pOffset, SeekOrigin::Current);
            break;

        case fbxsdk::FbxFile::eEnd:
            stream->Seek(pOffset, SeekOrigin::End);
            break;
        }
    }

    virtual fbxsdk::FbxInt64 GetPosition() const {
        return (fbxsdk::FbxInt64)stream->GetPosition();
    }

    virtual void SetPosition(fbxsdk::FbxInt64 pPosition) {
        stream->SetPosition((size_t)pPosition);
    }

    virtual int GetError() const {
        return 0;
    }

    virtual void ClearError() {

    }
};

struct LoaderState
{
    gptr<Model> model;
    fbxsdk::FbxAnimEvaluator* animEvaluator = nullptr;
    fbxsdk::FbxAnimLayer* fbxAnimationLayer = nullptr;
    ModelSettings* pImportSettings = nullptr;
    gmap<uint64_t, gptr<ModelNode>> nodesByUniqueID;
    gmap<uint64_t, gptr<ModelMesh>> meshesByUniqueID;
    gmap<std::string, gptr<ModelMaterial>> materials; // ByName
    std::vector<ModelNode*> skinnedMeshNodes;
    NamePool meshNames;

    LoaderState(){}
};

class FBXModelConverterImpl
{
public:
    inline static const int MaxBones = 4;

    FBXModelConverterImpl(){}

    static gptr<Model> Convert(
        const gptr<Stream>& stream,
        ModelSettings& importSettings)
    {
        LoaderState state;
        state.pImportSettings = &importSettings;
        state.model = gpnew<Model>();

        fbxsdk::FbxManager* sdkManager = fbxsdk::FbxManager::Create();

        try
        {
            fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(sdkManager, IOSROOT);
            ios->SetBoolProp(IMP_FBX_EXTRACT_EMBEDDED_DATA, false);
            sdkManager->SetIOSettings(ios);

            fbxsdk::FbxImporter* importer = fbxsdk::FbxImporter::Create(sdkManager, "");
            FbxMemoryStream fbxStream(sdkManager, stream);

            if (!importer->Initialize(&fbxStream, nullptr, -1, sdkManager->GetIOSettings()))
                throw Exception("failed to initialize FBX importer");

            fbxsdk::FbxScene* scene = fbxsdk::FbxScene::Create(sdkManager, "scene");
            bool didImport = importer->Import(scene);
            importer->Destroy();
            importer = nullptr;

            if (!didImport)
                throw Exception("failed to import FBX scene");

            fbxsdk::FbxGeometryConverter cvt(sdkManager);
            cvt.Triangulate(scene, true, false);

            // Y up, Z forward, X right
            fbxsdk::FbxAxisSystem axisSystemMW(
                fbxsdk::FbxAxisSystem::eYAxis,
                (fbxsdk::FbxAxisSystem::EFrontVector) -fbxsdk::FbxAxisSystem::eParityOdd,
                fbxsdk::FbxAxisSystem::eLeftHanded);

            axisSystemMW.DeepConvertScene(scene);

            fbxsdk::FbxAnimStack* pAnimStack = scene->GetSrcObject<fbxsdk::FbxAnimStack>(0);
            state.fbxAnimationLayer = pAnimStack ? pAnimStack->GetMember<fbxsdk::FbxAnimLayer>(0) : nullptr;
            state.model->animationFPS = (float)fbxsdk::FbxTime::GetFrameRate(scene->GetGlobalSettings().GetTimeMode());

            if (state.fbxAnimationLayer)
            {
                if (importSettings.clipSpecs.empty())
                {
                    auto& fbxSettings = scene->GetGlobalSettings();
                    auto timeMode = fbxSettings.GetTimeMode();

                    fbxsdk::FbxTimeSpan timeSpan;
                    fbxSettings.GetTimelineDefaultTimeSpan(timeSpan);
                    float startSec = (float)timeSpan.GetStart().GetSecondDouble();
                    float stopSec = (float)timeSpan.GetStop().GetSecondDouble();
                    int timelineStart = (int)timeSpan.GetStart().GetFrameCount(timeMode);
                    int timelineStop = (int)timeSpan.GetStop().GetFrameCount(timeMode);

                    ClipSpec spec;
                    spec.name = "Default";
                    spec.start = timelineStart;
                    spec.stop = timelineStop;
                    spec.wrapMode = AnimationWrapMode::Loop;
                    importSettings.clipSpecs.push_back(std::move(spec));
                }

                for (auto& spec : importSettings.clipSpecs)
                {
                    auto clip = gpnew<ModelAnimationClip>();
                    clip->name = spec.name;
                    clip->wrapMode = (ModelAnimationWrapMode)spec.wrapMode;
                    state.model->clips.push_back(std::move(clip));
                }
            }

            NamePool siblingNames;
            state.model->rootNode = CreateModelNode(state, nullptr, siblingNames, scene->GetRootNode());

            // resolve bone paths
            for (auto& meshNode : state.skinnedMeshNodes) // change to skeleton nodes
            {
                for (auto& bone : meshNode->mesh->bones)
                {
                    uint64_t linkNodeID = std::stoull(bone.linkNodePath.string());
                    auto linkNode = state.nodesByUniqueID[linkNodeID];
                    bone.linkNodePath = linkNode->GetFullPath(); // path from skeleton root
                }
            }

            if (!importSettings.keepRootTransform)
            {
                state.model->rootNode->localPosition = Vec3::Zero();
                state.model->rootNode->localRotation = Quat::Identity();
                state.model->rootNode->localScale = Vec3::One();
            }

            sdkManager->Destroy();
        }
        catch (...)
        {
            sdkManager->Destroy();
            throw;
        }

        return state.model;
    }

    static gptr<ModelNode> CreateModelNode(
        LoaderState& state,
        ModelNode* parent,
        NamePool& siblingNames,
        fbxsdk::FbxNode* fbxNode)
    {
        std::string baseNodeName = (const char*)fbxNode->GetNameOnly();
        if (baseNodeName.empty()) baseNodeName = "Node";

        std::string name = siblingNames.GetUniqueName(baseNodeName);

        fbxsdk::FbxAMatrix lTransform = fbxNode->EvaluateLocalTransform();
        Vec3 localPos = FromFBX(lTransform.GetT());
        Quat localRot = FromFBX(lTransform.GetQ());
        Vec3 localScale = FromFBX(lTransform.GetS());

        auto node = gpnew<ModelNode>();
        size_t nodeIndex = state.model->nodes.size();
        state.nodesByUniqueID[fbxNode->GetUniqueID()] = node;
        state.model->nodes.push_back(node);

        node->index = nodeIndex;
        node->name = name;
        node->active = fbxNode->Show.Get();
        node->parent = parent;
        node->localPosition = localPos;
        node->localRotation = localRot;
        node->localScale = localScale;

        ImportCollider(state, fbxNode, node.get());
        ImportMesh(state, fbxNode, node.get());
        ImportAnimation(state, fbxNode, node.get());

        // process children
        int childCount = fbxNode->GetChildCount();
        node->children.reserve(childCount);

        NamePool childSiblingNames;

        for (int i = 0; i < childCount; i++)
        {
            auto child = CreateModelNode(state, node.get(), childSiblingNames, fbxNode->GetChild(i));
            node->children.push_back(child);
        }

        return node;
    }

    static fbxsdk::FbxAMatrix GetGeometryOffset(fbxsdk::FbxNode* node)
    {
        fbxsdk::FbxVector4 t = node->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);
        fbxsdk::FbxVector4 s = node->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);
        fbxsdk::FbxVector4 r = node->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
        return fbxsdk::FbxAMatrix(t, r, s);
    }

    static void ImportCollider(
        LoaderState& state, fbxsdk::FbxNode* fbxNode, ModelNode* node)
    {
        gptr<ModelCollider> collider;

        fbxsdk::FbxProperty udp = fbxNode->FindProperty("UDP3DSMAX");
        if (udp.IsValid())
        {
            fbxsdk::FbxString fbxPropStr = udp.Get<fbxsdk::FbxString>();
            auto props = FBXUDPMap::Parse((const char*)fbxPropStr);

            auto colliderType = props.TryGet<std::string>("ColliderType");
            if (colliderType)
            {
                collider = gpnew<ModelCollider>();
                collider->name = *colliderType;

                if (auto colliderNodeName = props.TryGet<std::string>("ColliderNode"))
                {
                    auto colliderNode = fbxNode->FindChild(colliderNodeName->c_str(), false);
                    if (colliderNode)
                    {
                        fbxsdk::FbxString pivotID = colliderNode->GetNameOnly();
                        collider->pivotID = (const char*)pivotID;
                        collider->mesh = ImportCollisionMesh(state, colliderNode);
                    }
                }

                if (*colliderType == "Sphere")
                {
                    collider->type = ModelColliderType::Sphere;

                    if (auto radius = props.TryGet<float>("ColliderRadius"))
                        collider->radius = *radius;
                }
                else if (*colliderType == "Box")
                {
                    collider->type = ModelColliderType::Box;

                    if (auto length = props.TryGet<float>("ColliderLength"))
                        collider->length = *length;

                    if (auto width = props.TryGet<float>("ColliderWidth"))
                        collider->width = *width;

                    if (auto height = props.TryGet<float>("ColliderHeight"))
                        collider->height = *height;
                }
                else if (*colliderType == "Capsule")
                {
                    collider->type = ModelColliderType::Capsule;

                    if (auto radius = props.TryGet<float>("ColliderRadius"))
                        collider->radius = *radius;

                    if (auto height = props.TryGet<float>("ColliderHeight"))
                        collider->height = *height;
                }
                else if (*colliderType == "Convex")
                {
                    collider->type = ModelColliderType::Convex;
                }

                auto bodyType = props.TryGet<std::string>("ColliderBodyType");
                if (bodyType)
                {
                    if (*bodyType == "Dynamic")
                        collider->bodyType = ModelRigidBodyType::Dynamic;
                    else if (*bodyType == "Kinematic")
                        collider->bodyType = ModelRigidBodyType::Kinematic;
                    else if (*bodyType == "Static")
                        collider->bodyType = ModelRigidBodyType::Static;
                }

                auto friction = props.TryGet<float>("ColliderFriction");
                if (friction)
                {
                    collider->friction = *friction;
                }
            }
        }

        node->collider = collider;
    }

    static gptr<ModelMesh> ImportCollisionMesh(
        LoaderState& state, fbxsdk::FbxNode* fbxNode)
    {
        auto fbxMesh = fbxNode->GetMesh();
        if (fbxMesh == nullptr)
            return nullptr;

        // extract vertices and triangle indices
        fbxsdk::FbxVector4* fbxVertices = fbxMesh->GetControlPoints();
        int triCount = fbxMesh->GetPolygonCount();
        int fbxVertCount = fbxMesh->GetControlPointsCount();

        gptr<ModelMesh> mesh = gpnew<ModelMesh>();
        mesh->name = (const char*)fbxNode->GetNameOnly();
        mesh->elements.emplace_back();

        ModelMeshElement& element = mesh->elements.back();
        element.drawMode = DrawMode::Lines;

        mesh->vertices.reserve(fbxVertCount);

        for (int p = 0; p < triCount; ++p)
        {
            for (int v = 0; v < 3; ++v)
            {
                int fbxVertIndex = fbxMesh->GetPolygonVertex(p, v);
                Vec3 vertex = FromFBX(fbxVertices[fbxVertIndex]);

                auto IsSame = [](const Vec3& a, const Vec3& b) {
                    return abs(b.x - a.x) < 0.00001f &&
                        abs(b.y - a.y) < 0.00001f &&
                        abs(b.z - a.z) < 0.00001f;
                };

                int vertIndex = -1;
                for (size_t i = 0, sz = mesh->vertices.size(); i < sz; ++i)
                {
                    if (IsSame(vertex, mesh->vertices[i])) {
                        vertIndex = i;
                        break;
                    }
                }

                if (vertIndex == -1) {
                    vertIndex = mesh->vertices.size();
                    mesh->vertices.push_back(vertex);
                }

                element.indices.push_back(vertIndex);
            }
        }

        // apply geometry offset
        fbxsdk::FbxNode* meshNode = fbxMesh->GetNode();
        fbxsdk::FbxAMatrix offset = GetGeometryOffset(meshNode);

        Mat4 vxf = FromFBX(offset);
        for (Vec3& v : mesh->vertices)
            v = Vec4(v, 1) * vxf;

        // convert triangles to lines
        std::set<uint64_t> edges;
        Assert(element.indices.size() % 3 == 0);

        for (size_t i = 0; i != element.indices.size(); i += 3)
        {
            uint64_t a = element.indices[i + 0];
            uint64_t b = element.indices[i + 1];
            uint64_t c = element.indices[i + 2];

            uint64_t edge1 = (a < b) ? ((a << 32) | b) : ((b << 32) | a);
            uint64_t edge2 = (b < c) ? ((b << 32) | c) : ((c << 32) | b);
            uint64_t edge3 = (c < a) ? ((c << 32) | a) : ((a << 32) | c);

            edges.insert(edge1);
            edges.insert(edge2);
            edges.insert(edge3);
        }

        element.indices.clear();

        for (uint64_t edge : edges)
        {
            uint64_t a = (edge >> 32) & 0xFFFFFFFF;
            uint64_t b = edge & 0xFFFFFFFF;
            element.indices.push_back((int)a);
            element.indices.push_back((int)b);
        }

        auto removedAttrib = fbxNode->RemoveNodeAttribute(fbxMesh);
        Assert(removedAttrib); // if null, remove failed
        fbxMesh->Destroy();

        return mesh;
    }

    static void ImportMesh(LoaderState& state, fbxsdk::FbxNode* fbxNode, ModelNode* node)
    {
        auto fbxMesh = fbxNode->GetMesh();
        if (fbxMesh == nullptr)
            return;

        if (!fbxNode->GetVisibility())
        {
            auto removedAttrib = fbxNode->RemoveNodeAttribute(fbxMesh);
            Assert(removedAttrib); // if null, remove failed
            fbxMesh->Destroy();
            return;
        }

        uint64_t meshID = fbxMesh->GetUniqueID();

        auto it = state.meshesByUniqueID.find(meshID);
        if (it != state.meshesByUniqueID.end())
        {
            node->mesh = it->second;
            return;
        }

        std::string name = state.meshNames.GetUniqueName(node->name);

        // create actual mesh
        auto mesh = gpnew<ModelMesh>();
        mesh->name = name;

        // MESH
        std::map<int32_t, ModelMeshElement> elements;
        std::unordered_multimap<int, uint32_t> newVertexMapping;

        fbxsdk::FbxVector4* fbxVertices = fbxMesh->GetControlPoints();
        int triCount = fbxMesh->GetPolygonCount();
        int fbxVertCount = fbxMesh->GetControlPointsCount();

        mesh->vertices.reserve(fbxVertCount);
        mesh->normals.reserve(fbxVertCount);
        mesh->texcoords.reserve(fbxVertCount);

        // create one element per material
        auto matCount = fbxNode->GetMaterialCount();
        for (int i = 0; i != matCount; ++i)
        {
            ModelMeshElement& element = elements[i];
            element.drawMode = DrawMode::Triangles;

            fbxsdk::FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(i);
            if (fbxMaterial)
            {
                gptr<ModelMaterial> mat;

                std::string name = (const char*)fbxMaterial->GetNameOnly();

                if (auto it = state.materials.find(name);
                    it != state.materials.end())
                {
                    mat = it->second;
                }
                else
                {
                    mat = FromFBX(fbxMaterial);
                    state.materials[name] = mat;
                }

                element.material = mat;
            }
        }

        fbxsdk::FbxLayerElementArrayTemplate<int>* materialIDs;
        fbxMesh->GetMaterialIndices(&materialIDs);

        fbxsdk::FbxStringList uvSetNames;
        fbxMesh->GetUVSetNames(uvSetNames);
        fbxsdk::FbxLayerElementUV* uvLayer = fbxMesh->GetLayer(0)->GetUVs();

        for (int p = 0; p < triCount; ++p)
        {
            int elementID = materialIDs->GetAt(p);
            ModelMeshElement& element = elements[elementID];

            for (int v = 0; v < 3; ++v)
            {
                // Get Vertex Position
                int fbxVertIndex = fbxMesh->GetPolygonVertex(p, v);

                // Get Normal
                fbxsdk::FbxVector4 fbxNormal;
                fbxMesh->GetPolygonVertexNormal(p, v, fbxNormal);
                fbxNormal.Normalize();

                // Get Texture Coordinate
                fbxsdk::FbxVector2 fbxTexcoord(0, 0);

                if (uvLayer)
                {
                    switch (uvLayer->GetMappingMode())
                    {
                    case fbxsdk::FbxLayerElement::eByControlPoint:
                        fbxTexcoord = uvLayer->mDirectArray->GetAt(fbxVertIndex);
                        break;

                    case fbxsdk::FbxLayerElement::eByPolygonVertex:
                        bool unmapped;
                        // add lightmap/multi-uv later
                        fbxMesh->GetPolygonVertexUV(p, v, uvSetNames[0], fbxTexcoord, unmapped);
                        break;
                    }
                }

                Vec3 vertex = FromFBX(fbxVertices[fbxVertIndex]);
                Vec3 normal = FromFBX(fbxNormal);
                Vec2 texcoord = FromFBXTexcoord(fbxTexcoord);

                auto Identical = [](Vec3 a, Vec3 b) {
                    return
                        abs(b.x - a.x) < 0.00001f &&
                        abs(b.y - a.y) < 0.00001f &&
                        abs(b.z - a.z) < 0.00001f;
                };

                // check for existing identical verts
                int vertIndex = -1;
                for (size_t i = 0, sz = mesh->vertices.size(); i < sz; ++i)
                {
                    if (Identical(vertex, mesh->vertices[i]) &&
                        Identical(normal, mesh->normals[i]) &&
                        Identical(texcoord, mesh->texcoords[i]))
                    {
                        vertIndex = i;
                        break;
                    }
                }

                if (vertIndex == -1)
                {
                    vertIndex = mesh->vertices.size();

                    mesh->vertices.push_back(vertex);
                    mesh->normals.push_back(normal);
                    mesh->texcoords.push_back(texcoord);

                    newVertexMapping.insert(std::make_pair(fbxVertIndex, vertIndex));
                }

                element.indices.push_back(vertIndex);
            }
        }

        // fbxNode == meshNode ?
        fbxsdk::FbxNode* meshNode = fbxMesh->GetNode();
        fbxsdk::FbxAMatrix offset = GetGeometryOffset(meshNode);

        Mat4 vxf = FromFBX(offset);
        for (Vec3& v : mesh->vertices)
            v = Vec4(v, 1) * vxf;

        vxf = vxf.Inverse().Transposed();

        for (Vec3& n : mesh->normals)
            n = Vec4(n, 1) * vxf;

        for (auto e : elements)
            mesh->elements.push_back(std::move(e.second));

        elements.clear();

        //mesh->RecalcBounds();

        // SKIN DEFORMATION
        fbxsdk::FbxSkin* skin = (fbxsdk::FbxSkin*)fbxMesh->GetDeformer(0, fbxsdk::FbxDeformer::eSkin);
        if (skin)
        {
            state.skinnedMeshNodes.push_back(node);

            mesh->skinType = FromFBX(skin->GetSkinningType());

            int nClusters = skin->GetClusterCount();
            mesh->bones.reserve(nClusters);

            // one element per vertex
            // each element contains set of index/weight pairs
            std::vector<std::vector<std::pair<uint32_t, float>>> vertWeightSets;
            vertWeightSets.resize(mesh->vertices.size());

            for (int c = 0; c < nClusters; ++c)
            {
                Bone bone;

                fbxsdk::FbxCluster* fbxCluster = skin->GetCluster(c);

                // store unique id temporarily - resolved later
                bone.linkNodePath = std::to_string(fbxCluster->GetLink()->GetUniqueID());
                bone.linkMode = FromFBX(fbxCluster->GetLinkMode());

                // get bind matrices for this bone and the mesh
                fbxsdk::FbxAMatrix meshBindTransform;
                fbxsdk::FbxAMatrix boneBindTransform;
                fbxCluster->GetTransformMatrix(meshBindTransform);
                fbxCluster->GetTransformLinkMatrix(boneBindTransform);

                bone.meshBindMatrix = FromFBX(meshBindTransform);
                bone.boneBindMatrix = FromFBX(boneBindTransform);
                bone.invBoneBindMatrix = bone.boneBindMatrix.Inverse();

                mesh->bones.push_back(bone);

                int* controlPointIndices = fbxCluster->GetControlPointIndices();
                double* controlPointWeights = fbxCluster->GetControlPointWeights();
                int        clusterWeightCount = fbxCluster->GetControlPointIndicesCount();

                // match cluster control points with new vertex indices
                for (int v = 0; v < clusterWeightCount; ++v)
                {
                    auto range = newVertexMapping.equal_range(controlPointIndices[v]);
                    for (auto it = range.first; it != range.second; ++it)
                    {
                        auto weight = std::make_pair(c, (float)controlPointWeights[v]);
                        vertWeightSets[it->second].push_back(weight);
                    }
                }
            }

            // sort all vertex sets in descending order of weight so top 4 can be chosen
            for (auto& weightSet : vertWeightSets)
            {
                std::sort(weightSet.begin(), weightSet.end(),
                    [](std::pair<uint32_t, float>& x, std::pair<uint32_t, float>& y) {
                        return x.second > y.second;
                    });
            }

            int nVerts = (int)mesh->vertices.size();
            mesh->boneIndices.resize(nVerts);
            mesh->boneWeights.resize(nVerts);

            BoneLinkMode mode = mesh->bones[0].linkMode;

            // normalize and store the bone links.
            for (int v = 0; v < nVerts; ++v)
            {
                auto& weightSet = vertWeightSets[v];

                // attatch unbound vertices to bone 0.
                if (weightSet.size() == 0)
                {
                    weightSet.push_back(std::pair<uint32_t, float>(0, 1.0f));
                }

                // fill up extra bone slots with zero weights
                while (weightSet.size() < MaxBones)
                {
                    weightSet.push_back(std::pair<uint32_t, float>(0, 0.0f));
                }

                // normalize bone weights unless link mode is additive
                if (mode != BoneLinkMode::Additive)
                {
                    float totalWeight = 0.0f;

                    for (uint32_t i = 0; i < MaxBones; ++i)
                        totalWeight += weightSet[i].second;

                    if (totalWeight > 0.0f)
                    {
                        for (uint32_t b = 0; b < MaxBones; ++b)
                            weightSet[b].second /= totalWeight;
                    }
                }

                for (uint32_t b = 0; b < MaxBones; ++b)
                {
                    mesh->boneIndices[v][b] = weightSet[b].first;
                    mesh->boneWeights[v][b] = weightSet[b].second;
                }
            }
        }

        state.meshesByUniqueID[meshID] = mesh;
        node->mesh = mesh;
    }

    static void ImportAnimation(LoaderState& state, fbxsdk::FbxNode* fbxNode, ModelNode* node)
    {
        auto pAnimLayer = state.fbxAnimationLayer;
        if (!pAnimLayer)
            return;

        std::array<fbxsdk::FbxAnimCurve*, 9> curves {
            fbxNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X),
            fbxNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y),
            fbxNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z),
            fbxNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X),
            fbxNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y),
            fbxNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z),
            fbxNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X),
            fbxNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y),
            fbxNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z),
        };

        bool nodeHasAnimation = std::any_of(
            curves.begin(), curves.end(),
            [](fbxsdk::FbxAnimCurve* curve)
            {
                return curve && curve->KeyGetCount() > 0;
            });

        if (nodeHasAnimation)
        {
            fbxsdk::FbxTime::EMode timeMode = pAnimLayer->GetScene()->GetGlobalSettings().GetTimeMode();
            fbxsdk::FbxAMatrix offset = GetGeometryOffset(fbxNode);
            auto nodePath = node->GetFullPath();

            for(size_t i = 0; i != state.pImportSettings->clipSpecs.size(); ++i)
            {
                auto& spec = state.pImportSettings->clipSpecs[i];
                auto& clip = state.model->clips[i];

                auto frameCount = spec.stop - spec.start + 1;

                auto track = gpnew<AnimationTrack>();
                track->frames.reserve(frameCount);

                fbxsdk::FbxTime startFrameTime;
                startFrameTime.SetFrame((fbxsdk::FbxLongLong)spec.start, timeMode);
                float startTime = (float)startFrameTime.GetSecondDouble();

                for (int frame = spec.start; frame <= spec.stop; ++frame)
                {
                    fbxsdk::FbxTime frameTime;
                    frameTime.SetFrame((fbxsdk::FbxLongLong)frame, timeMode);
                    float time = (float)frameTime.GetSecondDouble() - startTime;

                    fbxsdk::FbxAMatrix trans = fbxNode->EvaluateLocalTransform(frameTime);// * offset;

                    Vec3 pos = FromFBX(trans.GetT());
                    Quat rot = FromFBX(trans.GetQ());
                    Vec3 scale = FromFBX(trans.GetS());

                    track->frames.push_back(Keyframe{ time, { pos, rot, scale } });
                }

                clip->tracks[nodePath] = std::move(track);
            }
        }
    }

    static gptr<ModelMaterial> FromFBX(fbxsdk::FbxSurfaceMaterial* fbxMaterial)
    {
        gptr<ModelMaterial> mat = gpnew<ModelMaterial>();

        mat->name = fbxMaterial->GetNameOnly();

        bool hasAlpha = false;

        // DIFFUSE MAP
        fbxsdk::FbxProperty diffuseProp = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse);
        if (diffuseProp.IsValid())
        {
            // diffuse map
            fbxsdk::FbxFileTexture* diffuseTex = diffuseProp.GetSrcObject<fbxsdk::FbxFileTexture>();
            if (diffuseTex)
            {
                Vec4 uvTransform;
                uvTransform.x = (float)diffuseTex->GetScaleU();
                uvTransform.y = (float)diffuseTex->GetScaleV();
                uvTransform.z = (float)diffuseTex->GetTranslationU();
                uvTransform.w = 1.0f - (float)diffuseTex->GetTranslationV();
                mat->uDiffuseTex_ST = uvTransform;
                mat->uDiffuseTex = path(diffuseTex->GetFileName()).filename();

                auto alphaSource = diffuseTex->GetAlphaSource();
                if (alphaSource == fbxsdk::FbxFileTexture::eBlack)
                    hasAlpha = true;
            }

            if(mat->uDiffuseTex.empty())
                mat->uDiffuseTex = "white.png";

            // diffuse color
            fbxsdk::FbxColor col = diffuseProp.Get<fbxsdk::FbxColor>();

            float alpha = 1;

            auto alphaProp = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sTransparencyFactor);
            if (alphaProp.IsValid() && alphaProp.Modified())
            {
                alpha = (float)alphaProp.Get<fbxsdk::FbxDouble>();
                if (alpha < 1.0f)
                    hasAlpha = true;
            }

            mat->uDiffuseColor = FromFBX(col, alpha);
        }

        if (hasAlpha)
        {
            mat->sourceColorBlendFactor = BlendFactor::SrcAlpha;
            mat->sourceAlphaBlendFactor = BlendFactor::SrcAlpha;
            mat->destColorBlendFactor = BlendFactor::OneMinusSrcAlpha;
            mat->destAlphaBlendFactor = BlendFactor::OneMinusSrcAlpha;
            mat->renderQueue = RenderQueue::Transparent;
            mat->depthTest = DepthTest::LessOrEqual;
            mat->depthWriteEnabled = false;
        }
        else
        {
            mat->sourceColorBlendFactor = BlendFactor::One;
            mat->sourceAlphaBlendFactor = BlendFactor::One;
            mat->destColorBlendFactor = BlendFactor::Zero;
            mat->destAlphaBlendFactor = BlendFactor::Zero;
            mat->renderQueue = RenderQueue::Opaque;
            mat->depthTest = DepthTest::LessOrEqual;
            mat->depthWriteEnabled = true;
        }

        // diffuse factor
        fbxsdk::FbxProperty diffFactorProp = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuseFactor);
        if (diffFactorProp.IsValid())
            mat->uDiffuseFactor = (float)diffFactorProp.Get<fbxsdk::FbxDouble>();

        // SPECULAR MAP
        fbxsdk::FbxProperty specularProp = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sSpecular);
        if (specularProp.IsValid())
        {
            // specular map
            fbxsdk::FbxFileTexture* specularTex = specularProp.GetSrcObject<fbxsdk::FbxFileTexture>();
            if (specularTex)
            {
                Vec4 uvTransform;
                uvTransform.x = (float)specularTex->GetScaleU();
                uvTransform.y = (float)specularTex->GetScaleV();
                uvTransform.z = (float)specularTex->GetTranslationU();
                uvTransform.w = 1.0f - (float)specularTex->GetTranslationV();
                mat->uSpecularTex_ST = uvTransform;
                mat->uSpecularTex = path(specularTex->GetFileName()).filename();
            }

            // specular color
            mat->uSpecularColor = FromFBX(specularProp.Get<fbxsdk::FbxColor>());
        }

        // shininess
        fbxsdk::FbxProperty shininessProp = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sShininess);
        if (shininessProp.IsValid())
            mat->uShininess = (float)(log2(shininessProp.Get<fbxsdk::FbxDouble>()) / 10.0);

        // specular factor
        fbxsdk::FbxProperty specFactorProp = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sSpecularFactor);
        if (specFactorProp.IsValid())
            mat->uSpecularFactor = (float)specFactorProp.Get<fbxsdk::FbxDouble>();

        // NORMAL MAP
        fbxsdk::FbxProperty normalProp = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sBump);
        if (normalProp.IsValid())
        {
            // normal map
            fbxsdk::FbxFileTexture* normalTex = normalProp.GetSrcObject<fbxsdk::FbxFileTexture>();
            if (normalTex)
            {
                Vec4 uvTransform;
                uvTransform.x = (float)normalTex->GetScaleU();
                uvTransform.y = (float)normalTex->GetScaleV();
                uvTransform.z = (float)normalTex->GetTranslationU();
                uvTransform.w = 1.0f - (float)normalTex->GetTranslationV();
                mat->uNormalTex_ST = uvTransform;
                mat->uNormalTex = path(normalTex->GetFileName()).filename();
            }
        }

        // normal factor
        fbxsdk::FbxProperty normFactorProp = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sBumpFactor);
        if (normFactorProp.IsValid())
            mat->uNormalFactor = (float)normFactorProp.Get<fbxsdk::FbxDouble>();

        return mat;
    }

    static Vec2 FromFBX(const fbxsdk::FbxVector2& v) {
        return Vec2((float)v[0], (float)v[1]);
    }

    static Vec2 FromFBXTexcoord(const fbxsdk::FbxVector2& v) {
        return Vec2((float)v[0], (float)(1.0f - v[1]));
    }

    static Vec4 FromFBX(const fbxsdk::FbxVector4& v) {
        return Vec4((float)v[0], (float)v[1], (float)v[2], (float)v[3]);
    }

    static Quat FromFBX(const fbxsdk::FbxQuaternion& q) {
        return Quat((float)q[0], (float)q[1], (float)q[2], (float)q[3]);
    }

    static Mat4 FromFBX(const fbxsdk::FbxAMatrix& m) {
        return Mat4::Transform(FromFBX(m.GetT()), FromFBX(m.GetS()), FromFBX(m.GetQ()));
    }

    static Color FromFBX(const fbxsdk::FbxColor& c, double alpha = 1.0) {
        return Color((float)c.mRed, (float)c.mGreen, (float)c.mBlue, (float)alpha);
    }

    static BoneLinkMode FromFBX(fbxsdk::FbxCluster::ELinkMode linkMode)
    {
        if (linkMode == fbxsdk::FbxCluster::eNormalize)
            return BoneLinkMode::Normalize;
        else if (linkMode == fbxsdk::FbxCluster::eAdditive)
            return BoneLinkMode::Additive;
        else if (linkMode == fbxsdk::FbxCluster::eTotalOne)
            return BoneLinkMode::TotalOne;

        return BoneLinkMode::Normalize;
    }

    static SkinType FromFBX(fbxsdk::FbxSkin::EType skinType)
    {
        if (skinType == fbxsdk::FbxSkin::eRigid)
            return SkinType::Rigid;
        else if (skinType == fbxsdk::FbxSkin::eLinear)
            return SkinType::Linear;
        else if (skinType == fbxsdk::FbxSkin::eDualQuaternion)
            return SkinType::DualQuaternion;
        else if (skinType == fbxsdk::FbxSkin::eBlend)
            return SkinType::Blend;

        return SkinType::Rigid;
    }
};

gptr<Model> FBXModelConverter::Convert(
    const gptr<Stream>& stream,
    ModelSettings& importSettings)
{
    return FBXModelConverterImpl::Convert(stream, importSettings);
}

} // data
} // mw
