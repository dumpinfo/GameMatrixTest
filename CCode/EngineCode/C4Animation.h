#ifndef C4Animation_h
#define C4Animation_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Creation.h"
#include "C4Resources.h"
#include "C4Time.h"


namespace C4
{
	//# \tree	Animator
	//
	//# \node	MergeAnimator
	//# \node	BlendAnimator
	//# \node	FrameAnimator


	typedef Type	AnimatorType;
	typedef Type	TrackType;
	typedef Type	CueType;


	//# \enum	AnimatorType

	enum : AnimatorType
	{
		kAnimatorMerge			= 'MERG',		//## Merge animator.
		kAnimatorBlend			= 'BLND',		//## Blend animator.
		kAnimatorFrame			= 'FRAM'		//## Frame animator.
	};


	enum : TrackType
	{
		kTrackTransform			= 'XFRM',
		kTrackMorphWeight		= 'MRPH',
		kTrackCue				= 'CUE '
	};


	class Model;


	struct TransformFrameData
	{
		Matrix3D		transform;
		Point3D			position;
	};


	struct TransformTrackHeader
	{
		struct NodeData
		{
			unsigned_int32		nodeHash;
			int32				transformIndex;
		};

		struct NodeBucket
		{
			unsigned_int16		bucketNodeCount;
			unsigned_int16		nodeDataOffset;
		};

		int32					transformNodeCount;
		unsigned_int32			transformFrameDataOffset;

		int32					bucketCount;
		NodeBucket				bucketData[1];

		const NodeData *GetNodeData(void) const
		{
			return (reinterpret_cast<const NodeData *>(bucketData + bucketCount));
		}

		const TransformFrameData *GetTransformFrameData(void) const
		{
			return (reinterpret_cast<const TransformFrameData *>(reinterpret_cast<const char *>(this) + transformFrameDataOffset));
		}
	};


	struct MorphWeightTrackHeader
	{
		struct NodeData
		{
			unsigned_int32		nodeHash;
			unsigned_int16		weightCount;
			unsigned_int16		dataOffset;
		};

		struct NodeBucket
		{
			unsigned_int16		bucketNodeCount;
			unsigned_int16		nodeDataOffset;
		};

		int32					morphWeightNodeCount;
		int32					frameMorphWeightCount;

		unsigned_int32			morphHashArrayOffset;
		unsigned_int32			morphWeightFrameDataOffset;

		int32					bucketCount;
		NodeBucket				bucketData[1];

		const NodeData *GetNodeData(void) const
		{
			return (reinterpret_cast<const NodeData *>(bucketData + bucketCount));
		}

		const unsigned_int32 *GetMorphHashArray(void) const
		{
			return (reinterpret_cast<const unsigned_int32 *>(reinterpret_cast<const char *>(this) + morphHashArrayOffset));
		}

		const float *GetMorphWeightFrameData(void) const
		{
			return (reinterpret_cast<const float *>(reinterpret_cast<const char *>(this) + morphWeightFrameDataOffset));
		}
	};


	struct CueData
	{
		CueType			cueType;
		float			cueTime;
	};


	struct CueTrackHeader
	{
		int32			cueCount;

		const CueData *GetCueData(void) const
		{
			return (reinterpret_cast<const CueData *>(this + 1));
		}
	};


	struct AnimationHeader
	{
		struct TrackData
		{
			TrackType			trackType;
			unsigned_int32		trackOffset;
		};

		int32				frameCount;
		float				frameDuration;

		int32				trackCount;
		TrackData			trackData[1];

		const void *GetTrackHeader(int32 index) const
		{
			return (reinterpret_cast<const char *>(this) + trackData[index].trackOffset);
		}
	};


	class AnimationResource : public Resource<AnimationResource>
	{
		friend class Resource<AnimationResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			~AnimationResource();

		public:

			static C4API const unsigned_int32	resourceSignature[2];

			C4API AnimationResource(const char *name, ResourceCatalog *catalog);

			int32 GetVersion(void) const
			{
				return (static_cast<const int32 *>(GetData())[1]);
			}

			const AnimationHeader *GetAnimationHeader(void) const
			{
				return (reinterpret_cast<const AnimationHeader *>(&reinterpret_cast<const int32 *>(GetData())[2]));
			}
	};


	//# \struct	AnimatorTransform		Holds a single animated node transform.
	//
	//# The $AnimatorTransform$ structure holds the transform for a single animated node.
	//
	//# \data	AnimatorTransform
	//
	//# \desc
	//# The $AnimatorTransform$ structure holds the position and orientation of a single animated node.
	//# An $@Animator@$ tree generates these transforms when animating a $@Model@$. A custom animator
	//# subclass allocates space for a table of $AnimatorTransform$ structures when it calls the
	//# $@Animator::AllocateStorage@$ function.
	//
	//# \also	$@Animator@$
	//# \also	$@Animator::AllocateStorage@$


	//# \member		AnimatorTransform

	struct AnimatorTransform
	{
		Quaternion		rotation;		//## A quaternion representing the orientation of the node.
		Point3D			position;		//## A point representing the position of the node.
	};


	//# \class	Animator		The base class for all model animation classes.
	//
	//# The $Animator$ class is the base class for all model animation classes.
	//
	//# \def	class Animator : public Tree<Animator>, public Packable, public Creatable<Animator>
	//
	//# \ctor	Animator(AnimatorType type, Model *model, Node *node = nullptr);
	//
	//# \param	type		The animator type.
	//# \param	model		The model to which the animator applies.
	//# \param	node		The target node, which is the root of the subtree within the model that is affected by the animator. If this is $nullptr$, then the root is the same as the $model$ parameter.
	//
	//# \desc
	//# The $Animator$ class is the base class for a variety of model animation classes that can be attached to
	//# a model node. A model node may have a single animator attached to it, or it may have multiple animators
	//# attached to it organized in a tree hierarchy. When a model is animated by calling the $@Model::Animate@$
	//# function, the animator objects attached to the model calculate new transforms for the nodes belonging to
	//# the model.
	//
	//# \base	Utilities/Tree<Animator>		Animators are organized in a tree hierarchy.
	//# \base	ResourceMgr/Packable			Animators can be packed for storage in resources.
	//# \base	System/Creatable<Animator>		New animator subclasses may be defined by an application, and a creator
	//#											function can be installed using the $Creatable$ class.
	//
	//# \also	$@MergeAnimator@$
	//# \also	$@BlendAnimator@$
	//# \also	$@FrameAnimator@$
	//# \also	$@Model::GetRootAnimator@$
	//# \also	$@Model::SetRootAnimator@$


	//# \function	Animator::GetAnimatorType		Returns the animator type.
	//
	//# \proto	AnimatorType GetAnimatorType(void) const;
	//
	//# \desc
	//# The $GetAnimatorType$ function returns the animator type. This may be one of the standard animator types
	//# in the following table or a custom animator type.
	//
	//# \table	AnimatorType


	//# \function	Animator::GetTargetModel		Returns the model to which an animator applies.
	//
	//# \proto	Model *GetTargetModel(void) const;
	//
	//# \desc
	//# The $GetTargetModel$ function returns the model node to which an animator is attached as part of the model's
	//# animator tree. The target model is established when the animator is constructed, and it cannot be changed.
	//#
	//# It is possible that the animator only affects a part of the whole model, and in that case, the
	//# $@Animator::GetTargetNode@$ function can be called to retrieve the root node of the affected subtree
	//# within the model.
	//
	//# \also	$@Animator::GetTargetNode@$
	//# \also	$@Animator::SetTargetNode@$


	//# \function	Animator::GetTargetNode			Returns the root of the subtree within the model that is affected by an animator.
	//
	//# \proto	Node *GetTargetNode(void) const;
	//
	//# \desc
	//# The $GetTargetNode$ function returns the root node of the subtree within the model that is affected by an animator.
	//# This node is often the same as the model node itself, meaning that the animator applies to the entire model, but
	//# it can be another node inside the model's node hierarchy in the case that the animator applies only to some part
	//# of the whole model. The target node is established when the animator is constructed, and it can later be changed
	//# by calling the $@Animator::SetTargetNode@$ function.
	//
	//# \also	$@Animator::SetTargetNode@$
	//# \also	$@Animator::GetTargetModel@$


	//# \function	Animator::SetTargetNode			Sets the root of the subtree within the model that is affected by an animator.
	//
	//# \proto	void SetTargetNode(Node *node);
	//
	//# \param	node	The new target node. This must be an animated node in the subtree rooted at the model node to which the animator applies.
	//
	//# \desc
	//# The $SetTargetMode$ function changes the target node of an animator to the node specified by the $node$ parameter.
	//# An animator affects only the target node within a model and its successor nodes in the tree hierarchy. The target
	//# node is often the same as the model node itself, meaning that the animator applies to the entire model, but a
	//# different target node can be specified when the animator is constructed or later through the $SetTargetNode$
	//# function to cause the animator to be applied only to part of the whole model.
	//#
	//# If the target node is changed after the $@Model::SetRootAnimator@$ function is called to attach the animator tree to
	//# a model, then the $@Animator::Preprocess@$ function must be called for the animator before the model is animated again.
	//
	//# \also	$@Animator::GetTargetNode@$
	//# \also	$@Animator::GetTargetModel@$


	//# \function	Animator::GetAnimatorTransformNodeStart		Returns the index of the first node targeted for transform animation.
	//
	//# \proto	int32 GetAnimatorTransformNodeStart(void) const;
	//
	//# \desc
	//# The $GetAnimatorTransformNodeStart$ function returns the index of the first animated node targeted by an animator.
	//# This index has a value in the range [0,&nbsp;<i>n</i>), where <i>n</i> is the number of animated nodes returned by
	//# the $@Model::GetAnimatedTransformNodeCount@$ function. The starting index is calculated when the animator is
	//# preprocessed based on the target node of the animator.
	//#
	//# The index of the first node actually affected by an animator may be larger than the index returned by the
	//# $GetAnimatorTransformNodeStart$ function. The actual range of affected nodes can be retrieved by calling the
	//# $@Animator::GetOutputTransformNodeStart@$ and $@Animator::GetOutputTransformNodeCount@$ functions.
	//
	//# \also	$@Animator::GetAnimatorTransformNodeCount@$
	//# \also	$@Animator::GetOutputTransformNodeStart@$
	//# \also	$@Animator::GetOutputTransformNodeCount@$


	//# \function	Animator::GetAnimatorTransformNodeCount		Returns the number of nodes targeted for transform animation.
	//
	//# \proto	int32 GetAnimatorTransformNodeCount(void) const;
	//
	//# \desc
	//# The $GetAnimatorTransformNodeCount$ function returns the number of animated nodes targeted by an animator.
	//# This number is at most the number of animated nodes returned by the $@Model::GetAnimatedTransformNodeCount@$ function.
	//#
	//# The number of nodes actually affected by an animator may be smaller than the number returned by the
	//# $GetAnimatorTransformNodeCount$ function. The actual range of affected nodes can be retrieved by calling the
	//# $@Animator::GetOutputTransformNodeStart@$ and $@Animator::GetOutputTransformNodeCount@$ functions.
	//
	//# \also	$@Animator::GetAnimatorTransformNodeStart@$
	//# \also	$@Animator::GetOutputTransformNodeStart@$
	//# \also	$@Animator::GetOutputTransformNodeCount@$


	//# \function	Animator::GetOutputTransformNodeStart		Returns the index of the first node for which transform animation is actually output.
	//
	//# \proto	int32 GetOutputTransformNodeStart(void) const;
	//
	//# \desc
	//# The $GetOutputTransformNodeStart$ function returns the index of the first node for which transform animation is actually
	//# output by an animator. This index is always in the range of indexes given by the $@Animator::GetAnimatorTransformNodeStart@$
	//# and $@Animator::GetAnimatorTransformNodeCount@$ functions, but it may be larger than the index of the first targeted
	//# node if an animator does not output transforms for all of the nodes in the targeted range.
	//
	//# \also	$@Animator::GetOutputTransformNodeCount@$
	//# \also	$@Animator::GetAnimatorTransformNodeStart@$
	//# \also	$@Animator::GetAnimatorTransformNodeCount@$


	//# \function	Animator::GetOutputTransformNodeCount		Returns the number of nodes for which transform animation is actually output.
	//
	//# \proto	int32 GetOutputTransformNodeCount(void) const;
	//
	//# \desc
	//# The $GetOutputTransformNodeCount$ function returns the number of nodes for which animation is actually output by an
	//# animator. This number is always no greater than the count given by the $@Animator::GetAnimatorTransform NodeCount@$
	//# function, but it may be smaller than the total number of targeted nodes if an animator does not output transforms for
	//# all of the nodes in the targeted range.
	//
	//# \also	$@Animator::GetOutputTransformNodeStart@$
	//# \also	$@Animator::GetAnimatorTransformNodeStart@$
	//# \also	$@Animator::GetAnimatorTransformNodeCount@$


	//# \function	Animator::GetOutputTransformTable			Returns the output table containing pointers to node transforms.
	//
	//# \proto	AnimatorTransform **GetOutputTransformTable(void) const;
	//
	//# \desc
	//# The $GetOutputTransformTable$ function returns the output table that contains a pointer to an $@AnimatorTransform@$
	//# structure for each node actually affected by an animator. The output table is created by the $@Animator::AllocateStorage@$
	//# function, and its size can be retrieved by calling the $@Animator::GetOutputTransformNodeCount@$ function. The output table
	//# is normally filled with data by the implementation of the $@Animator::Configure@$ function.
	//
	//# \also	$@Animator::GetOutputTransformNodeStart@$
	//# \also	$@Animator::GetOutputTransformNodeCount@$
	//# \also	$@Animator::GetAnimatorTransformTable@$
	//# \also	$@Animator::AllocateStorage@$
	//# \also	$@Animator::Configure@$


	//# \function	Animator::GetAnimatorTransformTable			Returns the transform table containing new node transforms.
	//
	//# \proto	AnimatorTransform *GetAnimatorTransformTable(void) const;
	//
	//# \desc
	//# The $GetAnimatorTransformTable$ function returns the transform table that contains new $@AnimatorTransform@$
	//# structures calculated by the animator in its $@Animator::Move@$ function. The transform table is created when
	//# the $@Animator::AllocateStorage@$ function is called, and pointers to entries in the transform table are stored
	//# in an animator's output transform table.
	//
	//# \also	$@Animator::GetOutputTransformTable@$
	//# \also	$@Animator::GetOutputTransformNodeStart@$
	//# \also	$@Animator::GetOutputTransformNodeCount@$
	//# \also	$@Animator::AllocateStorage@$


	//# \function	Animator::GetAnimatorData		Returns a pointer to the animator's custom storage space.
	//
	//# \proto	void *GetAnimatorData(void) const;
	//
	//# \desc
	//# The $GetAnimatorData$ function returns a pointer to the additional storage previously requested through a
	//# call to the $@Animator::AllocateStorage@$ function. The returned pointer is 4-byte aligned and points to
	//# a storage area having size equal to the value of the $animatorDataSize$ parameter that was passed to the
	//# $AllocateStorage$ function.
	//
	//# \also	$@Animator::AllocateStorage@$


	//# \function	Animator::GetWeightInterpolator		Returns the interpolator used to modify an animator's weight.
	//
	//# \proto	Interpolator *GetWeightInterpolator(void);
	//# \proto	const Interpolator *GetWeightInterpolator(void) const;
	//
	//# \desc
	//# The $GetWeightInterpolator$ function returns a pointer to the $@TimeMgr/Interpolator@$ object representing
	//# the weight of an animator. The animator weights are used by the $@BlendAnimator@$ class to combine the outputs
	//# of its subanimators. Custom animator classes may also use the weights of their subanimators in any way they want.
	//
	//# \also	$@TimeMgr/Interpolator@$
	//# \also	$@BlendAnimator@$


	//# \function	Animator::AllocateStorage		Allocates storage space for an animator's tables.
	//
	//# \proto	void AllocateStorage(int32 nodeStart, int32 nodeCount, int32 transformDataCount, unsigned_int32 animatorDataSize = 0);
	//
	//# \param	nodeStart				The index of the first node that is actually animated.
	//# \param	nodeCount				The total number of nodes that are actually animated.
	//# \param	transformDataCount		The number of new transforms calculated by the animator.
	//# \param	animatorDataSize		The size of extra storage space needed by the animator, in bytes.
	//
	//# \desc
	//# The $AllocateStorage$ function should be called by an animator's implementation of the $@Animator::Configure@$
	//# function to allocate space for the various tables to be used when the animator does its job. In particular,
	//# the $AllocateStorage$ function allocates space for an <i>output transform table</i> that holds pointers to the
	//# transforms of the animated nodes, and it allocates space for an <i>animator transform table</i> that holds any
	//# new transforms that are calculated by the animator inside its $@Animator::Move@$ function.
	//#
	//# After the $AllocateStorage$ function has been called, the output table can be retrieved by calling the
	//# $@Animator::GetOutputTransform Table@$ function. The number of entries in this table is given by the $nodeCount$
	//# parameter, and these entries represent the output transforms for $nodeCount$ consecutive animated nodes beginning
	//# at the index given by the $nodeStart$ parameter. The $@Animator::GetOutputTransformNodeStart@$ and
	//# $@Animator::GetOutputTransformNodeCount@$ functions subsequently return the values specified for the $nodeStart$
	//# and $nodeCount$ parameters.
	//#
	//# The animator transform table can be retrieved by calling the $@Animator::GetAnimatorTransformTable@$ function.
	//# The number of entries in this table is given by the $transformDataCount$ parameter, which only needs to be as large
	//# as the number of new transforms calculated by the animator. An animator is not required to calculate new transforms
	//# for all of the nodes that it animates and may reuse transforms calculated by subanimators. In particular, the
	//# $transformDataCount$ parameter can be zero when all transforms are taken from subanimators, as is the case for
	//# the $@MergeAnimator@$ class.
	//#
	//# The $animatorDataSize$ parameter specifies the size of any additional storage space that may be required by the
	//# animator to be used in any custom manner. If this is not zero, then a pointer to this storage can be retrieved by
	//# calling the $@Animator::GetAnimatorData@$ function.
	//
	//# \also	$@Animator::Preprocess@$
	//# \also	$@Animator::Move@$
	//# \also	$@Animator::GetAnimatorData@$


	//# \function	Animator::Preprocess		Called to allow an animator to perform any necessary preprocessing.
	//
	//# \proto	virtual void Preprocess(void);
	//
	//# \desc
	//# The $Preprocess$ function is called to allow an animator to perform any necessary preprocessing before it
	//# is used to animate a model. If a custom animator subclasses overrides this function, then it must always
	//# call the base class counterpart before performing any other actions. The base class implementation calls
	//# the $Preprocess$ function for all of its subanimators before returning.
	//#
	//# The $Preprocess$ function is called for the root animator in an animator tree when the $@Model::SetRootAnimator@$
	//# function is called to attached the animator tree to a model.
	//
	//# \also	$@Animator::Configure@$
	//# \also	$@Animator::Move@$
	//# \also	$@Model::SetRootAnimator@$


	//# \function	Animator::Configure			Called when an animator is affected by a change in the animation tree for a model.
	//
	//# \proto	virtual void Configure(void) = 0;
	//
	//# \desc
	//# The $Configure$ function is called when the animator tree is being initialized. A custom animator subclass must override
	//# this function, and its implementation should call the $@Animator::AllocateStorage@$ function with the appropriate parameters.
	//# The $Configure$ function is called for all subanimators before it is called for the predecessor in the animator tree, so the
	//# output information returned for subanimators is valid and may be used to determine the range of nodes actually affected by
	//# the animator.
	//#
	//# The animator for which the $Configure$ function is called must use the $@Animator::GetAnimatorTransformNodeStart@$ and
	//# $@Animator::GetAnimatorTransformNodeCount@$ functions to determine the maximum range of nodes that it can actually affect.
	//# The values of the $nodeStart$ and $nodeCount$ parameters passed to the $@Animator::AllocateStorage@$ function must not
	//# specify any nodes outside of this range, but they can specify a contiguous subset of nodes to be animated.
	//#
	//# An overriding implementation of the $Configure$ function should generally fill its output transform table with pointers
	//# to the transforms that it either calculates itself (stored in the animator transform table) or passes through from its
	//# subanimators. The $@Animator::Move@$ function then only needs to calculate the new transforms already referenced by
	//# entries of the output transform table. The output transform table may contain entries that are $nullptr$, and in this
	//# case no transform animation is applied to the corresponding node.
	//
	//# \also	$@Animator::AllocateStorage@$
	//# \also	$@Animator::Preprocess@$
	//# \also	$@Animator::Move@$


	//# \function	Animator::Move				Called to allow the animator to calculate its output.
	//
	//# \proto	virtual void Move(void);
	//
	//# \desc
	//# The $Move$ function is called for each animator in an animator tree when the $@Model::Animate@$ function is called
	//# for the model to which the animator tree is assigned. The $Move$ function can be overridden by any custom animator
	//# subclass, and the first action taken by the overriding implementation must be to call the base class's $Move$ function.
	//# The $Move$ function does not need to be overridden if all of the pointers in an animator's transform output table refer
	//# to transforms belonging to subanimators (and thus the animator calculates no transforms of its own).
	//#
	//# An overriding implementation must fill its animator transform table with any new transforms that it calculates.
	//# Pointers to these transforms are generally stored in the output transform table inside the overriding implementation
	//# of the $@Animator::Configure@$ function so that they don't need to be stored each time the $Move$ function is called.
	//
	//# \also	$@Animator::GetOutputTransformNodeCount@$
	//# \also	$@Animator::GetOutputTransformTable@$
	//# \also	$@Animator::Preprocess@$
	//# \also	$@Animator::Configure@$


	class Animator : public Tree<Animator>, public Packable, public Creatable<Animator>
	{
		private:

			AnimatorType				animatorType;
			bool						updateFlag;

			Model						*targetModel;
			Node						*targetNode;

			int32						outputTransformNodeStart;
			int32						outputTransformNodeCount;

			int32						outputMorphNodeStart;
			int32						outputMorphNodeCount;
			int32						outputMorphWeightStart;
			int32						outputMorphWeightCount;

			int32						animatorTransformNodeStart;
			int32						animatorTransformNodeCount;

			int32						animatorMorphNodeStart;
			int32						animatorMorphNodeCount;
			int32						animatorMorphWeightStart;
			int32						animatorMorphWeightCount;

			unsigned_int32				storageSize;
			char						*outputStorage;
			void						*animatorData;

			AnimatorTransform			**outputTransformTable;
			float						**outputMorphWeightTable;

			AnimatorTransform			*animatorTransformTable;
			float						*animatorMorphWeightTable;

			Interpolator				weightInterpolator;

			static Animator *Create(Unpacker& data, unsigned_int32 unpackFlags = 0);

		protected:

			C4API Animator(AnimatorType type);
			C4API Animator(AnimatorType type, Model *model, Node *node = nullptr);

			void *GetAnimatorData(void) const
			{
				return (animatorData);
			}

			C4API void AllocateStorage(int32 nodeStart, int32 nodeCount, int32 transformDataCount, unsigned_int32 animatorDataSize = 0);
			C4API void AllocateStorage(int32 transformNodeStart, int32 transformNodeCount, int32 transformDataCount, int32 morphNodeStart, int32 morphNodeCount, int32 morphWeightStart, int32 morphWeightCount, int32 morphDataCount, unsigned_int32 animatorDataSize = 0);

		public:

			C4API virtual ~Animator();

			AnimatorType GetAnimatorType(void) const
			{
				return (animatorType);
			}

			Model *GetTargetModel(void) const
			{
				return (targetModel);
			}

			Node *GetTargetNode(void) const
			{
				return (targetNode);
			}

			void SetTargetNode(Node *node)
			{
				targetNode = node;
			}

			int32 GetOutputTransformNodeStart(void) const
			{
				return (outputTransformNodeStart);
			}

			int32 GetOutputTransformNodeCount(void) const
			{
				return (outputTransformNodeCount);
			}

			int32 GetOutputMorphNodeStart(void) const
			{
				return (outputMorphNodeStart);
			}

			int32 GetOutputMorphNodeCount(void) const
			{
				return (outputMorphNodeCount);
			}

			int32 GetOutputMorphWeightStart(void) const
			{
				return (outputMorphWeightStart);
			}

			int32 GetOutputMorphWeightCount(void) const
			{
				return (outputMorphWeightCount);
			}

			int32 GetAnimatorTransformNodeStart(void) const
			{
				return (animatorTransformNodeStart);
			}

			int32 GetAnimatorTransformNodeCount(void) const
			{
				return (animatorTransformNodeCount);
			}

			int32 GetAnimatorMorphNodeStart(void) const
			{
				return (animatorMorphNodeStart);
			}

			int32 GetAnimatorMorphNodeCount(void) const
			{
				return (animatorMorphNodeCount);
			}

			int32 GetAnimatorMorphWeightStart(void) const
			{
				return (animatorMorphWeightStart);
			}

			int32 GetAnimatorMorphWeightCount(void) const
			{
				return (animatorMorphWeightCount);
			}

			AnimatorTransform **GetOutputTransformTable(void) const
			{
				return (outputTransformTable);
			}

			AnimatorTransform *GetAnimatorTransformTable(void) const
			{
				return (animatorTransformTable);
			}

			float **GetOutputMorphWeightTable(void) const
			{
				return (outputMorphWeightTable);
			}

			float *GetAnimatorMorphWeightTable(void) const
			{
				return (animatorMorphWeightTable);
			}

			Interpolator *GetWeightInterpolator(void)
			{
				return (&weightInterpolator);
			}

			const Interpolator *GetWeightInterpolator(void) const
			{
				return (&weightInterpolator);
			}

			void AppendNewSubnode(Animator *animator)
			{
				AppendSubnode(animator);
				animator->Preprocess();
			}

			C4API static Animator *New(AnimatorType type);

			C4API void PackType(Packer& data) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void Detach(void) override;

			C4API void AppendSubnode(Animator *node) override;
			C4API void PrependSubnode(Animator *node) override;
			C4API void InsertSubnodeBefore(Animator *node, Animator *before) override;
			C4API void InsertSubnodeAfter(Animator *node, Animator *after) override;
			C4API void RemoveSubnode(Animator *node) override;

			C4API void Invalidate(void);
			C4API void Update(void);

			C4API void SetTargetModel(Model *model, Node *node = nullptr);
			C4API int32 GetNodeTransformIndex(const Node *target) const;

			C4API void Premove(void);

			C4API virtual void Preprocess(void);
			C4API virtual void Configure(void) = 0;
			C4API virtual void Move(void);
	};


	//# \class	MergeAnimator		An animator that merges outputs from its subanimators.
	//
	//# The $MergeAnimator$ class is an animator that merges outputs from its subanimators.
	//
	//# \def	class MergeAnimator : public Animator
	//
	//# \ctor	MergeAnimator(Model *model, Node *node = nullptr);
	//
	//# \param	model		The model to which the animator applies.
	//# \param	node		The target node, which is the root of the subtree within the model that is affected by the animator. If this is $nullptr$, then the root is the same as the $model$ parameter.
	//
	//# \desc
	//# The $MergeAnimator$ class is used to merge the outputs of multiple animators applied to different parts of a model.
	//# The range of nodes affected by a merge animator is the union of the ranges affected by its immediate subanimators,
	//# restricted to the range of the merge animator itself.
	//#
	//# The $MergeAnimator$ class supports any number of subanimators. If any subanimators have overlapping ranges of affected
	//# nodes, then subanimators further to the right in the animator tree take priority over their predecessors.
	//
	//# \base	Animator	The $MergeAnimator$ class is a special type of animator.
	//
	//# \also	$@BlendAnimator@$


	class MergeAnimator : public Animator
	{
		public:

			C4API MergeAnimator();
			C4API MergeAnimator(Model *model, Node *node = nullptr);
			C4API ~MergeAnimator();

			C4API void Configure(void);
	};


	//# \class	BlendAnimator		An animator that blends outputs from its subanimators.
	//
	//# The $BlendAnimator$ class is an animator that blends outputs from its subanimators.
	//
	//# \def	class BlendAnimator : public Animator
	//
	//# \ctor	BlendAnimator(Model *model, Node *node = nullptr);
	//
	//# \param	model		The model to which the animator applies.
	//# \param	node		The target node, which is the root of the subtree within the model that is affected by the animator. If this is $nullptr$, then the root is the same as the $model$ parameter.
	//
	//# \desc
	//# The $BlendAnimator$ class is used to blend the outputs of two subanimators based on their current weights.
	//# The range of nodes affected by a blend animator is the union of the ranges affected by its immediate subanimators,
	//# restricted to the range of the blend animator itself. Blending takes place only inside the intersection of these
	//# ranges, and nodes outside of the intersection have their transforms passed through from the subanimators.
	//#
	//# The weights used by the blend animator are given by the current values of the weight interpolators belonging to
	//# the two subanimators. These interpolators are retrieved by calling the $@Animator::GetWeightInterpolator@$ function.
	//# The weights are always normalized by the blend animator, so they do not need to sum to one.
	//#
	//# If a blend animator has only one subanimator, then the transforms output by the subanimator are simply passed through.
	//
	//# \base	Animator	The $BlendAnimator$ class is a special type of animator.
	//
	//# \also	$@MergeAnimator@$


	class BlendAnimator : public Animator
	{
		private:

			int32		blendTransformStart;
			int32		blendTransformCount;

			int32		blendMorphNodeStart;
			int32		blendMorphWeightStart;
			int32		blendMorphNodeCount;
			int32		blendMorphWeightCount;

		public:

			C4API BlendAnimator();
			C4API BlendAnimator(Model *model, Node *node = nullptr);
			C4API ~BlendAnimator();

			C4API void Configure(void) override;
			C4API void Move(void) override;
	};


	//# \class	FrameAnimator		An animator that outputs transforms from frame data in an animation resource.
	//
	//# The $FrameAnimator$ class is an animator that outputs transforms from frame data in an animation resource.
	//
	//# \def	class FrameAnimator : public Animator, public ExclusiveObservable<FrameAnimator, CueType>
	//
	//# \ctor	FrameAnimator(Model *model, Node *node = nullptr);
	//
	//# \param	model		The model to which the animator applies.
	//# \param	node		The target node, which is the root of the subtree within the model that is affected by the animator. If this is $nullptr$, then the root is the same as the $model$ parameter.
	//
	//# \desc
	//# The $FrameAnimator$ class is used to apply the data stored in an animation resource to a model. A particular anaimation
	//# resource is specified by calling the $@FrameAnimator::SetAnimation@$ function, and animation playback is controlled
	//# through the $@TimeMgr/Interpolator@$ object returned by the $@FrameAnimator::GetFrameInterpolator@$ function.
	//#
	//# An animation resource may contain cues that are triggered at specific points in the animation. When a cue is passed in
	//# the playback of an animation, the $FrameAnimator$ class sends an event to its observer. An observer is installed by
	//# passing a pointer to a $@FrameAnimatorObserver@$ object to the $@Utilities/ExclusiveObservable::SetObserver@$ function,
	//# called for the $FrameAnimator$ object.
	//
	//# \base	Animator												The $FrameAnimator$ class is a special type of animator.
	//# \base	Utilities/ExclusiveObservable<FrameAnimator, CueType>	A $FrameAnimator$ object can be observed for animation cue events.


	//# \function	FrameAnimator::SetAnimation		Sets the animation resource from which the animator generates its output.
	//
	//# \proto	void SetAnimation(const char *name);
	//
	//# \param	name	The name of the animation resource.
	//
	//# \desc
	//# The $SetAnimation$ function sets the current animation for a frame animator to the animation stored
	//# in the resource specified by the $name$ parameter. Animations are typically stored in a subfolder
	//# having the model resource's name, and the $name$ parameter should include this. For example, to
	//# apply an animation named "walk" to a model named "monster", the $name$ parameter should point
	//# to the string "monster/walk". (A forward slash should be used on all platforms.)


	//# \function	FrameAnimator::GetFrameInterpolator		Returns the interpolator used to play animation frames.
	//
	//# \proto	Interpolator *GetFrameInterpolator(void);
	//# \proto	const Interpolator *GetFrameInterpolator(void) const;
	//
	//# \desc
	//# The $GetFrameInterpolator$ function returns the $@TimeMgr/Interpolator@$ object that controls the playback
	//# of an animation resource. The functions of the $Interpolator$ class are used to play and stop the
	//# animation as well as specify properties such as the playback rate, looping mode, and time range.
	//#
	//# A completion function can be installed for the $Interpolator$ object using the $@Utilities/Completable::SetCompletionProc@$
	//# function in order to receive a callback when an animation finishes playing.
	//
	//# \also	$@TimeMgr/Interpolator@$


	class FrameAnimator : public Animator, public ExclusiveObservable<FrameAnimator, CueType>
	{
		friend class Animator;

		private:

			AnimationResource				*animationResource;
			const AnimationHeader			*animationHeader;

			const TransformTrackHeader		*transformTrackHeader;
			const MorphWeightTrackHeader	*morphWeightTrackHeader;
			const CueTrackHeader			*cueTrackHeader;

			int16							*transformRemapTable;
			int16							*morphWeightRemapTable;

			float							animationDuration;
			float							animationFrame;

			float							frameFrequency;
			Interpolator					frameInterpolator;

			void GenerateNodeRemapTables(void) const;
			void ExecuteAnimationFrame(float frame);

		public:

			C4API FrameAnimator();
			C4API FrameAnimator(Model *model, Node *node = nullptr);
			C4API ~FrameAnimator();

			const AnimationHeader *GetAnimationHeader(void) const
			{
				return (animationHeader);
			}

			const TransformTrackHeader *GetTransformTrackHeader(void) const
			{
				return (transformTrackHeader);
			}

			const MorphWeightTrackHeader *GetMorphWeightTrackHeader(void) const
			{
				return (morphWeightTrackHeader);
			}

			const CueTrackHeader *GetCueTrackHeader(void) const
			{
				return (cueTrackHeader);
			}

			float GetAnimationDuration(void) const
			{
				return (animationDuration);
			}

			float GetFrameFrequency(void) const
			{
				return (frameFrequency);
			}

			Interpolator *GetFrameInterpolator(void)
			{
				return (&frameInterpolator);
			}

			const Interpolator *GetFrameInterpolator(void) const
			{
				return (&frameInterpolator);
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void Configure(void) override;
			C4API void Move(void) override;

			C4API void SetAnimationHeader(const AnimationHeader *header);
			C4API void SetAnimation(const char *name);
	};


	//# \class	FrameAnimatorObserver		An observer for cues triggered by a $FrameAnimator$ object.
	//
	//# The $FrameAnimatorObserver$ class is an observer that can be installed on a $@FrameAnimator@$ object to listen for animation cues.
	//
	//# \def	template <class observerType> class FrameAnimatorObserver : public ExclusiveObserver<observerType, FrameAnimator>
	//
	//# \tparam		observerType		The type of the class for which a notification function is called when an event occurs.
	//
	//# \ctor	FrameAnimatorObserver(observerType *observer, void (observerType::*callback)(FrameAnimator *, CueType));
	//
	//# \param	observer	A pointer to the object for which a notification function is called when an event occurs.
	//# \param	callback	A pointer to a member function (the notification function) of the object specified by the $observer$ parameter that is called when an event occurs.
	//
	//# \desc
	//# The $FrameAnimatorObserver$ class is an observer that can be installed on a $@FrameAnimator@$ object to listen for
	//# animation cues. Once it has been installed by calling the $@Utilities/ExclusiveObservable::SetObserver@$ function
	//# for a $FrameAnimator$ object, the notification function specified by the $callback$ parameter is called whenever
	//# a playing animation passes a cue, as defined inside the animation resource. A pointer to the $FrameAnimator$ object
	//# and the type of the cue are passed to the notification function.
	//
	//# \base	Utilities/ExclusiveObserver<observerType, FrameAnimator>	A $FrameAnimatorObserver$ object is a specific type of exclusive observer.


	template <class observerType> class FrameAnimatorObserver : public ExclusiveObserver<observerType, FrameAnimator>
	{
		public:

			FrameAnimatorObserver(observerType *observer, void (observerType::*callback)(FrameAnimator *, CueType)) : ExclusiveObserver<observerType, FrameAnimator>(observer, callback)
			{
			}
	};
}


#endif

// ZYUQURM
