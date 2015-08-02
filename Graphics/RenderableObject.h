#pragma once

#include "Doge.h"

class RenderableObject
{
public:
	virtual bool IsShown() const
	{
		return m_isShown;
	}

	virtual bool IsShadowCasting() const
	{
		return m_isShadowCasting;
	}

	virtual void Draw() const
	{
		// To Override
		DogeAssertAlways();
	}

	bool m_isShown = true;
	bool m_isShadowCasting = false;
};

