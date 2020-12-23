 

#ifndef C4Types_h
#define C4Types_h


//# \component	Utility Library
//# \prefix		Utilities/


#include "C4Constants.h"
#include "C4Shared.h"
#include "C4Array.h"
#include "C4Tree.h"
#include "C4Graph.h"
#include "C4Map.h"
#include "C4Hash.h"
#include "C4Link.h"
#include "C4Rect.h"
#include "C4String.h"
#include "C4Complex.h"
#include "C4Bivector4D.h"
#include "C4Quaternion.h"
#include "C4Completable.h"
#include "C4Observable.h"


namespace C4
{
	typedef unsigned_int32		Type;
	typedef Type				EventType;


	//# \enum	KeyCode

	enum
	{
		kKeyCodeEnter			= 13,		//## The enter key (return key on the Mac).
		kKeyCodeEscape			= 27,		//## The escape key.
		kKeyCodeTab				= 9,		//## The tab key.
		kKeyCodeLeftArrow		= 28,		//## The left arrow key.
		kKeyCodeRightArrow		= 29,		//## The right arrow key.
		kKeyCodeUpArrow			= 30,		//## The up arrow key.
		kKeyCodeDownArrow		= 31,		//## The down arrow key.
		kKeyCodePageUp			= 11,		//## The page up key.
		kKeyCodePageDown		= 12,		//## The page down key.
		kKeyCodeHome			= 1,		//## The home key.
		kKeyCodeEnd				= 4,		//## The end key.
		kKeyCodeDelete			= 2,		//## The delete key.
		kKeyCodeBackspace		= 8,		//## The backspace key.
		kKeyCodeF1				= 128,		//## The F1 key.
		kKeyCodeF2,							//## The F2 key.
		kKeyCodeF3,							//## The F3 key.
		kKeyCodeF4,							//## The F4 key.
		kKeyCodeF5,							//## The F5 key.
		kKeyCodeF6,							//## The F6 key.
		kKeyCodeF7,							//## The F7 key.
		kKeyCodeF8,							//## The F8 key.
		kKeyCodeF9,							//## The F9 key.
		kKeyCodeF10,						//## The F10 key.
		kKeyCodeF11,						//## The F11 key.
		kKeyCodeF12,						//## The F12 key.
		kKeyCodeF13,
		kKeyCodeF14,
		kKeyCodeF15,
		kKeyCodeF16,
		kKeyCodeF17,
		kKeyCodeF18,
		kKeyCodeF19,
		kKeyCodeF20,
		kKeyCodeF21,
		kKeyCodeF22,
		kKeyCodeF23,
		kKeyCodeF24
	};


	//# \enum	MouseEventFlags

	enum
	{
		kMouseDoubleClick		= 1 << 0		//## The mouse down event is the second click in a double-click.
	};


	//# \enum	KeyboardModifiers

	enum
	{
		kModifierKeyShift		= 1 << 0,		//## The shift key was held down.
		kModifierKeyOption		= 1 << 1,		//## The alt key (option key on the Mac) was held down.
		kModifierKeyCommand		= 1 << 2,		//## The control key (command key on the Mac) was held down.
		kModifierKeyConsole		= 1 << 16
	};


	enum : EventType
	{
		kEventNone							= 0,
		kEventMouseDown						= 'MSDN',
		kEventMouseUp						= 'MSUP',
		kEventRightMouseDown				= 'RMDN',
		kEventRightMouseUp					= 'RMUP', 
		kEventMiddleMouseDown				= 'MMDN',
		kEventMiddleMouseUp					= 'MMUP',
		kEventMouseMoved					= 'MSMV', 
		kEventMouseWheel					= 'MSWH',
		kEventMultiaxisMouseTranslation		= 'MATR', 
		kEventMultiaxisMouseRotation		= 'MART',
		kEventMultiaxisMouseButtonState		= 'MABS',
		kEventKeyDown						= 'KYDN', 
		kEventKeyUp							= 'KYUP',
		kEventKeyCommand					= 'KYCM' 
	}; 


	//# \struct	MouseEventData		Contains information about a mouse event.
	// 
	//# The $MouseEventData$ structure contains information about a mouse event.
	//
	//# \def	struct MouseEventData
	//
	//# \data	MouseEventData
	//
	//# \desc
	//# The $MouseEventData$ structure contains the event type, mouse position, and special flags
	//# for a mouse event. The $eventFlags$ field can be a combination (through logical OR) of the
	//# following constants.
	//
	//# \table	MouseEventFlags
	//
	//# \also	$@KeyboardEventData@$


	//# \member		MouseEventData

	struct MouseEventData
	{
		EventType		eventType;			//## The type of mouse event.
		unsigned_int32	eventFlags;			//## The event flags for the event.
		Point3D			mousePosition;		//## The mouse position associated with the event.
		Vector2D		wheelDelta;			//## The wheel delta for $kEventMouseWheel$ events.
	};


	//# \struct	KeyboardEventData		Contains information about a keyboard event.
	//
	//# The $KeyboardEventData$ structure contains information about a keyboard event.
	//
	//# \def	struct KeyboardEventData
	//
	//# \data	KeyboardEventData
	//
	//# \desc
	//# The $KeyboardEventData$ structure contains the type of event, the Unicode character, and
	//# information about modifiers keys for a keyboard event. The $keyCode$ field is set to one of
	//# the following values if a special key is pressed or released.
	//
	//# \table	KeyCode
	//
	//# Special keys always have codes in the ranges [0x01, 0x1F] or [0x80, 0x9F], and keyboard event handlers
	//# should not attempt to handle codes in these ranges as ordinary characters.
	//#
	//# The $modifierKeys$ field can be a combination (through logical OR) of the following constants.
	//
	//# \table	KeyboardModifiers
	//
	//# \also	$@MouseEventData@$


	//# \member		KeyboardEventData

	struct KeyboardEventData
	{
		EventType		eventType;			//## The type of keyboard event.
		unsigned_int32	keyCode;			//## The Unicode value for the character associated with the event.
		unsigned_int32	modifierKeys;		//## The modifier keys associated with the event.
	};


	//# \class	Range	Encapsulates a range of values.
	//
	//# The $Range$ class template encapsulates a range of values.
	//
	//# \def	template <typename type> struct Range
	//
	//# \tparam		type	The type of value used to represent the beginning and end of a range.
	//
	//# \data	Range
	//
	//# \ctor	Range();
	//# \ctor	Range(const type& x, const type& y);
	//
	//# \param	x	The beginning of the range.
	//# \param	y	The end of the range.
	//
	//# \desc
	//# The $Range$ class template encapsulates a range of values of the type given by the
	//# $type$ class template.
	//#
	//# The default constructor leaves the beginning and end values of the range undefined.
	//# If the values $x$ and $y$ are supplied, then they are assigned to the beginning and
	//# end of the range, respectively.
	//
	//# \operator	type& operator [](machine index);
	//#				Returns a reference to the minimum value if $index$ is 0, and returns a reference to the maximum value if $index$ is 1.
	//#				The $index$ parameter must be 0 or 1.
	//
	//# \operator	const type& operator [](machine index) const;
	//#				Returns a constant reference to the minimum value if $index$ is 0, and returns a constant reference to the maximum value if $index$ is 1.
	//#				The $index$ parameter must be 0 or 1.
	//
	//# \operator	bool operator ==(const Range& range) const;
	//#				Returns a boolean value indicating the equality of two ranges.
	//
	//# \operator	bool operator !=(const Range& range) const;
	//#				Returns a boolean value indicating the inequality of two ranges.


	//# \function	Range::Set		Sets the beginning and end of a range.
	//
	//# \proto	Range& Set(const type& x, const type& y);
	//
	//# \param	x	The new beginning of the range.
	//# \param	y	The new end of the range.
	//
	//# \desc
	//# The $Set$ function sets the beginning and end of a range to the values given by the
	//# $x$ and $y$ parameters, respectively.


	//# \member		Range

	template <typename type> struct Range
	{
		type	min;		//## The beginning of the range.
		type	max;		//## The end of the range.

		Range() = default;

		Range(const Range& range)
		{
			min = range.min;
			max = range.max;
		}

		Range(const type& x, const type& y)
		{
			min = x;
			max = y;
		}

		type& operator [](machine index)
		{
			return ((&min)[index]);
		}

		const type& operator [](machine index) const
		{
			return ((&min)[index]);
		}

		Range& operator =(const Range& range)
		{
			min = range.min;
			max = range.max;
			return (*this);
		}

		Range& Set(const type& x, const type& y)
		{
			min = x;
			max = y;
			return (*this);
		}

		bool operator ==(const Range& range) const
		{
			return ((min == range.min) && (max == range.max));
		}

		bool operator !=(const Range& range) const
		{
			return ((min != range.min) || (max != range.max));
		}
	};


	//# \class	Transformable	Encapsulates an object-to-world transform and its inverse.
	//
	//# The $Transformable$ class encapsulates an object-to-world transform and its inverse.
	//
	//# \def	class Transformable
	//
	//# \ctor	Transformable();
	//
	//# \desc
	//# The $Transformable$ class encapsulates a transform from object space to world space
	//# and maintains the corresponding inverse transform from world space to object space.
	//#
	//# The constructor leaves the transform undefined.


	//# \function	Transformable::SetIdentityTransform		Sets the transform to the identity.
	//
	//# \proto	void SetIdentityTransform(void);
	//
	//# \desc
	//# The $SetIdentityTransform$ function sets both the object-to-world transform and its
	//# inverse to the identity transform.
	//
	//# \also	$@Transformable::SetWorldTransform@$
	//# \also	$@Transformable::SetWorldPosition@$


	//# \function	Transformable::GetWorldTransform		Returns the object-to-world transform.
	//
	//# \proto	const Transform4D& GetWorldTransform(void) const;
	//
	//# \desc
	//# The $GetWorldTransform$ returns the $@Math/Transform4D@$ object corresponding to the
	//# transform from object space to world space.
	//
	//# \also	$@Transformable::GetInverseWorldTransform@$
	//# \also	$@Transformable::SetWorldTransform@$
	//# \also	$@Transformable::GetWorldPosition@$
	//# \also	$@Transformable::SetWorldPosition@$


	//# \function	Transformable::GetInverseWorldTransform		Returns the world-to-object transform.
	//
	//# \proto	const Transform4D& GetInverseWorldTransform(void) const;
	//
	//# \desc
	//# The $GetInverseWorldTransform$ returns the $@Math/Transform4D@$ object corresponding to the
	//# transform from world space to object space. The inverse is not calculated at the time
	//# this function is called, but when the transform is set, so the performance of this function
	//# is high.
	//
	//# \also	$@Transformable::GetWorldTransform@$
	//# \also	$@Transformable::SetWorldTransform@$
	//# \also	$@Transformable::GetWorldPosition@$
	//# \also	$@Transformable::SetWorldPosition@$


	//# \function	Transformable::SetWorldTransform		Sets the object-to-world transform and its inverse.
	//
	//# \proto	void SetWorldTransform(const Transform4D& transform);
	//# \proto	void SetWorldTransform(const Matrix3D& m, const Point3D& p);
	//# \proto	void SetWorldTransform(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4);
	//# \proto	void SetWorldTransform(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23);
	//
	//# \param	transform	The new transform.
	//# \param	m			The upper-left 3&nbsp;&times;&nbsp;3 portion of the 4D transform.
	//# \param	p			The fourth column of the 4D transform, representing the world position.
	//# \param	c1			The first column of the 4D transform.
	//# \param	c2			The second column of the 4D transform.
	//# \param	c3			The third column of the 4D transform.
	//# \param	c4			The fourth column of the 4D transform.
	//# \param	nij			The entry residing in row <i>i</i> and column <i>j</i> of the 4D transform.
	//
	//# \desc
	//# The $SetWorldTransform$ function sets the object-to-world transform. It also calculates
	//# and stores the inverse representing the transform from world space to object space.
	//
	//# \warning
	//# The $SetWorldTransform$ function is normally called only by internal components of the engine
	//# during routine update procedures. This function should not be called by external code to directly
	//# set the world transform of an object.
	//
	//# \also	$@Transformable::GetWorldTransform@$
	//# \also	$@Transformable::GetWorldPosition@$
	//# \also	$@Transformable::SetWorldPosition@$
	//# \also	$@Transformable::SetIdentityTransform@$


	//# \function	Transformable::GetWorldPosition		Returns the world-space position.
	//
	//# \proto	const Point3D& GetWorldPosition(void) const;
	//
	//# \desc
	//# The $GetWorldPosition$ function returns the world-space position represented by a
	//# transform. Calling $GetWorldPosition$ is equivalent to the following.
	//
	//# \source
	//# GetWorldTransform().GetTranslation();
	//
	//# \also	$@Transformable::SetWorldPosition@$
	//# \also	$@Transformable::GetWorldTransform@$
	//# \also	$@Transformable::SetWorldTransform@$


	//# \function	Transformable::SetWorldPosition		Sets the world-space position.
	//
	//# \proto	void SetWorldPosition(const Point3D& position);
	//
	//# \param	position	The new world-space position.
	//
	//# \desc
	//# The $SetWorldPosition$ function sets the world-space position to that given by the
	//# $position$ parameter and recalculates the inverse transform from world space to object space.
	//# The upper-left 3&nbsp;&times;&nbsp;3 portion of the 4D transform is not affected.
	//
	//# \warning
	//# The $SetWorldPosition$ function is normally called only by internal components of the engine
	//# during routine update procedures. This function should not be called by external code to directly
	//# set the world position of an object.
	//
	//# \also	$@Transformable::GetWorldPosition@$
	//# \also	$@Transformable::GetWorldTransform@$
	//# \also	$@Transformable::SetWorldTransform@$


	class Transformable
	{
		private:

			Transform4D		worldTransform;
			Transform4D		inverseWorldTransform;

		public:

			Transformable() = default;

			void SetIdentityTransform(void)
			{
				worldTransform.SetIdentity();
				inverseWorldTransform.SetIdentity();
			}

			const Transform4D& GetWorldTransform(void) const
			{
				return (worldTransform);
			}

			const Point3D& GetWorldPosition(void) const
			{
				return (worldTransform.GetTranslation());
			}

			const Transform4D& GetInverseWorldTransform(void) const
			{
				return (inverseWorldTransform);
			}

			void SetWorldTransform(const Transform4D& transform)
			{
				worldTransform = transform;
				inverseWorldTransform = Inverse(worldTransform);
			}

			void SetWorldTransform(const Matrix3D& m, const Point3D& p)
			{
				worldTransform.Set(m, p);
				inverseWorldTransform = Inverse(worldTransform);
			}

			void SetWorldTransform(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4)
			{
				worldTransform.Set(c1, c2, c3, c4);
				inverseWorldTransform = Inverse(worldTransform);
			}

			void SetWorldTransform(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23)
			{
				worldTransform.Set(n00, n01, n02, n03, n10, n11, n12, n13, n20, n21, n22, n23);
				inverseWorldTransform = Inverse(worldTransform);
			}

			void SetWorldPosition(const Point3D& position)
			{
				worldTransform.SetTranslation(position);
				inverseWorldTransform = Inverse(worldTransform);
			}
	};


	template <class type> class Reference : public ListElement<Reference<type>>
	{
		private:

			type	*referenceTarget;

		public:

			explicit Reference(type *target)
			{
				referenceTarget = target;
			}

			~Reference()
			{
			}

			type *GetTarget(void) const
			{
				return (referenceTarget);
			}
	};


	template <class type> class AutoDelete
	{
		private:

			type	*reference;

			AutoDelete(const AutoDelete&) = delete;

		public:

			explicit AutoDelete(type *ptr)
			{
				reference = ptr;
			}

			~AutoDelete()
			{
				delete reference;
			}

			operator type *(void) const
			{
				return (reference);
			}

			type *const *operator &(void) const
			{
				return (&reference);
			}

			type *operator ->(void) const
			{
				return (reference);
			}

			AutoDelete& operator =(type *ptr)
			{
				reference = ptr;
				return (*this);
			}
	};


	class Buffer
	{
		private:

			char	*buffer;

		public:

			explicit Buffer(unsigned_int32 size)
			{
				buffer = new char[size];
			}

			~Buffer()
			{
				delete[] buffer;
			}

			operator void *(void) const
			{
				return (buffer);
			}

			template <typename type> type *GetPtr(void) const
			{
				return (reinterpret_cast<type *>(buffer));
			}
	};


	template <class type> class Storage
	{
		private:

			alignas(16) char	storage[sizeof(type)];

		public:

			operator type *(void)
			{
				return (reinterpret_cast<type *>(storage));
			}

			operator const type *(void) const
			{
				return (reinterpret_cast<const type *>(storage));
			}

			type *operator ->(void)
			{
				return (reinterpret_cast<type *>(storage));
			}

			const type *operator ->(void) const
			{
				return (reinterpret_cast<const type *>(storage));
			}
	};
}


#endif

// ZYUQURM
