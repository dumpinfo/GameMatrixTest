#include "../../Leadwerks.h"

/*
Coding Standards
-All Leadwerks objects are derived from the Object class.
-All classes have a constructor and virtual destructor, even if they don't do anything.
-All class functions are always virtual.
-All class members that are pointers are always initialized to NULL in the constructor.
-All class members and functions are always public.
-Classes are always declared in the following order:
	-Members
	-Constructor(s) & destructor
	-Functions
	-Static members
	-Static functions
-In .cpp files, the constructor(s) is/are always at the top of the file, followed by the destructor.
*/

namespace Leadwerks
{

	
	class Attractor : public Entity//lua
	{
	public:
		
		float range;//lua
		float force;//lua
		bool type1;
		bool fadeOut;//lua 
		std::list<Attractor*>::iterator link;
		
		Attractor();
		virtual ~Attractor();
		
		//Required entity abstract class functions
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		virtual int GetClass();

		virtual bool GetAlphaMode();//lua
		virtual void SetAlphaMode(const bool mode = 0);//lua
		virtual float GetForce();//lua
		virtual void SetForce(const float Force = 1);//lua
		virtual float GetRange();//lua
		virtual void SetRange(const float Range =10);//lua
		virtual std::string GetClassName();

		//Static functions
		static Attractor* Create(const float Range = 10, const float Force = 1, Entity* parent=NULL);//lua


	};
}
