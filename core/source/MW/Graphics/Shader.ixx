/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Shader;
import Microwave.Graphics.Color;
import Microwave.Graphics.ShaderInfo;
import Microwave.Graphics.Buffer;
import Microwave.Graphics.Texture;
import Microwave.Graphics.Internal.HWShader;
import Microwave.Math;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <initializer_list>;
import <string>;
import <unordered_map>;
import <vector>;

export namespace mw {
inline namespace gfx {

struct VertexSlot
{
    Semantic semantic = {};
    int semanticIndex = 0;
    sptr<Buffer> buffer;
    size_t offset = 0;
    size_t stride = 0;

    VertexSlot(
        Semantic semantic,
        int semanticIndex,
        const sptr<Buffer>& buffer,
        size_t offset,
        size_t stride)
        : semantic(semantic)
        , semanticIndex(semanticIndex)
        , buffer(buffer)
        , offset(offset)
        , stride(stride){}

    VertexSlot(
        Semantic semantic,
        const sptr<Buffer>& buffer,
        size_t offset,
        size_t size)
        : semantic(semantic)
        , semanticIndex(0)
        , buffer(buffer)
        , offset(offset)
        , stride(stride){}
};

// maps one or more Buffer objects to vertex input slots
struct VertexMapping
{
    std::vector<VertexSlot> slots;

    VertexMapping(){}

    VertexMapping(std::initializer_list<VertexSlot> init)
        : slots(init){}

    VertexMapping& operator=(std::initializer_list<VertexSlot> init) {
        slots.assign(init);
        return *this;
    }
};

class Shader : public Object
{
protected:
    sptr<HWShader> shader;
public:
    Shader(const std::string& source);
    Shader(const sptr<ShaderInfo>& info);
    virtual ~Shader(){}

    int GetAttributeID(const std::string& name);
    int GetAttributeID(Semantic semantic, int index);
    int GetUniformID(const std::string& name);

    bool HasAttribute(const std::string& name);
    bool HasUniform(const std::string& name);

    int GetAttributeCount() const;
    int GetUniformCount() const;

    ShaderVarType GetAttributeType(const std::string& name);
    ShaderVarType GetAttributeType(int id);

    ShaderVarType GetUniformType(const std::string& name);
    ShaderVarType GetUniformType(int id);
    
    void Bind();
    void Unbind();

    void SetVertexBuffer(const VertexMapping& vm);
    void SetVertexBuffer(Semantic semantic, int semanticIndex, const sptr<Buffer>& buffer, size_t offset, size_t stride);
    void SetVertexBuffer(const std::string& name, const sptr<Buffer>& buffer, size_t offset, size_t stride);
    void SetIndexBuffer(const sptr<Buffer>& buffer);

    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, const Vec2& value);
    void SetUniform(const std::string& name, const Vec3& value);
    void SetUniform(const std::string& name, const Vec4& value);
    void SetUniform(const std::string& name, const Mat2& value);
    void SetUniform(const std::string& name, const Mat3& value);
    void SetUniform(const std::string& name, const Mat4& value);
    void SetUniform(const std::string& name, const Color& value);
    void SetUniform(const std::string& name, const sptr<Texture>& texture);
};

} // gfx
} // mw
