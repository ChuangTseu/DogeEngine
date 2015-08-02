#include "renderer.h"

#include <iostream>
#include <string>

#include "GL.h"

#include "orthobase.h"
#include "texture.h"
#include "light.h"
#include "shader.h"
#include "camera.h"
#include "fbo.h"
#include "cubemap.h"
#include "skybox.h"
#include "shadowmap.h"

#include "Utils/timer.h"

void GLAPIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                     const GLchar * message, const void * userParam) {
    (void) source;
    (void) type;
    (void) id;
    (void) severity;
    (void) length;
    (void) message;
    (void) userParam;

    std::cerr << "Error\n";
    return;
}

static void glBreak_print_source(GLenum source)
{
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
        {
            fputs("GL_DEBUG_SOURCE_API", stderr);
        }
        break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        {
            fputs("GL_DEBUG_SOURCE_WINDOW_SYSTEM", stderr);
        }
        break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
        {
            fputs("GL_DEBUG_SOURCE_SHADER_COMPILER", stderr);
        }
        break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
        {
            fputs("GL_DEBUG_SOURCE_THIRD_PARTY", stderr);
        }
        break;

        case GL_DEBUG_SOURCE_APPLICATION:
        {
            fputs("GL_DEBUG_SOURCE_APPLICATION", stderr);
        }
        break;

        case GL_DEBUG_SOURCE_OTHER:
        {
            fputs("GL_DEBUG_SOURCE_APPLICATION", stderr);
        }
        break;
    }
}

static void glBreak_print_type(GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
        {
            fputs("GL_DEBUG_TYPE_ERROR", stderr);
        }
        break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        {
            fputs("GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR", stderr);
        }
        break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        {
            fputs("GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR", stderr);
        }
        break;

        case GL_DEBUG_TYPE_PORTABILITY:
        {
            fputs("GL_DEBUG_TYPE_PORTABILITY", stderr);
        }
        break;

        case GL_DEBUG_TYPE_PERFORMANCE:
        {
            fputs("GL_DEBUG_TYPE_PERFORMANCE", stderr);
        }
        break;

        case GL_DEBUG_TYPE_MARKER:
        {
            fputs("GL_DEBUG_TYPE_MARKER", stderr);
        }
        break;

        case GL_DEBUG_TYPE_PUSH_GROUP:
        {
            fputs("GL_DEBUG_TYPE_PUSH_GROUP", stderr);
        }
        break;

        case GL_DEBUG_TYPE_POP_GROUP:
        {
            fputs("GL_DEBUG_TYPE_POP_GROUP", stderr);
        }
        break;

        case GL_DEBUG_TYPE_OTHER:
        {
            fputs("GL_DEBUG_TYPE_OTHER", stderr);
        }
        break;
    }
}

static void glBreak_print_severity(GLenum severity)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_LOW:
        {
            fputs("GL_DEBUG_SEVERITY_LOW", stderr);
        }
        break;

        case GL_DEBUG_SEVERITY_MEDIUM:
        {
            fputs("GL_DEBUG_SEVERITY_MEDIUM", stderr);
        }
        break;

        case GL_DEBUG_SEVERITY_HIGH:
        {
            fputs("GL_DEBUG_SEVERITY_HIGH", stderr);
        }
        break;
    }
}


void GLAPIENTRY glBreak_debug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam)
{
    (void) id;
    (void) length;
    (void) userParam;

    fputs("---- GL BREAKPOINT ----\n", stderr);

    fputs("Source : ", stderr);
    glBreak_print_source(source);

    fputs("\n", stderr);

    fputs("Type : ", stderr);
    glBreak_print_type(type);

    fputs("\n", stderr);

    fputs("Severity : ", stderr);
    glBreak_print_severity(severity);

    fputs("\n", stderr);

    fputs("Message : ", stderr);
    fputs(message, stderr);

    fputs("-----------------------\n", stderr);

//    if (GL_DEBUG_TYPE_ERROR == type)
//    {
//        asm("int3");
//    }
}



Renderer::Renderer(int width, int height) :
    m_width(width),
    m_height(height)
{
}

void Renderer::initializeGL(void) {
    initGL();

    GL(glViewport(0, 0, m_width, m_height));

    m_scene = new Scene(m_width, m_height);
    m_scene->initScene();

    m_scene->resize(m_width, m_height);
}

void Renderer::resizeGL(int width, int height) {
    GL(glViewport(0, 0, width, height));

    m_width = width;
    m_height = height;

    m_scene->resize(width, height);

    std::cerr << "Resizing to " << width << " x " << height << '\n';
}

void Renderer::paintGL(void) {
    render();
}

/*!!!
void Renderer::loadModel(const std::string &filename)
{
    m_scene->mainModel.loadFromFile(filename);
}
*/

void Renderer::onKeyPress(int qt_key)
{
    vec3 kright = normalize(cross(m_scene->forward, m_scene->up));
    vec3 kdown = normalize(cross(m_scene->forward, kright));

//    forward += kright*(mouse_x_rel/100.f);
//    forward += kdown*(mouse_y_rel/100.f);

//    forward.normalize();

    if (qt_key == Qt::Key_Space) {
        m_scene->freezeLamp = !m_scene->freezeLamp;
    }

    if (qt_key == Qt::Key_Z /*Z*/) {
        m_scene->position += m_scene->forward*0.1f;        }
    if (qt_key == Qt::Key_S /*S*/) {
        m_scene->position -= m_scene->forward*0.1f;        }
    if (qt_key == Qt::Key_Q /*Q*/) {
        m_scene->position -= kright*0.1f;        }
    if (qt_key == Qt::Key_D /*D*/) {
        m_scene->position += kright*0.1f;        }
    if (qt_key == Qt::Key_Shift /*SHIFT*/) {
        m_scene->position += m_scene->up*0.1f;        }
    if (qt_key == Qt::Key_Control /*CTRL*/) {
        m_scene->position -= m_scene->up*0.1f;        }

    if (qt_key == Qt::Key_Plus) {
        //m_scene->userDisplacementFactor += 0.005f;
    }
    else if (qt_key == Qt::Key_Minus) {
        //m_scene->userDisplacementFactor -= 0.005f;
    }

//    if (qt_key == Qt::Key_W) {
//        m_scene->wireframe = !(m_scene->wireframe);
//    }

//    if (qt_key == Qt::Key_0) {
//        m_scene->fboTexId = 0;      }
//    else if (qt_key == Qt::Key_1) {
//        m_scene->fboTexId = 1;      }
//    else if (qt_key == Qt::Key_2) {
//        m_scene->fboTexId = 2;      }
//    else if (qt_key == Qt::Key_3) {
//        m_scene->fboTexId = 3;      }
//    else if (qt_key == Qt::Key_4) {
//        m_scene->fboTexId = 4;      }
//    else if (qt_key == Qt::Key_5) {
//        m_scene->fboTexId = 5;      }
//    else if (qt_key == Qt::Key_6) {
//        m_scene->fboTexId = 6;      }
}

void Renderer::reloadShader()
{
    m_scene->reloadShaders();
}

void Renderer::toggleWireframe()
{
    m_scene->wireframe = !m_scene->wireframe;
}

void Renderer::setFinalFboTarget(int targetIndex)
{
    m_scene->fboTexId = targetIndex;
}

bool Renderer::initGL()
{
    std::cerr << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
	GL((void)0);
    std::cerr << "OpenGL Vendor : " << glGetString(GL_VENDOR) << std::endl;
	GL((void)0);


    // EVERYONE ON EVERY PLATFORM WILL ENJOY THIS
//    #ifdef WIN32

    /* Potential fix for crash when SDL context > OpenGL 3.1 + GLEW */
    glewExperimental = GL_TRUE;
    GLenum GLEWinitialization( glewInit() );
    GL_flushError(); // GLEW might set the GL error flag even when apparently successful (know "bug")

    std::cerr << "Initializing GLEW...\n";

    if(GLEWinitialization != GLEW_OK)
    {
        std::cout << "Error initializing GLEW : " << glewGetErrorString(GLEWinitialization) << std::endl;
        return false;
    }

//    #endif

    // Activation du Depth Buffer

    GL(glEnable(GL_DEPTH_TEST));

    GL(glFrontFace(GL_CW));
    GL(glCullFace(GL_BACK));

    GL(glEnable(GL_CULL_FACE));

    return true;
}

void Renderer::rotateCamera(int mouse_x_rel, int mouse_y_rel)
{
    vec3 kright = normalize(cross(m_scene->forward, m_scene->up));
    vec3 kdown = normalize(cross(m_scene->forward, kright));

    m_scene->forward += kright*(mouse_x_rel/100.f);
    m_scene->forward += kdown*(mouse_y_rel/100.f);

//    forward.normalize();

//    if (input.getKey(SDL_SCANCODE_W)) {
//        position += forward*0.1f;        }
//    if (input.getKey(SDL_SCANCODE_S)) {
//        position -= forward*0.1f;        }
//    if (input.getKey(SDL_SCANCODE_A)) {
//        position -= kright*0.1f;        }
//    if (input.getKey(SDL_SCANCODE_D)) {
//        position += kright*0.1f;        }
//    if (input.getKey(SDL_SCANCODE_LSHIFT)) {
//        position += up*0.1f;        }
//    if (input.getKey(SDL_SCANCODE_LCTRL)) {
//        position -= up*0.1f;        }

    m_scene->camera.UpdateLookAtExplicit(m_scene->position, m_scene->position + m_scene->forward, m_scene->up);
}

void Renderer::translateCamera(int mouse_x_rel, int mouse_y_rel, int mouse_z_rel)
{
    vec3 kright = normalize(cross(m_scene->forward, m_scene->up));
    vec3 kdown = normalize(cross(m_scene->forward, kright));

    m_scene->position += kright*(-0.1f*mouse_x_rel);
    m_scene->position += m_scene->up*(0.1f*mouse_y_rel);
    m_scene->position += m_scene->forward*(0.1f*mouse_z_rel);
}

void Renderer::gammaChanged(float value)
{
    m_scene->gamma = value;
}

void Renderer::keyValueChanged(float value)
{
    m_scene->keyValue = value;
}

/* !!! *************************************************************************/

void Renderer::setTesselationFactor( float tesselation_factor )
{
    m_scene->tessFactor = tesselation_factor;
}

void Renderer::setDispFactor( float disp_factor )
{
    m_scene->userDisplacementFactor = disp_factor*0.005f;
}

void Renderer::setNbSamples( int nb_samples )
{
    m_scene->nbSample = nb_samples;
}

void Renderer::setRoughnessOffset(double roughnessOffset)
{
    m_scene->roughnessOffset = roughnessOffset;
}

void Renderer::setFresnel0(double fresnel0)
{
    m_scene->fresnel0 = fresnel0;
}

void Renderer::loadModel( const std::string& model_path )
{
    m_scene->mainModel.loadFromFile(model_path);
}

void Renderer::loadLEADRTexture( const std::string & leadr )
{
    importLeadrTexture(leadr.c_str(), m_scene->leadr1, m_scene->leadr2);
}

void Renderer::loadTexture( const std::string & texture)
{
    m_scene->texture.loadFromFile( texture );
}
void Renderer::loadDispMap( const std::string & disp_map )
{
    m_scene->dogeMap.loadFromFile( disp_map );
}
void Renderer::loadNormalMap( const std::string & normal_map )
{
    m_scene->normalMap.loadFromFile( normal_map );
}

void Renderer::loadEnvTexture( const std::string & env_path )
{
    m_scene->envmap.loadFromFile( env_path );
}

void Renderer::loadIrradianceMap( const std::string & irr_path )
{
    importCoeffs( irr_path.c_str(), m_scene->shc );
    computeMatrixRepresentation( m_scene->shc );
}

void Renderer::setStandardRendering()
{
    m_scene->wireframe = false;
}

void Renderer::setWireframeRendering()
{
    m_scene->wireframe = !m_scene->wireframe;
}

void Renderer::setDepthRendering()
{
    
}

void Renderer::setBRDF( int brdf_choice )
{
    m_scene->currentBRDF = brdf_choice;
}

void Renderer::setNormalMode( bool normal_mode )
{
    
}

void Renderer::setFilteringMode(bool enabled)
{
    m_scene->filtering = enabled;
}

void Renderer::setDiffuseEnabled(bool enabled)
{
    m_scene->diffuse = enabled;
}

void Renderer::setSpecularDirectEnabled(bool enabled)
{
    m_scene->specularDirect = enabled;
}

void Renderer::setSpecularEnvEnabled(bool enabled)
{
    m_scene->specularEnv = enabled;
}

void Renderer::setDiffuseDirectEnabled(bool enabled)
{
    m_scene->diffuseDirect = enabled;
}

void Renderer::setDiffuseEnvEnabled(bool enabled)
{
    m_scene->diffuseEnv = enabled;
}

/******************************************************************************/

void Renderer::render()
{
//    Timer timer;

//    timer.start();

    //m_scene->renderLeadrQuadOnly();
    m_scene->render();

//    timer.stop();

//    std::cerr << "Render time: " << timer.getElapsedTimeInMilliSec() << '\n';
}
