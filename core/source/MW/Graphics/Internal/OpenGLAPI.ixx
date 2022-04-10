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
constexpr GLenum TEXTURE_2D = GL_TEXTURE_2D;
constexpr GLenum UNPACK_SKIP_ROWS = GL_UNPACK_SKIP_ROWS;
constexpr GLenum UNPACK_SKIP_PIXELS = GL_UNPACK_SKIP_PIXELS;
constexpr GLenum UNPACK_ROW_LENGTH = GL_UNPACK_ROW_LENGTH;
constexpr GLenum TEXTURE_WRAP_S = GL_TEXTURE_WRAP_S;
constexpr GLenum TEXTURE_WRAP_T = GL_TEXTURE_WRAP_T;
constexpr GLenum TEXTURE_MIN_FILTER = GL_TEXTURE_MIN_FILTER;
constexpr GLenum TEXTURE_MAG_FILTER = GL_TEXTURE_MAG_FILTER;
constexpr GLenum TEXTURE_MAX_ANISOTROPY = GL_TEXTURE_MAX_ANISOTROPY_EXT;
constexpr GLenum MAX_TEXTURE_MAX_ANISOTROPY = GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT;
constexpr GLenum CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE;
constexpr GLenum REPEAT = GL_REPEAT;
constexpr GLenum ALPHA = GL_ALPHA;
constexpr GLenum RGB = GL_RGB;
constexpr GLenum RGBA = GL_RGBA;
constexpr GLenum UNSIGNED_BYTE = GL_UNSIGNED_BYTE;
constexpr GLenum FLOAT = GL_FLOAT;
constexpr GLenum NEAREST = GL_NEAREST;
constexpr GLenum LINEAR = GL_LINEAR;
constexpr GLenum NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST;
constexpr GLenum LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST;
constexpr GLenum LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR;

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

inline void Disable(GLenum cap) {
    glDisable(cap);
    __mwHandleOpenGLErrors(Disable);
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

inline void Enable(GLenum cap) {
    glEnable(cap);
    __mwHandleOpenGLErrors(Enable);
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

} // gl
} // gfx
} // mw
