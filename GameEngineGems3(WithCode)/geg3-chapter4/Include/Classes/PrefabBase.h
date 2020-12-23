#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class AssetReference;

	class PrefabBase : public Asset
	{
	public:
		Entity* entity;
		
		PrefabBase();
		virtual ~PrefabBase();
		
		virtual bool Reload(const int flags=0);
		virtual void Reset();
		virtual Asset* Copy();
		virtual Entity* Instance();
		
		static void SetEntityAsPrefab(Entity* entity);
	};
}
