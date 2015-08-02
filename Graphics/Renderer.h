#ifndef RENDERER_H
#define RENDERER_H

#include "scene.h"
#include <QKeyEvent>
#include <string>

#include "leadr_tools_import.h"

#ifdef USE_SDL2
#include "Sdl2GUI/rendererinterface.h"

class Renderer : public RendererInterface {
#else
class Renderer /*: public RendererInterface*/ {
#endif
public:
    Renderer(int width, int height);

    /*virtual*/ void initializeGL(void);
    /*virtual*/ void resizeGL(int width, int height);
    /*virtual*/ void paintGL(void);

    //virtual void loadModel(const std::string &filename);

    virtual void onKeyPress(int qt_key);

    /*virtual */void reloadShader();
    /*virtual */void toggleWireframe();
    /*virtual */void setFinalFboTarget(int targetIndex);

    bool initGL();
    void initScene();

    void render();

    void rotateCamera(int mouse_x_rel, int mouse_y_rel);
    void translateCamera(int mouse_x_rel, int mouse_y_rel, int mouse_z_rel);

    void gammaChanged(float value);
    void keyValueChanged(float value);

    /** **/
    void setTesselationFactor( float tesselation_factor );
    void setDispFactor( float disp_factor );
    void setNbSamples( int nb_samples );
    void setRoughnessOffset(double roughnessOffset);
    void setFresnel0( double fresnel0 );
    
    void loadModel( const std::string &model_path );
    
    void loadLEADRTexture( const std::string & leadr );
    void loadTexture( const std::string & texture);
    void loadDispMap( const std::string & disp_map );
    void loadNormalMap( const std::string & normal_map );
    
    void loadEnvTexture( const std::string & env_path );
    void loadIrradianceMap( const std::string & irr_path );

    void setFilteringMode(bool enabled);
    void setDiffuseEnabled(bool enabled);
    void setSpecularDirectEnabled(bool enabled);
    void setSpecularEnvEnabled(bool enabled);
    void setDiffuseDirectEnabled(bool enabled);
    void setDiffuseEnvEnabled(bool enabled);
    
    void setStandardRendering();
    void setWireframeRendering();
    void setDepthRendering();
    
    void setBRDF(int brdf_choice );
    void setNormalMode( bool normal_mode );
    ~Renderer(){};
    /** **/

private:
    int m_width;
    int m_height;

//CURRENT RENDERLOOP DATA: TO REFACTOR !!!!
private:
    Scene* m_scene{nullptr};
};

#endif // RENDERER_H
