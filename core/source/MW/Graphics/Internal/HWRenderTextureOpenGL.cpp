/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWRenderTextureOpenGL;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.Internal.HWTextureOpenGL;
import Microwave.System.Exception;
import <stdexcept>;
import <string>;

namespace mw {
inline namespace gfx {

HWRenderTextureOpenGL::HWRenderTextureOpenGL(const gptr<HWTexture>& backingTex)
	: tex(gpcast<HWTextureOpenGL>(backingTex))
{
	if(!tex)
		throw Exception("'tex' cannot be null");

	size = tex->size;

	auto graphics = GraphicsContext::GetCurrent();
	if (!graphics)
		throw Exception("no active graphics context");

	if(tex->format != PixelDataFormat::RGBA32)
		throw Exception("The only supported texture format is RGBA32");

	// create framebuffer object
	gl::Int fb;
	gl::GetIntegerv(gl::FRAMEBUFFER_BINDING, &fb);

	// create frame buffer
	gl::GenFramebuffers(1, &frameBuffer);
	gl::BindFramebuffer(gl::FRAMEBUFFER, frameBuffer);
	gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, tex->textureID, 0);

	// create depth buffer
	gl::GenRenderbuffers(1, &depthBuffer);
	gl::BindRenderbuffer(gl::RENDERBUFFER, depthBuffer);
	gl::RenderbufferStorage(gl::RENDERBUFFER, gl::DEPTH_COMPONENT24, size.x, size.y);
	
	// attach depth buffer to frame buffer
	gl::FramebufferRenderbuffer(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::RENDERBUFFER, depthBuffer);

	// check for errors
	gl::Enum status = gl::CheckFramebufferStatus(gl::FRAMEBUFFER);
	if(status != gl::FRAMEBUFFER_COMPLETE)
	{
		gl::DeleteRenderbuffers(1, &depthBuffer);
		gl::DeleteFramebuffers(1, &frameBuffer);

		std::string err = "RenderTexture creation failed: ";

		switch(status)
		{
		case gl::FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			err += "Incomplete Attachment";
			break;
		case gl::FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			err += "Incomplete Missing Attachment";
			break;
		case gl::FRAMEBUFFER_UNSUPPORTED:
			err += "Unsupported";
			break;
		}

		throw Exception(err);
	}

	gl::BindFramebuffer(gl::FRAMEBUFFER, fb);
}

HWRenderTextureOpenGL::~HWRenderTextureOpenGL()
{
	if(gl::IsRenderbuffer(depthBuffer))
		gl::DeleteRenderbuffers(1, &depthBuffer);

	if(gl::IsFramebuffer(frameBuffer))
		gl::DeleteFramebuffers(1, &frameBuffer);
}

IVec2 HWRenderTextureOpenGL::GetSize() {
    return size;
}

gptr<HWTexture> HWRenderTextureOpenGL::GetTexture() {
	return tex;
}

} // gfx
} // mw
