/*
Copyright (c) 2015 Leadwerks Software

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

You may not distribute the included "Leadwerks.lib" precompiled library alone or as part of a compiled executable.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#include "App.h"

namespace Leadwerks
{
	class VegetationCell;
	class NewtonDynamicsShape;
	class VegetationLayer;

	//This subclass is created to show how the internal VegetationLayer class works
	class VegetationLayerSample : public VegetationLayer
	{
	public:
		int seed;
		int collisiontype;
		int resolution;
		int variationmapresolution;
		float density;
		Texture* variationmap;
		std::vector<Surface*> surfaces;
		int variationresolution;
		std::vector<float> variationmatrices;
		Model* model;
		Terrain* terrain;
		float billboarddistance;
		Shader* shader;
		Shader* shader_frustumcull;
		Shader* shader_copybillboard;
		Surface* instancesurface;
		Surface* billboardsurface;
		Texture* instancetexture;
		GLuint buffertexture;
		unsigned int maxinstancesdrawn;
		Vec2 sloperange;
		Vec2 heightrange;
		float viewrange;
		Buffer* billboardbuffer[2];
		std::vector<Material*> modelmaterial;
		std::vector<Material*> billboardmaterial;
		std::map<Camera*,GLuint> billboardfeedbackbuffer;
		std::map<Camera*,GLuint> modelfeedbackbuffer;
		std::map<Camera*, std::map<Context*, GLuint> > billboardocclusionquery;
        std::map<Camera*, std::map<Context*, GLuint> > modelocclusionquery;
		std::map <Camera*, Vec2> gridoffset;
		Vec2 scalerange;
		Vec2 colorrange;
		int shadowmode;
		float billboardtransition;
		bool billboardsenabled;		
		int visiblegridsize;

		VegetationLayerSample(Terrain* terrain);
		virtual ~VegetationLayerSample();

		virtual bool IntersectsAABB(const AABB& aabb, const float padding = 0.1);
		virtual Vec4 GetInstanceColor(const int x, const int z);//lua
		virtual int GetInstancesInAABB(const AABB& aabb, std::vector<Mat4>& instances, const float padding=0.1);
		virtual bool BuildShape(const bool dynamic = false, const bool recursive = true);//lua
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction = true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction = true);
		virtual void SetShadowMode(const int shadowmode);//lua
		virtual int GetShadowMode();//lua
		virtual void SetScale(const float minscale, const float maxscale);//lua
		virtual void SetColor(const float minbrightness, const float maxbrightness);//lua
		virtual void SetBillboardDistance(const float distance);//lua
		virtual void SetViewRange(const float range);//lua
		virtual void SetSlopeConstraints(const float minslope, const float maxslope);//lua
		virtual void SetHeightConstraints(const float minheight, const float maxheight);//lua
		virtual void BuildVariationMatrices();
		virtual Mat4 GetInstanceMatrix(const int x, const int z);//lua
		virtual void SetModel(Model* model);//lua
		virtual bool SetModel(const std::string& path);//lua
		virtual void PrePass(Camera* camera);
		virtual void Draw(Camera* camera);
		virtual void SetDensity(const float density);//lua
		virtual void RenderBillboard();
		virtual std::string GetClassName();

		//Physics
		NewtonMesh* newtonmesh;		
		NewtonDynamicsShape* instanceshape;
		Surface* collisionsurface;
		std::vector<int> indices;
		std::vector<float> facenormals;
		std::vector<float> vertexpositions;
		std::vector<int> collisionsurfaceindicecounts[32];
		std::vector<int> collisionsurfaceindices[32];
		std::vector<float> collisionsurfacevertices[32];
		std::vector<Mat4> instances[32];
		virtual void DrawPhysics(Camera* camera);
		NewtonCollision* newtoncollision;
		static void CollideCallback(NewtonUserMeshCollisionCollideDesc* const collideDescData, const void* const continueCollisionHandle);
		static dFloat RayHitCallback(NewtonUserMeshCollisionRayHitDesc* const lineDescData);
		static int AABBTest(void* const userData, const dFloat* const boxP0, const dFloat* const boxP1);
		static int GetFacesInAABB(void* const userData, const dFloat* const p0, const dFloat* const p1, const dFloat** const vertexArray, int* const vertexCount, int* const vertexStrideInBytes, const int* const indexList, int maxIndexCount, const int* const userDataLis);
		
		//Global
		static World* billboardworld;
		static Camera* billboardcamera;
	};
}
