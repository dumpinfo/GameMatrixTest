#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class ModelFactory : public Asset
	{
	public:
		Entity* model;
		int boneiterator;
		bool occlusionmode;
		int version;
		Shape* shape[2];
		bool dynamicphysics;

		ModelFactory();
		virtual ~ModelFactory();
		
        virtual Asset* Copy();
		virtual bool ReadChunk(Chunk* chunk, Stream* stream, Object* node, const int flags);
		virtual bool Reload(const int flags=0);
		virtual bool Reload(Stream* stream, const int flags=0);
        virtual Model* Instance();
		virtual void Reset();
		virtual Shape* GetShape(const bool dynamic);

        static std::map<std::string,ModelFactory*> loadedmodels;
	};
}
