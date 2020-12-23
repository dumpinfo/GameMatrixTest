#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Bone : public Entity//lua
	{
	public:
		int id;
		Mat4 originalinversematrix;
		float animationmatrix[16];
		Entity* root;

		Bone();
		virtual ~Bone();
		
		//virtual void SetAnimationFrame(const float time, const float blend=1.0, const int index=0, const bool recursive=true);
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		//virtual void SetParent(Entity* parent, const bool global = true);//lua
		//virtual void UpdateAABB(const int mode);
		virtual std::string GetClassName();
		//virtual void UpdateOctreeNode(const bool immediate=false);
		//virtual void SetParent(Entity* parent, const bool global=true);
		virtual void PopulateFileRootBoneArray(Entity* root);
		virtual void UpdateMatrix();
		virtual void BuildPickBox(const bool recursive);

		static Bone* Create(Entity* parent=NULL);
	};
}
