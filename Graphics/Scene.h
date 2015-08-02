#ifndef SCENE_H
#define SCENE_H

#include "orthobase.h"
#include "texture.h"
#include "light.h"
#include "shader.h"
#include "camera.h"
#include "fbo.h"
#include "cubemap.h"
#include "skybox.h"
#include "shadowmap.h"
#include "screenpass.h"
#include "envmap.h"

#include "MathsTools/mat4.h"
#include "MathsTools/vec3.h"

#include <memory>

#include "leadr_tools_import.h"

class Scene
{
public:
    Scene(int width, int height);

    void initScene();

    void resize(int width, int height);

    void render();

    void reloadShaders();


    void renderLeadrQuadOnly();

public:
    int m_width;
    int m_height;

    std::unique_ptr<FBO> fbo{nullptr};

    /* SHADERS */
    Shader s;

    ScreenPass reducePass;
    ScreenPass finalScreenPass;
    ScreenPass toLuminancePass;
    ScreenPass tonemapPass;

    std::vector<Shader*> renewableShadersList;

//    Shader quadFboShader;

//    Shader reduceShader;

//    Mesh quadFbo;

    Camera camera;

    vec3 up = vec3{0, 1, 0};
    vec3 position = vec3{5.f, 5.f, 5.f};
    vec3 forward = normalize(vec3{0, 0, 0} - position);

    Model mainModel;


    Model basicLamp;
    Shader basicLampShader;


    OrthoBase base;

    Texture texture;
    Texture normalMap;
    Texture dogeMap;

    EnvMap envmap;

    Skybox skybox;

    Shadowmap shadowmap{1024, 1024};

    float theta = 0;

    float userDisplacementFactor = 0.f;

    bool wireframe = false;

    int fboTexId = 0;

    float gamma = 2.2f;
    float keyValue = 0.18f;

    SphericalHarmonicsCoeffs shc;

    Texture leadr1;
    Texture leadr2;

    LEADR_screenpass leadrScreenpass;

    float tessFactor;
    int nbSample;

    bool filtering = true;

    bool diffuse = true;
    bool specularDirect = true;
    bool specularEnv = true;
    bool diffuseDirect = true;
    bool diffuseEnv = true;

    int currentBRDF = 0;

    float roughnessOffset = 0.f;
    float fresnel0 = 0.05f;

    ScreenPass gradPass;

    bool freezeLamp = false;
};

#endif // SCENETORENAME_H
