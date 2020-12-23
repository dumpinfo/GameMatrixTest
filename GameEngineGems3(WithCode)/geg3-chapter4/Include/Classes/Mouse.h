	class Mouse : public Device
	{
		int GetX();
		int GetY();
		int GetZ();
		bool ButtonDown(const int button=1);	
		bool ButtonHit(const int button=1);	
	}
