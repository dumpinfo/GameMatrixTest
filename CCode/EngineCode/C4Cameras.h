 

#ifndef C4Cameras_h
#define C4Cameras_h


//# \component	World Manager
//# \prefix		WorldMgr/

//# \import		C4CameraObjects.h


#include "C4CameraObjects.h"
#include "C4Portals.h"
#include "C4Regions.h"
#include "C4Time.h"


namespace C4
{
	enum : CameraType
	{
		kCameraCube		= 'CUBE'
	};


	enum
	{
		kFacePositiveX,
		kFaceNegativeX,
		kFacePositiveY,
		kFaceNegativeY,
		kFacePositiveZ,
		kFaceNegativeZ
	};


	enum
	{
		kSpectatorMoveForward		= 1 << 0,
		kSpectatorMoveBackward		= 1 << 1,
		kSpectatorMoveLeft			= 1 << 2,
		kSpectatorMoveRight			= 1 << 3,
		kSpectatorMoveUp			= 1 << 4,
		kSpectatorMoveDown			= 1 << 5,
		kSpectatorSlide1			= 1 << 8,
		kSpectatorSlide2			= 1 << 9,

		kSpectatorPlanarMask		= kSpectatorMoveForward | kSpectatorMoveBackward |
									  kSpectatorMoveLeft | kSpectatorMoveRight,

		kSpectatorMoveMask			= kSpectatorMoveForward | kSpectatorMoveBackward |
									  kSpectatorMoveLeft | kSpectatorMoveRight |
									  kSpectatorMoveUp | kSpectatorMoveDown
	};


	class Shaker;


	//# \class	Camera		Represents a camera node in a world.
	//
	//# The $Camera$ class represents a camera node in a world.
	//
	//# \def	class Camera : public Node
	//
	//# \ctor	Camera(CameraType type);
	//
	//# The constructor has protected access. A $Camera$ class can only exist as the base class for a more specific type of camera.
	//
	//# \param	type	The type of the camera. See below for a list of possible types.
	//
	//# \desc
	//# The $Camera$ class is the base class for all camera nodes. The $type$ parameter can be one of the following
	//# constants, and it is specified by the subclass's constructor.
	//
	//# \table	CameraType
	//
	//# \base	Node	A $Camera$ node is a scene graph node.
	//
	//# \also	$@GraphicsMgr/CameraObject@$
	//
	//# \wiki	Cameras


	//# \function	Camera::GetCameraType		Returns the camera type.
	//
	//# \proto	CameraType GetCameraType(void) const;
	//
	//# \desc
	//# The $GetCameraType$ function returns the camera type, which can be one of the following constants or may
	//# be a type defined by a derived class.
	//
	//# \table	CameraType
	//
	//# \also	$@GraphicsMgr/CameraObject@$


	//# \function	Camera::LookAtPoint		Orients the camera so that it looks at a given point.
	//
	//# \proto	void LookAtPoint(const Point3D& point);
	//# \proto	void LookAtPoint(const Point3D& point, const Vector3D& up);
	// 
	//# \param	point		The point at which to look.
	//# \param	up			The general up direction. This must be unit length.
	// 
	//# \desc
	//# The $LookAtPoint$ function modifies a camera's node transform so that the camera looks directly 
	//# toward the point given by the $point$ parameter. The $up$ parameter specifies the general up
	//# direction for the camera and defines the vertical plane containing both the camera and the
	//# point being looked at. If the $point$ parameter coincides with the camera position, or the 
	//# $up$ parameter is parallel to the difference between the $point$ parameter and the camera
	//# position, then the resulting node transform will be singular and subsequent rendering through 
	//# the camera is undefined. 
	//#
	//# The point being looked at should be specified in the camera's local coordinate space. Ordinarily,
	//# a camera resides in the infinite zone at the root of a scene, so the camera's local space is
	//# world space. In the case that the camera is a subnode of another node having a non-identity 
	//# world transform, the $point$ and $up$ parameters must be transformed into the camera's local
	//# space using the parent node's inverse world transform.
	//#
	//# If the $up$ parameter is omitted, then the direction (0,0,1) is used as the up direction.
	//
	//# \also	$@Camera::LookInDirection@$
	//# \also	$@Node::SetNodeTransform@$


	//# \function	Camera::LookInDirection		Orients the camera so that it looks in a given direction.
	//
	//# \proto	void LookInDirection(const Vector3D& direction);
	//# \proto	void LookInDirection(const Vector3D& direction, const Vector3D& up);
	//
	//# \param	direction	The direction in which to look. This must be unit length.
	//# \param	up			The general up direction. This must be unit length.
	//
	//# \desc
	//# The $LookInDirection$ function modifies a camera's node transform so that the camera looks in
	//# the direction given by the $direction$ parameter. The $up$ parameter specifies the general up
	//# direction for the camera and defines the vertical plane containing both the direction vector.
	//# If the $up$ parameter is parallel to the $direction$ parameter, then the resulting node
	//# transform will be singular and subsequent rendering through the camera is undefined.
	//#
	//# The direction in which to look should be specified in the camera's local coordinate space. Ordinarily,
	//# a camera resides in the infinite zone at the root of a scene, so the camera's local space is
	//# world space. In the case that the camera is a subnode of another node having a non-identity
	//# world transform, the $direction$ and $up$ parameters must be transformed into the camera's local
	//# space using the parent node's inverse world transform.
	//#
	//# If the $up$ parameter is omitted, then the direction (0,0,1) is used as the up direction.
	//
	//# \also	$@Camera::LookAtPoint@$
	//# \also	$@Node::SetNodeTransform@$


	//# \div
	//# \function	Camera::BoxVisible		Returns a boolean value indicating whether a box is visible to the camera.
	//
	//# \proto	virtual bool BoxVisible(const Point3D& center, const Vector3D *axis) const;
	//
	//# \param	center		The center of the box in world space.
	//# \param	axis		A pointer to an array of three vectors representing the world-space semi-axes of the box.
	//
	//# \desc
	//# The $BoxVisible$ function determines whether the oriented box specified by the $center$ and $axis$ parameters
	//# is visible to the camera and returns $true$ if the box is visible. The $center$ parameter represents the world-space
	//# coordinates of the box's center, and the $axis$ parameter points to an array containing three vectors that
	//# represent the semi-axes connecting the box's center to the world-space centers of three mutually adjacent faces.
	//#
	//# The value $false$ is returned only if it is absolutely certain that the box is not visible to the camera. There may
	//# be cases in which this function returns $true$ even though no part of the box is visible due to the fact that the
	//# method used is a conservative approximation designed for best speed.
	//
	//# \also	$@Camera::SphereVisible@$


	//# \function	Camera::SphereVisible		Returns a boolean value indicating whether a sphere is visible to the camera.
	//
	//# \proto	virtual bool SphereVisible(const Point3D& center, float radius) const;
	//
	//# \param	center		The center of the sphere in world space.
	//# \param	radius		The radius of the sphere.
	//
	//# \desc
	//# The $SphereVisible$ function determines whether the sphere specified by the $center$ and $radius$ parameters
	//# is visible to the camera and returns $true$ if the sphere is visible. The $center$ parameter represents the world-space
	//# coordinates of the sphere's center.
	//#
	//# The value $false$ is returned only if it is absolutely certain that the sphere is not visible to the camera. There may
	//# be cases in which this function returns $true$ even though no part of the sphere is visible due to the fact that the
	//# method used is a conservative approximation designed for best speed.
	//
	//# \also	$@Camera::BoxVisible@$


	class Camera : public Node
	{
		friend class Node;

		private:

			CameraType					cameraType;

			static Camera *Create(Unpacker& data, unsigned_int32 unpackFlags);

			C4API bool CalculateBoundingBox(Box3D *box) const override;
			C4API bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		protected:

			List<RootCameraRegion>		rootRegionList;

			Camera(CameraType type);
			Camera(const Camera& camera);

		public:

			C4API virtual ~Camera();

			CameraType GetCameraType(void) const
			{
				return (cameraType);
			}

			CameraObject *GetObject(void) const
			{
				return (static_cast<CameraObject *>(Node::GetObject()));
			}

			RootCameraRegion *GetFirstRootRegion(void) const
			{
				return (rootRegionList.First());
			}

			void AddRootRegion(RootCameraRegion *region)
			{
				rootRegionList.Append(region);
			}

			C4API void PackType(Packer& data) const override;

			C4API void LookAtPoint(const Point3D& point);
			C4API void LookAtPoint(const Point3D& point, const Vector3D& up);
			C4API void LookInDirection(const Vector3D& direction);
			C4API void LookInDirection(const Vector3D& direction, const Vector3D& up);

			virtual void CastRay(float x, float y, Ray *ray) const = 0;

			virtual bool BoxVisible(const Point3D& center, const Vector3D *axis) const = 0;
			virtual bool SphereVisible(const Point3D& center, float radius) const = 0;

			C4API virtual void Move(void);
	};


	//# \class	OrthoCamera		Represents an orthographic camera node in a world.
	//
	//# The $OrthoCamera$ class represents an orthographic camera node in a world.
	//
	//# \def	class OrthoCamera : public Camera
	//
	//# \ctor	OrthoCamera();
	//
	//# \desc
	//#
	//
	//# \base	Camera		An orthographic camera is a type of camera.
	//
	//# \also	$@GraphicsMgr/OrthoCameraObject@$


	class OrthoCamera : public Camera
	{
		friend class Camera;

		private:

			OrthoCamera(bool);

			Node *Replicate(void) const override;

		protected:

			C4API OrthoCamera(const OrthoCamera& orthoCamera);

		public:

			C4API OrthoCamera();
			C4API ~OrthoCamera();

			OrthoCameraObject *GetObject(void) const
			{
				return (static_cast<OrthoCameraObject *>(Node::GetObject()));
			}

			C4API void CastRay(float x, float y, Ray *ray) const;

			C4API bool BoxVisible(const Point3D& center, const Vector3D *axis) const override;
			C4API bool SphereVisible(const Point3D& center, float radius) const override;

			void CalculateOrthoCameraRegion(CameraRegion *region) const;
	};


	//# \class	FrustumCamera		Represents a frustum camera node in a world.
	//
	//# The $FrustumCamera$ class represents a frustum camera node in a world.
	//
	//# \def	class FrustumCamera : public Camera, public LinkTarget<FrustumCamera>
	//
	//# \ctor	FrustumCamera(float focal, float aspect);
	//
	//# \param	focal		The focal length of the perspective projection. This determines the field of view.
	//# \param	aspect		The aspect ratio of the projection. This is normally the ratio between the height
	//#						and the width of the viewport.
	//
	//# \desc
	//#
	//
	//# \base	Camera									A frustum camera is a type of camera.
	//# \base	Utilities/LinkTarget<FrustumCamera>		Used internally by the World Manager.
	//
	//# \also	$@GraphicsMgr/FrustumCameraObject@$


	class FrustumCamera : public Camera, public LinkTarget<FrustumCamera>
	{
		friend class Camera;

		private:

			float			sineHalfField;

			Vector3D		frustumPlaneNormal[4];
			Point3D			frustumVertex[8];
			Point3D			nearPlaneCenter;
			Vector2D		maxFrustumExtent;

			Shaker			*cameraShaker;

			FrustumCamera();

			C4API Node *Replicate(void) const override;

			bool EstablishRootRegions(Zone *zone, int32 maxDepth, int32 forcedDepth);

		protected:

			C4API FrustumCamera(CameraType type);
			C4API FrustumCamera(const FrustumCamera& frustumCamera);

			Point3D *GetFrustumVertexArray(void)
			{
				return (frustumVertex);
			}

			C4API void HandlePostprocessUpdate(void) override;

		public:

			C4API FrustumCamera(float focal, float aspect);
			C4API ~FrustumCamera();

			FrustumCameraObject *GetObject(void) const
			{
				return (static_cast<FrustumCameraObject *>(Node::GetObject()));
			}

			float GetSineHalfField(void) const
			{
				return (sineHalfField);
			}

			const Vector3D& GetFrustumPlaneNormal(int32 index) const
			{
				return (frustumPlaneNormal[index]);
			}

			const Point3D *GetFrustumVertexArray(void) const
			{
				return (frustumVertex);
			}

			const Point3D& GetNearPlaneCenter(void) const
			{
				return (nearPlaneCenter);
			}

			const Vector2D& GetMaxFrustumExtent(void) const
			{
				return (maxFrustumExtent);
			}

			Shaker *GetShaker(void) const
			{
				return (cameraShaker);
			}

			C4API void CastRay(float x, float y, Ray *ray) const;

			C4API bool BoxVisible(const Point3D& center, const Vector3D *axis) const override;
			C4API bool SphereVisible(const Point3D& center, float radius) const override;
			C4API bool DirectionVisible(const Vector3D& direction, float radius) const;

			C4API void SetShaker(Shaker *shaker);

			void UpdateRootRegions(Zone *zone);
			void CalculateFrustumCameraRegion(CameraRegion *region) const;
	};


	class CubeCamera : public FrustumCamera
	{
		private:

			int32		faceIndex;

			CubeCamera(const CubeCamera& cubeCamera);

			Node *Replicate(void) const override;

			void HandleTransformUpdate(void) override;

		public:

			C4API static const ConstMatrix3D cameraRotation[6];

			C4API CubeCamera();
			C4API ~CubeCamera();

			int32 GetFaceIndex(void) const
			{
				return (faceIndex);
			}

			void SetFaceIndex(int32 index)
			{
				faceIndex = index;
			}
	};


	//# \class	RemoteCamera		Represents a remote camera node in a world.
	//
	//# The $RemoteCamera$ class represents a remote camera node in a world.
	//
	//# \def	class RemoteCamera : public FrustumCamera
	//
	//# \ctor	RemoteCamera(float focal, float aspect, const Transform4D& transform, const Vector4D& clipPlane);
	//
	//# \param	focal		The focal length of the perspective projection. This determines the field of view.
	//# \param	aspect		The aspect ratio of the projection. This is normally the ratio between the height
	//#						and the width of the viewport.
	//# \param	transform	The world-space remote transform. This is applied to the camera's world transform.
	//# \param	clipPlane	The world-space geometric clipping plane.
	//
	//# \desc
	//#
	//
	//# \base	FrustumCamera	A remote camera is a special type of frustum camera.
	//
	//# \also	$@GraphicsMgr/RemoteCameraObject@$


	class RemoteCamera : public FrustumCamera
	{
		friend class Camera;

		private:

			int32		remoteVertexCount;
			Point3D		remoteVertex[kMaxClippedPortalVertexCount];
			Point3D		remoteCenter;

			RemoteCamera();
			RemoteCamera(const RemoteCamera& remoteCamera);

			Node *Replicate(void) const override;

			void HandleTransformUpdate(void) override;

		public:

			RemoteCamera(float focal, float aspect, const Transform4D& transform, const Antivector4D& clipPlane);
			~RemoteCamera();

			RemoteCameraObject *GetObject(void) const
			{
				return (static_cast<RemoteCameraObject *>(Node::GetObject()));
			}

			int32 GetRemoteVertexCount(void) const
			{
				return (remoteVertexCount);
			}

			const Point3D *GetRemoteVertexArray(void) const
			{
				return (remoteVertex);
			}

			const Point3D& GetRemoteCenter(void) const
			{
				return (remoteCenter);
			}

			void EstablishFixedRootCameraRegion(Zone *zone)
			{
				rootRegionList.Purge();
				rootRegionList.Append(new RootCameraRegion(this, zone));
			}

			void SetRemotePolygon(int32 vertexCount, const Point3D *vertex);
			void CalculateRemoteCameraRegion(CameraRegion *region);
	};


	//# \class	OrientedCamera		Represents an oriented camera node in a world.
	//
	//# The $OrientedCamera$ class represents an oriented camera node in a world.
	//
	//# \def	class OrientedCamera : public FrustumCamera
	//
	//# \ctor	OrientedCamera(float focal, float aspect);
	//
	//# \param	focal		The focal length of the perspective projection. This determines the field of view.
	//# \param	aspect		The aspect ratio of the projection. This is normally the ratio between the height
	//#						and the width of the viewport.
	//
	//# \desc
	//# An oriented camera is a specialization of the frustum camera that holds two angle values, called the
	//# azimuth and altitude angles, describing the camera's orientation in space. The azimuth angle represents
	//# the camera's rotation in the horizontal plane, and the altitude angle represents the camera's rotation
	//# above the horizontal plane. Both angles are initially zero, and this corresponds to a camera pointed in
	//# the positive <i>x</i> direction, parallel to the horizontal plane.
	//
	//# \base	FrustumCamera	An oriented camera is a special type of frustum camera.


	//# \function	OrientedCamera::GetCameraAzimuth	Returns the current azimuth angle for an oriented camera.
	//
	//# \proto	float GetCameraAzimuth(void) const;
	//
	//# \desc
	//# The $GetCameraAzimuth$ function returns the current azimuth angle, in radians. The azimuth angle represents
	//# the camera's rotation in the horizontal plane, and increasing angles move counterclockwise about the <i>z</i>
	//# axis. The angle zero corresponds to the positive <i>x</i> direction. The initial azimuth angle is zero radians.
	//
	//# \also	$@OrientedCamera::SetCameraAzimuth@$
	//# \also	$@OrientedCamera::GetCameraAltitude@$
	//# \also	$@OrientedCamera::SetCameraAltitude@$


	//# \function	OrientedCamera::SetCameraAzimuth	Sets the current azimuth angle for an oriented camera.
	//
	//# \proto	void SetCameraAzimuth(float azimuth);
	//
	//# \param	azimuth		The new azimuth angle for the camera, in radians.
	//
	//# \desc
	//# The $SetCameraAzimuth$ function sets the current azimuth angle to the value specified by the $azimuth$ parameter.
	//# The azimuth angle represents the camera's rotation in the horizontal plane, and increasing angles move counterclockwise
	//# about the <i>z</i> axis. The angle zero corresponds to the positive <i>x</i> direction. The initial azimuth angle is zero radians.
	//
	//# \also	$@OrientedCamera::GetCameraAzimuth@$
	//# \also	$@OrientedCamera::GetCameraAltitude@$
	//# \also	$@OrientedCamera::SetCameraAltitude@$


	//# \function	OrientedCamera::GetCameraAltitude	Returns the current altitude angle for an oriented camera.
	//
	//# \proto	float GetCameraAltitude(void) const;
	//
	//# \desc
	//# The $GetCameraAltitude$ function returns the current altitude angle, in radians. The altitude angle represents
	//# the camera's rotation above the horizontal plane. (Negative angles rotate the camera below the horizontal plane.)
	//# The initial altitude angle is zero radians, meaning that the camera points in a direction parallel to the horizontal plane.
	//
	//# \also	$@OrientedCamera::SetCameraAltitude@$
	//# \also	$@OrientedCamera::GetCameraAzimuth@$
	//# \also	$@OrientedCamera::SetCameraAzimuth@$


	//# \function	OrientedCamera::SetCameraAltitude	Sets the current altitude angle for an oriented camera.
	//
	//# \proto	void SetCameraAltitude(float altitude);
	//
	//# \param	altitude	The new altitude angle for the camera, in radians.
	//
	//# \desc
	//# The $SetCameraAltitude$ function sets the current altitude angle to the value specified by the $altitude$ parameter.
	//# The altitude angle represents the camera's rotation above the horizontal plane. (Negative angles rotate the camera
	//# below the horizontal plane.) The initial altitude angle is zero radians, meaning that the camera points in a direction
	//# parallel to the horizontal plane.
	//
	//# \also	$@OrientedCamera::GetCameraAltitude@$
	//# \also	$@OrientedCamera::GetCameraAzimuth@$
	//# \also	$@OrientedCamera::SetCameraAzimuth@$


	class OrientedCamera : public FrustumCamera
	{
		private:

			float		cameraAzimuth;
			float		cameraAltitude;

		protected:

			C4API OrientedCamera(float focal, float aspect);

		public:

			C4API ~OrientedCamera();

			float GetCameraAzimuth(void) const
			{
				return (cameraAzimuth);
			}

			void SetCameraAzimuth(float azimuth)
			{
				cameraAzimuth = azimuth;
			}

			float GetCameraAltitude(void) const
			{
				return (cameraAltitude);
			}

			void SetCameraAltitude(float altitude)
			{
				cameraAltitude = altitude;
			}

			C4API void Move(void) override;
	};


	//# \class	SpectatorCamera		Represents a spectator camera node in a world.
	//
	//# The $SpectatorCamera$ class represents a spectator camera node in a world.
	//
	//# \def	class SpectatorCamera : public OrientedCamera
	//
	//# \ctor	SpectatorCamera(float focal, float aspect, float radius);
	//
	//# \param	focal		The focal length of the perspective projection. This determines the field of view.
	//# \param	aspect		The aspect ratio of the projection. This is normally the ratio between the height
	//#						and the width of the viewport.
	//# \param	radius		The collision radius of the spectator camera.
	//
	//# \desc
	//#
	//
	//# \base	OrientedCamera		A spectator camera is a special type of oriented camera.


	class SpectatorCamera : public OrientedCamera
	{
		private:

			unsigned_int32	spectatorFlags;

			float			spectatorRadius;
			float			spectatorSpeed;

			Vector3D		slideNormal[2];
			Vector3D		collisionOffset;

			Vector3D CalculateGuardOffset(unsigned_int32 flags) const;
			bool TestSlide(unsigned_int32 flags, int32 index) const;

		public:

			C4API SpectatorCamera(float focal, float aspect, float radius);
			C4API ~SpectatorCamera();

			unsigned_int32 GetSpectatorFlags(void) const
			{
				return (spectatorFlags);
			}

			void SetSpectatorFlags(unsigned_int32 flags)
			{
				spectatorFlags = flags;
			}

			C4API void Move(void) override;
	};


	//# \class	BenchmarkCamera		Represents a benchmark camera node in a world.
	//
	//# The $BenchmarkCamera$ class represents a benchmark camera node in a world.
	//
	//# \def	class BenchmarkCamera : public OrientedCamera
	//
	//# \ctor	BenchmarkCamera(float focal, float aspect);
	//
	//# \param	focal		The focal length of the perspective projection. This determines the field of view.
	//# \param	aspect		The aspect ratio of the projection. This is normally the ratio between the height
	//#						and the width of the viewport.
	//
	//# \desc
	//#
	//
	//# \base	BenchmarkCamera		A benchmark camera is a special type of oriented camera.


	class BenchmarkCamera : public OrientedCamera
	{
		private:

			DeferredTask		completionTask;

			World				*benchmarkWorld;
			FrustumCamera		*worldCamera;

			Marker				*currentMarker;
			int32				angleIndex;

			int32				frameCount;
			int32				totalTime;

			void SetCurrentMarker(Marker *marker);

			static void EndBenchmark(DeferredTask *task, void *cookie);

		public:

			C4API BenchmarkCamera(float focal, float aspect);
			C4API ~BenchmarkCamera();

			C4API void Move(void) override;

			C4API void BeginBenchmark(World *world, Marker *marker);
	};
}


#endif

// ZYUQURM
