#ifndef TEXTURE_H
#define TEXTURE_H

#include "GL.h"

#include "image.h"

#include "MathsTools/vec3.h"

#include <string>

//#include "material.h"

class Texture
{
    GLuint m_tex;

public:
    Texture();

    ~Texture();

    enum class TargetType {
        COLOR, DEPTH
    };

    void bindToTarget(GLuint target) const;

    bool loadFromFile(std::string filename, GLuint internalFormat = GL_RGB);
    bool loadEmpty(int width, int height, TargetType type, GLuint channelsType = GL_RGBA);

    bool loadFromMaterialColor(Color3f matColor);

    GLuint getId() const {
        return m_tex;
    }
    bool loadFromBlob(int width, int height, GLuint internalFormat, GLuint format, GLuint dataType, void *pixelData);
};

#endif // TEXTURE_H
