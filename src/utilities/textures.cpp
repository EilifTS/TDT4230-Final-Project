#include "textures.h"
#include "imageLoader.hpp"
#include <glad/glad.h>
#include <assert.h>

unsigned int Textures::LoadPNG(const std::string& file_path)
{
    PNGImage image = loadPNGFile(file_path);

    unsigned int id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return id;
}

Textures::RenderTarget Textures::CreateRenderTarget(int width, int height, int textureCount, bool depth)
{
    assert(textureCount > 0 && textureCount <= 4);
    RenderTarget out;
    unsigned int renderTargetID = 0;
    glGenFramebuffers(1, &renderTargetID);
    glBindFramebuffer(GL_FRAMEBUFFER, renderTargetID);

    for (int i = 0; i < textureCount; i++)
    {
        unsigned int targetTexture;
        glGenTextures(1, &targetTexture);
        glBindTexture(GL_TEXTURE_2D, targetTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, targetTexture, 0);

        out.textureIDs.push_back(targetTexture);
    }
    GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(textureCount, DrawBuffers);

    if (depth)
    {
        unsigned int depthBuffer;
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        out.depthID = depthBuffer;
    }

    out.targetID = renderTargetID;

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    return out;
}