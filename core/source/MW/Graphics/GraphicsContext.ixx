/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.Buffer;
import Microwave.Graphics.GraphicsTypes;
import Microwave.Graphics.RenderTarget;
import Microwave.Graphics.RenderTexture;
import Microwave.Graphics.Shader;
import Microwave.Graphics.Texture;
import Microwave.Graphics.Color;
import Microwave.Graphics.Internal.HWBuffer;
import Microwave.Graphics.Internal.HWContext;
import Microwave.Graphics.Internal.HWRenderTexture;
import Microwave.Graphics.Internal.HWTexture;
import Microwave.Math;
import Microwave.System.Object;
import Microwave.System.Pointers;
import <cstddef>;
import <cstdlib>;
import <span>;
import <vector>;

export namespace mw {
inline namespace gfx {

class GraphicsContext : public Object
{
    thread_local static sptr<GraphicsContext> currentContext;
public:
    sptr<HWContext> context;
    sptr<RenderTarget> renderTarget;
    IntRect viewportRect = IntRect();
    IntRect scissorRect = IntRect();
    Color clearColor = Color::White();
    bool scissorTestEnabled = false;
    bool blendingEnabled = true;
    bool depthWriteEnabled = true;
    DepthTest depthTest = DepthTest::LessOrEqual;
    CullMode cullMode = CullMode::Back;
    BlendFactor srcColorFactor = BlendFactor::One;
    BlendFactor dstColorFactor = BlendFactor::Zero;
    BlendFactor srcAlphaFactor = BlendFactor::One;
    BlendFactor dstAlphaFactor = BlendFactor::Zero;
    BlendOperation colorBlendOperation = BlendOperation::Add;
    BlendOperation alphaBlendOperation = BlendOperation::Add;
    Color blendColor = Color::White();
    bool colorMask[4] = { true, true, true, true };
    int swapInterval = 1;

    static void SetCurrent(const sptr<GraphicsContext>& current);
    static sptr<GraphicsContext> GetCurrent();

    GraphicsContext(GraphicsDriverType type = GraphicsDriverType::Default);
    ~GraphicsContext();

    Vec2 GetDepthRangeNDC() const;

    IntRect GetViewport() const;
    void SetViewport(const IntRect& rect);

    bool IsBlendingEnabled() const;
    void SetBlendingEnabled(bool enabled);

    bool IsScissorTestEnabled() const;
    void SetScissorTestEnabled(bool enabled);

    IntRect GetScissorRect() const;
    void SetScissorRect(const IntRect& rect);

    Color GetClearColor() const;
    void SetClearColor(const Color& color);

    void SetColorMask(bool red, bool green, bool blue, bool alpha);
    
    void SetBlendFactors(BlendFactor source, BlendFactor dest);
    void SetBlendFactors(
        BlendFactor sourceColor, BlendFactor destColor,
        BlendFactor sourceAlpha, BlendFactor destAlpha);

    BlendFactor GetSourceColorBlendFactor() const;
    BlendFactor GetDestColorBlendFactor() const;
    BlendFactor GetSourceAlphaBlendFactor() const;
    BlendFactor GetDestAlphaBlendFactor() const;
    
    void SetBlendOperations(BlendOperation blendOp);
    void SetBlendOperations(BlendOperation colorBlendOp, BlendOperation alphaBlendOp);

    BlendOperation GetColorBlendOperation() const;
    BlendOperation GetAlphaBlendOperation() const;

    Color GetBlendColor() const;
    void SetBlendColor(Color color);

    CullMode GetCullMode() const;
    void SetCullMode(CullMode mode);

    DepthTest GetDepthTest() const;
    void SetDepthTest(DepthTest test);

    bool IsDepthWriteEnabled() const;
    void SetDepthWriteEnabled(bool enabled);

    int GetSwapInterval() const;
    void SetSwapInterval(int interval);

    sptr<RenderTarget> GetRenderTarget() const;
    void SetRenderTarget(const sptr<RenderTarget>& target);
    void RebindRenderTarget();

    void Clear(bool depth, bool color);
    void Flush();
    void Flip();

    void DrawArray(int start, int count, DrawMode mode);
    void DrawIndexed(int start, int count, DrawMode mode);

    Mat4 GetOrthoMatrix(
        float left, float right,
        float bottom, float top,
        float zNear, float zFar);

    Mat4 GetPerspectiveMatrix(
        float fovY, float aspect,
        float zNear, float zFar);
};

} // gfx
} // mw
