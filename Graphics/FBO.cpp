#include "fbo.h"



FBO::FBO(int width, int height, int numColorTextures, bool useTextureForDepth) : m_width(width), m_height(height) {
    GL(glGenFramebuffers(1, &m_fboId));

    GL(glBindFramebuffer(GL_FRAMEBUFFER, m_fboId));

    m_colorTextures.reserve(numColorTextures + useTextureForDepth);

    if (numColorTextures > 0) {
        m_colorTextures.emplace_back();

        m_colorTextures[0].loadEmpty(width, height, Texture::TargetType::COLOR, GL_RGBA16F);

        GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTextures[0].getId(), 0));
    }

    for (int i = 1; i < numColorTextures; ++i) {
        m_colorTextures.emplace_back();

        m_colorTextures[i].loadEmpty(width, height, Texture::TargetType::COLOR);

        GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorTextures[i].getId(), 0));
    }

    if (useTextureForDepth) {
        m_colorTextures.emplace_back();

        m_colorTextures[numColorTextures].loadEmpty(width, height, Texture::TargetType::DEPTH);

        GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_colorTextures[numColorTextures].getId(), 0));
    }
    else {
        GL(glGenRenderbuffers(1, &m_depthRenderBufferId));
        GL(glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBufferId));
        GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height));
        GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBufferId));
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error. Framebuffer INCOMPLETE." << '\n';
    }

    GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

FBO::~FBO() {
    (glDeleteRenderbuffers(1, &m_depthRenderBufferId));
    (glDeleteFramebuffers(1, &m_fboId));
}
