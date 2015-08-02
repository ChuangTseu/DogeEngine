#include "GL.h"

void BindTextureToSlot(GLuint glTexId, GLuint slot)
{
	GL(glActiveTexture(GL_TEXTURE0 + slot));
	GL(glBindTexture(GL_TEXTURE_2D, glTexId));
}

void BindTextureAndSamplerToSlot(GLuint glTexId, GLuint glSamplerId, GLuint slot)
{
	GL(glActiveTexture(GL_TEXTURE0 + slot));
	GL(glBindTexture(GL_TEXTURE_2D, glTexId));
	GL(glBindSampler(slot, glSamplerId));
}

void UnbindTextureSlot(GLuint slot)
{
	GL(glActiveTexture(GL_TEXTURE0 + slot));
	GL(glBindTexture(GL_TEXTURE_2D, 0));
}
