/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

export module Microwave.Graphics.Internal.OpenGLAPI;
import Microwave.System.Console;
import <cassert>;
import <string>;
import <unordered_map>;
import <MW/System/Internal/PlatformHeaders.h>;

export namespace mw {
inline namespace gfx {
namespace gl {

inline void HandleOpenGLErrors(const char* funcName, bool throwOnFailure = false)
{
    for (GLenum err = glGetError(); err != GL_NO_ERROR; err = glGetError())
    {
        switch (err)
        {
        case GL_INVALID_ENUM:
            Console::WriteLine("gl::%: GL_INVALID_ENUM", funcName);
            assert(!throwOnFailure);
            break;

        case GL_INVALID_VALUE:
            Console::WriteLine("gl::%: GL_INVALID_VALUE", funcName);
            assert(!throwOnFailure);
            break;

        case GL_INVALID_OPERATION:
            Console::WriteLine("gl::%: GL_INVALID_OPERATION", funcName);
            assert(!throwOnFailure);
            break;

        case GL_OUT_OF_MEMORY:
            Console::WriteLine("gl::%: GL_OUT_OF_MEMORY", funcName);
            assert(!throwOnFailure);
            break;

        default:
            Console::WriteLine("gl::%: UNKNOWN ERROR", funcName);
            assert(!throwOnFailure);
            break;
        }
    }
}

#define __mwHandleOpenGLErrors(func) //HandleOpenGLErrors(#func, false)

#if PLATFORM_ANDROID
inline void HandleEGLErrors(const char* funcName, bool throwOnFailure = false)
{
    static const std::unordered_map<EGLint, std::string> errorTypes{
        { EGL_SUCCESS, "EGL_SUCCESS" },
        { EGL_NOT_INITIALIZED, "EGL_NOT_INITIALIZED" },
        { EGL_BAD_ACCESS, "EGL_BAD_ACCESS" },
        { EGL_BAD_ALLOC, "EGL_BAD_ALLOC" },
        { EGL_BAD_ATTRIBUTE, "EGL_BAD_ATTRIBUTE" },
        { EGL_BAD_CONTEXT, "EGL_BAD_CONTEXT" },
        { EGL_BAD_CONFIG, "EGL_BAD_CONFIG" },
        { EGL_BAD_CURRENT_SURFACE, "EGL_BAD_CURRENT_SURFACE" },
        { EGL_BAD_DISPLAY, "EGL_BAD_DISPLAY" },
        { EGL_BAD_SURFACE, "EGL_BAD_SURFACE" },
        { EGL_BAD_MATCH, "EGL_BAD_MATCH" },
        { EGL_BAD_PARAMETER, "EGL_BAD_PARAMETER" },
        { EGL_BAD_NATIVE_PIXMAP, "EGL_BAD_NATIVE_PIXMAP" },
        { EGL_BAD_NATIVE_WINDOW, "EGL_BAD_NATIVE_WINDOW" },
        { EGL_CONTEXT_LOST, "EGL_CONTEXT_LOST" }
    };

    for (GLenum err = glGetError(); err != GL_NO_ERROR; err = glGetError())
    {
        auto it = errorTypes.find(err);
        std::string type = (it != errorTypes.end()) ? it->second : "UNKNOWN ERROR";

        Console::WriteLine("gl::%: %", funcName, type);
        assert(!throwOnFailure);
    }
}

#define __mwHandleEGLErrors(func) HandleEGLErrors(#func, true)
#endif

typedef GLchar Char;
typedef GLenum Enum;
typedef GLbitfield Bitfield;
typedef GLuint Uint;
typedef GLint Int;
typedef GLsizei Sizei;
typedef GLboolean Boolean;
typedef GLbyte Byte;
typedef GLshort Short;
typedef GLubyte Ubyte;
typedef GLushort Ushort;
typedef GLulong Ulong;
typedef GLfloat Float;
typedef GLclampf Clampf;
typedef GLdouble Double;
typedef GLclampd Clampd;
typedef GLvoid Void;

constexpr Enum TEXTURE_2D = GL_TEXTURE_2D;
constexpr Enum UNPACK_SKIP_ROWS = GL_UNPACK_SKIP_ROWS;
constexpr Enum UNPACK_SKIP_PIXELS = GL_UNPACK_SKIP_PIXELS;
constexpr Enum UNPACK_ROW_LENGTH = GL_UNPACK_ROW_LENGTH;
constexpr Enum TEXTURE_WRAP_S = GL_TEXTURE_WRAP_S;
constexpr Enum TEXTURE_WRAP_T = GL_TEXTURE_WRAP_T;
constexpr Enum TEXTURE_MIN_FILTER = GL_TEXTURE_MIN_FILTER;
constexpr Enum TEXTURE_MAG_FILTER = GL_TEXTURE_MAG_FILTER;
constexpr Enum TEXTURE_MAX_ANISOTROPY = GL_TEXTURE_MAX_ANISOTROPY_EXT;
constexpr Enum MAX_TEXTURE_MAX_ANISOTROPY = GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT;
constexpr Enum CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE;
constexpr Enum REPEAT = GL_REPEAT;
constexpr Enum ALPHA = GL_ALPHA;
constexpr Enum RGB = GL_RGB;
constexpr Enum RGBA = GL_RGBA;
constexpr Enum NEAREST = GL_NEAREST;
constexpr Enum LINEAR = GL_LINEAR;
constexpr Enum NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST;
constexpr Enum LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST;
constexpr Enum LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR;

// frame buffer
constexpr Enum FRAMEBUFFER = GL_FRAMEBUFFER;
constexpr Enum RENDERBUFFER = GL_RENDERBUFFER;
constexpr Enum FRAMEBUFFER_BINDING = GL_FRAMEBUFFER_BINDING;
constexpr Enum COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0;
constexpr Enum DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT;
constexpr Enum DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24;
constexpr Enum FRAMEBUFFER_COMPLETE = GL_FRAMEBUFFER_COMPLETE;
constexpr Enum FRAMEBUFFER_INCOMPLETE_ATTACHMENT = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
constexpr Enum FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;
constexpr Enum FRAMEBUFFER_UNSUPPORTED = GL_FRAMEBUFFER_UNSUPPORTED;

// texture units
constexpr Enum TEXTURE0 = GL_TEXTURE0;
constexpr Enum TEXTURE1 = GL_TEXTURE1;
constexpr Enum TEXTURE2 = GL_TEXTURE2;
constexpr Enum TEXTURE3 = GL_TEXTURE3;
constexpr Enum TEXTURE4 = GL_TEXTURE4;
constexpr Enum TEXTURE5 = GL_TEXTURE5;
constexpr Enum TEXTURE6 = GL_TEXTURE6;
constexpr Enum TEXTURE7 = GL_TEXTURE7;
constexpr Enum TEXTURE8 = GL_TEXTURE8;
constexpr Enum TEXTURE9 = GL_TEXTURE9;
constexpr Enum TEXTURE10 = GL_TEXTURE10;
constexpr Enum TEXTURE11 = GL_TEXTURE11;
constexpr Enum TEXTURE12 = GL_TEXTURE12;
constexpr Enum TEXTURE13 = GL_TEXTURE13;
constexpr Enum TEXTURE14 = GL_TEXTURE14;
constexpr Enum TEXTURE15 = GL_TEXTURE15;
constexpr Enum TEXTURE16 = GL_TEXTURE16;
constexpr Enum TEXTURE17 = GL_TEXTURE17;
constexpr Enum TEXTURE18 = GL_TEXTURE18;
constexpr Enum TEXTURE19 = GL_TEXTURE19;
constexpr Enum TEXTURE20 = GL_TEXTURE20;
constexpr Enum TEXTURE21 = GL_TEXTURE21;
constexpr Enum TEXTURE22 = GL_TEXTURE22;
constexpr Enum TEXTURE23 = GL_TEXTURE23;
constexpr Enum TEXTURE24 = GL_TEXTURE24;
constexpr Enum TEXTURE25 = GL_TEXTURE25;
constexpr Enum TEXTURE26 = GL_TEXTURE26;
constexpr Enum TEXTURE27 = GL_TEXTURE27;
constexpr Enum TEXTURE28 = GL_TEXTURE28;
constexpr Enum TEXTURE29 = GL_TEXTURE29;
constexpr Enum TEXTURE30 = GL_TEXTURE30;
constexpr Enum TEXTURE31 = GL_TEXTURE31;

// sized internal formats
constexpr Enum R8 = GL_R8; // GL_RED
constexpr Enum R8_SNORM = GL_R8_SNORM; // GL_RED
constexpr Enum R16 = GL_R16; // GL_RED
constexpr Enum R16_SNORM = GL_R16_SNORM; // GL_RED
constexpr Enum RG8 = GL_RG8; // GL_RG
constexpr Enum RG8_SNORM = GL_RG8_SNORM; // GL_RG
constexpr Enum RG16 = GL_RG16; // GL_RG
constexpr Enum RG16_SNORM = GL_RG16_SNORM; // GL_RG
constexpr Enum R3_G3_B2 = GL_R3_G3_B2; // GL_RGB
constexpr Enum RGB4 = GL_RGB4; // GL_RGB
constexpr Enum RGB5 = GL_RGB5; // GL_RGB
constexpr Enum RGB8 = GL_RGB8; // GL_RGB
constexpr Enum RGB8_SNORM = GL_RGB8_SNORM; // GL_RGB
constexpr Enum RGB10 = GL_RGB10; // GL_RGB
constexpr Enum RGB12 = GL_RGB12; // GL_RGB
constexpr Enum RGB16_SNORM = GL_RGB16_SNORM; // GL_RGB
constexpr Enum RGBA2 = GL_RGBA2; // GL_RGB
constexpr Enum RGBA4 = GL_RGBA4; // GL_RGB
constexpr Enum RGB5_A1 = GL_RGB5_A1; // GL_RGBA
constexpr Enum RGBA8 = GL_RGBA8; // GL_RGBA
constexpr Enum RGBA8_SNORM = GL_RGBA8_SNORM; // GL_RGBA
constexpr Enum RGB10_A2 = GL_RGB10_A2; // GL_RGBA
constexpr Enum RGB10_A2UI = GL_RGB10_A2UI; // GL_RGBA
constexpr Enum RGBA12 = GL_RGBA12; // GL_RGBA
constexpr Enum RGBA16 = GL_RGBA16; // GL_RGBA
constexpr Enum SRGB8 = GL_SRGB8; // GL_RGB
constexpr Enum SRGB8_ALPHA8 = GL_SRGB8_ALPHA8; // GL_RGBA
constexpr Enum R16F = GL_R16F; // GL_RED
constexpr Enum RG16F = GL_RG16F; // GL_RG
constexpr Enum RGB16F = GL_RGB16F; // GL_RGB
constexpr Enum RGBA16F = GL_RGBA16F; // GL_RGBA
constexpr Enum R32F = GL_R32F; // GL_RED
constexpr Enum RG32F = GL_RG32F; // GL_RG
constexpr Enum RGB32F = GL_RGB32F; // GL_RGB
constexpr Enum RGBA32F = GL_RGBA32F; // GL_RGBA
constexpr Enum R11F_G11F_B10F = GL_R11F_G11F_B10F; // GL_RGB
constexpr Enum RGB9_E5 = GL_RGB9_E5; // GL_RGB
constexpr Enum R8I = GL_R8I; // GL_RED
constexpr Enum R8UI = GL_R8UI; // GL_RED
constexpr Enum R16I = GL_R16I; // GL_RED
constexpr Enum R16UI = GL_R16UI; // GL_RED
constexpr Enum R32I = GL_R32I; // GL_RED
constexpr Enum R32UI = GL_R32UI; // GL_RED
constexpr Enum RG8I = GL_RG8I; // GL_RG
constexpr Enum RG8UI = GL_RG8UI; // GL_RG
constexpr Enum RG16I = GL_RG16I; // GL_RG
constexpr Enum RG16UI = GL_RG16UI; // GL_RG
constexpr Enum RG32I = GL_RG32I; // GL_RG
constexpr Enum RG32UI = GL_RG32UI; // GL_RG
constexpr Enum RGB8I = GL_RGB8I; // GL_RGB
constexpr Enum RGB8UI = GL_RGB8UI; // GL_RGB
constexpr Enum RGB16I = GL_RGB16I; // GL_RGB
constexpr Enum RGB16UI = GL_RGB16UI; // GL_RGB
constexpr Enum RGB32I = GL_RGB32I; // GL_RGB
constexpr Enum RGB32UI = GL_RGB32UI; // GL_RGB
constexpr Enum RGBA8I = GL_RGBA8I; // GL_RGBA
constexpr Enum RGBA8UI = GL_RGBA8UI; // GL_RGBA
constexpr Enum RGBA16I = GL_RGBA16I; // GL_RGBA
constexpr Enum RGBA16UI = GL_RGBA16UI; // GL_RGBA
constexpr Enum RGBA32I = GL_RGBA32I; // GL_RGBA
constexpr Enum RGBA32UI = GL_RGBA32UI; // GL_RGBA

// blend factors
constexpr Enum ZERO = GL_ZERO;
constexpr Enum ONE = GL_ONE;
constexpr Enum SRC_COLOR = GL_SRC_COLOR;
constexpr Enum ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR;
constexpr Enum DST_COLOR = GL_DST_COLOR;
constexpr Enum ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR;
constexpr Enum SRC_ALPHA = GL_SRC_ALPHA;
constexpr Enum ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA;
constexpr Enum DST_ALPHA = GL_DST_ALPHA;
constexpr Enum ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA;
constexpr Enum SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE;
constexpr Enum CONSTANT_COLOR = GL_CONSTANT_COLOR;
constexpr Enum ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR;
constexpr Enum SRC1_COLOR = GL_SRC1_COLOR;
constexpr Enum ONE_MINUS_SRC1_COLOR = GL_ONE_MINUS_SRC1_COLOR;
constexpr Enum SRC1_ALPHA = GL_SRC1_ALPHA;
constexpr Enum ONE_MINUS_SRC1_ALPHA = GL_ONE_MINUS_SRC1_ALPHA;

// blend operations
constexpr Enum FUNC_ADD = GL_FUNC_ADD;
constexpr Enum FUNC_SUBTRACT = GL_FUNC_SUBTRACT;
constexpr Enum FUNC_REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT;
constexpr Enum MIN = GL_MIN;
constexpr Enum MAX = GL_MAX;

// draw modes
constexpr Enum POINTS = GL_POINTS;
constexpr Enum LINES = GL_LINES;
constexpr Enum LINE_STRIP = GL_LINE_STRIP;
constexpr Enum TRIANGLES = GL_TRIANGLES;
constexpr Enum TRIANGLE_STRIP = GL_TRIANGLE_STRIP;

// component types
constexpr Enum UNSIGNED_BYTE = GL_UNSIGNED_BYTE;
constexpr Enum UNSIGNED_INT = GL_UNSIGNED_INT;
constexpr Enum FLOAT = GL_FLOAT;

// bool
constexpr Boolean FALSE = GL_FALSE;
constexpr Boolean TRUE = GL_TRUE;

// misc
constexpr Enum CW = GL_CW;
constexpr Enum DITHER = GL_DITHER;
constexpr Enum CULL_FACE = GL_CULL_FACE;
constexpr Enum FRONT = GL_FRONT;
constexpr Enum BACK = GL_BACK;
constexpr Enum SCISSOR_TEST = GL_SCISSOR_TEST;
constexpr Enum BLEND = GL_BLEND;
constexpr Enum DEPTH_BUFFER_BIT = GL_DEPTH_BUFFER_BIT;
constexpr Enum COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT;

// shaders
constexpr Enum VERTEX_SHADER = GL_VERTEX_SHADER;
constexpr Enum FRAGMENT_SHADER = GL_FRAGMENT_SHADER;
constexpr Enum COMPILE_STATUS = GL_COMPILE_STATUS;
constexpr Enum LINK_STATUS = GL_LINK_STATUS;
constexpr Enum INFO_LOG_LENGTH = GL_INFO_LOG_LENGTH;

// depth testing
constexpr Enum DEPTH_TEST = GL_DEPTH_TEST;
constexpr Enum ALWAYS = GL_ALWAYS;
constexpr Enum NEVER = GL_NEVER;
constexpr Enum LESS = GL_LESS;
constexpr Enum EQUAL = GL_EQUAL;
constexpr Enum LEQUAL = GL_LEQUAL;
constexpr Enum GREATER = GL_GREATER;
constexpr Enum NOTEQUAL = GL_NOTEQUAL;
constexpr Enum GEQUAL = GL_GEQUAL;

// debug callback
constexpr Enum DEBUG_SEVERITY_HIGH = GL_DEBUG_SEVERITY_HIGH;
constexpr Enum DEBUG_SEVERITY_MEDIUM = GL_DEBUG_SEVERITY_MEDIUM;
constexpr Enum DEBUG_SEVERITY_LOW = GL_DEBUG_SEVERITY_LOW;
constexpr Enum DEBUG_SEVERITY_NOTIFICATION = GL_DEBUG_SEVERITY_NOTIFICATION;
constexpr Enum DEBUG_OUTPUT = GL_DEBUG_OUTPUT;


constexpr Enum ARRAY_BUFFER = GL_ARRAY_BUFFER;
constexpr Enum ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER;
constexpr Enum COPY_READ_BUFFER = GL_COPY_READ_BUFFER;
constexpr Enum COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER;
constexpr Enum DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER;
constexpr Enum DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER;
constexpr Enum ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER;
constexpr Enum PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER;
constexpr Enum PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER;
constexpr Enum QUERY_BUFFER = GL_QUERY_BUFFER;
constexpr Enum SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER;
constexpr Enum TEXTURE_BUFFER = GL_TEXTURE_BUFFER;
constexpr Enum TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER;
constexpr Enum UNIFORM_BUFFER = GL_UNIFORM_BUFFER;

constexpr Enum READ_ONLY = GL_READ_ONLY;
constexpr Enum WRITE_ONLY = GL_WRITE_ONLY;
constexpr Enum READ_WRITE = GL_READ_WRITE;
constexpr Enum BUFFER_ACCESS = GL_BUFFER_ACCESS;
constexpr Enum BUFFER_MAPPED = GL_BUFFER_MAPPED;
constexpr Enum BUFFER_MAP_POINTER = GL_BUFFER_MAP_POINTER;
constexpr Enum STREAM_DRAW = GL_STREAM_DRAW;
constexpr Enum STREAM_READ = GL_STREAM_READ;
constexpr Enum STREAM_COPY = GL_STREAM_COPY;
constexpr Enum STATIC_DRAW = GL_STATIC_DRAW;
constexpr Enum STATIC_READ = GL_STATIC_READ;
constexpr Enum STATIC_COPY = GL_STATIC_COPY;
constexpr Enum DYNAMIC_DRAW = GL_DYNAMIC_DRAW;
constexpr Enum DYNAMIC_READ = GL_DYNAMIC_READ;
constexpr Enum DYNAMIC_COPY = GL_DYNAMIC_COPY;

constexpr Bitfield MAP_READ_BIT = GL_MAP_READ_BIT;
constexpr Bitfield MAP_WRITE_BIT = GL_MAP_WRITE_BIT;
constexpr Bitfield MAP_PERSISTENT_BIT = GL_MAP_PERSISTENT_BIT;
constexpr Bitfield MAP_COHERENT_BIT = GL_MAP_COHERENT_BIT;
constexpr Bitfield DYNAMIC_STORAGE_BIT = GL_DYNAMIC_STORAGE_BIT;
constexpr Bitfield CLIENT_STORAGE_BIT = GL_CLIENT_STORAGE_BIT;
constexpr Bitfield MAP_INVALIDATE_RANGE_BIT = GL_MAP_INVALIDATE_RANGE_BIT;
constexpr Bitfield MAP_INVALIDATE_BUFFER_BIT = GL_MAP_INVALIDATE_BUFFER_BIT;
constexpr Bitfield MAP_FLUSH_EXPLICIT_BIT = GL_MAP_FLUSH_EXPLICIT_BIT;
constexpr Bitfield MAP_UNSYNCHRONIZED_BIT = GL_MAP_UNSYNCHRONIZED_BIT;

inline void ActiveTexture(GLenum texture) {
    glActiveTexture(texture);
    __mwHandleOpenGLErrors(ActiveTexture);
}

inline void AttachShader(GLuint program, GLuint shader) {
    glAttachShader(program, shader);
    __mwHandleOpenGLErrors(AttachShader);
}

inline void BindAttribLocation(GLuint program, GLuint index, const GLchar* name) {
    glBindAttribLocation(program, index, name);
    __mwHandleOpenGLErrors(BindAttribLocation);
}

inline void BindBuffer(GLenum target, GLuint buffer) {
    glBindBuffer(target, buffer);
    __mwHandleOpenGLErrors(BindBuffer);
}

inline void BindFramebuffer(GLenum target, GLuint framebuffer) {
    glBindFramebuffer(target, framebuffer);
    __mwHandleOpenGLErrors(BindFramebuffer);
}

inline void BindRenderbuffer(GLenum target, GLuint renderbuffer) {
    glBindRenderbuffer(target, renderbuffer);
    __mwHandleOpenGLErrors(BindRenderbuffer);
}

inline void BindTexture(GLenum target, GLuint texture) {
    glBindTexture(target, texture);
    __mwHandleOpenGLErrors(BindTexture);
}

inline void BlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    glBlendColor(red, green, blue, alpha);
    __mwHandleOpenGLErrors(BlendColor);
}

inline void BlendEquation(GLenum mode) {
    glBlendEquation(mode);
    __mwHandleOpenGLErrors(BlendEquation);
}

inline void BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) {
    glBlendEquationSeparate(modeRGB, modeAlpha);
    __mwHandleOpenGLErrors(BlendEquationSeparate);
}

inline void BlendFunc(GLenum sfactor, GLenum dfactor) {
    glBlendFunc(sfactor, dfactor);
    __mwHandleOpenGLErrors(BlendFunc);
}

inline void BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) {
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    __mwHandleOpenGLErrors(BlendFuncSeparate);
}

inline void BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage) {
    glBufferData(target, size, data, usage);
    __mwHandleOpenGLErrors(BufferData);
}

inline void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data) {
    glBufferSubData(target, offset, size, data);
    __mwHandleOpenGLErrors(BufferSubData);
}

inline void* MapBuffer(GLenum target, GLenum access) {
    void* pData = glMapBuffer(target, access);
    __mwHandleOpenGLErrors(MapBuffer);
    return pData;
}

inline void* MapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
    void* pData = glMapBufferRange(target, offset, length, access);
    __mwHandleOpenGLErrors(MapBufferRange);
    return pData;
}

inline GLboolean UnmapBuffer(GLenum target) {
    GLboolean unmapped = glUnmapBuffer(target);
    __mwHandleOpenGLErrors(UnmapBuffer);
    return unmapped;
}

inline GLenum CheckFramebufferStatus(GLenum target) {
    auto ret = glCheckFramebufferStatus(target);
    __mwHandleOpenGLErrors(CheckFramebufferStatus);
    return ret;
}

inline void Clear(GLbitfield mask) {
    glClear(mask);
    __mwHandleOpenGLErrors(Clear);
}

inline void ClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    glClearColor(red, green, blue, alpha);
    __mwHandleOpenGLErrors(ClearColor);
}

inline void ClearDepthf(GLclampf depth) {
    glClearDepthf(depth);
    __mwHandleOpenGLErrors(ClearDepthf);
}

inline void ClearStencil(GLint s) {
    glClearStencil(s);
    __mwHandleOpenGLErrors(ClearStencil);
}

inline void ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) {
    glColorMask(red, green, blue, alpha);
    __mwHandleOpenGLErrors(ColorMask);
}

inline void CompileShader(GLuint shader) {
    glCompileShader(shader);
    __mwHandleOpenGLErrors(CompileShader);
}

inline void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data) {
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
    __mwHandleOpenGLErrors(CompressedTexImage2D);
}

inline void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data) {
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
    __mwHandleOpenGLErrors(CompressedTexSubImage2D);
}

inline void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) {
    glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
    __mwHandleOpenGLErrors(CopyTexImage2D);
}

inline void CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) {
    glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
    __mwHandleOpenGLErrors(CopyTexSubImage2D);
}

inline GLuint CreateProgram() {
    auto ret = glCreateProgram();
    __mwHandleOpenGLErrors(CreateProgram);
    return ret;
}

inline GLuint CreateShader(GLenum type) {
    auto ret = glCreateShader(type);
    __mwHandleOpenGLErrors(CreateShader);
    return ret;
}

inline void CullFace(GLenum mode) {
    glCullFace(mode);
    __mwHandleOpenGLErrors(CullFace);
}

inline void DeleteBuffers(GLsizei n, const GLuint* buffers) {
    glDeleteBuffers(n, buffers);
    __mwHandleOpenGLErrors(DeleteBuffers);
}

inline void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers) {
    glDeleteFramebuffers(n, framebuffers);
    __mwHandleOpenGLErrors(DeleteFramebuffers);
}

inline void DeleteProgram(GLuint program) {
    glDeleteProgram(program);
    __mwHandleOpenGLErrors(DeleteProgram);
}

inline void DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers) {
    glDeleteRenderbuffers(n, renderbuffers);
    __mwHandleOpenGLErrors(DeleteRenderbuffers);
}

inline void DeleteShader(GLuint shader) {
    glDeleteShader(shader);
    __mwHandleOpenGLErrors(DeleteShader);
}

inline void DeleteTextures(GLsizei n, const GLuint* textures) {
    glDeleteTextures(n, textures);
    __mwHandleOpenGLErrors(DeleteTextures);
}

inline void DepthFunc(GLenum func) {
    glDepthFunc(func);
    __mwHandleOpenGLErrors(DepthFunc);
}

inline void DepthMask(GLboolean flag) {
    glDepthMask(flag);
    __mwHandleOpenGLErrors(DepthMask);
}

inline void DepthRangef(GLclampf zNear, GLclampf zFar) {
    glDepthRangef(zNear, zFar);
    __mwHandleOpenGLErrors(DepthRangef);
}

inline void DetachShader(GLuint program, GLuint shader) {
    glDetachShader(program, shader);
    __mwHandleOpenGLErrors(DetachShader);
}

inline void DisableVertexAttribArray(GLuint index) {
    glDisableVertexAttribArray(index);
    __mwHandleOpenGLErrors(DisableVertexAttribArray);
}

inline void DrawArrays(GLenum mode, GLint first, GLsizei count) {
    glDrawArrays(mode, first, count);
    __mwHandleOpenGLErrors(DrawArrays);
}

inline void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices) {
    glDrawElements(mode, count, type, indices);
    __mwHandleOpenGLErrors(DrawElements);
}

inline void SetEnabled(GLenum cap, bool enable) {
    if(enable) glEnable(cap);
    else glDisable(cap);
    __mwHandleOpenGLErrors(SetEnabled);
}

inline void Enable(GLenum cap) {
    glEnable(cap);
    __mwHandleOpenGLErrors(Enable);
}

inline void Disable(GLenum cap) {
    glDisable(cap);
    __mwHandleOpenGLErrors(Disable);
}

inline void DebugMessageCallback(GLDEBUGPROC callback, void* userParam) {
    glDebugMessageCallback(callback, userParam);
    __mwHandleOpenGLErrors(DebugMessageCallback);
}

inline void EnableVertexAttribArray(GLuint index) {
    glEnableVertexAttribArray(index);
    __mwHandleOpenGLErrors(EnableVertexAttribArray);
}

inline void Finish() {
    glFinish();
    __mwHandleOpenGLErrors(Finish);
}

inline void Flush() {
    glFlush();
    __mwHandleOpenGLErrors(Flush);
}

inline void FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
    glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    __mwHandleOpenGLErrors(FramebufferRenderbuffer);
}

inline void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
    __mwHandleOpenGLErrors(FramebufferTexture2D);
}

inline void FrontFace(GLenum mode) {
    glFrontFace(mode);
    __mwHandleOpenGLErrors(FrontFace);
}

inline void GenBuffers(GLsizei n, GLuint* buffers) {
    glGenBuffers(n, buffers);
    __mwHandleOpenGLErrors(GenBuffers);
}

inline void GenerateMipmap(GLenum target) {
    glGenerateMipmap(target);
    __mwHandleOpenGLErrors(GenerateMipmap);
}

inline void GenFramebuffers(GLsizei n, GLuint* framebuffers) {
    glGenFramebuffers(n, framebuffers);
    __mwHandleOpenGLErrors(GenFramebuffers);
}

inline void GenRenderbuffers(GLsizei n, GLuint* renderbuffers) {
    glGenRenderbuffers(n, renderbuffers);
    __mwHandleOpenGLErrors(GenRenderbuffers);
}

inline void GenTextures(GLsizei n, GLuint* textures) {
    glGenTextures(n, textures);
    __mwHandleOpenGLErrors(GenTextures);
}

inline void GetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) {
    glGetActiveAttrib(program, index, bufsize, length, size, type, name);
    __mwHandleOpenGLErrors(GetActiveAttrib);
}

inline void GetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) {
    glGetActiveUniform(program, index, bufsize, length, size, type, name);
    __mwHandleOpenGLErrors(GetActiveUniform);
}

inline void GetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders) {
    glGetAttachedShaders(program, maxcount, count, shaders);
    __mwHandleOpenGLErrors(GetAttachedShaders);
}

inline int GetAttribLocation(GLuint program, const GLchar* name) {
    return glGetAttribLocation(program, name);
    __mwHandleOpenGLErrors(GetAttribLocation);
}

inline void GetBooleanv(GLenum pname, GLboolean* params) {
    glGetBooleanv(pname, params);
    __mwHandleOpenGLErrors(GetBooleanv);
}

inline void GetBufferParameteriv(GLenum target, GLenum pname, GLint* params) {
    glGetBufferParameteriv(target, pname, params);
    __mwHandleOpenGLErrors(GetBufferParameteriv);
}

inline GLenum GetError() {
    auto ret = glGetError();
    __mwHandleOpenGLErrors(GetError);
    return ret;
}

inline void GetFloatv(GLenum pname, GLfloat* params) {
    glGetFloatv(pname, params);
    __mwHandleOpenGLErrors(GetFloatv);
}

inline void GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params) {
    glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
    __mwHandleOpenGLErrors(GetFramebufferAttachmentParameteriv);
}

inline void GetIntegerv(GLenum pname, GLint* params) {
    glGetIntegerv(pname, params);
    __mwHandleOpenGLErrors(GetIntegerv);
}

inline void GetProgramiv(GLuint program, GLenum pname, GLint* params) {
    glGetProgramiv(program, pname, params);
    __mwHandleOpenGLErrors(GetProgramiv);
}

inline void GetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog) {
    glGetProgramInfoLog(program, bufsize, length, infolog);
    __mwHandleOpenGLErrors(GetProgramInfoLog);
}

inline void GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params) {
    glGetRenderbufferParameteriv(target, pname, params);
    __mwHandleOpenGLErrors(GetRenderbufferParameteriv);
}

inline void GetShaderiv(GLuint shader, GLenum pname, GLint* params) {
    glGetShaderiv(shader, pname, params);
    __mwHandleOpenGLErrors(GetShaderiv);
}

inline void GetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog) {
    glGetShaderInfoLog(shader, bufsize, length, infolog);
    __mwHandleOpenGLErrors(GetShaderInfoLog);
}

inline void GetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) {
    glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
    __mwHandleOpenGLErrors(GetShaderPrecisionFormat);
}

inline void GetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source) {
    glGetShaderSource(shader, bufsize, length, source);
    __mwHandleOpenGLErrors(GetShaderSource);
}

inline const GLubyte* GetString(GLenum name) {
    auto ret = glGetString(name);
    __mwHandleOpenGLErrors(GetString);
    return ret;
}

inline void GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params) {
    glGetTexParameterfv(target, pname, params);
    __mwHandleOpenGLErrors(GetTexParameterfv);
}

inline void GetTexParameteriv(GLenum target, GLenum pname, GLint* params) {
    glGetTexParameteriv(target, pname, params);
    __mwHandleOpenGLErrors(GetTexParameteriv);
}

inline void GetUniformfv(GLuint program, GLint location, GLfloat* params) {
    glGetUniformfv(program, location, params);
    __mwHandleOpenGLErrors(GetUniformfv);
}

inline void GetUniformiv(GLuint program, GLint location, GLint* params) {
    glGetUniformiv(program, location, params);
    __mwHandleOpenGLErrors(GetUniformiv);
}

inline int GetUniformLocation(GLuint program, const GLchar* name) {
    auto ret = glGetUniformLocation(program, name);
    __mwHandleOpenGLErrors(GetUniformLocation);
    return ret;
}

inline void GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params) {
    glGetVertexAttribfv(index, pname, params);
    __mwHandleOpenGLErrors(GetVertexAttribfv);
}

inline void GetVertexAttribiv(GLuint index, GLenum pname, GLint* params) {
    glGetVertexAttribiv(index, pname, params);
    __mwHandleOpenGLErrors(GetVertexAttribiv);
}

inline void GetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid** pointer) {
    glGetVertexAttribPointerv(index, pname, pointer);
    __mwHandleOpenGLErrors(GetVertexAttribPointerv);
}

inline void Hint(GLenum target, GLenum mode) {
    glHint(target, mode);
    __mwHandleOpenGLErrors(Hint);
}

inline GLboolean IsBuffer(GLuint buffer) {
    auto ret = glIsBuffer(buffer);
    __mwHandleOpenGLErrors(IsBuffer);
    return ret;
}

inline GLboolean IsEnabled(GLenum cap) {
    auto ret = glIsEnabled(cap);
    __mwHandleOpenGLErrors(IsEnabled);
    return ret;
}

inline GLboolean IsFramebuffer(GLuint framebuffer) {
    auto ret = glIsFramebuffer(framebuffer);
    __mwHandleOpenGLErrors(IsFramebuffer);
    return ret;
}

inline GLboolean IsProgram(GLuint program) {
    auto ret = glIsProgram(program);
    __mwHandleOpenGLErrors(IsProgram);
    return ret;
}

inline GLboolean IsRenderbuffer(GLuint renderbuffer) {
    auto ret = glIsRenderbuffer(renderbuffer);
    __mwHandleOpenGLErrors(IsRenderbuffer);
    return ret;
}

inline GLboolean IsShader(GLuint shader) {
    auto ret = glIsShader(shader);
    __mwHandleOpenGLErrors(IsShader);
    return ret;
}

inline GLboolean IsTexture(GLuint texture) {
    auto ret = glIsTexture(texture);
    __mwHandleOpenGLErrors(IsTexture);
    return ret;
}

inline void LineWidth(GLfloat width) {
    glLineWidth(width);
    __mwHandleOpenGLErrors(LineWidth);
}

inline void LinkProgram(GLuint program) {
    glLinkProgram(program);
    __mwHandleOpenGLErrors(LinkProgram);
}

inline void PixelStorei(GLenum pname, GLint param) {
    glPixelStorei(pname, param);
    __mwHandleOpenGLErrors(PixelStorei);
}

inline void PolygonOffset(GLfloat factor, GLfloat units) {
    glPolygonOffset(factor, units);
    __mwHandleOpenGLErrors(PolygonOffset);
}

inline void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels) {
    glReadPixels(x, y, width, height, format, type, pixels);
    __mwHandleOpenGLErrors(ReadPixels);
}

inline void ReleaseShaderCompiler() {
    glReleaseShaderCompiler();
    __mwHandleOpenGLErrors(ReleaseShaderCompiler);
}

inline void RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
    glRenderbufferStorage(target, internalformat, width, height);
    __mwHandleOpenGLErrors(RenderbufferStorage);
}

inline void SampleCoverage(GLclampf value, GLboolean invert) {
    glSampleCoverage(value, invert);
    __mwHandleOpenGLErrors(SampleCoverage);
}

inline void Scissor(GLint x, GLint y, GLsizei width, GLsizei height) {
    glScissor(x, y, width, height);
    __mwHandleOpenGLErrors(Scissor);
}

inline void ShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length) {
    glShaderBinary(n, shaders, binaryformat, binary, length);
    __mwHandleOpenGLErrors(ShaderBinary);
}

inline void ShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length) {
    glShaderSource(shader, count, string, length);
    __mwHandleOpenGLErrors(ShaderSource);
}

inline void StencilFunc(GLenum func, GLint ref, GLuint mask) {
    glStencilFunc(func, ref, mask);
    __mwHandleOpenGLErrors(StencilFunc);
}

inline void StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask) {
    glStencilFuncSeparate(face, func, ref, mask);
    __mwHandleOpenGLErrors(StencilFuncSeparate);
}

inline void StencilMask(GLuint mask) {
    glStencilMask(mask);
    __mwHandleOpenGLErrors(StencilMask);
}

inline void StencilMaskSeparate(GLenum face, GLuint mask) {
    glStencilMaskSeparate(face, mask);
    __mwHandleOpenGLErrors(StencilMaskSeparate);
}

inline void StencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
    glStencilOp(fail, zfail, zpass);
    __mwHandleOpenGLErrors(StencilOp);
}

inline void StencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass) {
    glStencilOpSeparate(face, fail, zfail, zpass);
    __mwHandleOpenGLErrors(StencilOpSeparate);
}

inline void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels) {
    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    __mwHandleOpenGLErrors(TexImage2D);
}

inline void TexParameterf(GLenum target, GLenum pname, GLfloat param) {
    glTexParameterf(target, pname, param);
    __mwHandleOpenGLErrors(TexParameterf);
}

inline void TexParameterfv(GLenum target, GLenum pname, const GLfloat* params) {
    glTexParameterfv(target, pname, params);
    __mwHandleOpenGLErrors(TexParameterfv);
}

inline void TexParameteri(GLenum target, GLenum pname, GLint param) {
    glTexParameteri(target, pname, param);
    __mwHandleOpenGLErrors(TexParameteri);
}

inline void TexParameteriv(GLenum target, GLenum pname, const GLint* params) {
    glTexParameteriv(target, pname, params);
    __mwHandleOpenGLErrors(TexParameteriv);
}

inline void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels) {
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
    __mwHandleOpenGLErrors(TexSubImage2D);
}

inline void Uniform1f(GLint location, GLfloat x) {
    glUniform1f(location, x);
    __mwHandleOpenGLErrors(Uniform1f);
}

inline void Uniform1fv(GLint location, GLsizei count, const GLfloat* v) {
    glUniform1fv(location, count, v);
    __mwHandleOpenGLErrors(Uniform1fv);
}

inline void Uniform1i(GLint location, GLint x) {
    glUniform1i(location, x);
    __mwHandleOpenGLErrors(Uniform1i);
}

inline void Uniform1iv(GLint location, GLsizei count, const GLint* v) {
    glUniform1iv(location, count, v);
    __mwHandleOpenGLErrors(Uniform1iv);
}

inline void Uniform2f(GLint location, GLfloat x, GLfloat y) {
    glUniform2f(location, x, y);
    __mwHandleOpenGLErrors(Uniform2f);
}

inline void Uniform2fv(GLint location, GLsizei count, const GLfloat* v) {
    glUniform2fv(location, count, v);
    __mwHandleOpenGLErrors(Uniform2fv);
}

inline void Uniform2i(GLint location, GLint x, GLint y) {
    glUniform2i(location, x, y);
    __mwHandleOpenGLErrors(Uniform2i);
}

inline void Uniform2iv(GLint location, GLsizei count, const GLint* v) {
    glUniform2iv(location, count, v);
    __mwHandleOpenGLErrors(Uniform2iv);
}

inline void Uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z) {
    glUniform3f(location, x, y, z);
    __mwHandleOpenGLErrors(Uniform3f);
}

inline void Uniform3fv(GLint location, GLsizei count, const GLfloat* v) {
    glUniform3fv(location, count, v);
    __mwHandleOpenGLErrors(Uniform3fv);
}

inline void Uniform3i(GLint location, GLint x, GLint y, GLint z) {
    glUniform3i(location, x, y, z);
    __mwHandleOpenGLErrors(Uniform3i);
}

inline void Uniform3iv(GLint location, GLsizei count, const GLint* v) {
    glUniform3iv(location, count, v);
    __mwHandleOpenGLErrors(Uniform3iv);
}

inline void Uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    glUniform4f(location, x, y, z, w);
    __mwHandleOpenGLErrors(Uniform4f);
}

inline void Uniform4fv(GLint location, GLsizei count, const GLfloat* v) {
    glUniform4fv(location, count, v);
    __mwHandleOpenGLErrors(Uniform4fv);
}

inline void Uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w) {
    glUniform4i(location, x, y, z, w);
    __mwHandleOpenGLErrors(Uniform4i);
}

inline void Uniform4iv(GLint location, GLsizei count, const GLint* v) {
    glUniform4iv(location, count, v);
    __mwHandleOpenGLErrors(Uniform4iv);
}

inline void UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
    glUniformMatrix2fv(location, count, transpose, value);
    __mwHandleOpenGLErrors(UniformMatrix2fv);
}

inline void UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
    glUniformMatrix3fv(location, count, transpose, value);
    __mwHandleOpenGLErrors(UniformMatrix3fv);
}

inline void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
    glUniformMatrix4fv(location, count, transpose, value);
    __mwHandleOpenGLErrors(UniformMatrix4fv);
}

inline void UseProgram(GLuint program) {
    glUseProgram(program);
    __mwHandleOpenGLErrors(UseProgram);
}

inline void ValidateProgram(GLuint program) {
    glValidateProgram(program);
    __mwHandleOpenGLErrors(ValidateProgram);
}

inline void VertexAttrib1f(GLuint indx, GLfloat x) {
    glVertexAttrib1f(indx, x);
    __mwHandleOpenGLErrors(VertexAttrib1f);
}

inline void VertexAttrib1fv(GLuint indx, const GLfloat* values) {
    glVertexAttrib1fv(indx, values);
    __mwHandleOpenGLErrors(VertexAttrib1fv);
}

inline void VertexAttrib2f(GLuint indx, GLfloat x, GLfloat y) {
    glVertexAttrib2f(indx, x, y);
    __mwHandleOpenGLErrors(VertexAttrib2f);
}

inline void VertexAttrib2fv(GLuint indx, const GLfloat* values) {
    glVertexAttrib2fv(indx, values);
    __mwHandleOpenGLErrors(VertexAttrib2fv);
}

inline void VertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z) {
    glVertexAttrib3f(indx, x, y, z);
    __mwHandleOpenGLErrors(VertexAttrib3f);
}

inline void VertexAttrib3fv(GLuint indx, const GLfloat* values) {
    glVertexAttrib3fv(indx, values);
    __mwHandleOpenGLErrors(VertexAttrib3fv);
}

inline void VertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    glVertexAttrib4f(indx, x, y, z, w);
    __mwHandleOpenGLErrors(VertexAttrib4f);
}

inline void VertexAttrib4fv(GLuint indx, const GLfloat* values) {
    glVertexAttrib4fv(indx, values);
    __mwHandleOpenGLErrors(VertexAttrib4fv);
}

inline void VertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr) {
    glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
    __mwHandleOpenGLErrors(VertexAttribPointer);
}

inline void Viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    glViewport(x, y, width, height);
    __mwHandleOpenGLErrors(Viewport);
}

// *** OpenGL 4.5 ***
inline void CreateTextures(GLenum target, GLsizei n, GLuint* textures) {
    glCreateTextures(target, n, textures);
    __mwHandleOpenGLErrors(CreateTextures);
}

inline void TextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) {
    glTextureStorage2D(texture, levels, internalformat, width, height);
    __mwHandleOpenGLErrors(TextureStorage2D);
}

inline void TextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) {
    glTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, type, pixels);
    __mwHandleOpenGLErrors(TextureSubImage2D);
}

inline void GenerateTextureMipmap(GLuint texture) {
    glGenerateTextureMipmap(texture);
    __mwHandleOpenGLErrors(GenerateTextureMipmap);
}

inline void TextureParameteri(GLuint texture, GLenum pname, GLint param) {
    glTextureParameteri(texture, pname, param);
    __mwHandleOpenGLErrors(TextureParameteri);
}

inline void TextureParameterf(GLuint texture, GLenum pname, GLfloat param) {
    glTextureParameterf(texture, pname, param);
    __mwHandleOpenGLErrors(TextureParameterf);
}

} // gl
} // gfx
} // mw
