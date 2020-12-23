 

#ifndef C4Paths_h
#define C4Paths_h


#include "C4Markers.h"


namespace C4
{
	typedef Type	PathType;


	enum : PathType
	{
		kPathLinear			= 'LINE',
		kPathElliptical		= 'ELLP',
		kPathBezier			= 'BEZR'
	};


	C4API extern const char kConnectorKeyPath[];


	class PathComponent : public ListElement<PathComponent>, public Packable
	{
		friend class Path;

		private:

			PathType		pathType;

			virtual PathComponent *Replicate(void) const = 0;

			static PathComponent *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			PathComponent(PathType type);
			PathComponent(const PathComponent& pathComponent);

		public:

			virtual ~PathComponent();

			PathType GetPathType(void) const
			{
				return (pathType);
			}

			PathComponent *Clone(void) const
			{
				return (Replicate());
			}

			void PackType(Packer& data) const override;

			virtual Point3D GetPosition(float t) const = 0;
			virtual Vector3D GetTangent(float t) const = 0;
			virtual const Point3D& GetBeginPosition(void) const = 0;
			virtual Vector3D GetBeginTangent(void) const = 0;
			virtual const Point3D& GetEndPosition(void) const = 0;
			virtual Vector3D GetEndTangent(void) const = 0;

			virtual int32 GetControlPointCount(void) const = 0;
			virtual void GetBoundingBox(Box3D *box) const = 0;
	};


	class LinearPathComponent final : public PathComponent
	{
		friend class PathComponent;

		private:

			Point3D		controlPoint[2];

			LinearPathComponent();
			LinearPathComponent(const LinearPathComponent& linearPathComponent);

			PathComponent *Replicate(void) const override;

		public:

			C4API LinearPathComponent(const Point3D& p1, const Point3D& p2);
			C4API ~LinearPathComponent();

			const Point3D& GetControlPoint(int32 index) const
			{
				return (controlPoint[index]);
			}

			void SetControlPoint(int32 index, const Point3D& p)
			{
				controlPoint[index] = p;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API Point3D GetPosition(float t) const override; 
			C4API Vector3D GetTangent(float t) const override;
			C4API const Point3D& GetBeginPosition(void) const override;
			C4API Vector3D GetBeginTangent(void) const override; 
			C4API const Point3D& GetEndPosition(void) const override;
			C4API Vector3D GetEndTangent(void) const override; 

			C4API int32 GetControlPointCount(void) const override;
			C4API void GetBoundingBox(Box3D *box) const override; 
	};
 
 
	class EllipticalPathComponent final : public PathComponent
	{
		friend class PathComponent;
 
		private:

			Point3D		controlPoint[3];

			EllipticalPathComponent();
			EllipticalPathComponent(const EllipticalPathComponent& ellipticalPathComponent);

			PathComponent *Replicate(void) const override;

		public:

			C4API EllipticalPathComponent(const Point3D& p1, const Point3D& p2, const Point3D& p3);
			C4API ~EllipticalPathComponent();

			const Point3D& GetControlPoint(int32 index) const
			{
				return (controlPoint[index]);
			}

			void SetControlPoint(int32 index, const Point3D& p)
			{
				controlPoint[index] = p;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API Point3D GetPosition(float t) const override;
			C4API Vector3D GetTangent(float t) const override;
			C4API const Point3D& GetBeginPosition(void) const override;
			C4API Vector3D GetBeginTangent(void) const override;
			C4API const Point3D& GetEndPosition(void) const override;
			C4API Vector3D GetEndTangent(void) const override;

			C4API int32 GetControlPointCount(void) const override;
			C4API void GetBoundingBox(Box3D *box) const override;
	};


	class BezierPathComponent final : public PathComponent
	{
		friend class PathComponent;

		private:

			Point3D		controlPoint[4];

			BezierPathComponent();
			BezierPathComponent(const BezierPathComponent& bezierPathComponent);

			PathComponent *Replicate(void) const override;

		public:

			C4API BezierPathComponent(const Point3D& p1, const Point3D& p2, const Point3D& p3, const Point3D& p4);
			C4API ~BezierPathComponent();

			const Point3D& GetControlPoint(int32 index) const
			{
				return (controlPoint[index]);
			}

			void SetControlPoint(int32 index, const Point3D& p)
			{
				controlPoint[index] = p;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API Point3D GetPosition(float t) const override;
			C4API Vector3D GetTangent(float t) const override;
			C4API const Point3D& GetBeginPosition(void) const override;
			C4API Vector3D GetBeginTangent(void) const override;
			C4API const Point3D& GetEndPosition(void) const override;
			C4API Vector3D GetEndTangent(void) const override;

			C4API int32 GetControlPointCount(void) const override;
			C4API void GetBoundingBox(Box3D *box) const override;
	};


	class Path : public Packable
	{
		friend class PathMarker;

		private:

			List<PathComponent>		pathComponentList;
			Vector3D				pathNormal;

		public:

			C4API Path();
			C4API Path(const Vector3D& normal);
			C4API Path(const Path& path);
			C4API ~Path();

			PathComponent *GetFirstPathComponent(void) const
			{
				return (pathComponentList.First());
			}

			PathComponent *GetLastPathComponent(void) const
			{
				return (pathComponentList.Last());
			}

			void AppendPathComponent(PathComponent *component)
			{
				pathComponentList.Append(component);
			}

			void PrependPathComponent(PathComponent *component)
			{
				pathComponentList.Prepend(component);
			}

			int32 GetPathComponentCount(void) const
			{
				return (pathComponentList.GetElementCount());
			}

			const Vector3D& GetPathNormal(void) const
			{
				return (pathNormal);
			}

			void SetPathNormal(const Vector3D& normal)
			{
				pathNormal = normal;
			}

			C4API Path& operator =(const Path& path);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			C4API Point3D GetPathState(float t, Vector3D *tangent = nullptr, Vector3D *binormal = nullptr) const;

			C4API void GetBoundingBox(Box3D *box) const;
			C4API bool LinearPath(void) const;
	};


	class PathMarker : public Marker
	{
		friend class Marker;

		private:

			Path		markerPath;

			Node *Replicate(void) const override;

		protected:

			C4API PathMarker();
			C4API PathMarker(const PathMarker& pathMarker);

		public:

			C4API PathMarker(const Vector3D& normal);
			C4API ~PathMarker();

			Path *GetPath(void)
			{
				return (&markerPath);
			}

			const Path *GetPath(void) const
			{
				return (&markerPath);
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			C4API bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			C4API void *BeginSettingsUnpack(void) override;
			C4API void EndSettingsUnpack(void *cookie) override;
	};
}


#endif

// ZYUQURM
