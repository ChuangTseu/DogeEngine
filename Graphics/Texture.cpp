#include "texture.h"

#include <iostream>

Texture::Texture()
{
    GL(glGenTextures(1, &m_tex));
}

Texture::~Texture()
{
    (glDeleteTextures(1, &m_tex));
}

void Texture::bindToTarget(GLuint target) const
{
    GL(glActiveTexture(target));
    GL(glBindTexture(GL_TEXTURE_2D, m_tex));
}

bool Texture::loadFromFile(std::string filename, GLuint internalFormat)
{
    Image image;

    if (!image.loadFromFile(filename))
    {
        return false;
    }

    GL(glBindTexture(GL_TEXTURE_2D, m_tex));

    GLuint formatFrom;
    switch(image.getBytesPerPixel()) {
    case 1:
        formatFrom = GL_RED;
        break;
    case 3:
        formatFrom = GL_RGB;
        break;
    case 4:
        formatFrom = GL_RGBA;
        break;
    }

    GL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.getWidth(), image.getHeight(), 0, formatFrom, GL_UNSIGNED_BYTE, image.getData()));

    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));

    GL(glGenerateMipmap(GL_TEXTURE_2D));

    return true;
}


bool Texture::loadEmpty(int width, int height, TargetType type, GLuint channelsType)
{
    GL(glBindTexture(GL_TEXTURE_2D, m_tex));

//    GLuint pixelDataType;

//    if (gl_format == GL_DEPTH_COMPONENT24) {
//        pixelDataType = GL_FLOAT;
//    }

    if (type == TargetType::COLOR) {
        GL(glTexImage2D(GL_TEXTURE_2D, 0, channelsType, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

//        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
//        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    }
    else if (type == TargetType::DEPTH) {
        GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));

        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
    }
    else {
        std::cerr << "Wrong TargetType type for empty texture \n";

        return false;
    }


    return true;
}

bool Texture::loadFromMaterialColor(Color3f matColor) {
    GL(glBindTexture(GL_TEXTURE_2D, m_tex));

    GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, matColor.data()));

    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    return true;
}

bool Texture::loadFromBlob(int width, int height,
                           GLuint internalFormat, GLuint format, GLuint dataType,
                           void* pixelData)
{
    GL(glBindTexture(GL_TEXTURE_2D, m_tex));

    GL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, pixelData));

    GL(glGenerateMipmap(GL_TEXTURE_2D));

    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));




    return true;
}
