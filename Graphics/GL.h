#ifndef GL_H
#define GL_H

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <GL/glew.h>

#define BUFFER_OFFSET(p) ((uint8_t*)0 + p)

inline bool checkGlErrorEnum(int glEnumErrorCode) {
    if (glEnumErrorCode == GL_NO_ERROR) {
        return true;
    }

    fprintf(stderr, "glGetError() returned : ");

    switch (glEnumErrorCode) {
    case GL_INVALID_ENUM:
        fprintf(stderr, "GL_INVALID_ENUM");
        break;
    case GL_INVALID_VALUE:
         fprintf(stderr, "GL_INVALID_VALUE");
        break;
    case GL_INVALID_OPERATION:
        fprintf(stderr, "GL_INVALID_OPERATION");
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        fprintf(stderr, "GL_INVALID_FRAMEBUFFER_OPERATION");
        break;
    case GL_OUT_OF_MEMORY:
        fprintf(stderr, "GL_OUT_OF_MEMORY");
        break;
    case GL_STACK_UNDERFLOW:
        fprintf(stderr, "GL_STACK_UNDERFLOW");
        break;
    case GL_STACK_OVERFLOW:
        fprintf(stderr, "GL_STACK_OVERFLOW");
        break;
    }

    fprintf(stderr, "\n");

    return false;
}

#define DEBUG 1
#define GL_DEBUG_ASSERT 1

#if DEBUG && GL_DEBUG_ASSERT
#define GL(glexpr) \
    glexpr; \
    assert(checkGlErrorEnum(glGetError()));
#else
#define GL(glexpr) glexpr;
#endif

#define GL_flushError() (void) glGetError();

#include <algorithm>

class SafeGLid {
protected:
    GLuint glId = 0;

public:
    GLuint* operator&() { return &glId; }
    GLuint& operator*() { return glId; }
    const GLuint& operator*() const { return glId; }

    GLuint get() const { return glId; }
    GLuint id() const { return glId; }
};


/* MACRO for generating Safe Gluint ids handlers. Might exist a better way through pure template. */
#define GENERATE_GL_TypeId_SafeHandler(ClassName, glGenFunction, glDeleteFunction)      \
    class ClassName : public SafeGLid {                                                 \
    public:                                                                             \
        ClassName() { GL(glGenFunction(1, &glId)); }                                        \
        ~ClassName() { (glDeleteFunction(1, &glId)); }                                    \
        ClassName(const ClassName& other) = delete;                                     \
        void operator=(const ClassName& other) = delete;                                \
        ClassName(ClassName&& other) { std::swap(glId, other.glId); }                   \
        void operator=(ClassName&& other) { std::swap(glId, other.glId); }              \
    };

GENERATE_GL_TypeId_SafeHandler(SafeGlVboId, glGenBuffers, glDeleteBuffers)
GENERATE_GL_TypeId_SafeHandler(SafeGlFboId, glGenFramebuffers, glDeleteFramebuffers)
GENERATE_GL_TypeId_SafeHandler(SafeGlTexId, glGenTextures, glDeleteTextures)

enum EGlFunc
{
	EGlFunc_NEVER = 0x0200,
	EGlFunc_LESS = 0x0201,
	EGlFunc_EQUAL = 0x0202,
	EGlFunc_LEQUAL = 0x0203,
	EGlFunc_GREATER = 0x0204,
	EGlFunc_NOTEQUAL = 0x0205,
	EGlFunc_GEQUAL = 0x0206,
	EGlFunc_ALWAYS = 0x0207
};

enum EGlSrcFactor
{
	EGlSrcFactor_ZERO = GL_ZERO,
	EGlSrcFactor_ONE = GL_ONE, // (default)
	EGlSrcFactor_SRC_COLOR = GL_SRC_COLOR,
	EGlSrcFactor_ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
	EGlSrcFactor_DST_COLOR = GL_DST_COLOR,
	EGlSrcFactor_ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
	EGlSrcFactor_SRC_ALPHA = GL_SRC_ALPHA,
	EGlSrcFactor_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
	EGlSrcFactor_DST_ALPHA = GL_DST_ALPHA,
	EGlSrcFactor_ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
	EGlSrcFactor_CONSTANT_COLOR = GL_CONSTANT_COLOR,
	EGlSrcFactor_ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
	EGlSrcFactor_CONSTANT_ALPHA = GL_CONSTANT_ALPHA,
	EGlSrcFactor_ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
	EGlSrcFactor_SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE
};

enum EGlDstFactor
{
	EGlDstFactor_ZERO = GL_ZERO,
	EGlDstFactor_ONE = GL_ONE, // (default)
	EGlDstFactor_SRC_COLOR = GL_SRC_COLOR,
	EGlDstFactor_ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
	EGlDstFactor_DST_COLOR = GL_DST_COLOR,
	EGlDstFactor_ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
	EGlDstFactor_SRC_ALPHA = GL_SRC_ALPHA,
	EGlDstFactor_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
	EGlDstFactor_DST_ALPHA = GL_DST_ALPHA,
	EGlDstFactor_ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
	EGlDstFactor_CONSTANT_COLOR = GL_CONSTANT_COLOR,
	EGlDstFactor_ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
	EGlDstFactor_CONSTANT_ALPHA = GL_CONSTANT_ALPHA,
	EGlDstFactor_ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA
};

enum EGlBlendOp
{
	EGlBlendOp_FUNC_ADD = GL_FUNC_ADD,
	EGlBlendOp_FUNC_SUBTRACT = GL_FUNC_SUBTRACT,
	EGlBlendOp_FUNC_REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT,
	EGlBlendOp_MIN = GL_MIN,
	EGlBlendOp_MAX = GL_MAX
};

enum EGlCullMode
{
	EGlCullMode_FRONT = GL_FRONT,
	EGlCullMode_BACK = GL_BACK,
	EGlCullMode_FRONT_AND_BACK = GL_FRONT_AND_BACK
};

enum EGlFailAction
{
	EGlFailAction_KEEP = GL_KEEP,
	EGlFailAction_ZERO = GL_ZERO,
	EGlFailAction_REPLACE = GL_REPLACE,
	EGlFailAction_INCR = GL_INCR,
	EGlFailAction_INCR_WRAP = GL_INCR_WRAP,
	EGlFailAction_DECR = GL_DECR,
	EGlFailAction_DECR_WRAP = GL_DECR_WRAP,
	EGlFailAction_INVERT = GL_INVERT
};

enum EGlFillMode
{
	EGlFillMode_POINT = GL_POINT,
	EGlFillMode_LINE = GL_LINE,
	EGlFillMode_FILL = GL_FILL
};

enum EGlCullOrientation
{
	EGlCullOrientation_CW = GL_CW,
	EGlCullOrientation_CCW = GL_CCW
};

void BindTextureToSlot(GLuint glTexId, GLuint slot);
void BindTextureAndSamplerToSlot(GLuint glTexId, GLuint glSamplerId, GLuint slot);
void UnbindTextureSlot(GLuint slot);

#endif // GL_H
