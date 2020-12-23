 

#ifndef C4Hash_h
#define C4Hash_h


//# \component	Utility Library
//# \prefix		Utilities/


#include "C4Memory.h"


namespace C4
{
	class HashTableBucket;
	class HashTableBase;


	class HashTableElementBase
	{
		friend class HashTableBucket;
		friend class HashTableBase;
		template <class type> friend class HashTable;

		private:

			HashTableElementBase	*prevBucketElement;
			HashTableElementBase	*nextBucketElement;
			HashTableBucket			*owningHashTableBucket;

			unsigned_int32			hashValue;

			HashTableElementBase(const HashTableElementBase&) = delete;
			HashTableElementBase& operator =(const HashTableElementBase&) = delete;

		protected:

			HashTableElementBase()
			{
				prevBucketElement = nullptr;
				nextBucketElement = nullptr;
				owningHashTableBucket = nullptr;
			}

			C4API virtual ~HashTableElementBase();

		public:

			HashTableElementBase *Previous(void) const
			{
				return (prevBucketElement);
			}

			HashTableElementBase *Next(void) const
			{
				return (nextBucketElement);
			}
	};


	class HashTableBucket
	{
		friend class HashTableBase;

		private:

			HashTableElementBase	*firstBucketElement;
			HashTableElementBase	*lastBucketElement;
			HashTableBase			*owningHashTable;

			void RemoveAll(void);
			void Purge(void);

		public:

			HashTableBucket(HashTableBase *hashTable)
			{
				firstBucketElement = nullptr;
				lastBucketElement = nullptr;
				owningHashTable = hashTable;
			}

			~HashTableBucket();

			HashTableElementBase *First(void) const
			{
				return (firstBucketElement);
			}

			HashTableElementBase *Last(void) const
			{
				return (lastBucketElement);
			}

			C4API void Add(HashTableElementBase *element);
			C4API void Insert(HashTableElementBase *element, HashTableElementBase *after);
			C4API void Remove(HashTableElementBase *element);
	};


	class HashTableBase
	{ 
		friend class HashTableBucket;
		template <class type> friend class HashTable;
 
		private:
 
			int32				elementCount;
			int32				bucketCount;
			int32				resizeLimit; 

			HashTableBucket		*bucketTable; 
 
			HashTableBase(const HashTableBase&) = delete;
			HashTableBase& operator =(const HashTableBase&) = delete;

		protected: 

			C4API HashTableBase(int32 initialBucketCount, int32 maxAverageDepth);
			C4API virtual ~HashTableBase();

			HashTableElementBase *GetFirstBucketElement(int32 index) const
			{
				return (bucketTable[index].First());
			}

			HashTableElementBase *GetLastBucketElement(int32 index) const
			{
				return (bucketTable[index].Last());
			}

			HashTableBucket *GetBucket(unsigned_int32 hashValue) const
			{
				return (&bucketTable[hashValue & (bucketCount - 1)]);
			}

			C4API void ResizeBucketTable(void);

		public:

			int32 GetElementCount(void) const
			{
				return (elementCount);
			}

			int32 GetBucketCount(void) const
			{
				return (bucketCount);
			}

			C4API void RemoveAll(void);
			C4API void Purge(void);
	};


	//# \class	HashTableElement	The base class for objects that can be stored in a hash table.
	//
	//# Objects inherit from the $HashTableElement$ class so that they can be stored in a hash table.
	//
	//# \def	template <class type> class HashTableElement : public HashTableElementBase
	//
	//# \tparam		type	The type of the class that can be stored in a hash table. This parameter should be the
	//#						type of the class that inherits directly from the $HashTableElement$ class.
	//
	//# \ctor	HashTableElement();
	//
	//# The constructor has protected access takes no parameters. The $HashTableElement$ class can only exist
	//# as a base class for another class.
	//
	//# \desc
	//# The $HashTableElement$ class should be declared as a base class for objects that need to be stored in a hash table.
	//# The $type$ template parameter should match the class type of such objects, and these objects can be
	//# stored in a $@HashTable@$ container declared with the same $type$ template parameter.
	//
	//# \privbase	HashTableElementBase	Used internally to encapsulate common functionality that is independent
	//#										of the template parameter.
	//
	//# \also	$@HashTable@$


	template <class type> class HashTableElement : public HashTableElementBase
	{
		public:

			type *Previous(void) const
			{
				return (static_cast<type *>(static_cast<HashTableElement<type> *>(HashTableElementBase::Previous())));
			}

			type *Next(void) const
			{
				return (static_cast<type *>(static_cast<HashTableElement<type> *>(HashTableElementBase::Next())));
			}
	};


	//# \class	HashTable	A container class that organizes objects in a hash table.
	//
	//# The $HashTable$ class encapsulates a dynamically resizable hash table.
	//
	//# \def	template <class type> class HashTable : public HashTableBase
	//
	//# \tparam		type	The type of the class that can be stored in the hash table. The class specified
	//#						by this parameter should inherit directly from the $@HashTableElement@$ class
	//#						using the same template parameter.
	//
	//# \ctor	HashTable(int32 initialBucketCount, int32 maxAverageDepth);
	//
	//# \param	initialBucketCount		The number of buckets initially used by the hash table. This must be a power of two.
	//# \param	maxAverageDepth			The maximum average size of each bucket allowed before the hash table is expanded.
	//
	//# \desc
	//# The $HashTable$ class template is a container used to organize a homogeneous set of objects.
	//# The class type of objects that are to be stored in the hash table must be a subclass of the $@HashTableElement@$
	//# class template using the same template parameter as the $HashTable$ container. A particular object can be a
	//# member of only one hash table at a time.
	//#
	//# Upon construction, a $HashTable$ object is empty. The initial number of buckets used by the hash table is specified
	//# by the $initialBucketCount$ parameter, which must be a power of two in size. When the average size of all the buckets
	//# exceeds the value specified by the $maxAverageDepth$ parameter, the number of buckets is doubled, and the elements of
	//# the hash table are redistributed among the larger set of buckets.
	//#
	//# When a $HashTable$ object is destroyed, all of the members of the hash table are also destroyed. To avoid deleting
	//# the members of a hash table when a $HashTable$ object is destroyed, first call the $@HashTable::RemoveAll@$ function
	//# to remove all of the hash table's members.
	//#
	//# The class specified by the $type$ template parameter must define a type named $KeyType$ and a
	//# function named $GetKey$ that has one of the following two prototypes.
	//
	//# \source
	//# KeyType GetKey(void) const;\n
	//# const KeyType& GetKey(void) const;
	//
	//# \desc
	//# This function should return the key associated with the object for which it is called. The $KeyType$
	//# type must be capable of being compared to other key values using the $==$ operator.
	//#
	//# The class specified by the $type$ template parameter must also define a function named $Hash$ that has
	//# one of the following two prototypes.
	//
	//# \source
	//# static unsigned_int32 Hash(KeyType key);\n
	//# static unsigned_int32 Hash(const KeyType& key);
	//
	//# \desc
	//# This function should return a 32-bit hash value corresponding to the key passed to it.
	//
	//# \privbase	HashTableBase	Used internally to encapsulate common functionality that is independent
	//#								of the template parameter.
	//
	//# \also	$@HashTableElement@$


	//# \function	HashTable::GetElementCount		Returns the number of elements in a hash table.
	//
	//# \proto	int32 GetElementCount(void) const;
	//
	//# \desc
	//# The $GetElementCount$ function returns the number of elements organized in a hash table.


	//# \function	HashTable::Insert		Inserts an element into a hash table.
	//
	//# \proto	void Insert(type *element);
	//
	//# \param	element		A pointer to the object to insert into the hash table.
	//
	//# \desc
	//# The $Insert$ function inserts the object specified by the $element$ parameter into a hash table.
	//
	//# \also	$@HashTable::Remove@$


	//# \function	HashTable::Remove		Removes a particular element from a hash table.
	//
	//# \proto	static void Remove(HashTableElement<type> *element);
	//
	//# \param	element		A pointer to the object to remove from the hash table.
	//
	//# \desc
	//# The $Remove$ function removes the object specified by the $element$ parameter from a hash table.
	//# The object must belong to hash table for which the $Remove$ function is called, or the internal links will become corrupted.
	//
	//# \also	$@HashTable::RemoveAll@$
	//# \also	$@HashTable::Purge@$


	//# \function	HashTable::RemoveAll		Removes all elements from a hash table.
	//
	//# \proto	void RemoveAll(void);
	//
	//# \desc
	//# The $RemoveAll$ function removes all objects organized in a hash table, but does not delete them.
	//# The hash table is subsequently empty.
	//
	//# \also	$@HashTable::Purge@$
	//# \also	$@HashTable::Remove@$


	//# \function	HashTable::Purge		Deletes all elements in a hash table.
	//
	//# \proto	void Purge(void);
	//
	//# \desc
	//# The $Purge$ function deletes all objects organized in a hash table. The hash table is subsequently empty.
	//# To remove all elements of a hash table without destroying them, use the $@HashTable::RemoveAll@$ function.
	//
	//# \also	$@HashTable::Remove@$
	//# \also	$@HashTable::RemoveAll@$


	//# \function	HashTable::Find		Finds an element in a hash table.
	//
	//# \proto	type *Find(const KeyType& key) const;
	//
	//# \param	key		The key value of the element to find.
	//
	//# \desc
	//# The $Find$ function searches a hash table for an element having a key that matches the $key$ parameter.
	//# If a matching element is found, then a pointer to it is returned. Otherwise, the return value is $nullptr$.
	//
	//# \also	$@HashTable::Insert@$
	//# \also	$@HashTable::Remove@$


	template <class type> class HashTable : public HashTableBase
	{
		private:

			typedef typename type::KeyType		KeyType;

		public:

			HashTable(int32 initialBucketCount, int32 maxAverageDepth);
			~HashTable();

			type *GetFirstBucketElement(int32 index) const
			{
				return (static_cast<type *>(static_cast<HashTableElement<type> *>(HashTableBase::GetFirstBucketElement(index))));
			}

			type *GetLastBucketElement(int32 index) const
			{
				return (static_cast<type *>(static_cast<HashTableElement<type> *>(HashTableBase::GetLastBucketElement(index))));
			}

			static void Remove(HashTableElement<type> *element)
			{
				HashTableBucket *bucket = element->owningHashTableBucket;
				if (bucket)
				{
					bucket->Remove(element);
				}
			}

			void Insert(type *element);

			type *Find(const KeyType& key) const;
	};


	template <class type> HashTable<type>::HashTable(int32 initialBucketCount, int32 maxAverageDepth) : HashTableBase(initialBucketCount, maxAverageDepth)
	{
	}

	template <class type> HashTable<type>::~HashTable()
	{
	}

	template <class type> void HashTable<type>::Insert(type *element)
	{
		HashTableBucket *bucket = element->owningHashTableBucket;
		if (bucket)
		{
			bucket->Remove(element);
		}

		const KeyType& key = element->GetKey();
		unsigned_int32 hash = type::Hash(key);
		element->hashValue = hash;

		bucket = GetBucket(hash);
		HashTableElementBase *after = bucket->Last();
		while (after)
		{
			type *object = static_cast<type *>(static_cast<HashTableElement<type> *>(after));
			if (object->GetKey() == key)
			{
				bucket->Insert(element, after);
				goto end;
			}

			after = after->Previous();
		}

		bucket->Add(element);

		end:
		if (elementCount >= resizeLimit)
		{
			ResizeBucketTable();
		}
	}

	template <class type> type *HashTable<type>::Find(const KeyType& key) const
	{
		const HashTableBucket *bucket = GetBucket(type::Hash(key));
		HashTableElementBase *element = bucket->First();
		while (element)
		{
			type *object = static_cast<type *>(static_cast<HashTableElement<type> *>(element));
			if (object->GetKey() == key)
			{
				return (object);
			}

			element = element->Next();
		}

		return (nullptr);
	}
}


#endif

// ZYUQURM
