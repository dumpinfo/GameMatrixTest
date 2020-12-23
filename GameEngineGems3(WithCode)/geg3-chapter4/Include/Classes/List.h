namespace Leadwerks3D
{
	class List
	{
	public:
		Link first;
		Link last;
		
		Link AddFirst();
		Link AddLast();
		
		List();
		~List();
	};
}
