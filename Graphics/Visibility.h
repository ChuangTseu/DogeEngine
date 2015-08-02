#pragma once

#include "Doge.h"

enum EVisibility
{
	EVisibility_Any,
	EVisibility_ViewVisible,
	EVisibility_ShadowCasting,

	EVisibility_Meta_ViewVisible_And_ShadowCasting,

	EVisibility_COUNT
};

enum BitVisibility
{
	BIT_VISIBILITY_VIEWVISIBLE = (1 << EVisibility_ViewVisible),
	BIT_VISIBILITY_SHADOWCASTING = (1 << EVisibility_ShadowCasting),
	BIT_VISIBILITY_ANY = (1 << EVisibility_Any),


	BITS_VISIBILITY_META_VIEWVISIBLE_AND_SHADOWCASTING = BIT_VISIBILITY_VIEWVISIBLE | BIT_VISIBILITY_SHADOWCASTING,
	BITS_VISIBILITY_META_VIEWVISIBLE_ONLY = BIT_VISIBILITY_VIEWVISIBLE,
	BITS_VISIBILITY_META_SHADOWCASTING_ONLY = BIT_VISIBILITY_SHADOWCASTING,
	BITS_VISIBILITY_META_ANY_ONLY = BIT_VISIBILITY_ANY,
};

inline EVisibility VisibilityBitfieldToVisibilityEnum(uint32_t visibilityBitfield)
{
	switch (visibilityBitfield)
	{
	case BITS_VISIBILITY_META_ANY_ONLY:
		return EVisibility_Any;
	case BITS_VISIBILITY_META_VIEWVISIBLE_ONLY:
		return EVisibility_ViewVisible;
	case BITS_VISIBILITY_META_SHADOWCASTING_ONLY:
		return EVisibility_ShadowCasting;
	case BITS_VISIBILITY_META_VIEWVISIBLE_AND_SHADOWCASTING:
		return EVisibility_Meta_ViewVisible_And_ShadowCasting;
	default:
		DogeAssertAlways();
		return static_cast<EVisibility>(-1);
	}
}
