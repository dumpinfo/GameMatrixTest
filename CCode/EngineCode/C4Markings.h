 

#ifndef C4Markings_h
#define C4Markings_h


//# \component	Effect Manager
//# \prefix		EffectMgr/


#include "C4Effects.h"


namespace C4
{
	//# \enum	MarkingFlags

	enum
	{
		kMarkingLight			= 1 << 0,		//## The marking receives full lighting.
		kMarkingBlendLight		= 1 << 1,		//## Alpha blending is applied to a lighted marking (valid only if the $kMarkingLight$ flag is set).
		kMarkingDepthWrite		= 1 << 2,		//## The marking writes to the depth buffer (valid only if the $kMarkingLight$ flag is set).
		kMarkingTwoSided		= 1 << 3,		//## The marking is rendered two-sided (valid only if the $kMarkingLight$ flag is <i>not</i> set).
		kMarkingClipRange		= 1 << 4,		//## The minimum and maximum depths to which the marking is clipped are given by the $clip$ field of the $@MarkingData@$ structure.
		kMarkingFullPolygon		= 1 << 5,		//## The marking is always created with complete, unclipped polygons from the mesh to which it is applied.
		kMarkingForceClip		= 1 << 6		//## The $kGeometryMarkingFullPolygon$ flag is ignored if specified for any geometry node, forcing the marking polygons to be clipped. This flag has no effect if the $kMarkingFullPolygon$ flag is specified.
	};


	class MarkingList;


	//# \struct	MarkingData		Defines the parameters of a surface marking effect.
	//
	//# The $MarkingData$ structure defines the parameters of a surface marking effect.
	//
	//# \ctor	MarkingData(unsigned_int32 flags = 0, MarkingList *list = nullptr);
	//# \ctor	MarkingData(const Vector2D& scale, const Vector2D& offset, unsigned_int32 flags = 0, MarkingList *list = nullptr);
	//
	//# \param	flags		The marking effect flags.
	//# \param	list		A pointer to the $@MarkingList@$ object into which each $@MarkingEffect@$ is inserted.
	//# \param	scale		A scale to apply to the (<i>s</i>, <i>t</i>) texture coordinates used by the marking.
	//# \param	offset		An offset to apply to the (<i>s</i>, <i>t</i>) texture coordinates used by the marking.
	//
	//# \data	MarkingData
	//
	//# \desc
	//# The $MarkingData$ structure holds information that is passed to the constructor for the $@MarkingEffect@$ class.
	//# The $markingFlags$ member can be a combination (through logical OR) of the following constants.
	//
	//# \table	MarkingFlags
	//
	//# If the $list$ parameter is not $nullptr$, then each $@MarkingEffect@$ node created during a call to the $@MarkingEffect::New@$
	//# function is added to the $@MarkingList@$ object (which can be a subclass of $MarkingList$).
	//#
	//# If the $scale$ and $offset$ parameters are specified, then the $texcoordScale$ and $texcoordOffset$ members of
	//# the $MarkingData$ structure are initialized to those values. Otherwise, the scale is initialized to (1, 1), and the
	//# offset is initialized to (0, 0) so that the entire texture map is used by the marking.
	//#
	//# Polygons belonging to the marking effect are ordinarily created by clipping polygons belonging to a geometry node to the
	//# bounding box if the marking effect. If the $kMarkingFullPolygon$ flag is specified, then the geometry's polygons are not
	//# clipped but are instead copied so that an exact duplicate appears in the marking effect's mesh. This results in larger
	//# polygons being rendered, but it avoids depth testing issues related to offsetting coplanar polygons. In this case, the
	//# $markingOffset$ member of the $MarkingData$ structure is ignored, and the marking effect is always offset by the minimum
	//# amount possible toward the camera in viewport space.
	//
	//# \also	$@MarkingEffect@$


	//# \member		MarkingData

	struct MarkingData
	{
		unsigned_int32		markingFlags;			//## The marking effect flags. This is set to zero by the $MarkingData$ constructors.
		float				markingOffset;			//## The depth offset distance used by the marking effect. This is set to 1/128 by the default constructor. The depth offset is ignored if the $kMarkingFullPolygon$ flag is specified.
		MarkingList			*markingList;			//## A pointer to a $@MarkingList@$ (or a subclass) to which marking effects are added.

		Point3D				center;					//## The world-space center of the surface marking.
		Vector3D			normal;					//## The world-space normal direction of the surface marking. This vector must have unit length.
		Vector3D			tangent;				//## The world-space tangent direction of the surface marking. (This determines texture orientation.) This vector does not have to be unit length.
		float				radius;					//## The radius of the surface marking.
		Range<float>		clip;					//## The range of depths, with respect to the center and normal direction, to which the marking is clipped if the $kMarkingClipRange$ flag is specified. If $kMarkingClipRange$ is not specified, then the range is [&minus;<i>r</i>,&nbsp;<i>r</i>], where <i>r</i> is the radius of the marking.

		Vector2D			texcoordScale;			//## The (<i>s</i>, <i>t</i>) scale to apply to the texture coordinates of the marking. This is set to (1, 1) by the default constructor.
		Vector2D			texcoordOffset;			//## The (<i>s</i>, <i>t</i>) offset to apply to the texture coordinates of the marking. This is set to (0, 0) by the default constructor.

		union
		{
			const char		*textureName;			//## A pointer to the name of the texture map used by the surface marking. This occupies the same space as $materialObject$ and should not be specified if the $kMarkingLight$ is set in the $markingFlags$ field.
			MaterialObject	*materialObject;		//## A pointer to the material object used by the surface marking. This occupies the same space as $textureName$ and should only be specified if the $kMarkingLight$ is set in the $markingFlags$ field.
		};

		ColorRGBA			color;					//## The marking color. Ignored if a material object containing a $DiffuseAttribute$ is specified.
		int32				lifeTime;				//## The time for which the surface marking is rendered. If this is set to -1, then the marking is rendered for exactly one frame.

		MarkingData(unsigned_int32 flags = 0, MarkingList *list = nullptr)
		{
			markingFlags = flags;
			markingOffset = 0.015625F;
			markingList = list;

			texcoordScale.Set(1.0F, 1.0F);
			texcoordOffset.Set(0.0F, 0.0F); 
		}

		MarkingData(const Vector2D& scale, const Vector2D& offset, unsigned_int32 flags = 0, MarkingList *list = nullptr) 
		{
			markingFlags = flags; 
			markingOffset = 0.015625F;
			markingList = list;
 
			texcoordScale = scale;
			texcoordOffset = offset; 
		} 
	};


	//# \class	MarkingEffect		Represents a surface marking node in a world. 
	//
	//# The $MarkingEffect$ class represents a surface marking node in a world.
	//
	//# \def	class MarkingEffect final : public Effect, public ListElement<MarkingEffect>, public Memory<MarkingEffect>
	//
	//# \ctor	MarkingEffect(const Geometry *geometry, const MarkingData *data);
	//
	//# \param	geometry	The geometry to which the marking is to be applied.
	//# \param	data		A pointer to a $@MarkingData@$ data structure defining the marking's parameters.
	//
	//# \desc
	//# The $MarkingEffect$ class represents a surface marking node that is associated with a single geometry node.
	//#
	//# Surface markings are normally generated for all of the geometries intersecting a particular location by calling
	//# the $@MarkingEffect::New@$ function, but it is also possible to construct a $MarkingEffect$ directly. If the
	//# $MarkingEffect$ constructor is explicitly called (by using the $new$ operator), then the calling code should
	//# subsequently call the $@MarkingEffect::Empty@$ function to determine whether any triangles were generated.
	//# If the marking effect is not empty, then it should be added to the scene as a subnode of the geometry node
	//# specified by the $geometry$ parameter by calling the $@WorldMgr/Node::AppendNewSubnode@$ function. If the
	//# $MarkingEffect::Empty$ function returns $true$, then the marking effect should simply be deleted.
	//
	//# \base	Effect									A $MarkingEffect$ node is a specific type of effect.
	//# \base	Utilities/ListElement<MarkingEffect>	Each $MarkingEffect$ node belonging to a single surface marking can be stored in a $@MarkingList@$ object.
	//# \base	MemoryMgr/Memory<MarkingEffect>			Storage for marking effects is allocated in a dedicated heap for speed.
	//
	//# \also	$@MarkingData@$
	//# \also	$@MarkingList@$


	//# \function	MarkingEffect::New		Creates a set of marking effects at a particular location.
	//
	//# \proto	static void New(World *world, const MarkingData *data);
	//
	//# \param	world	The world in which the marking effects should be applied.
	//# \param	data	A pointer to a $@MarkingData@$ structure describing the marking.
	//
	//# \desc
	//# The $New$ function creates a set of marking effects at a particular location using the parameters specified in the
	//# $@MarkingData@$ structure pointed to by the $data$ parameter. The $New$ function finds all enabled geometry nodes that
	//# intersect the marking's bounds, excludes those having either the $kGeometryMarkingInhibit$ or $kGeometryInvisible$ flag set,
	//# creates a new marking effect for each geometry, and adds each nonempty marking effect to the world.
	//#
	//# If the $markingList$ field of the $@MarkingData@$ structure points to a $@MarkingList@$ object, then each nonempty
	//# marking effect created by the $New$ function is added to this list so that it's possible to track all pieces of the
	//# complete surface marking.
	//
	//# \also	$@MarkingData@$
	//# \also	$@MarkingList@$


	//# \function	MarkingEffect::Empty		Returns a boolean value indicating whether a surface marking contains any triangles.
	//
	//# \proto	bool Empty(void) const;
	//
	//# \desc
	//# The $Empty$ function returns a boolean value indicating whether a surface marking contains any triangles. If the return
	//# value is $false$, then at least one triangle was generated for the surface marking. Otherwise, the surface marking is empty,
	//# and it should be deleted. This function is intended to be used when a surface marking is constructed directly&mdash;it is
	//# not necessary when the $@MarkingEffect::New@$ function is called.


	class MarkingEffect final : public Effect, public ListElement<MarkingEffect>, public EngineMemory<MarkingEffect>
	{
		friend class Effect;

		private:

			enum
			{
				kMaxSmallMarkingVertexCount = 64
			};

			struct MarkingVertex
			{
				Point3D		position;
				Color4C		color;
				Point2D		texcoord;
			};

			struct LitMarkingVertex
			{
				Point3D		position;
				Vector3D	normal;
				Vector3D	tangent;
				Point2D		texcoord;
			};

			struct ClippingData
			{
				int32			geometryVertexCount;
				int32			maxMarkingVertexCount;

				Antivector4D	leftPlane;
				Antivector4D	rightPlane;
				Antivector4D	bottomPlane;
				Antivector4D	topPlane;
				Antivector4D	frontPlane;
				Antivector4D	backPlane;
			};

			unsigned_int32				markingFlags;
			float						markingAlpha;

			float						markingDepthOffset;

			int32						markingLifeTime;
			int32						markingFadeTime;
			int32						markingKillTime;
			int32						markingInvisibleTime;

			Point3D						effectPosition;
			float						effectRadius;

			int32						markingVertexCount;
			int32						markingTriangleCount;

			VertexBuffer				vertexBuffer;
			VertexBuffer				indexBuffer;

			MaterialObject				*materialObject;
			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;
			DiffuseTextureAttribute		textureAttribute;

			char						*largeArrayStorage;
			Point3D						*positionArray;
			Vector3D					*normalArray;
			Color4C						*colorArray;
			Vector3D					*tangentArray;
			Point2D						*texcoordArray;
			Triangle					*triangleArray;

			Point3D						smallPositionArray[kMaxSmallMarkingVertexCount];
			Vector3D					smallNormalArray[kMaxSmallMarkingVertexCount];
			Color4C						smallColorArray[kMaxSmallMarkingVertexCount];
			Vector3D					smallTangentArray[kMaxSmallMarkingVertexCount];
			Point2D						smallTexcoordArray[kMaxSmallMarkingVertexCount];
			Triangle					smallTriangleArray[kMaxSmallMarkingVertexCount * 3];

			MarkingEffect();

			static void MaterialObjectLinkProc(Object *object, void *cookie);

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

			static ProximityResult MarkGeometry(Node *node, const Point3D& center, float radius, void *cookie);

			void AllocateLargeArrays(int32 vertexCount, int32 triangleCount);
			bool AddPolygon(int32 vertexCount, const Point3D *position, const Vector3D *normal, ClippingData *clippingData);
			static int32 ClipPolygon(const Antivector4D& plane, int32 vertexCount, const Point3D *position, const Vector3D *normal, Point3D *newPosition, Vector3D *newNormal);

		public:

			C4API MarkingEffect(const Geometry *geometry, const MarkingData *data);
			C4API ~MarkingEffect();

			bool Empty(void) const
			{
				return (markingVertexCount == 0);
			}

			const char *GetTextureName(void) const
			{
				return (textureAttribute.GetTextureName());
			}

			void Prepack(List<Object> *linkList) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			void Preprocess(void) override;

			void Move(void) override;
			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;

			C4API static void New(const World *world, const MarkingData *data);
	};


	//# \class	MarkingList		Contains a list of marking effects.
	//
	//# The $MarkingList$ class contains a list of marking effects.
	//
	//# \def	class MarkingList : public List<MarkingEffect>
	//
	//# \ctor	MarkingList();
	//
	//# \desc
	//# The $MarkingList$ class is used to contain a list of the marking effects created during a single call to the
	//# $@MarkingEffect::New@$ function. When a new surface marking is created, it can be split over multiple geometries,
	//# and each part gets its own $@MarkingEffect@$ node. If a pointer to a $MarkingList$ object (or a subclass object)
	//# is specified in the $@MarkingData@$ structure, then all of the $MarkingEffect$ nodes are added to that list.
	//#
	//# When the last marking effect in the list is destroyed because its lifetime has expired, the $@MarkingList::HandleDestruction@$
	//# function is called. This function should be implemented by a custom subclass of the $MarkingList$ class to perform
	//# any action required when the marking completely disappears from the scene. The $HandleDestruction$ function is not
	//# called if no marking effects are created in the first place during a call to the $MarkingEffect::New$ function.
	//#
	//# Destroying a $MarkingList$ object effectively removes the entire surface marking from the scene.
	//
	//# \base	Utilities/List<MarkingEffect>		A $QuadEffectObject$ is an object that can be shared by multiple quad effect nodes.
	//
	//# \also	$@MarkingEffect@$
	//# \also	$@MarkingData@$


	//# \function	MarkingList::HandleDestruction		Called when all parts of a surface marking have been destroyed.
	//
	//# \proto	virtual void HandleDestruction(void);
	//
	//# \desc
	//# The $HandleDestruction$ function is called when the last marking effect contained in a $MarkingList$ object is destroyed
	//# because its lifetime has expired. This function should be implemented by a custom subclass of the $MarkingList$ class to
	//# perform any action required when the marking completely disappears from the scene. The $HandleDestruction$ function is not
	//# called if no marking effects are created in the first place during a call to the $@MarkingEffect::New@$ function.
	//
	//# \also	$@MarkingEffect@$
	//# \also	$@MarkingData@$


	class MarkingList : public List<MarkingEffect>
	{
		public:

			virtual void HandleDestruction(void);
	};
}


#endif

// ZYUQURM
