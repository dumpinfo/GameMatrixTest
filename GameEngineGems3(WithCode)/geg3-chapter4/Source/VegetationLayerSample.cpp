/*
Copyright (c) 2015 Leadwerks Software

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

You may not distribute the included "Leadwerks.lib" precompiled library alone or as part of a compiled executable.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "VegetationLayerSample.h"

namespace Leadwerks
{
	#define BILLBOARDCELLSIZE 256
	#define BILLBOARDCELLSPACES 16
	#define BILLBOARDSIZE BILLBOARDCELLSIZE*BILLBOARDCELLSPACES
	#define BILLBOARDCELLS 16

	World* VegetationLayerSample::billboardworld = NULL;
	Camera* VegetationLayerSample::billboardcamera = NULL;
	
	VegetationLayerSample::VegetationLayerSample(Terrain* terrain)
	{
		seed = terrain->vegetationlayers.size();
		terrain->vegetationlayers.push_back(this);
		countrenderableentities = 1;
		isrenderable = true;
		classid = VegetationLayerClass;
		instanceshape = NULL;
		collisiontype = Collision::Scene;
		billboardsenabled = true;
		billboardtransition = 5.0;
		shadowmode = Light::Dynamic;
		colorrange = Vec2(0.5, 1.5);
		scalerange = Vec2(0.75, 1.25);
		sloperange = Vec2(0, 25);
		heightrange = Vec2(0, 10000);
		billboarddistance = 30;
		viewrange = 500;
		maxinstancesdrawn = 65536;
		this->terrain = terrain;
		variationmap = NULL;
		variationmapresolution = 16;
		resolution = 2048;
		collisiontype = 0;
		density = 4.0;
		model = NULL;
		shader_copybillboard = Shader::Load("Shaders/Vegetation/copybillboard.shader");
		shader_frustumcull = Shader::Load("Shaders/Vegetation/FrustumCull.shader");
		shader = Shader::Load("Shaders/Vegetation/diffuse.shader");

		if (billboardworld == NULL)
		{
			World* prevworld = World::GetCurrent();
			billboardworld = World::Create();
			World::SetCurrent(billboardworld);
			billboardcamera = Camera::Create();
			World::SetCurrent(prevworld);
		}

		billboardsurface = Surface::Create();

		//Horizontal face
		billboardsurface->AddVertex(-0.5, -0.5, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0);
		billboardsurface->AddVertex(0.5, -0.5, 0.0, 0.0, 0.0, -1.0, 1.0, 1.0);
		billboardsurface->AddVertex(0.5, 0.5, 0.0, 0.0, 0.0, -1.0, 1.0, 0.0);
		billboardsurface->AddVertex(-0.5, 0.5, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0);
		billboardsurface->AddTriangle(0, 2, 1);
		billboardsurface->AddTriangle(3, 2, 0);

		//Vertical face
		billboardsurface->AddVertex(-0.5, 0.0, -0.5, 0.0, -1.0, 0.0, 0.0, 1.0);
		billboardsurface->AddVertex(0.5, 0.0, -0.5, 0.0, -1.0, 0.0, 1.0, 1.0);
		billboardsurface->AddVertex(0.5, 0.0, 0.5, 0.0, -1.0, 0.0, 1.0, 0.0);
		billboardsurface->AddVertex(-0.5, 0.0, 0.5, 0.0, -1.0, 0.0, 0.0, 0.0);
		billboardsurface->AddTriangle(4 + 0, 4 + 2, 4 + 1);
		billboardsurface->AddTriangle(4 + 3, 4 + 2, 4 + 0);

		billboardsurface->UpdateTangentsAndBinormals();

		billboardsurface->material = Material::Create();
		billboardsurface->material->shader[0] = Shader::Load("Shaders/Vegetation/billboard.shader");

		billboardbuffer[0] = Buffer::Create(BILLBOARDSIZE, BILLBOARDCELLSIZE, 3, 0, 0);
		billboardbuffer[1] = Buffer::Create(BILLBOARDCELLSIZE, BILLBOARDCELLSIZE, 3, 0, 0);

		const int batchsize = 8192;
		instancesurface = Surface::Create();
		instancesurface->AddVertex(0, 0, 0);
		instancesurface->positionarray->data->Resize(batchsize*sizeof(float) * 3);
		instancesurface->mode = SURFACE_POINTS;
	}

	VegetationLayerSample::~VegetationLayerSample()
	{
		if (shader_frustumcull)
		{
			shader_frustumcull->Release();
			shader_frustumcull = NULL;
		}
		for (int n = 0; n < 2; n++)
		{
			if (billboardbuffer[n])
			{
				billboardbuffer[n]->Release();
				billboardbuffer[n] = NULL;
			}
		}
		if (billboardsurface)
		{
			billboardsurface->Release();
			billboardsurface = NULL;
		}
		if (model)
		{
			model->Release();
			model = NULL;
		}
	}

	std::string VegetationLayerSample::GetClassName()
	{
		return "VegetationLayer";
	}
	
	void VegetationLayerSample::DrawPhysics(Camera* camera)
	{
		if (instanceshape)
		{
			Surface* surface = instanceshape->GetSurface(1);
			for (int x = 0; x < variationmapresolution; x++)
			{
				for (int y = 0; y < variationmapresolution; y++)
				{
					Mat4 mat = GetInstanceMatrix(x, y);
					surface->batch[camera->islight]->AddInstance(camera, mat, Vec4(0.5, 0.5, 1.0, 1.0), this);
				}
			}
		}
	}

	void VegetationLayerSample::CollideCallback(NewtonUserMeshCollisionCollideDesc* const collideDescData, const void* const continueCollisionHandle)
	{
		//Set face count and vertex stride
		collideDescData->m_faceCount = 0;
		collideDescData->m_vertexStrideInBytes = 3 * sizeof(float);

		//Get the vegetation layer object
		VegetationLayerSample* layer = (VegetationLayerSample*)collideDescData->m_userData;
		if (layer->instanceshape == NULL)return;

		//Calculate an AABB
		AABB aabb = AABB(collideDescData->m_boxP0[0], collideDescData->m_boxP0[1], collideDescData->m_boxP0[2], collideDescData->m_boxP1[0], collideDescData->m_boxP1[1], collideDescData->m_boxP1[2]);
		
		//Get current thread ID
		int threadNumber = collideDescData->m_threadNumber;
		
		//Retrieve all intersecting instances
		layer->instances[threadNumber].resize(0);
		int count = layer->GetInstancesInAABB(aabb, layer->instances[threadNumber]);
		if (count == 0) return;

		//For each intersecting instance, add that info to the collision data
		Surface* collisionsurface = layer->instanceshape->GetSurface(0);
		
		Vec3 pos, result;
		int sz, index_offset, p;
		Mat4 identity;
		float m, offset;
		int tris_count = collisionsurface->CountTriangles();
		int vert_count = collisionsurface->CountVertices();

		collideDescData->m_faceCount = tris_count * count;

		layer->collisionsurfacevertices[threadNumber].resize(count * (vert_count + tris_count) * 3);
		layer->collisionsurfaceindices[threadNumber].resize(count * layer->indices.size());
		layer->collisionsurfaceindicecounts[threadNumber].resize(tris_count * count);

		//Fill face indice counts with '3' (triangles)
		std::fill(layer->collisionsurfaceindicecounts[threadNumber].begin(), layer->collisionsurfaceindicecounts[threadNumber].end(), 3);

		//Collate all intersecting instances into vertex / indice arrays
		for (int n = 0; n < count; n++)
		{
			//Transform and add vertices
			for (p = 0; p < vert_count; p++)
			{
				offset = p * 3;

				pos.x = layer->vertexpositions[offset + 0];
				pos.y = layer->vertexpositions[offset + 1];
				pos.z = layer->vertexpositions[offset + 2];

				FastMath::Mat4MultiplyVec3(layer->instances[threadNumber][n], pos, result);
				
				offset = ((n * (vert_count + tris_count)) + p) * 3;
				layer->collisionsurfacevertices[threadNumber][offset + 0] = result.x;
				layer->collisionsurfacevertices[threadNumber][offset + 1] = result.y;
				layer->collisionsurfacevertices[threadNumber][offset + 2] = result.z;
			}

			for (p = 0; p < tris_count; p++)
			{
				offset = p * 3;
				pos.x = layer->facenormals[offset + 0];
				pos.y = layer->facenormals[offset + 1];
				pos.z = layer->facenormals[offset + 2];

				FastMath::Mat3MultiplyVec3(Mat3(layer->instances[threadNumber][n]), pos, result);
				
				m = result.Length();

				offset = ((n * (vert_count + tris_count)) + vert_count + p) * 3;
				layer->collisionsurfacevertices[threadNumber][offset + 0] = result.x / m;
				layer->collisionsurfacevertices[threadNumber][offset + 1] = result.y / m;
				layer->collisionsurfacevertices[threadNumber][offset + 2] = result.z / m;
			}
			
			memcpy(&layer->collisionsurfaceindices[threadNumber][n * layer->indices.size()], &layer->indices[0], layer->indices.size() * sizeof(int));

			//Offset indices
			index_offset = n * (collisionsurface->CountVertices() + collisionsurface->CountTriangles());
			for (p = 0; p < tris_count; p++)
			{
				offset = n * layer->indices.size() + p * 9;
				layer->collisionsurfaceindices[threadNumber][offset + 0] += index_offset;
				layer->collisionsurfaceindices[threadNumber][offset + 1] += index_offset;
				layer->collisionsurfaceindices[threadNumber][offset + 2] += index_offset;
				layer->collisionsurfaceindices[threadNumber][offset + 4] += index_offset;
				layer->collisionsurfaceindices[threadNumber][offset + 5] += index_offset;
				layer->collisionsurfaceindices[threadNumber][offset + 6] += index_offset;
				layer->collisionsurfaceindices[threadNumber][offset + 7] += index_offset;
			}
		}
		//Set collisioninfo array pointers
		collideDescData->m_faceIndexCount = &layer->collisionsurfaceindicecounts[threadNumber][0];
		collideDescData->m_faceVertexIndex = &layer->collisionsurfaceindices[threadNumber][0];
		collideDescData->m_vertex = &layer->collisionsurfacevertices[threadNumber][0];
	}

	int VegetationLayerSample::GetInstancesInAABB(const AABB& aabb, std::vector<Mat4>& instances, const float padding)
	{
		int count = 0;
		iVec2 gridmin;
		iVec2 gridmax;
		float r = 0.0;
		Mat4 mat, identity;
		Vec3 scale;
		Vec3 center;
		AABB instanceaabb;
		
		if (model) r = Vec2(model->recursiveaabb.size.x, model->recursiveaabb.size.z).Length() + Vec2(model->recursiveaabb.center.x, model->recursiveaabb.center.z).Length();
		gridmin.x = floor((aabb.min.x - scalerange[1] * r) / density - 0.5);
		gridmin.y = floor((aabb.min.z - scalerange[1] * r) / density - 0.5);
		gridmax.x = ceil((aabb.max.x + scalerange[1] * r) / density + 0.5);
		gridmax.y = ceil((aabb.max.z + scalerange[1] * r) / density + 0.5);
		int x, y;
		for (x = gridmin.x; x <= gridmax.x; x++)
		{
			for (y = gridmin.y; y <= gridmax.y; y++)
			{
				mat = GetInstanceMatrix(x, y);
				instanceaabb = Transform::AABB(model->recursiveaabb, mat, identity, false);
				if (instanceaabb.IntersectsAABB(aabb, padding))
				{
					instanceaabb = Transform::AABB(model->recursiveaabb, mat, identity, true);
					if (instanceaabb.IntersectsAABB(aabb, padding))
					{
						count++;
						instances.push_back(mat);
					}
				}
			}
		}
		return count;
	}

	//Not implemented
	dFloat VegetationLayerSample::RayHitCallback(NewtonUserMeshCollisionRayHitDesc* const lineDescData)
	{
		return 0.0;
	}

	int VegetationLayerSample::AABBTest(void* const userData, const dFloat* const boxP0, const dFloat* const boxP1)
	{
		//Get the vegetation layer object
		VegetationLayerSample* layer = (VegetationLayerSample*)userData;
		if (layer->instanceshape == NULL) return 0;

		//Calculate an AABB
		AABB aabb = AABB(boxP0[0], boxP0[1], boxP0[2], boxP1[0], boxP1[1], boxP1[2]);

		//Retrieve all intersecting instances
		return layer->IntersectsAABB(aabb);
	}

	bool VegetationLayerSample::IntersectsAABB(const AABB& aabb, const float padding)
	{
		iVec2 gridmin;
		iVec2 gridmax;
		float r = 0.0;
		Mat4 mat, identity;
		Vec3 scale;
		Vec3 center;
		AABB instanceaabb;

		if (model) r = Vec2(model->recursiveaabb.size.x, model->recursiveaabb.size.z).Length() + Vec2(model->recursiveaabb.center.x, model->recursiveaabb.center.z).Length();
		gridmin.x = floor((aabb.min.x - scalerange[1] * r) / density - 0.5);
		gridmin.y = floor((aabb.min.z - scalerange[1] * r) / density - 0.5);
		gridmax.x = ceil((aabb.max.x + scalerange[1] * r) / density + 0.5);
		gridmax.y = ceil((aabb.max.z + scalerange[1] * r) / density + 0.5);
		int x, y;
		for (x = gridmin.x; x <= gridmax.x; x++)
		{
			for (y = gridmin.y; y <= gridmax.y; y++)
			{
				mat = GetInstanceMatrix(x, y);
				instanceaabb = Transform::AABB(model->recursiveaabb, mat, identity, false);
				if (instanceaabb.IntersectsAABB(aabb, padding))
				{
					instanceaabb = Transform::AABB(model->recursiveaabb, mat, identity, true);
					if (instanceaabb.IntersectsAABB(aabb, padding))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	//Not implemented
	int VegetationLayerSample::GetFacesInAABB(void* const userData, const dFloat* const p0, const dFloat* const p1,
		const dFloat** const vertexArray, int* const vertexCount, int* const vertexStrideInBytes,
		const int* const indexList, int maxIndexCount, const int* const userDataLis)
	{
		return 0;
	}

	//Not implemented
	Entity* VegetationLayerSample::Copy(const bool recursive, const bool callstartfunction)
	{
		return NULL;
	}

	//Not implemented
	Entity* VegetationLayerSample::Instance(const bool recursive, const bool callstartfunction)
	{
		return NULL;
	}

	bool VegetationLayerSample::BuildShape(const bool dynamic, const bool recursive)
	{
		NewtonDynamicsPhysicsDriver* driver = (NewtonDynamicsPhysicsDriver*)terrain->physicsdriver;
		NewtonDynamicsSimulation* simulation = (NewtonDynamicsSimulation*)terrain->world->simulation;
		float minBox[3];
		minBox[0] = -(terrain->scale.x * terrain->resolution) * 0.5 - model->recursiveaabb.radius * scalerange.y;
		minBox[1] = 0;
		minBox[2] = -(terrain->scale.z * terrain->resolution) * 0.5 - model->recursiveaabb.radius * scalerange.y;
		float maxBox[3];
		maxBox[0] = (terrain->scale.x * terrain->resolution) * 0.5 + model->recursiveaabb.radius * scalerange.y;
		maxBox[1] = terrain->scale.y + model->recursiveaabb.size.y;
		maxBox[2] = (terrain->scale.z * terrain->resolution) * 0.5 + model->recursiveaabb.radius * scalerange.y;
		NewtonDynamicsShape* shape = new NewtonDynamicsShape;
		shape->driver = driver;
		shape->position = Vec3(0);
		shape->rotation = Vec3(0);
		shape->scale = Vec3(1);
		shape->shapeid = SHAPE_CUSTOM;
		shape->collisions.resize(1);
		shape->collisions[0] = NewtonCreateUserMeshCollision(simulation->newtonworld, minBox, maxBox, this, CollideCallback, RayHitCallback, NULL, NULL, AABBTest, GetFacesInAABB, NULL, 0);
		Entity::SetShape(shape);
		shape->Release();
		return true;
	}

	void VegetationLayerSample::RenderBillboard()
	{
		Context* context = Context::GetCurrent();
		Buffer* prevbuffer = Buffer::GetCurrent();
		World* prevworld = World::GetCurrent();
		World::SetCurrent(billboardworld);
		billboardcamera->SetMultisampleMode(0);
		billboardcamera->SetOcclusionCullingMode(false);
		billboardcamera->SetProjectionMode(Camera::Orthographic);
		billboardcamera->Move(0, 0, -2);
		const float rangepadding = 1.0;
		billboardcamera->SetRange(1.0, (model->recursiveaabb.radius + rangepadding) * 2.0);
		float size;

		model->Show();
		billboardworld->Update();

		for (int n = 0; n < model->CountSurfaces(); n++)
		{
			model->surfaces[n]->SetMaterial(billboardmaterial[n]);
		}

		billboardbuffer[0]->Enable();
		context->SetColor(0, 0, 0, 0);
		billboardbuffer[0]->Clear();
		context->SetColor(1, 1, 1, 1);
		
		int mipmaps = Math::Log2(BILLBOARDCELLSIZE) + 1;
		AABB aabb = model->recursiveaabb;
		size = model->recursiveaabb.radius * 2.0;

		//Pre-generate mipmaps
		for (int n = 0; n < 2; n++)
		{
			billboardbuffer[n]->GetColorTexture(0)->BuildMipmaps();
			billboardbuffer[n]->GetColorTexture(1)->BuildMipmaps();
			billboardbuffer[n]->GetColorTexture(2)->BuildMipmaps();
		}

		int w = BILLBOARDCELLSIZE;
		int h = BILLBOARDCELLSIZE;

		//Render sides
		for (int miplevel = 0; miplevel < mipmaps; miplevel++)
		{
			//Bind mipmap level
			billboardbuffer[0]->SetColorTexture(billboardbuffer[0]->GetColorTexture(0), 0, 0, miplevel);
			billboardbuffer[0]->SetColorTexture(billboardbuffer[0]->GetColorTexture(1), 1, 0, miplevel);
			billboardbuffer[0]->SetColorTexture(billboardbuffer[0]->GetColorTexture(2), 2, 0, miplevel);
			
			Buffer* temp = Buffer::Create(w, h, 3, 1, 0);
			float ts = w;
			for (int n = 0; n < BILLBOARDCELLS; n++)
			{
				temp->Enable();
				billboardcamera->SetRotation(0, -(360.0 / (float)BILLBOARDCELLS)*n, 0);
				billboardcamera->SetClearColor(0, 0, 0, 0);
				billboardcamera->SetZoom(ts / size);
				billboardcamera->SetPosition(model->recursiveaabb.center);
				billboardcamera->Move(0, 0, -model->recursiveaabb.radius - rangepadding);
				billboardworld->Render();
				billboardbuffer[0]->Enable();
				shader_copybillboard->Enable();
				billboardcamera->gbuffer->colorcomponent[0]->Bind(0);
				billboardcamera->gbuffer->colorcomponent[1]->Bind(1);
				billboardcamera->gbuffer->colorcomponent[2]->Bind(2);
				context->DrawRect(n * w, 0, w, h);
			}
			temp->Release();
			w /= 2;
			h /= 2;
		}

		//Render top
		size = max(model->recursiveaabb.size.x, model->recursiveaabb.size.z);
		w = BILLBOARDCELLSIZE;
		h = BILLBOARDCELLSIZE;

		for (int miplevel = 0; miplevel < mipmaps; miplevel++)
		{
			//Bind mipmap level
			billboardbuffer[1]->SetColorTexture(billboardbuffer[1]->GetColorTexture(0), 0, 0, miplevel);
			billboardbuffer[1]->SetColorTexture(billboardbuffer[1]->GetColorTexture(1), 1, 0, miplevel);
			billboardbuffer[1]->SetColorTexture(billboardbuffer[1]->GetColorTexture(2), 2, 0, miplevel);
			billboardbuffer[1]->Enable();
			float ts = w;
			billboardcamera->SetRotation(90,0,0);
			billboardcamera->SetClearColor(0, 0, 0, 0);
			billboardcamera->SetZoom(ts / size);
			billboardcamera->SetPosition(model->recursiveaabb.center);
			billboardcamera->Move(0, 0, -model->recursiveaabb.radius - rangepadding);
			billboardworld->Render();
			billboardbuffer[1]->Enable();
			Shader::Load("Shaders/Vegetation/copybillboard.shader")->Enable();
			billboardcamera->gbuffer->colorcomponent[0]->Bind(0);
			billboardcamera->gbuffer->colorcomponent[1]->Bind(1);
			billboardcamera->gbuffer->colorcomponent[2]->Bind(2);
			context->DrawRect(0, 0, w, w);
			w /= 2;
			h /= 2;
		}

		for (int n = 0; n < model->CountSurfaces(); n++)
		{
			model->surfaces[n]->SetMaterial(NULL);
		}
		model->Hide();

		World::SetCurrent(prevworld);
		Buffer::SetCurrent(prevbuffer);
	}

	void VegetationLayerSample::SetDensity(const float density)
	{
		this->density = density;
		variationmatrices.resize(0);
	}

	bool VegetationLayerSample::SetModel(const std::string& path)
	{
		Model* model = Model::Load(path);
		if (model)
		{
			SetModel(model);
			model->Release();
			return true;
		}
		else
		{
			return false;
		}
	}

	void VegetationLayerSample::SetModel(Model* model)
	{
		billboardmaterial.clear();
		modelmaterial.clear();
		if (this->model) this->model->Release();
		World* prevworld = World::GetCurrent();
		World::SetCurrent(billboardworld);
		this->model = Model::Create();

		model->Collapse(this->model);
		for (int i = 0; i < this->model->CountSurfaces(); i++)
		{
			Surface* surf = this->model->GetSurface(i);
			Material* mtl = NULL;
			mtl = surf->GetMaterial();
			if (mtl == NULL)
			{
				mtl = Material::Create();
			}
			else
			{
				mtl = (Material*)mtl->Copy();
			}
			billboardmaterial.push_back(mtl);
			mtl = surf->GetMaterial();
			if (mtl == NULL)
			{
				mtl = Material::Create();
			}
			else
			{
				mtl = (Material*)mtl->Copy();
			}
			mtl->SetShader("Shaders/Vegetation/diffuse.shader");
			modelmaterial.push_back(mtl);
		}
		this->model->UpdateAABB(Entity::LocalAABB | Entity::GlobalAABB | Entity::RecursiveAABB);
		this->model->UpdateOctreeNode();

		RenderBillboard();

		this->model->Hide();
		World::SetCurrent(prevworld);
		if (model->shape != instanceshape)
		{
			if (instanceshape) instanceshape->Release();
			instanceshape = (NewtonDynamicsShape*)model->shape;
			if (instanceshape)
			{				
				instanceshape->AddRef();

				Vec3 p;
				int n;
				Surface* collisionsurface = instanceshape->GetSurface(0);
				int tris_count = collisionsurface->CountTriangles();				
				int vert_count = collisionsurface->CountVertices();

				//Pre-build vertex position array for collision
				vertexpositions.resize(vert_count * 3);
				for (n = 0; n < vert_count; n++)
				{
					p = collisionsurface->GetVertexPosition(n);
					vertexpositions[n * 3 + 0] = p.x;
					vertexpositions[n * 3 + 1] = p.y;
					vertexpositions[n * 3 + 2] = p.z;
				}

				//Pre-build face normals array for collision
				facenormals.resize(tris_count*3);
				for (n = 0; n < tris_count; n++)
				{
					p = collisionsurface->GetTriangleNormal(n);
					facenormals[n * 3 + 0] = p.x;
					facenormals[n * 3 + 1] = p.y;
					facenormals[n * 3 + 2] = p.z;
				}

				//Pre-build indice array for collision
				indices.resize(tris_count * 9);
				for (n = 0; n < tris_count; n++)
				{
					//Triangle vertex
					indices[n * 9 + 0] = collisionsurface->GetTriangleVertex(n, 0);
					indices[n * 9 + 1] = collisionsurface->GetTriangleVertex(n, 1);
					indices[n * 9 + 2] = collisionsurface->GetTriangleVertex(n, 2);

					//Face attribute
					indices[n * 9 + 3] = 0;

					//Face normal
					indices[n * 9 + 4] = collisionsurface->CountVertices() + n;

					// face adjacent index (infinite plane does not have shared edge with other faces)
					indices[n * 9 + 5] = indices[n * 9 + 4];
					indices[n * 9 + 6] = indices[n * 9 + 4];
					indices[n * 9 + 7] = indices[n * 9 + 4];

					// face area (the plane is clipped around the box, the face size is always optimal)
					indices[n * 9 + 8] = 1;
				}
			}
			else
			{
				indices.resize(0);
				facenormals.resize(0);
			}
		}
		if (model) BuildShape();
	}

	void VegetationLayerSample::BuildVariationMatrices()
	{
		int x, z;
		Mat4 mat;
		Vec3 rotation;
		variationmatrices.resize(variationmapresolution * variationmapresolution * 16);
		srand(seed);
		for (x = 0; x < variationmapresolution; x++)
		{
			for (z = 0; z < variationmapresolution; z++)
			{
				mat = Mat4();
				float angle = Math::Random(0.0, 360.0);
				mat.MakeRotation(0, angle, 0);
				mat[3][0] += Math::Random(-density, density)*0.5;
				mat[3][2] += Math::Random(-density, density)*0.5;
				float scale = Math::Random(0.0, 1.0);
				float brightness = Math::Random();
				mat[0][3] = brightness;
				mat[1][3] = brightness;
				mat[2][3] = brightness;
				mat[3][3] = scale;
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 0] = mat[0][0];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 1] = mat[0][1];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 2] = mat[0][2];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 3] = mat[0][3];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 4] = mat[1][0];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 5] = mat[1][1];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 6] = mat[1][2];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 7] = mat[1][3];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 8] = mat[2][0];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 9] = mat[2][1];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 10] = mat[2][2];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 11] = mat[2][3];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 12] = mat[3][0];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 13] = mat[3][1];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 14] = mat[3][2];
				variationmatrices[z * variationmapresolution * 16 + x * 16 + 15] = mat[3][3];
			}
		}
		if (variationmap)
		{
			if (variationmap->GetWidth() != variationmapresolution*16)
			{
				variationmap->Release();
				variationmap = NULL;
			}
		}
		if (variationmap == NULL)
		{
			variationmap = Texture::Create(variationmapresolution * 16, variationmapresolution, Texture::Intensity32);
			variationmap->SetFilter(Texture::Pixel);
		}
		variationmap->SetPixels((const char*)&variationmatrices[0]);
	}

	void VegetationLayerSample::PrePass(Camera* camera)
	{
		if (model == NULL) return;

		Context* context = Context::GetCurrent();

		if (variationmatrices.size()==0) BuildVariationMatrices();
		variationmap->Bind(5);

		if (modelocclusionquery[camera][context] == 0)
		{
			OpenGL4VertexArray* va = (OpenGL4VertexArray*)(GraphicsDriver::GetCurrent()->CreateVertexArray(SURFACE_TEXCOORDS1,GMF_FLOAT,16));
			glGenQueries(1, &billboardocclusionquery[camera][context]);
			glGenQueries(1, &modelocclusionquery[camera][context]);
			glGenBuffers(1, &billboardfeedbackbuffer[camera]);
			glGenBuffers(1, &modelfeedbackbuffer[camera]);
			glBindBuffer(GL_TEXTURE_BUFFER, billboardfeedbackbuffer[camera]);
			glBufferData(GL_TEXTURE_BUFFER, maxinstancesdrawn * 4, NULL, GL_DYNAMIC_COPY);
			glBindBuffer(GL_TEXTURE_BUFFER, modelfeedbackbuffer[camera]);
			glBufferData(GL_TEXTURE_BUFFER, maxinstancesdrawn * 4, NULL, GL_DYNAMIC_COPY);
			glBindBuffer(GL_TEXTURE_BUFFER, 0);
		}

		Mat4 mat;
		Vec4 color = Vec4(1.0);
		Vec3 campos = camera->GetPosition(true);
		int count = pow((terrain->scale.x * terrain->resolution / density), 2.0);
		
		Camera* cam = camera;
		if (camera->classid == DirectionalLightClass) cam = ((DirectionalLight*)camera)->currentrendercamera;

		visiblegridsize = ceil(viewrange * 2.0 / density);
		gridoffset[camera].x = Math::Round(cam->position.x / density - visiblegridsize / 2.0);
		gridoffset[camera].y = Math::Round(cam->position.z / density - visiblegridsize / 2.0);
		count = visiblegridsize*visiblegridsize;

		instancesurface->Enable();
		shader_frustumcull->Enable();
		for (int n = 0; n < 6; n++)
		{
			shader_frustumcull->SetVec4("frustumplane" + String(n), Vec4(camera->frustum->face[n]->plane.x, camera->frustum->face[n]->plane.y, camera->frustum->face[n]->plane.z, camera->frustum->face[n]->plane.d));
		}
		shader_frustumcull->SetVec3("aabbmin", model->localaabb.min);
		shader_frustumcull->SetVec3("aabbmax", model->localaabb.max);
		shader_frustumcull->SetVec2("scalerange", scalerange);
		shader_frustumcull->SetVec3("aabboffset", model->recursiveaabb.center);
		shader_frustumcull->SetFloat("TerrainSize", terrain->resolution * terrain->scale.x);
		shader_frustumcull->SetFloat("TerrainHeight", terrain->scale.y);
		shader_frustumcull->SetFloat("TerrainResolution", terrain->resolution);
		shader_frustumcull->SetFloat("CellResolution", visiblegridsize);
		shader_frustumcull->SetFloat("Density", density);
		shader_frustumcull->SetFloat("BillboardDistance", billboarddistance);
		shader_frustumcull->SetVec2("InstanceOffset", Vec2(0));
		shader_frustumcull->SetInt("VBOSize", instancesurface->CountVertices());
		shader_frustumcull->SetInt("NumInstances", count);
		shader_frustumcull->SetVec2("sloperange", sloperange);
		shader_frustumcull->SetVec2("heightrange", heightrange);
		shader_frustumcull->SetVec2("gridoffset",gridoffset[camera]);
		shader_frustumcull->SetInt("density", density);
		shader_frustumcull->SetFloat("variationmapresolution", variationmapresolution);

		//Bind textures
		terrain->heightmap->SetFilter(Texture::Smooth);
		terrain->heightmap->Bind(6);
		terrain->normalmap->SetFilter(Texture::Smooth);
		terrain->normalmap->Bind(7);
		variationmap->Bind(5);
		
		int passsize = instancesurface->CountVertices();
		int passes = ceil((float)count / (float)passsize);

		terrain->world->stats_vegetationculling += count;

		glEnable(GL_RASTERIZER_DISCARD);

		for (int n = 0; n < 2; n++)
		{
			switch (n)
			{
			case 0:
				//Model pass				
				if (camera->classid == DirectionalLightClass)
				{
					shader_frustumcull->SetVec3("cameraposition", ((DirectionalLight*)camera)->currentrendercamera->GetPosition(true));
				}
				else
				{
					shader_frustumcull->SetVec3("cameraposition", camera->GetPosition(true));					
				}
				shader_frustumcull->SetVec2("viewrange", Vec2(0, billboarddistance + billboardtransition*2.0 + model->recursiveaabb.radius * scalerange.y));
				break;
			case 1:
				//Billboard pass
				if (camera->classid == DirectionalLightClass)
				{
					shader_frustumcull->SetVec3("cameraposition", ((DirectionalLight*)camera)->currentrendercamera->GetPosition(true));					
				}
				else
				{
					shader_frustumcull->SetVec3("cameraposition", camera->GetPosition(true));
				}
				shader_frustumcull->SetVec2("viewrange", Vec2(billboarddistance, viewrange));
				break;
			}
			switch (n)
			{
            case 0:
			    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, modelfeedbackbuffer[camera]);
			    break;
            case 1:
                glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, billboardfeedbackbuffer[camera]);
			    break;
			}
			glBeginTransformFeedback(GL_POINTS);

			switch (n)
			{
			case 0:
                glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, modelocclusionquery[camera][context]);
				break;
			case 1:
                glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, billboardocclusionquery[camera][context]);
				break;
			}

			instancesurface->Draw(passes);

			glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
			glEndTransformFeedback();

			if (!billboardsenabled) break;
		}

		instancesurface->Disable();
		glDisable(GL_RASTERIZER_DISCARD);
		glCheckError();
	}

	void VegetationLayerSample::SetBillboardDistance(const float distance)
	{
		billboarddistance = distance;
	}

	void VegetationLayerSample::SetViewRange(const float range)
	{
		viewrange = range;
	}

	void VegetationLayerSample::SetSlopeConstraints(const float minslope, const float maxslope)
	{
		sloperange = Vec2(minslope, maxslope);
	}

	void VegetationLayerSample::SetHeightConstraints(const float minheight, const float maxheight)
	{
		heightrange = Vec2(minheight, maxheight);
	}

	void VegetationLayerSample::Draw(Camera* camera)
	{
		if (model == NULL) return;

		Context* context = Context::GetCurrent();

		if (modelocclusionquery[camera][context] == 0 || billboardocclusionquery[camera][context] == 0) return;

		//Get the query results
		GLuint result[2] = {0,0};
		int available;
		for (int n = 0; n < 2; n++)
		{
		    switch (n)
		    {
            case 0:
                glGetQueryObjectuiv(modelocclusionquery[camera][context], GL_QUERY_RESULT, &result[n]);
                break;
            case 1:
				glGetQueryObjectuiv(billboardocclusionquery[camera][context], GL_QUERY_RESULT, &result[n]);
                break;
		    }
			result[n] = min(result[n], maxinstancesdrawn);
		}
		if (result[0] == 0 && result[1]==0) return;

		//Bind textures
		terrain->heightmap->SetFilter(Texture::Smooth);
		variationmap->Bind(5);
		terrain->heightmap->Bind(6);
		terrain->normalmap->Bind(7);
		
		//Model Pass
		if (result[0] > 0)
		{
			terrain->world->stats_vegetationinstances += result[0];

			for (int n = 0; n < model->CountSurfaces(); n++)
			{
				if (camera->islight)
				{
					terrain->world->stats_shadowpolysdrawn += model->surfaces[n]->CountTriangles() * result[0];
				}
				else
				{
					terrain->world->stats_polysdrawn += model->surfaces[n]->CountTriangles() * result[0];
				}

				model->surfaces[n]->Enable();
				modelmaterial[n]->Enable();
                glBindBuffer(GL_ARRAY_BUFFER, modelfeedbackbuffer[camera]);
				glEnableVertexAttribArray(SURFACE_TEXCOORDS1);
				glVertexAttribIPointer(SURFACE_TEXCOORDS1, 1, GL_UNSIGNED_INT, 0, NULL);
				glVertexAttribDivisorARB(SURFACE_TEXCOORDS1, 1);

				modelmaterial[n]->shader[0]->SetVec2("colorrange", colorrange);
				modelmaterial[n]->shader[0]->SetVec2("scalerange", scalerange);
				modelmaterial[n]->shader[0]->SetVec2("faderange", Vec2(billboarddistance + billboardtransition, billboarddistance + billboardtransition*2.0));
				modelmaterial[n]->shader[0]->SetFloat("TerrainHeight", terrain->scale.y);
				modelmaterial[n]->shader[0]->SetFloat("TerrainResolution", terrain->resolution);
				modelmaterial[n]->shader[0]->SetFloat("TerrainSize", terrain->resolution * terrain->scale.x);
				modelmaterial[n]->shader[0]->SetFloat("CellResolution", visiblegridsize);
				modelmaterial[n]->shader[0]->SetVec2("gridoffset", gridoffset[camera]);
				modelmaterial[n]->shader[0]->SetFloat("Density", density);
				modelmaterial[n]->shader[0]->SetVec2("InstanceOffset", Vec2(0));
				modelmaterial[n]->shader[0]->SetInt("density", density);
				modelmaterial[n]->shader[0]->SetFloat("variationmapresolution", variationmapresolution);

				if (camera->GetClass()==DirectionalLightClass)
				{
					modelmaterial[n]->shader[0]->SetVec3("cameraposition", ((DirectionalLight*)camera)->currentrendercamera->GetPosition(true));
				}
				else
				{
					modelmaterial[n]->shader[0]->SetVec3("cameraposition", camera->GetPosition(true));
				}
				model->surfaces[n]->Draw(result[0]);
				model->surfaces[n]->Disable();
				modelmaterial[n]->Disable();
				glVertexAttribDivisorARB(SURFACE_TEXCOORDS1, 0);
				glDisableVertexAttribArray(SURFACE_TEXCOORDS1);
			}
		}

		//Billboard pass
		if (result[1] > 0 && billboardsenabled)
		{
			if (camera->islight)
			{
				terrain->world->stats_shadowpolysdrawn += 2 * result[1];
			}
			else
			{
				terrain->world->stats_polysdrawn += 2 * result[1];
			}

			terrain->world->stats_vegetationinstances += result[1];
			billboardsurface->Enable();
			billboardbuffer[0]->colorcomponent[0]->Bind(0);
			billboardbuffer[0]->colorcomponent[1]->Bind(1);
			billboardbuffer[0]->colorcomponent[2]->Bind(2);
			billboardbuffer[1]->colorcomponent[0]->Bind(9);
			billboardbuffer[1]->colorcomponent[1]->Bind(10);
			billboardbuffer[1]->colorcomponent[2]->Bind(11);
			
			glBindBuffer(GL_ARRAY_BUFFER, billboardfeedbackbuffer[camera]);
			glEnableVertexAttribArray(SURFACE_TEXCOORDS1);
			glVertexAttribIPointer(SURFACE_TEXCOORDS1, 1, GL_UNSIGNED_INT, 0, NULL);
			glVertexAttribDivisorARB(SURFACE_TEXCOORDS1, 1);

			float size = model->recursiveaabb.radius * 2.0;
			float hsize = max(model->recursiveaabb.size.x, model->recursiveaabb.size.z);
			billboardsurface->material->shader[0]->SetVec2("colorrange", colorrange);
			billboardsurface->material->shader[0]->SetVec2("scalerange", scalerange);
			billboardsurface->material->shader[0]->SetVec2("faderange", Vec2(billboarddistance, billboarddistance + billboardtransition));
			billboardsurface->material->shader[0]->SetFloat("viewrange", viewrange);
			billboardsurface->material->shader[0]->SetFloat("billboardviews", BILLBOARDCELLS);
			billboardsurface->material->shader[0]->SetVec3("aabboffset", model->recursiveaabb.center);
			billboardsurface->material->shader[0]->SetFloat("billboardvscale", hsize);
			billboardsurface->material->shader[0]->SetVec3("billboardscale", Vec3(size));
			billboardsurface->material->shader[0]->SetVec3("positionoffset",model->recursiveaabb.center);
			billboardsurface->material->shader[0]->SetFloat("TerrainHeight", terrain->scale.y);
			billboardsurface->material->shader[0]->SetFloat("TerrainResolution", terrain->resolution);
			billboardsurface->material->shader[0]->SetFloat("TerrainSize", terrain->resolution * terrain->scale.x);
			billboardsurface->material->shader[0]->SetFloat("CellResolution", visiblegridsize);
			billboardsurface->material->shader[0]->SetFloat("Density", density);
			billboardsurface->material->shader[0]->SetFloat("BillboardDistance", billboarddistance);
			billboardsurface->material->shader[0]->SetVec2("InstanceOffset", Vec2(0));
			billboardsurface->material->shader[0]->SetInt("cameraprojectionmode", camera->projectionmode);
			billboardsurface->material->shader[0]->SetVec2("gridoffset", gridoffset[camera]);
			billboardsurface->material->shader[0]->SetFloat("variationmapresolution", variationmapresolution);

			if (camera->GetClass() == DirectionalLightClass)
			{
				billboardsurface->material->shader[0]->SetVec3("cameraposition", ((DirectionalLight*)camera)->currentrendercamera->GetPosition(true));
			}
			else
			{
				billboardsurface->material->shader[0]->SetVec3("cameraposition", camera->GetPosition(true));
			}

			billboardsurface->Draw(result[1]);

			billboardsurface->Disable();
			glVertexAttribDivisorARB(SURFACE_TEXCOORDS1, 0);
			glDisableVertexAttribArray(SURFACE_TEXCOORDS1);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VegetationLayerSample::SetShadowMode(const int shadowmode)
	{
		this->shadowmode = shadowmode;
	}

	int VegetationLayerSample::GetShadowMode()
	{
		return shadowmode;
	}

	Mat4 VegetationLayerSample::GetInstanceMatrix(const int x, const int z)
	{
		Mat4 mat;
		int ix = Math::Mod(x, variationmapresolution);
		int iz = Math::Mod(z + variationmapresolution / 2, variationmapresolution);
		int offset = (iz * variationmapresolution + ix) * 16;
		if (variationmatrices.size() == 0) BuildVariationMatrices();
		mat[0][0] = variationmatrices[offset + 0];
		mat[0][1] = variationmatrices[offset + 1];
		mat[0][2] = variationmatrices[offset + 2];
		mat[0][3] = 0.0;
		mat[1][0] = variationmatrices[offset + 4];
		mat[1][1] = variationmatrices[offset + 5];
		mat[1][2] = variationmatrices[offset + 6];
		mat[1][3] = 0.0;
		mat[2][0] = variationmatrices[offset + 8];
		mat[2][1] = variationmatrices[offset + 9];
		mat[2][2] = variationmatrices[offset + 10];
		mat[2][3] = 0.0;
		mat[3][0] = variationmatrices[offset + 12] + x * density;
		mat[3][2] = variationmatrices[offset + 14] + z * density;
		mat[3][1] = terrain->GetElevation(mat[3][0], mat[3][2]);
		mat[3][3] = 1.0;
		float scale = scalerange.x + variationmatrices[offset + 15] * (scalerange.y - scalerange.x);
		mat[0] *= scale;
		mat[1] *= scale;
		mat[2] *= scale;
		return mat;
	}

	Vec4 VegetationLayerSample::GetInstanceColor(const int x, const int z)
	{
		Vec4 color;
		int ix = Math::Mod(x, variationmapresolution);
		int iz = Math::Mod(z + variationmapresolution / 2, variationmapresolution);
		int offset = (iz * variationmapresolution + ix) * 64;
		if (variationmatrices.size()==0) BuildVariationMatrices();
		color.r = variationmatrices[offset + 3];
		color.g = variationmatrices[offset + 7];
		color.b = variationmatrices[offset + 11];
		color.a = variationmatrices[offset + 15];
		return color;
	}

	void VegetationLayerSample::SetScale(const float minscale, const float maxscale)
	{
		scalerange.x = minscale;
		scalerange.y = maxscale;
		if (model) BuildShape();
	}

	void VegetationLayerSample::SetColor(const float minbrightness, const float maxbrightness)
	{
		colorrange.x = minbrightness;
		colorrange.y = maxbrightness;
	}
}