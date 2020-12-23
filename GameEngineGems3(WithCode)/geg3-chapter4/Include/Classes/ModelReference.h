#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class ModelReference : public AssetReference
	{
	public:
		//Attributes
		//int instancecount;
		//std::string path;
		Entity* model;
		GraphicsDriver* driver;
		int boneiterator;
		
		//Constructor
		ModelReference();
		virtual ~ModelReference();
		
		//Procedures
		virtual bool ReadChunk(Chunk* chunk, Stream* stream, Object* node, const int flags);
		virtual bool Reload(const int flags=0);
		virtual bool Reload(Stream* stream, const int flags=0);
		virtual Asset* Instance();
        virtual Model* ModelInstance();
		virtual void Reset();
		virtual AssetReference* Copy();

        static std::map<std::string,ModelReference*> loadedmodels;
	};
}
