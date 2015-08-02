#include "envmap.h"

const char* EnvMap::TypesStr[5] = { "SPHERICAL", "CROSS_VERTICAL", "CROSS_HORIZONTAL", "LATLONG", "LINE" };

EnvMap::EnvMap() {
    GL(glGenTextures(1, &m_tex));

    m_cube.loadFromFile("cube.obj");

    m_shader.addVertexShader("skybox.vert");
    m_shader.addFragmentShader("skybox.frag");
    m_shader.link();
}

EnvMap::~EnvMap() {
    (glDeleteTextures(1, &m_tex));
}

void EnvMap::renew() {
    (glDeleteTextures(1, &m_tex));
    GL(glGenTextures(1, &m_tex));

    m_shader.renew();
}

std::string getFilenameExt(const std::string& filename) {
    std::string::size_type pos = filename.find_first_of('.');

    if((pos == std::string::npos) || (pos == 0)) return {};

    return filename.substr(pos + 1);
}

struct squareFloatImgBlob {
    int width;
    float* data;

    void loadFromFloatFormat(const std::string& filename) {
        FILE *fp ;
        assert(fp = fopen(filename.c_str(),"rb")) ;

        fseek(fp, 0, SEEK_END);

        long fileSize = ftell(fp);

        width = static_cast<int>(sqrt(fileSize/12));

        fseek(fp, 0, SEEK_SET);

        data = (float*) malloc(fileSize);

        size_t bytesRead = fread(data, 1, fileSize, fp);

        printf("File %s has width %d and %d\n", filename.c_str(), width, bytesRead);

        fclose(fp) ;
    }
};

bool EnvMap::loadFromFile(std::string filename) {
    if (m_tex != 0) renew();

    std::cerr << filename << " extension : " << getFilenameExt(filename) << '\n';

    if (getFilenameExt(filename) == "float") {
        squareFloatImgBlob imageBlob;

        imageBlob.loadFromFloatFormat(filename);

        vec3* vData = (vec3*) imageBlob.data;
        size_t size = imageBlob.width*imageBlob.width;
        int w = imageBlob.width;

        for (int i = 0; i < imageBlob.width / 2; ++i) {
            for (int j = 0; j < imageBlob.width; ++j) {
                std::swap(vData[i*w + j], vData[(w - i - 1)*w + j]);
            }
        }

        return loadAsSphericalFloat(imageBlob.width, imageBlob.data);
    }

    Image image;

    if (!image.loadFromFile(filename))
    {
        return false;
    }

    int width = image.getWidth();
    int height = image.getHeight();

    int envType = determineTypeFromDimension(width, height);

    switch (envType) {
    case SPHERICAL:
        std::cerr << "Loading " << TypesStr[SPHERICAL] << " " << filename << " (" << image.getBytesPerPixel() << " bpp)" << '\n';
        loadAsSpherical(image);
        break;
    case CROSS_VERTICAL:
        std::cerr << "Loading " << TypesStr[CROSS_VERTICAL] << " " << filename << " (" << image.getBytesPerPixel() << " bpp)" << '\n';
        break;
    case CROSS_HORIZONTAL:
        std::cerr << "Loading " << TypesStr[CROSS_HORIZONTAL] << " " << filename << " (" << image.getBytesPerPixel() << " bpp)" << '\n';
        break;
    case LATLONG:
        std::cerr << "Loading " << TypesStr[LATLONG] << " " << filename << " (" << image.getBytesPerPixel() << " bpp)" << '\n';
        break;
    case LINE:
        std::cerr << "Loading " << TypesStr[LINE] << " " << filename << " (" << image.getBytesPerPixel() << " bpp)" << '\n';
        break;
    case UNKNOWN:
        std::cerr << "Error. Unknown environnement map type for " << filename << '\n';
        break;
    }

    return true;
}

void EnvMap::render(const mat4 &projection, const mat4 &pureView) {
    GL(glDepthMask(GL_FALSE));

    m_shader.use();

    GL(glActiveTexture(GL_TEXTURE0));
    GL(glBindTexture(GL_TEXTURE_2D, m_tex));

    mat4 PureViewProjection = projection * pureView;

    GL(glUniformMatrix4fv(glGetUniformLocation(m_shader.getProgramId(), "PureViewProjection"), 1, GL_FALSE,
                       PureViewProjection.data()));

    m_cube.drawAsTriangles();

    Shader::unbind();

    GL(glDepthMask(GL_TRUE));
}

void EnvMap::bindTextureToTarget(GLuint target) const
{
    GL(glActiveTexture(target));
    GL(glBindTexture(GL_TEXTURE_2D, m_tex));
}

int EnvMap::channelToRgbFormatHDR(int numChannels) const {
    switch(numChannels) {
    case 1:
        return GL_RED;
    case 3:
        return GL_RGB16F;
    case 4:
        return GL_RGBA16F;
    default:
        std::cerr << "Channel number " << numChannels << " not handled. Return -1" << '\n';
        return -1;
    }
}

int EnvMap::channelToSRgbFormat(int numChannels) const {
    switch(numChannels) {
    case 3:
        return GL_SRGB;
    case 4:
        return GL_SRGB_ALPHA;
    default:
        std::cerr << "Channel number " << numChannels << " not handled. Return -1" << '\n';
        return -1;
    }
}

bool EnvMap::loadAsSpherical(Image &image) {
    GL(glBindTexture(GL_TEXTURE_2D, m_tex));

    GLuint formatFrom = image.getGlFormat();
    GLuint typeFrom = image.getGlType();

    GLuint internalFormat;

    if (image.isHdr()) {
        std::cerr << "HDR!\n";
        internalFormat = channelToRgbFormatHDR(image.getNumChannels());
    }
    else {
        internalFormat = channelToSRgbFormat(image.getNumChannels());
    }

    GL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.getWidth(), image.getHeight(), 0, formatFrom, typeFrom, image.getData()));

    GL(glGenerateMipmap(GL_TEXTURE_2D));

    GL(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));


    m_shader.addVertexShader("skybox.vert");
    m_shader.addFragmentShader("env_spherical.frag");

    m_shader.link();

    return true;
}

bool EnvMap::loadAsSphericalFloat(int width, float* imageData) {
    GL(glBindTexture(GL_TEXTURE_2D, m_tex));

    GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, width, 0, GL_RGB, GL_FLOAT, imageData));

    GL(glGenerateMipmap(GL_TEXTURE_2D));

    GL(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));


    m_shader.addVertexShader("skybox.vert");
    m_shader.addFragmentShader("env_spherical.frag");

    m_shader.link();

    return true;
}

int EnvMap::determineTypeFromDimension(int width, int height) {
    int w = width;
    int h = height;

    if (w == h) {
        return SPHERICAL;
    }

    if (w == 2*h) {
        return LATLONG;
    }

    if (3*w == 4*h) {
        return CROSS_HORIZONTAL;
    }

    if (4*w == 3*h) {
        return CROSS_VERTICAL;
    }

    if (w == 6*h) {
        return LINE;
    }

    return UNKNOWN;
}
