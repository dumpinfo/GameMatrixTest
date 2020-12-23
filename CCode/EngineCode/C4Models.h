 

#ifndef C4Models_h
#define C4Models_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Geometries.h"
#include "C4Controller.h"
#include "C4Animation.h"
#include "C4Markers.h"


namespace C4
{
	typedef Type	ModelType;


	enum : ModelType
	{
		kModelUnknown		= 0,
		kModelGeneric		= 1
	};


	//# \enum	ModelRegistrationFlags

	enum
	{
		kModelPrecache		= 1 << 0,		//## Precache the model resource.
		kModelPrivate		= 1 << 1		//## Do not display the model type in the World Editor.
	};


	enum : ControllerType
	{
		kControllerMorph		= 'MRPH',
		kControllerSkin			= 'SKIN',
		kControllerAnimation	= 'ANIM'
	};


	enum : FunctionType
	{
		kFunctionPlayAnimation	= 'PLAY',
		kFunctionStopAnimation	= 'STOP'
	};


	class Model;
	class RagdollController;


	class ModelResource : public Resource<ModelResource>
	{
		friend class Resource<ModelResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			~ModelResource();

		public:

			C4API ModelResource(const char *name, ResourceCatalog *catalog);
	};


	//# \class	Bone	Represents a skeletal component used by a skinnable mesh.
	//
	//# The $Bone$ class represents a skeletal component used by a skinnable mesh.
	//
	//# \def	class Bone final : public Node
	//
	//# \ctor	Bone(unsigned_int32 hash = 0);
	//
	//# \param	hash	The hash value for the name of the bone node.
	//
	//# \desc
	//# The $Bone$ class represents a single bone in a skeleton used by a skinnable mesh. An entire skeleton
	//# is typically composed of many bones arranged in a transform hierarchy.
	//
	//# \base	Node		A $Bone$ node is a scene graph node.
	//
	//# \also	$@Model@$


	//# \function	Bone::GetModelTransform		Returns the transform from bone space to model space.
	//
	//# \proto	const Transform4D& GetModelTransform(void) const;
	//
	//# \desc
	//# The $GetModelTransform$ function returns the transform that maps points in the object space of the bone to
	//# the object space of the model to which the bone belongs. This transform is valid only after the bone has been
	//# preprocessed and updated. The model node itself can be retrieved by calling the $@Bone::GetSkeletonRoot@$ function.
	//
	//# \also	$@Bone::GetSkeletonRoot@$

 
	//# \function	Bone::GetSkeletonRoot		Returns the root node of the model to which a bone belongs.
	//
	//# \proto	Node *GetSkeletonRoot(void) const; 
	//
	//# \desc 
	//# The $GetSkeletonRoot$ function returns a pointer to the model node to which a bone belongs. The return value is
	//# only valid after the bone has been preprocessed. If there is no model node above the bone in the transform hierarchy,
	//# then the return value is $nullptr$. 
	//
	//# \also	$@Bone::GetModelTransform@$ 
 

	class Bone final : public Node
	{
		private: 

			Node			*skeletonRoot;

			Transform4D		modelTransform;

			bool			boundingBoxFlag;
			Box3D			boneBoundingBox;

			Bone(const Bone& bone);

			Node *Replicate(void) const override;

			void HandlePostprocessUpdate(void) override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API Bone();
			C4API ~Bone();

			Node *GetSkeletonRoot(void) const
			{
				return (skeletonRoot);
			}

			const Transform4D& GetModelTransform(void) const
			{
				return (modelTransform);
			}

			const Box3D& GetBoneBoundingBox(void) const
			{
				return (boneBoundingBox);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void Neutralize(void) override;

			void SetBoneBoundingBox(const Box3D& box);
	};


	//# \class	MorphController		Controls a mesh with multiple morph targets.
	//
	//# The $MorphController$ class controls a mesh with multiple morph targets.
	//
	//# \def	class MorphController : public Controller
	//
	//# \ctor	MorphController();
	//
	//# \desc
	//# The $MorphController$ class represents the controller that is assigned to meshes having multiple
	//# morph targets. It is responsible for calculating new vertex positions whenever the morph weights
	//# are changed.
	//
	//# \base	Controller/Controller		A $MorphController$ is a specific type of controller.
	//
	//# \also	$@SkinController@$
	//# \also	$@Model@$


	class MorphController : public Controller
	{
		protected:

			class MorphJob : public BatchJob
			{
				public:

					volatile void		*attributeBuffer;

					MorphJob(ExecuteProc *execProc, FinalizeProc *finalProc, void *cookie);
			};

		private:

			struct MorphVertex
			{
				Point3D				position;
				Vector3D			normal;
				Vector4D			tangent;
			};

			struct MorphWeight
			{
				float				weight;
				int32				index;
				unsigned_int32		hash;
			};

			struct MorphTerm
			{
				float				morphWeight;
				const MorphAttrib	*morphAttrib;
			};

			bool						morphUpdateFlag;

			float						baseMeshWeight;
			Array<MorphWeight, 16>		morphWeightArray;

			char						*morphStorage;
			MorphTerm					*morphTermArray;

			Point3D						*morphPositionArray;
			Vector3D					*morphNormalArray;
			Vector4D					*morphTangentArray;

			MorphJob					morphUpdateJob;
			MorphJob					morphIsolatedUpdateJob;
			MorphJob					*activeMorphUpdateJob;

			VertexBuffer				morphVertexBuffer;

			Controller *Replicate(void) const override;

			static void FinalizeMorphUpdate(Job *job, void *cookie);
			static void JobUpdateMorphIsolated(Job *job, void *cookie);
			static void FinalizeMorphIsolatedUpdate(Job *job, void *cookie);

		protected:

			MorphController(ControllerType type);
			MorphController(const MorphController& morphController);

			bool TakeMorphUpdateFlag(void)
			{
				bool flag = morphUpdateFlag;
				morphUpdateFlag = false;
				return (flag);
			}

			const Vector3D *GetMorphNormalArray(void) const
			{
				return (morphNormalArray);
			}

			const Vector4D *GetMorphTangentArray(void) const
			{
				return (morphTangentArray);
			}

			static void JobUpdateMorph(Job *job, void *cookie);

		public:

			C4API MorphController();
			C4API ~MorphController();

			GenericGeometry *GetTargetNode(void) const
			{
				return (static_cast<GenericGeometry *>(Controller::GetTargetNode()));
			}

			void InvalidateMorph(void)
			{
				morphUpdateFlag = true;
			}

			float GetBaseMeshWeight(void) const
			{
				return (baseMeshWeight);
			}

			void SetBaseMeshWeight(float weight)
			{
				baseMeshWeight = weight;
			}

			int32 GetMorphWeightCount(void) const
			{
				return (morphWeightArray.GetElementCount());
			}

			void SetMorphWeightCount(int32 count)
			{
				morphWeightArray.SetElementCount(count);
			}

			float GetMorphWeight(unsigned_int32 index) const
			{
				return (morphWeightArray[index].weight);
			}

			void SetMorphWeight(unsigned_int32 index, float weight) const
			{
				morphWeightArray[index].weight = weight;
			}

			unsigned_int32 GetMorphHash(unsigned_int32 index) const
			{
				return (morphWeightArray[index].hash);
			}

			void SetMorphHash(unsigned_int32 index, unsigned_int32 hash) const
			{
				morphWeightArray[index].hash = hash;
			}

			const Point3D *GetMorphPositionArray(void) const
			{
				return (morphPositionArray);
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Neutralize(void) override;
			void Update(void) override;

			void SetDetailLevel(int32 level) override;
	};


	//# \class	SkinController		Controls a skinned mesh.
	//
	//# The $SkinController$ class controls a skinned mesh.
	//
	//# \def	class SkinController final : public MorphController
	//
	//# \ctor	SkinController();
	//
	//# \desc
	//# The $SkinController$ class represents the controller that is assigned to skinned meshes. It is responsible
	//# for calculating new vertex positions whenever the skeleton to which the mesh is attached moves.
	//
	//# \base	MorphController		A $SkinController$ includes morphing functionality.
	//
	//# \also	$@Bone@$
	//# \also	$@Model@$


	class SkinController final : public MorphController
	{
		private:

			struct SkinVertex
			{
				Point3D		position;
				Point3D		previous;
				Vector3D	normal;
				Vector4D	tangent;
			};

			char				*skinStorage;

			int32				skinBoneCount;
			Bone				**skinBoneTable;
			Transform4D			*transformTable;

			Point3D				*skinPositionArray[2];
			Box3D				skinBoundingBox;

			unsigned_int8		vertexParity;
			bool				motionBlurFlag;

			MorphJob			skinUpdateJob;
			VertexBuffer		skinVertexBuffer;

			SkinController(const SkinController& skinController);

			Controller *Replicate(void) const override;

			void CalculateBoneBoundingBoxes(void) const;

			static void HandlePostprocessUpdate(GenericGeometry *geometry);

			static void JobUpdateSkin(Job *job, void *cookie);
			static void FinalizeSkinUpdate(Job *job, void *cookie);

		public:

			C4API SkinController();
			C4API ~SkinController();

			GenericGeometry *GetTargetNode(void) const
			{
				return (static_cast<GenericGeometry *>(Controller::GetTargetNode()));
			}

			const Point3D *GetSkinPositionArray(void) const
			{
				return (skinPositionArray[vertexParity ^ 1]);
			}

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void Neutralize(void) override;
			void StopMotion(void) override;
			void Update(void) override;

			void SetDetailLevel(int32 level) override;
	};


	//# \class	ModelRegistration		Contains information about an application-defined model type.
	//
	//# The $ModelRegistration$ class contains information about an application-defined model type.
	//
	//# \def	class ModelRegistration : public MapElement<ModelRegistration>
	//
	//# \ctor	ModelRegistration(ModelType type, const char *name, const char *path, unsigned_int32 flags = 0,
	//# \ctor2	ControllerType cntrlType = 0, int32 propCount = 0, const PropertyType *propTypeArray = nullptr);
	//
	//# \param	type			The model type.
	//# \param	name			The model name.
	//# \param	path			The resource name for the model.
	//# \param	flags			Flags pertaining to the model type.
	//# \param	cntrlType		The model's default controller type.
	//# \param	propCount		The number of default property types.
	//# \param	propTypeArray	A pointer to an array containing the model's default property types.
	//
	//# \desc
	//# The $ModelRegistration$ class is used to register an application-defined model type so that
	//# instances of the model can easily be cloned and so that they can be placed in a world using
	//# the World Editor. The act of instantiating an $ModelRegistration$ object automatically registers
	//# the corresponding model type. The model type is unregistered when the $ModelRegistration$ object
	//# is destroyed.
	//#
	//# Each model type must have a unique 32-bit identifier which is usually a four-character code.
	//# This identifier is specified in the $type$ parameter in the model registration, and is later
	//# passed to the $@Model::Get@$ function to create instances of the specific type of model.
	//#
	//# The $name$ parameter specifies the human-readable model name that is displayed in the World Editor.
	//# If the $kModelPrivate$ flag is specified in the $flags$ parameter, then the $name$ parameter may
	//# be $nullptr$.
	//#
	//# The $path$ parameter specifies the name of the model resource corresponding to the model type.
	//#
	//# The $flags$ parameter is optional and assigns special properties to the model registration.
	//# It can be a combination (through logical OR) of the following constants.
	//
	//# \table	ModelRegistrationFlags
	//
	//# If the $kModelPrivate$ flag is specified, then the model type cannot be placed in a world using
	//# the World Editor, but the $@Model::Get@$ function still produces instances of the model.
	//#
	//# The $cntrlType$ parameter is optional and identifies the type of controller that should be
	//# automatically assigned to a model of the registration's type when it is placed in a world
	//# using the World Editor. The type of the controller specified should be registered using the
	//# $@Controller/ControllerRegistration@$ class so that the controller's settings are accessible
	//# in the World Editor. The default value of 0 means that no controller is assigned.
	//#
	//# The $propCount$ and $propTypeArray$ parameters are optional and identify the number and type of
	//# properties that should be automatically assigned to a model of the registration's type when it
	//# is placed in a world using the World Editor. The types of the properties specified should be
	//# registered using the $@WorldMgr/PropertyRegistration@$ class so that each property's settings
	//# are accessible in the World Editor. The data referenced by the $propTypeArray$ parameter must
	//# persist for the lifetime of the model registration&mdash;it is not copied.
	//
	//# \base	Utilities/MapElement<ModelRegistration>		Used internally by the World Manager.
	//
	//# \also	$@Model@$


	//# \function	ModelRegistration::GetModelType		Returns the registered model type.
	//
	//# \proto	ModelType GetModelType(void) const;
	//
	//# \desc
	//# The $GetKey$ function returns the unique 32-bit identifier associated with a model type.
	//
	//# \also	$@ModelRegistration::GetModelName@$


	//# \function	ModelRegistration::GetModelFlags		Returns the model registration flags.
	//
	//# \proto	unsigned_int32 GetModelFlags(void) const;
	//
	//# \desc
	//# The $GetModelFlags$ function returns the flags that were assigned to the model type
	//# when the model registration was created. The flags can be a combination (through logical
	//# OR) of the following constants.
	//
	//# \table	ModelRegistrationFlags
	//
	//# \also	$@ModelRegistration::GetModelType@$
	//# \also	$@ModelRegistration::GetModelName@$


	//# \function	ModelRegistration::GetModelName		Returns the model name.
	//
	//# \proto	const char *GetModelName(void) const;
	//
	//# \desc
	//# The $GetModelName$ function returns the human-readable model name for a particular model registration.
	//# The model name is established when the model registration is constructed.
	//
	//# \also	$@ModelRegistration::GetResourceName@$
	//# \also	$@ModelRegistration::GetModelType@$


	//# \function	ModelRegistration::GetResourceName		Returns the name of the model resource.
	//
	//# \proto	const char *GetResourceName(void) const;
	//
	//# \desc
	//# The $GetResourceName$ function returns the resource name corresponding to a model's data.
	//# The resource name is established when the model registration is constructed.
	//
	//# \also	$@ModelRegistration::GetModelName@$
	//# \also	$@ModelRegistration::GetModelType@$


	class ModelRegistration : public Registration<Model, ModelRegistration>
	{
		private:

			unsigned_int32			modelFlags;

			const char				*modelName;
			const char				*resourceName;

			ControllerType			controllerType;

			int32					propertyCount;
			const PropertyType		*propertyTypeArray;

			Model					*prototypeModel;
			List<Model>				cloneList;

			void LoadPrototype(void);

		public:

			C4API ModelRegistration(ModelType type, const char *name, const char *rsrcName, unsigned_int32 flags = 0, ControllerType cntrlType = 0, int32 propCount = 0, const PropertyType *propTypeArray = nullptr);
			C4API ~ModelRegistration();

			ModelType GetModelType(void) const
			{
				return (GetRegistrableType());
			}

			unsigned_int32 GetModelFlags(void) const
			{
				return (modelFlags);
			}

			const char *GetModelName(void) const
			{
				return (modelName);
			}

			const char *GetResourceName(void) const
			{
				return (resourceName);
			}

			ControllerType GetControllerType(void) const
			{
				return (controllerType);
			}

			int32 GetPropertyCount(void) const
			{
				return (propertyCount);
			}

			const PropertyType *GetPropertyTypeArray(void) const
			{
				return (propertyTypeArray);
			}

			Model *Create(void) const;

			C4API void Reload(void);

			Model *Clone(Model *model = nullptr);
			Node *CloneGroup(void);

			void Retire(Model *model);
	};


	//# \class	Model		The base class for all animatable models.
	//
	//# The $Model$ class is the base class for all animatable models.
	//
	//# \def	class Model : public Node, public ListElement<Model>, public Registrable<Model, ModelRegistration>
	//
	//# \ctor	Model(ModelType type = kModelUnknown);
	//
	//# \param	type	The model type.
	//
	//# \desc
	//# The $Model$ class serves as the base class for all animatable model nodes. A $Model$ instance
	//# is not normally constructed directly, but is created by calling the $@Model::Get@$ function or
	//# by constructing an instance of the $@GenericModel@$ class. The $@Model::Get@$ function should
	//# be used to create instances of models whose type has been registered with the $@ModelRegistration@$
	//# class. A $@GenericModel@$ instance should be constructed to explicitly create a model without
	//# a registered type.
	//
	//# \base	Node											A $Model$ node is a scene graph node.
	//# \base	Utilities/ListElement<Model>					Used internally by the World Manager.
	//# \base	System/Registrable<Model, ModelRegistration>	Custom model types can be registered with the engine.
	//
	//# \also	$@ModelRegistration@$
	//# \also	$@GenericModel@$
	//# \also	$@Bone@$
	//
	//# \wiki	Models


	//# \function	Model::Get		Returns a new instances of a particular type of model.
	//
	//# \proto	static Model *Get(ModelType type);
	//
	//# \desc
	//# The $Get$ function is used to create new instances of the type of model specified by the
	//# $type$ parameter. This function will always clone an existing model of the same type if
	//# possible, and it will load the model's model resource if no instances have been created
	//# yet and the model has not been precached.
	//#
	//# In order to create new model instances with this function, the model type corresponding
	//# to the value of the $type$ parameter must have previously been registered through the
	//# instantiation of a $@ModelRegistration@$ object. If there is no registration matching the
	//# $type$ parameter, then the return value is $nullptr$.
	//#
	//# If the model type corresponding to the value of the $type$ parameter has been registered,
	//# but the model resource named in the registration does not exist, then a generic model is
	//# loaded, and a pointer to a clone of it is returned by the $Get$ function.
	//
	//# \also	$@ModelRegistration@$


	//# \function	Model::GetModelType		Returns the model type.
	//
	//# \proto	ModelType GetModelType(void) const;
	//
	//# \desc
	//# The $GetModelType$ function returns the type of a model node. If the model was created
	//# using the $@Model::Get@$ function, then the returned type is the same value as the type
	//# used to create the model. If the model was created as a $@GenericModel@$ node, then the
	//# returned type is $kModelGeneric$.
	//
	//# \also	$@Model::Get@$
	//# \also	$@GenericModel@$


	//# \function	Model::GetRootAnimator		Returns the root animator attached to a model.
	//
	//# \proto	Animator *GetRootAnimator(void) const;
	//
	//# \desc
	//# The $GetRootAnimator$ function returns the root animator in the animator tree attached
	//# to a model.
	//
	//# \also	$@Model::SetRootAnimator@$
	//# \also	$@Model::GetAnimatedTransformNodeCount@$
	//# \also	$@Model::Animate@$
	//# \also	$@Animator@$


	//# \function	Model::SetRootAnimator		Sets the root animator attached to a model.
	//
	//# \proto	void SetRootAnimator(Animator *animator);
	//
	//# \param	animator	The new root animator.
	//
	//# \desc
	//# The $SetRootAnimator$ function attaches an animator tree to a model, where the root of the
	//# animator tree is specified by the $animator$ parameter. When the $SetRootAnimator$ function
	//# is called, the $@Animator::Preprocess@$ function is called for each animator in the tree
	//# through a post-order traversal.
	//#
	//# The $animator$ parameter can be $nullptr$, in which case any existing animator tree is
	//# detached from the model node, and the model subsequently has no animators attached to it.
	//#
	//# The animator tree is not owned by the model node and is not deleted when the model is deleted.
	//
	//# \also	$@Model::GetRootAnimator@$
	//# \also	$@Model::GetAnimatedTransformNodeCount@$
	//# \also	$@Model::Animate@$
	//# \also	$@Animator@$


	//# \function	Model::GetAnimatedTransformNodeCount	Returns the number of nodes with animated transforms in a model.
	//
	//# \proto	int32 GetAnimatedTransformNodeCount(void) const;
	//
	//# \desc
	//# The $GetAnimatedTransformNodeCount$ function returns the number of nodes in a model that can have
	//# animated transforms. When a model is preprocessed, this number is calculated by examining the nodes
	//# in the model's subtree to determine whether each one can have an animated transform and whether the
	//# nodes in its own subtree are eligible for animation.
	//#
	//# If a model's subtree contains other model nodes, then those nodes and their entire subtrees are
	//# not eligible for transform animation directly within the owning model and do not count toward the
	//# number of animated nodes.
	//#
	//# If a particular subnode of the model has the $kNodeAnimationInhibit$ flag set, then it is not
	//# counted as a node with an animated transform. In this case, if the $kNodeAnimateSubtree$ flag is
	//# <i>not</i> set for the same node, then the entire subtree is not elgible for animation and is not
	//# counted. Otherwise, if the $kNodeAnimateSubtree$ flag is set, then nodes in the subtree are counted
	//# if they are not disqualified from transform animation by any other rule.
	//#
	//# The table of pointers to the nodes with animated transforms in a model can be retrieved by calling
	//# the $@Model::GetAnimatedTransformNodeTable@$ function.
	//
	//# \also	$@Model::GetAnimatedTransformNodeTable@$
	//# \also	$@Model::GetTransformAnimationIndex@$
	//# \also	$@Model::GetRootAnimator@$
	//# \also	$@Model::SetRootAnimator@$
	//# \also	$@Model::Animate@$
	//# \also	$@Animator@$


	//# \function	Model::GetAnimatedTransformNodeTable	Returns the table of nodes with animated transforms in a model.
	//
	//# \proto	Node *const *GetAnimatedTransformNodeTable(void) const;
	//
	//# \desc
	//# The $GetAnimatedTransformNodeTable$ returns the table of pointers to the nodes that can have
	//# animatedtransforms in a model. The number of nodes in this table can be retrieved by calling
	//# the $@Model::GetAnimatedTransformNodeCount@$ function.
	//
	//# \also	$@Model::GetAnimatedTransformNodeCount@$
	//# \also	$@Model::GetTransformAnimationIndex@$
	//# \also	$@Model::GetRootAnimator@$
	//# \also	$@Model::SetRootAnimator@$
	//# \also	$@Model::Animate@$
	//# \also	$@Animator@$


	//# \function	Model::GetTransformAnimationIndex		Returns the index of a node with an animated transform.
	//
	//# \proto	int32 GetTransformAnimationIndex(const Node *node) const;
	//
	//# \param	node	The node within the model whose index is returned.
	//
	//# \desc
	//# The $GetTransformAnimationIndex$ function returns the index of the node specified by the $node$ parameter
	//# among all of the nodes with animated transforms belonging to a model. The index is always less than the total
	//# number of nodes with animated transforms returned by the $@Model::GetAnimatedTransformNodeCount@$ function.
	//# If the node specified by the $node$ parameter does not have an animated transform, then the return value is $-1$.
	//
	//# \also	$@Model::GetAnimatedTransformNodeCount@$
	//# \also	$@Model::GetRootAnimator@$
	//# \also	$@Model::SetRootAnimator@$
	//# \also	$@Model::Animate@$
	//# \also	$@Animator@$


	//# \function	Model::FindNode		Finds a node having a specific hash value.
	//
	//# \proto	Node *FindNode(const char *name) const;
	//# \proto	Node *FindNode(unsigned_int32 hash) const;
	//
	//# \param	name	The name of the node to search for.
	//# \param	hash	The hash value of the name of the node to search for.
	//
	//# \desc
	//# When a model is first preprocessed, it creates a hash table of pointers to its subnodes for
	//# quick access. The $FindNode$ function searches this table for a node having the name whose
	//# hash value is specified by the $hash$ parameter. If the node is found, then a pointer to it
	//# is returned. Otherwise, the return value is $nullptr$. If more than one node has the specified
	//# hash value because they have the same name, then a pointer to one of the nodes will be returned,
	//# but which one is undefined.
	//#
	//# If the $name$ parameter is specified instead of the $hash$ parameter, then the hash value of the
	//# name string is calculated automatically and passed to the version of the $FindNode$ function that
	//# takes a hash value.
	//#
	//# The hash value corresponding a name string can be calculated by calling the $@Utilities/Text::GetTextHash@$
	//# function at run time. A compile-time constant hash value for a name can be calculated using the
	//# $@Utilities/StaticHash@$ class template.
	//#
	//# A model node must have been added to a world and preprocessed before the $FindNode$ function
	//# can be called for it.
	//
	//# \also	$@Node::GetNodeName@$
	//# \also	$@Node::SetNodeName@$
	//# \also	$@Utilities/Text::GetTextHash@$
	//# \also	$@Utilities/StaticHash@$


	//# \function	Model::Animate			Runs the animators attached to a model.
	//
	//# \proto	void Animate(void);
	//
	//# \desc
	//# The $Animate$ function causes the animator tree attached to a model to calculate new transforms
	//# for the affected nodes within the model. This function is normally called from within the
	//# $@Controller/Controller::Move@$ function for a controller attached to the model node.
	//
	//# \also	$@Model::GetRootAnimator@$
	//# \also	$@Model::SetRootAnimator@$
	//# \also	$@Animator@$


	class Model : public Node, public ListElement<Model>, public Registrable<Model, ModelRegistration>
	{
		friend class Node;
		friend class ModelRegistration;

		private:

			enum
			{
				kModelHashBucketCount = 32
			};

			struct HashBucket
			{
				unsigned_int16		count;
				unsigned_int16		start;
			};

			ModelType						modelType;
			bool							loadedFlag;

			int32							transformNodeCount;
			int32							morphNodeCount;
			int32							morphWeightCount;

			Node							**modelHashTable;
			Node							**transformNodeTable;
			Geometry						**morphNodeTable;

			Animator						*rootAnimator;

			Link<RagdollController>			ragdollController;
			Array<Controller *, 4>			modelControllerArray;

			HashBucket						hashBucket[kModelHashBucketCount];

			Node *Replicate(void) const override;

			static Model *Create(Unpacker& data, unsigned_int32 unpackFlags);

			static void RagdollLinkProc(Node *node, void *cookie);

			void ExecuteAnimationFrame(float frame);

		protected:

			Model(const Model& model);

		public:

			C4API Model(ModelType type = kModelUnknown);
			C4API virtual ~Model();

			ModelType GetModelType(void) const
			{
				return (modelType);
			}

			int32 GetAnimatedTransformNodeCount(void) const
			{
				return (transformNodeCount);
			}

			int32 GetAnimatedMorphNodeCount(void) const
			{
				return (morphNodeCount);
			}

			int32 GetAnimatedMorphWeightCount(void) const
			{
				return (morphWeightCount);
			}

			Node *const *GetAnimatedTransformNodeTable(void) const
			{
				return (transformNodeTable);
			}

			Geometry *const *GetAnimatedMorphNodeTable(void) const
			{
				return (morphNodeTable);
			}

			Animator *GetRootAnimator(void) const
			{
				return (rootAnimator);
			}

			Node *FindNode(const char *name) const
			{
				return (FindNode(Text::Hash(name)));
			}

			static bool ActiveMorphController(const Controller *controller)
			{
				return ((controller) && (controller->GetBaseControllerType() == kControllerMorph) && (static_cast<const MorphController *>(controller)->GetMorphWeightCount() != 0));
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Invalidate(void) override;

			void Preprocess(void) override;
			void Neutralize(void) override;

			C4API Node *FindNode(unsigned_int32 hash) const;
			C4API int32 GetTransformAnimationIndex(const Node *node) const;

			C4API void SetRootAnimator(Animator *animator);
			C4API void Animate(void);

			virtual void Load(World *world);
			virtual void Unload(void);

			C4API static Model *New(const char *name, ModelType type = kModelUnknown, unsigned_int32 unpackFlags = 0);
			C4API static Model *Get(ModelType type);

			C4API RagdollController *AttachRagdoll(ModelType type);
	};


	//# \class	GenericModel	Represents a generic model node in a world.
	//
	//# The $GenericModel$ class represents a generic model node in a world.
	//
	//# \def	class GenericModel final : public Model, public ListElement<GenericModel>
	//
	//# \ctor	GenericModel(const char *name);
	//
	//# \param	name	The name of the model resource.
	//
	//# \desc
	//# The $GenericModel$ class serves as the root node for a generic model in a world.
	//# An instance of $GenericModel$ should be constructed to explicitly create a model that
	//# does not have a type that was previously registered through the $@ModelRegistration@$ class.
	//#
	//# When a $GenericModel$ node is created, the model resource specified by the $name$
	//# parameter is loaded as a subtree of the $GenericModel$ node. If another $GenericModel$ node
	//# already exists for the same name, then the resource is not reloaded, but a clone of the existing
	//# model is created to share the same object data.
	//#
	//# If no model resource matching the $name$ parameter can be found, then no subnodes are created
	//# beneath the $GenericModel$ node.
	//
	//# \base	Model									A $GenericModel$ node is a specific type of model.
	//# \base	Utilities/ListElement<GenericModel>		Used internally by the World Manager.
	//
	//# \also	$@ModelRegistration@$
	//# \also	$@Model::Get@$


	//# \function	GenericModel::GetModelName		Returns the model resource name.
	//
	//# \proto	const ResourceName& GetModelName(void) const;
	//
	//# \desc
	//# The $GetModelName$ function returns the name of the model resource used to create the generic model node.
	//
	//# \also	$@Model::GetModelType@$


	class GenericModel final : public Model, public ListElement<GenericModel>
	{
		friend class Model;

		private:

			ResourceName		modelName;

			GenericModel();
			C4API GenericModel(const GenericModel& genericModel);

			Node *Replicate(void) const override;

		public:

			C4API GenericModel(const char *name);
			C4API ~GenericModel();

			const ResourceName& GetModelName(void) const
			{
				return (modelName);
			}

			void SetModelName(const char *name)
			{
				modelName = name;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			void Load(World *world);
			void Unload(void);
	};


	//# \class	AnimationController		Plays animations for a model node.
	//
	//# The $AnimationController$ class plays animations for a model node.
	//
	//# \def	class AnimationController final : public Controller
	//
	//# \ctor	AnimationController();
	//
	//# \desc
	//# The $AnimationController$ class plays simple animations for a model by attaching
	//# a $@FrameAnimator@$ to the model node.
	//
	//# \base	Controller/Controller		A $AnimationController$ is a specific type of controller.
	//
	//# \also	$@Model@$


	//# \function	AnimationController::PlayAnimation		Plays an animation from a resource.
	//
	//# \proto	void PlayAnimation(const char *name, unsigned_int32 mode);
	//
	//# \param	name	The name of the animation resource.
	//# \param	mode	An interpolator mode for the animation.
	//
	//# \desc
	//# The $PlayAnimation$ function loads the animation resource specified by the $name$ parameter and
	//# plays the animation using the interpolator mode specified by the $mode$ parameter. See the
	//# $@TimeMgr/Interpolator@$ class for a description of the modes.
	//
	//# \also	$@AnimationController::StopAnimation@$


	//# \function	AnimationController::StopAnimation		Stops an animation.
	//
	//# \proto	void StopAnimation(void);
	//
	//# \desc
	//# The $StopAnimation$ function stops the frame interpolator for the current animation.
	//
	//# \also	$@AnimationController::PlayAnimation@$


	class AnimationController final : public Controller
	{
		private:

			unsigned_int32		animationMode;
			ResourceName		animationName;

			FrameAnimator		*frameAnimator;

			AnimationController(const AnimationController& animationController);

			Controller *Replicate(void) const override;

		public:

			enum
			{
				kAnimationMessageState
			};

			C4API AnimationController();
			C4API ~AnimationController();

			Model *GetTargetNode(void) const
			{
				return (static_cast<Model *>(Controller::GetTargetNode()));
			}

			FrameAnimator *GetFrameAnimator(void) const
			{
				return (frameAnimator);
			}

			static void RegisterFunctions(ControllerRegistration *registration);
			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;
			void SendInitialStateMessages(Player *player) const override;

			void Move(void) override;

			void PlayAnimation(const char *name, unsigned_int32 mode);
			void StopAnimation(void);
	};


	class AnimationStateMessage : public ControllerMessage
	{
		friend class AnimationController;

		private:

			unsigned_int32		animationMode;
			float				animatorValue;
			ResourceName		animationName;

			AnimationStateMessage(int32 controllerIndex);

		public:

			AnimationStateMessage(int32 controllerIndex, const char *name, float value, unsigned_int32 mode);
			~AnimationStateMessage();

			unsigned_int32 GetAnimationMode(void) const
			{
				return (animationMode);
			}

			float GetAnimatorValue(void) const
			{
				return (animatorValue);
			}

			const char *GetAnimationName(void) const
			{
				return (animationName);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class PlayAnimationFunction final : public Function
	{
		private:

			unsigned_int32		animationMode;
			ResourceName		animationName;

			PlayAnimationFunction(const PlayAnimationFunction& playAnimationFunction);

			Function *Replicate(void) const override;

		public:

			PlayAnimationFunction();
			~PlayAnimationFunction();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool OverridesFunction(const Function *function) const;
			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class StopAnimationFunction final : public Function
	{
		private:

			StopAnimationFunction(const StopAnimationFunction& stopAnimationFunction);

			Function *Replicate(void) const override;

		public:

			StopAnimationFunction();
			~StopAnimationFunction();

			bool OverridesFunction(const Function *function) const override;
			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
