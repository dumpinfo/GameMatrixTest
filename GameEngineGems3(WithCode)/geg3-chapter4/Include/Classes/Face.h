#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Edge;
	class GraphicsDriver;
	class Material;
	class ConvexHull;
	class Texture;
	
	class Face : public Object
	{
	public:
		ConvexHull* convexhull;
		Plane plane;
		vector<int> indices;
		vector<Vec2> lmcoords;
		Surface* surface;
		vector<int> vertex;
		Plane mappingaxis[2];
		Plane simplemappingaxis[2];
		Plane unscaledrotatedmappingaxis[2];
		Vec2 mappingtranslation;
		Vec2 mappingscale;
		Vec2 mappingrotation;
		int visiblesurfaceindex;
		int visiblesurfacevertexindice;
		int smoothgroups;
		int solidsurfacevertexbeginindice;
		bool visible;
		bool texcoordsinvalidated;
		bool planeinvalidated;
		Material* material;
		bool selectionstate;
		Texture* lightmap[2];
		//Mat4 mappingmatrix;
		bool evermapped;
		//Mat4 simplemappingmatrix;
		Plane rotatedmappingaxis[2];
		//Mat4 mappingrotationmatrix;
		Vec2 materialmappingscale;

		Face();
		virtual ~Face();
		
		virtual Texture* GetLightmap(const int index);
		virtual void SetLightmap(Texture* texture, const int index);
		virtual Vec2 GetVertexTexCoords(const int vert, const int texcoordset=0);
		virtual void SetVertexTexCoords(const int v, const float x, const float y, const int texcoordset=0);
		virtual void SetSelectionState(const bool selectionstate);
		virtual bool GetSelectionState();
		virtual bool Hidden();
		virtual void Hide();
		virtual void Show();
		virtual void SetVertexNormal(const int v, const float x, const float y, const float z);
		//virtual void BuildNormals();
		virtual void SetSmoothGroups(const int smoothgroups);
		virtual int GetSmoothGroups();
		virtual Face* Copy();
		virtual void SetMaterial(Material* material);
		virtual Material* GetMaterial();
		virtual void FixErrors();
		virtual void BuildTexCoords(const Mat4& mat);
		virtual void Invert();
		virtual void BuildSurface(const Mat4& mat);
		virtual void BuildPlane();
		virtual int ContainsVertex(const int v);
		virtual int CountIndices();
		virtual int AddIndice(const int v);
		virtual int GetIndiceVertex(const int i);
		virtual void SetTextureMappingTranslation(const float x, const float y);
		virtual void SetTextureMappingRotation(const float x, const float y);
		virtual void SetTextureMappingScale(const float x, const float y);
		virtual Vec2 GetTextureMappingTranslation();
		virtual Vec2 GetTextureMappingRotation();
		virtual Vec2 GetTextureMappingScale();
		virtual void SetTextureMappingPlane(const Plane& plane, const int index, const int mode);
		virtual Plane GetTextureMappingPlane(const int index, const int mode);
		virtual int GetIndex();
	};
}
