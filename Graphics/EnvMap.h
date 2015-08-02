#ifndef ENVMAP_H
#define ENVMAP_H

#include "GL.h"

#include "shader.h"
#include "model.h"
#include "image.h"

#include <string>

class EnvMap
{
    GLuint m_tex = 0;

    Shader m_shader;

    Model m_cube;

public:
    enum {
        SPHERICAL = 0, CROSS_VERTICAL, CROSS_HORIZONTAL, LATLONG, LINE, UNKNOWN
    };

    static const char* TypesStr[5];

    EnvMap();

    ~EnvMap();

    void renew();

    bool loadFromFile(std::string filename);

    Shader& getShader() {
        return m_shader;
    }

    void render(const mat4& projection, const mat4& pureView);

    void bindTextureToTarget(GLuint target) const;
private:
    int channelToRgbFormatHDR(int numChannels) const;

    int channelToSRgbFormat(int numChannels) const;

    bool loadAsSpherical(Image& image);
    bool loadAsSphericalFloat(int width, float *imageData);

    int determineTypeFromDimension(int width, int height);
};

#endif // ENVMAP_H
