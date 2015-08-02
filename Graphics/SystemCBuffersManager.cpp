#include "SystemCBuffersManager.h"

template <class CBSTRUCT>
void CommitCBufferFromCStruct(GLuint glCbId, const CBSTRUCT& cbStruct)
{
	GL(glBindBuffer(GL_UNIFORM_BUFFER, glCbId));
	GL(glBufferData(GL_UNIFORM_BUFFER, sizeof(CBSTRUCT), &cbStruct, GL_DYNAMIC_DRAW));
	GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void CreateAndBindSystemCBuffer(GLuint& glCbId, int definitiveBindSlot)
{
	GL(glGenBuffers(1, &glCbId));
	GL(glBindBufferBase(GL_UNIFORM_BUFFER, definitiveBindSlot, glCbId));
}

PerFrameCB SystemCBuffersManager::m_perFrameCb;
GLuint SystemCBuffersManager::m_perFrameCbId;
PerViewPointCB SystemCBuffersManager::m_perViewPointCb;
GLuint SystemCBuffersManager::m_perViewPointId;
PerObjectCB SystemCBuffersManager::m_perObjectCb;
GLuint SystemCBuffersManager::m_perObjectCbId;
PerLightCB SystemCBuffersManager::m_perLightCb;
GLuint SystemCBuffersManager::m_perLightCbId;
PerMaterialCB SystemCBuffersManager::m_perMaterialCb;
GLuint SystemCBuffersManager::m_perMaterialCbId;
PerFboCB SystemCBuffersManager::m_perFboCb;
GLuint SystemCBuffersManager::m_perFboCbId;
UIConfigCB SystemCBuffersManager::m_UIConfigCb;
GLuint SystemCBuffersManager::m_UIConfigCbId;

void SystemCBuffersManager::Initialize()
{
	CreateAndBindSystemCBuffer(m_perFrameCbId, 0);
	CreateAndBindSystemCBuffer(m_perViewPointId, 1);
	CreateAndBindSystemCBuffer(m_perObjectCbId, 2);
	CreateAndBindSystemCBuffer(m_perLightCbId, 3);
	CreateAndBindSystemCBuffer(m_perMaterialCbId, 4);
	CreateAndBindSystemCBuffer(m_perFboCbId, 5);

	CreateAndBindSystemCBuffer(m_UIConfigCbId, 8);
}

void SystemCBuffersManager::CommitPerFrameCB()
{
	CommitCBufferFromCStruct(m_perFrameCbId, m_perFrameCb);
}

void SystemCBuffersManager::CommitPerViewPointCB()
{
	CommitCBufferFromCStruct(m_perViewPointId, m_perViewPointCb);
}

void SystemCBuffersManager::CommitPerObjectCB()
{
	CommitCBufferFromCStruct(m_perObjectCbId, m_perObjectCb);
}

void SystemCBuffersManager::CommitPerLightCB()
{
	CommitCBufferFromCStruct(m_perLightCbId, m_perLightCb);
}

void SystemCBuffersManager::CommitPerMaterialCB()
{
	CommitCBufferFromCStruct(m_perMaterialCbId, m_perMaterialCb);
}

void SystemCBuffersManager::CommitPerFboCB()
{
	CommitCBufferFromCStruct(m_perFboCbId, m_perFboCb);
}

void SystemCBuffersManager::CommitUIConfigCB()
{
	CommitCBufferFromCStruct(m_UIConfigCbId, m_UIConfigCb);
}
