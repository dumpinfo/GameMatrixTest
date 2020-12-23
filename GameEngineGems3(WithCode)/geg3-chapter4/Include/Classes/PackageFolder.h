namespace Leadwerks3D
{
	class PackageDirectory : public PackageFile
	{
	public:
		std::string name;
		list<PackageFile*> files;
		
		PackageDirectory();
		
	};
}
