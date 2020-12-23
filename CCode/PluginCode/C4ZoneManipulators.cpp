//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#include "C4ZoneManipulators.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kZoneSelectedColor = {1.0F, 1.0F, 0.25F, 1.0F};
	const ConstColorRGBA kZoneUnselectedColor = {0.5F, 0.5F, 0.0F, 1.0F};
	const ConstColorRGBA kTargetSelectedColor = {0.0F, 1.0F, 0.5F, 1.0F};
	const ConstColorRGBA kTargetUnselectedColor = {0.0F, 0.5F, 0.25F, 1.0F};


	const TextureHeader zoneTextureHeader =
	{
		kTexture2D,
		kTextureForceHighQuality,
		kTextureSemanticEmission,
		kTextureSemanticNone,
		kTextureL8,
		8, 8, 1,
		{kTextureClamp, kTextureRepeat, kTextureRepeat},
		4
	};


	const unsigned_int8 zoneTextureImage[85] =
	{
		0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
		0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
		0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
		0x00, 0xFF, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0x00,
		0x00, 0xFF, 0xFF, 0x00,
		0xFF, 0x00,
		0x00, 0xFF,
		0x80
	};
}


const ConstPoint3D BoxZoneManipulator::outlinePosition[48] =
{
	{0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F},
	{1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F},
	{1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F},
	{0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F},

	{0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F},
	{1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F},
	{1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F},
	{0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F},

	{0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F},
	{1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F},
	{1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F},
	{0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}
};

const ConstVector4D BoxZoneManipulator::outlineTangent[48] =
{
	{1.0F, 0.0F, 0.0F, -1.0F}, {1.0F, 0.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 0.0F, -1.0F},
	{0.0F, 1.0F, 0.0F, -1.0F}, {0.0F, 1.0F, 0.0F, 1.0F}, {0.0F, 1.0F, 0.0F, 1.0F}, {0.0F, 1.0F, 0.0F, -1.0F},
	{-1.0F, 0.0F, 0.0F, -1.0F}, {-1.0F, 0.0F, 0.0F, 1.0F}, {-1.0F, 0.0F, 0.0F, 1.0F}, {-1.0F, 0.0F, 0.0F, -1.0F},
	{0.0F, -1.0F, 0.0F, -1.0F}, {0.0F, -1.0F, 0.0F, 1.0F}, {0.0F, -1.0F, 0.0F, 1.0F}, {0.0F, -1.0F, 0.0F, -1.0F},

	{1.0F, 0.0F, 0.0F, -1.0F}, {1.0F, 0.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 0.0F, -1.0F},
	{0.0F, 1.0F, 0.0F, -1.0F}, {0.0F, 1.0F, 0.0F, 1.0F}, {0.0F, 1.0F, 0.0F, 1.0F}, {0.0F, 1.0F, 0.0F, -1.0F},
	{-1.0F, 0.0F, 0.0F, -1.0F}, {-1.0F, 0.0F, 0.0F, 1.0F}, {-1.0F, 0.0F, 0.0F, 1.0F}, {-1.0F, 0.0F, 0.0F, -1.0F},
	{0.0F, -1.0F, 0.0F, -1.0F}, {0.0F, -1.0F, 0.0F, 1.0F}, {0.0F, -1.0F, 0.0F, 1.0F}, {0.0F, -1.0F, 0.0F, -1.0F},

	{0.0F, 0.0F, 1.0F, -1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, -1.0F},
	{0.0F, 0.0F, 1.0F, -1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, -1.0F},
	{0.0F, 0.0F, 1.0F, -1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, -1.0F},
	{0.0F, 0.0F, 1.0F, -1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, -1.0F}
};


const ConstPoint3D CylinderZoneManipulator::outlinePosition[144] =
{
	{1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {0.9238795F, 0.3826834F, 1.0F}, {0.9238795F, 0.3826834F, 1.0F},
	{0.9238795F, 0.3826834F, 1.0F}, {0.9238795F, 0.3826834F, 1.0F}, {0.7071068F, 0.7071068F, 1.0F}, {0.7071068F, 0.7071068F, 1.0F},
	{0.7071068F, 0.7071068F, 1.0F}, {0.7071068F, 0.7071068F, 1.0F}, {0.3826834F, 0.9238795F, 1.0F}, {0.3826834F, 0.9238795F, 1.0F},
	{0.3826834F, 0.9238795F, 1.0F}, {0.3826834F, 0.9238795F, 1.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F},

	{0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {-0.3826834F, 0.9238795F, 1.0F}, {-0.3826834F, 0.9238795F, 1.0F},
	{-0.3826834F, 0.9238795F, 1.0F}, {-0.3826834F, 0.9238795F, 1.0F}, {-0.7071068F, 0.7071068F, 1.0F}, {-0.7071068F, 0.7071068F, 1.0F},
	{-0.7071068F, 0.7071068F, 1.0F}, {-0.7071068F, 0.7071068F, 1.0F}, {-0.9238795F, 0.3826834F, 1.0F}, {-0.9238795F, 0.3826834F, 1.0F},
	{-0.9238795F, 0.3826834F, 1.0F}, {-0.9238795F, 0.3826834F, 1.0F}, {-1.0F, 0.0F, 1.0F}, {-1.0F, 0.0F, 1.0F},

	{-1.0F, 0.0F, 1.0F}, {-1.0F, 0.0F, 1.0F}, {-0.9238795F, -0.3826834F, 1.0F}, {-0.9238795F, -0.3826834F, 1.0F},
	{-0.9238795F, -0.3826834F, 1.0F}, {-0.9238795F, -0.3826834F, 1.0F}, {-0.7071068F, -0.7071068F, 1.0F}, {-0.7071068F, -0.7071068F, 1.0F},
	{-0.7071068F, -0.7071068F, 1.0F}, {-0.7071068F, -0.7071068F, 1.0F}, {-0.3826834F, -0.9238795F, 1.0F}, {-0.3826834F, -0.9238795F, 1.0F}, 
	{-0.3826834F, -0.9238795F, 1.0F}, {-0.3826834F, -0.9238795F, 1.0F}, {0.0F, -1.0F, 1.0F}, {0.0F, -1.0F, 1.0F},

	{0.0F, -1.0F, 1.0F}, {0.0F, -1.0F, 1.0F}, {0.3826834F, -0.9238795F, 1.0F}, {0.3826834F, -0.9238795F, 1.0F}, 
	{0.3826834F, -0.9238795F, 1.0F}, {0.3826834F, -0.9238795F, 1.0F}, {0.7071068F, -0.7071068F, 1.0F}, {0.7071068F, -0.7071068F, 1.0F},
	{0.7071068F, -0.7071068F, 1.0F}, {0.7071068F, -0.7071068F, 1.0F}, {0.9238795F, -0.3826834F, 1.0F}, {0.9238795F, -0.3826834F, 1.0F}, 
	{0.9238795F, -0.3826834F, 1.0F}, {0.9238795F, -0.3826834F, 1.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F},

	{1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.9238795F, 0.3826834F, 0.0F}, {0.9238795F, 0.3826834F, 0.0F}, 
	{0.9238795F, 0.3826834F, 0.0F}, {0.9238795F, 0.3826834F, 0.0F}, {0.7071068F, 0.7071068F, 0.0F}, {0.7071068F, 0.7071068F, 0.0F},
	{0.7071068F, 0.7071068F, 0.0F}, {0.7071068F, 0.7071068F, 0.0F}, {0.3826834F, 0.9238795F, 0.0F}, {0.3826834F, 0.9238795F, 0.0F}, 
	{0.3826834F, 0.9238795F, 0.0F}, {0.3826834F, 0.9238795F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, 

	{0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {-0.3826834F, 0.9238795F, 0.0F}, {-0.3826834F, 0.9238795F, 0.0F},
	{-0.3826834F, 0.9238795F, 0.0F}, {-0.3826834F, 0.9238795F, 0.0F}, {-0.7071068F, 0.7071068F, 0.0F}, {-0.7071068F, 0.7071068F, 0.0F},
	{-0.7071068F, 0.7071068F, 0.0F}, {-0.7071068F, 0.7071068F, 0.0F}, {-0.9238795F, 0.3826834F, 0.0F}, {-0.9238795F, 0.3826834F, 0.0F}, 
	{-0.9238795F, 0.3826834F, 0.0F}, {-0.9238795F, 0.3826834F, 0.0F}, {-1.0F, 0.0F, 0.0F}, {-1.0F, 0.0F, 0.0F},

	{-1.0F, 0.0F, 0.0F}, {-1.0F, 0.0F, 0.0F}, {-0.9238795F, -0.3826834F, 0.0F}, {-0.9238795F, -0.3826834F, 0.0F},
	{-0.9238795F, -0.3826834F, 0.0F}, {-0.9238795F, -0.3826834F, 0.0F}, {-0.7071068F, -0.7071068F, 0.0F}, {-0.7071068F, -0.7071068F, 0.0F},
	{-0.7071068F, -0.7071068F, 0.0F}, {-0.7071068F, -0.7071068F, 0.0F}, {-0.3826834F, -0.9238795F, 0.0F}, {-0.3826834F, -0.9238795F, 0.0F},
	{-0.3826834F, -0.9238795F, 0.0F}, {-0.3826834F, -0.9238795F, 0.0F}, {0.0F, -1.0F, 0.0F}, {0.0F, -1.0F, 0.0F},

	{0.0F, -1.0F, 0.0F}, {0.0F, -1.0F, 0.0F}, {0.3826834F, -0.9238795F, 0.0F}, {0.3826834F, -0.9238795F, 0.0F},
	{0.3826834F, -0.9238795F, 0.0F}, {0.3826834F, -0.9238795F, 0.0F}, {0.7071068F, -0.7071068F, 0.0F}, {0.7071068F, -0.7071068F, 0.0F},
	{0.7071068F, -0.7071068F, 0.0F}, {0.7071068F, -0.7071068F, 0.0F}, {0.9238795F, -0.3826834F, 0.0F}, {0.9238795F, -0.3826834F, 0.0F},
	{0.9238795F, -0.3826834F, 0.0F}, {0.9238795F, -0.3826834F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F},

	{1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F},
	{0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F},
	{-1.0F, 0.0F, 0.0F}, {-1.0F, 0.0F, 0.0F}, {-1.0F, 0.0F, 1.0F}, {-1.0F, 0.0F, 1.0F},
	{0.0F, -1.0F, 0.0F}, {0.0F, -1.0F, 0.0F}, {0.0F, -1.0F, 1.0F}, {0.0F, -1.0F, 1.0F}
};

const ConstVector4D CylinderZoneManipulator::outlineTangent[144] =
{
	{-0.1950904F, 0.9807853F, 0.0F, -1.0F}, {-0.1950904F, 0.9807853F, 0.0F, 1.0F}, {-0.1950904F, 0.9807853F, 0.0F, 1.0F}, {-0.1950904F, 0.9807853F, 0.0F, -1.0F},
	{-0.55557F, 0.8314697F, 0.0F, -1.0F}, {-0.55557F, 0.8314697F, 0.0F, 1.0F}, {-0.55557F, 0.8314697F, 0.0F, 1.0F}, {-0.55557F, 0.8314697F, 0.0F, -1.0F},
	{-0.8314697F, 0.55557F, 0.0F, -1.0F}, {-0.8314697F, 0.55557F, 0.0F, 1.0F}, {-0.8314697F, 0.55557F, 0.0F, 1.0F}, {-0.8314697F, 0.55557F, 0.0F, -1.0F},
	{-0.9807853F, 0.1950904F, 0.0F, -1.0F}, {-0.9807853F, 0.1950904F, 0.0F, 1.0F}, {-0.9807853F, 0.1950904F, 0.0F, 1.0F}, {-0.9807853F, 0.1950904F, 0.0F, -1.0F},

	{-0.9807853F, -0.1950904F, 0.0F, -1.0F}, {-0.9807853F, -0.1950904F, 0.0F, 1.0F}, {-0.9807853F, -0.1950904F, 0.0F, 1.0F}, {-0.9807853F, -0.1950904F, 0.0F, -1.0F},
	{-0.8314697F, -0.55557F, 0.0F, -1.0F}, {-0.8314697F, -0.55557F, 0.0F, 1.0F}, {-0.8314697F, -0.55557F, 0.0F, 1.0F}, {-0.8314697F, -0.55557F, 0.0F, -1.0F},
	{-0.55557F, -0.8314697F, 0.0F, -1.0F}, {-0.55557F, -0.8314697F, 0.0F, 1.0F}, {-0.55557F, -0.8314697F, 0.0F, 1.0F}, {-0.55557F, -0.8314697F, 0.0F, -1.0F},
	{-0.1950904F, -0.9807853F, 0.0F, -1.0F}, {-0.1950904F, -0.9807853F, 0.0F, 1.0F}, {-0.1950904F, -0.9807853F, 0.0F, 1.0F}, {-0.1950904F, -0.9807853F, 0.0F, -1.0F},

	{0.1950904F, -0.9807853F, 0.0F, -1.0F}, {0.1950904F, -0.9807853F, 0.0F, 1.0F}, {0.1950904F, -0.9807853F, 0.0F, 1.0F}, {0.1950904F, -0.9807853F, 0.0F, -1.0F},
	{0.55557F, -0.8314697F, 0.0F, -1.0F}, {0.55557F, -0.8314697F, 0.0F, 1.0F}, {0.55557F, -0.8314697F, 0.0F, 1.0F}, {0.55557F, -0.8314697F, 0.0F, -1.0F},
	{0.8314697F, -0.55557F, 0.0F, -1.0F}, {0.8314697F, -0.55557F, 0.0F, 1.0F}, {0.8314697F, -0.55557F, 0.0F, 1.0F}, {0.8314697F, -0.55557F, 0.0F, -1.0F},
	{0.9807853F, -0.1950904F, 0.0F, -1.0F}, {0.9807853F, -0.1950904F, 0.0F, 1.0F}, {0.9807853F, -0.1950904F, 0.0F, 1.0F}, {0.9807853F, -0.1950904F, 0.0F, -1.0F},

	{0.9807853F, 0.1950904F, 0.0F, -1.0F}, {0.9807853F, 0.1950904F, 0.0F, 1.0F}, {0.9807853F, 0.1950904F, 0.0F, 1.0F}, {0.9807853F, 0.1950904F, 0.0F, -1.0F},
	{0.8314697F, 0.55557F, 0.0F, -1.0F}, {0.8314697F, 0.55557F, 0.0F, 1.0F}, {0.8314697F, 0.55557F, 0.0F, 1.0F}, {0.8314697F, 0.55557F, 0.0F, -1.0F},
	{0.55557F, 0.8314697F, 0.0F, -1.0F}, {0.55557F, 0.8314697F, 0.0F, 1.0F}, {0.55557F, 0.8314697F, 0.0F, 1.0F}, {0.55557F, 0.8314697F, 0.0F, -1.0F},
	{0.1950904F, 0.9807853F, 0.0F, -1.0F}, {0.1950904F, 0.9807853F, 0.0F, 1.0F}, {0.1950904F, 0.9807853F, 0.0F, 1.0F}, {0.1950904F, 0.9807853F, 0.0F, -1.0F},

	{-0.1950904F, 0.9807853F, 0.0F, -1.0F}, {-0.1950904F, 0.9807853F, 0.0F, 1.0F}, {-0.1950904F, 0.9807853F, 0.0F, 1.0F}, {-0.1950904F, 0.9807853F, 0.0F, -1.0F},
	{-0.55557F, 0.8314697F, 0.0F, -1.0F}, {-0.55557F, 0.8314697F, 0.0F, 1.0F}, {-0.55557F, 0.8314697F, 0.0F, 1.0F}, {-0.55557F, 0.8314697F, 0.0F, -1.0F},
	{-0.8314697F, 0.55557F, 0.0F, -1.0F}, {-0.8314697F, 0.55557F, 0.0F, 1.0F}, {-0.8314697F, 0.55557F, 0.0F, 1.0F}, {-0.8314697F, 0.55557F, 0.0F, -1.0F},
	{-0.9807853F, 0.1950904F, 0.0F, -1.0F}, {-0.9807853F, 0.1950904F, 0.0F, 1.0F}, {-0.9807853F, 0.1950904F, 0.0F, 1.0F}, {-0.9807853F, 0.1950904F, 0.0F, -1.0F},

	{-0.9807853F, -0.1950904F, 0.0F, -1.0F}, {-0.9807853F, -0.1950904F, 0.0F, 1.0F}, {-0.9807853F, -0.1950904F, 0.0F, 1.0F}, {-0.9807853F, -0.1950904F, 0.0F, -1.0F},
	{-0.8314697F, -0.55557F, 0.0F, -1.0F}, {-0.8314697F, -0.55557F, 0.0F, 1.0F}, {-0.8314697F, -0.55557F, 0.0F, 1.0F}, {-0.8314697F, -0.55557F, 0.0F, -1.0F},
	{-0.55557F, -0.8314697F, 0.0F, -1.0F}, {-0.55557F, -0.8314697F, 0.0F, 1.0F}, {-0.55557F, -0.8314697F, 0.0F, 1.0F}, {-0.55557F, -0.8314697F, 0.0F, -1.0F},
	{-0.1950904F, -0.9807853F, 0.0F, -1.0F}, {-0.1950904F, -0.9807853F, 0.0F, 1.0F}, {-0.1950904F, -0.9807853F, 0.0F, 1.0F}, {-0.1950904F, -0.9807853F, 0.0F, -1.0F},

	{0.1950904F, -0.9807853F, 0.0F, -1.0F}, {0.1950904F, -0.9807853F, 0.0F, 1.0F}, {0.1950904F, -0.9807853F, 0.0F, 1.0F}, {0.1950904F, -0.9807853F, 0.0F, -1.0F},
	{0.55557F, -0.8314697F, 0.0F, -1.0F}, {0.55557F, -0.8314697F, 0.0F, 1.0F}, {0.55557F, -0.8314697F, 0.0F, 1.0F}, {0.55557F, -0.8314697F, 0.0F, -1.0F},
	{0.8314697F, -0.55557F, 0.0F, -1.0F}, {0.8314697F, -0.55557F, 0.0F, 1.0F}, {0.8314697F, -0.55557F, 0.0F, 1.0F}, {0.8314697F, -0.55557F, 0.0F, -1.0F},
	{0.9807853F, -0.1950904F, 0.0F, -1.0F}, {0.9807853F, -0.1950904F, 0.0F, 1.0F}, {0.9807853F, -0.1950904F, 0.0F, 1.0F}, {0.9807853F, -0.1950904F, 0.0F, -1.0F},

	{0.9807853F, 0.1950904F, 0.0F, -1.0F}, {0.9807853F, 0.1950904F, 0.0F, 1.0F}, {0.9807853F, 0.1950904F, 0.0F, 1.0F}, {0.9807853F, 0.1950904F, 0.0F, -1.0F},
	{0.8314697F, 0.55557F, 0.0F, -1.0F}, {0.8314697F, 0.55557F, 0.0F, 1.0F}, {0.8314697F, 0.55557F, 0.0F, 1.0F}, {0.8314697F, 0.55557F, 0.0F, -1.0F},
	{0.55557F, 0.8314697F, 0.0F, -1.0F}, {0.55557F, 0.8314697F, 0.0F, 1.0F}, {0.55557F, 0.8314697F, 0.0F, 1.0F}, {0.55557F, 0.8314697F, 0.0F, -1.0F},
	{0.1950904F, 0.9807853F, 0.0F, -1.0F}, {0.1950904F, 0.9807853F, 0.0F, 1.0F}, {0.1950904F, 0.9807853F, 0.0F, 1.0F}, {0.1950904F, 0.9807853F, 0.0F, -1.0F},

	{0.0F, 0.0F, 1.0F, -1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, -1.0F},
	{0.0F, 0.0F, 1.0F, -1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, -1.0F},
	{0.0F, 0.0F, 1.0F, -1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, -1.0F},
	{0.0F, 0.0F, 1.0F, -1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F, -1.0F}
};


ZoneManipulator::ZoneManipulator(Zone *zone) :
		EditorManipulator(zone, "WorldEditor/zone/Box"),
		zoneMaterial(new MaterialObject),
		zoneColorAttribute(kZoneUnselectedColor, kAttributeMutable),
		zoneTextureAttribute(&zoneTextureHeader, zoneTextureImage),
		zoneVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		zoneRenderable(kRenderQuads, kRenderDepthTest)
{
	zoneRenderable.SetAmbientBlendState(kBlendPremultInterp);
	zoneRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard | kShaderOrthoPolyboard | kShaderScaleVertex);
	zoneRenderable.SetRenderParameterPointer(&zoneSizeVector);
	zoneRenderable.SetTransformable(zone);

	zoneRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &zoneVertexBuffer, sizeof(ZoneVertex));
	zoneRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	zoneRenderable.SetVertexAttributeArray(kArrayTangent, sizeof(Point3D), 4);
	zoneRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Vector4D), 2);

	zoneMaterial->AddAttribute(&zoneColorAttribute);
	zoneMaterial->AddAttribute(&zoneTextureAttribute);
	zoneMaterial->SetTexcoordOffset(0, Vector2D(0.0F, 1.0F));
	zoneRenderable.SetMaterialObjectPointer(&zoneMaterial);

	zoneSizeVector.Set(1.0F, 1.0F, 1.0F, 1.0F);
}

ZoneManipulator::~ZoneManipulator()
{
}

ZoneManipulator *ZoneManipulator::Create(Zone *zone)
{
	switch (zone->GetZoneType())
	{
		case kZoneInfinite:

			return (new InfiniteZoneManipulator(static_cast<InfiniteZone *>(zone)));

		case kZoneBox:

			return (new BoxZoneManipulator(static_cast<BoxZone *>(zone)));

		case kZoneCylinder:

			return (new CylinderZoneManipulator(static_cast<CylinderZone *>(zone)));

		case kZonePolygon:

			return (new PolygonZoneManipulator(static_cast<PolygonZone *>(zone)));
	}

	return (new ZoneManipulator(zone));
}

const char *ZoneManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeZone, GetTargetNode()->GetZoneType())));
}

void ZoneManipulator::SetTarget(bool target)
{
	unsigned_int32 state = GetManipulatorState();
	if (target)
	{
		state |= kManipulatorTarget;
	}
	else
	{
		state &= ~kManipulatorTarget;
	}

	SetManipulatorState(state);

	if (Selected())
	{
		if (target)
		{
			zoneColorAttribute.SetDiffuseColor(kTargetSelectedColor);
		}
		else
		{
			zoneColorAttribute.SetDiffuseColor(kZoneSelectedColor);
		}
	}
	else
	{
		if (target)
		{
			zoneColorAttribute.SetDiffuseColor(kTargetUnselectedColor);
		}
		else
		{
			zoneColorAttribute.SetDiffuseColor(kZoneUnselectedColor);
		}
	}
}

void ZoneManipulator::Invalidate(void)
{
	EditorManipulator::Invalidate();

	Zone *zone = GetTargetNode();
	zone->InvalidateLightRegions();
	zone->InvalidateSourceRegions();
}

void ZoneManipulator::Select(void)
{
	EditorManipulator::Select();

	if (GetManipulatorState() & kManipulatorTarget)
	{
		zoneColorAttribute.SetDiffuseColor(kTargetSelectedColor);
	}
	else
	{
		zoneColorAttribute.SetDiffuseColor(kZoneSelectedColor);
	}
}

void ZoneManipulator::Unselect(void)
{
	EditorManipulator::Unselect();

	if (GetManipulatorState() & kManipulatorTarget)
	{
		zoneColorAttribute.SetDiffuseColor(kTargetUnselectedColor);
	}
	else
	{
		zoneColorAttribute.SetDiffuseColor(kZoneUnselectedColor);
	}
}

void ZoneManipulator::HandleDelete(bool undoable)
{
	EditorManipulator::HandleDelete(undoable);

	Editor *editor = GetEditor();
	editor->InvalidateNode(editor->GetRootNode());

	if (editor->GetTargetZone() == GetTargetNode())
	{
		editor->SetTargetZone(nullptr);
	}
}

void ZoneManipulator::HandleUndelete(void)
{
	EditorManipulator::HandleUndelete();

	Editor *editor = GetEditor();
	editor->InvalidateNode(editor->GetRootNode());
}

void ZoneManipulator::HandleSizeUpdate(int32 count, const float *size)
{
	EditorManipulator::HandleSizeUpdate(count, size);

	Editor *editor = GetEditor();
	editor->InvalidateNode(editor->GetRootNode());
}

void ZoneManipulator::HandleSettingsUpdate(void)
{
	EditorManipulator::HandleSettingsUpdate();

	Editor *editor = GetEditor();
	editor->InvalidateNode(editor->GetRootNode());
}

void ZoneManipulator::HandleConnectorUpdate(void)
{
	EditorManipulator::HandleConnectorUpdate();
	GetEditor()->InvalidateAllShaderData();
}

bool ZoneManipulator::Pick(const Ray *ray, PickData *data) const
{
	bool result = false;

	if (zoneRenderable.AttributeArrayEnabled(kArrayPosition))
	{
		const Point3D *position = zonePositionArray;
		int32 vertexCount = zoneRenderable.GetVertexCount();
		const Vector3D& size = zoneSizeVector.GetVector3D();

		float r = (ray->radius != 0.0F) ? ray->radius : Editor::kFrustumRenderScale;
		float r2 = r * r * 16.0F;

		Ray zoneRay = *ray;
		for (machine a = 0; a < vertexCount; a += 4)
		{
			if (PickLineSegment(&zoneRay, position[0] & size, position[2] & size, r2, &data->rayParam))
			{
				zoneRay.tmax = data->rayParam;
				result = true;
			}

			position += 4;
		}
	}

	return (result);
}

bool ZoneManipulator::RegionPick(const VisibilityRegion *region) const
{
	if (zoneRenderable.AttributeArrayEnabled(kArrayPosition))
	{
		const Point3D *position = zonePositionArray;
		int32 vertexCount = zoneRenderable.GetVertexCount();
		const Vector3D& size = zoneSizeVector.GetVector3D();

		for (machine a = 0; a < vertexCount; a += 4)
		{
			if (RegionPickLineSegment(region, position[0] & size, position[2] & size))
			{
				return (true);
			}

			position += 4;
		}
	}

	return (false);
}

void ZoneManipulator::EndTransform(void)
{
	Editor *editor = GetEditor();
	editor->InvalidateNode(editor->GetRootNode());
}

void ZoneManipulator::EndResize(const ManipulatorResizeData *resizeData)
{
	Editor *editor = GetEditor();
	editor->InvalidateNode(editor->GetRootNode());
}

void ZoneManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		zoneRenderable.InvalidateVertexData();

		int32 vertexCount = zoneRenderable.GetVertexCount();
		zoneVertexBuffer.Establish(sizeof(ZoneVertex) * vertexCount);

		volatile ZoneVertex *restrict zoneVertex = zoneVertexBuffer.BeginUpdate<ZoneVertex>();

		const Point3D *position = zonePositionArray;
		const Vector4D *tangent = zoneTangentArray;
		const Vector3D& size = zoneSizeVector.GetVector3D();

		for (machine a = 0; a < vertexCount; a += 4)
		{
			float dp = Magnitude((position[2] & size) - (position[0] & size));

			zoneVertex[0].position = position[0];
			zoneVertex[0].tangent = tangent[0];
			zoneVertex[0].texcoord.Set(0.0F, 0.0F);

			zoneVertex[1].position = position[1];
			zoneVertex[1].tangent = tangent[1];
			zoneVertex[1].texcoord.Set(1.0F, 0.0F);

			zoneVertex[2].position = position[2];
			zoneVertex[2].tangent = tangent[2];
			zoneVertex[2].texcoord.Set(1.0F, dp);

			zoneVertex[3].position = position[3];
			zoneVertex[3].tangent = tangent[3];
			zoneVertex[3].texcoord.Set(0.0F, dp);

			zoneVertex += 4;
			position += 4;
			tangent += 4;
		}

		zoneVertexBuffer.EndUpdate();
	}

	EditorManipulator::Update();
}

void ZoneManipulator::Render(const ManipulatorRenderData *renderData)
{
	List<Renderable> *renderList = renderData->manipulatorList;
	if ((renderList) && (zoneRenderable.AttributeArrayEnabled(kArrayPosition)))
	{
		float scale = renderData->viewportScale;
		zoneSizeVector.w = scale * 3.0F;
		zoneMaterial->SetTexcoordScale(0, Vector2D(1.0F, 0.125F / scale));
		renderList->Append(&zoneRenderable);
	}

	EditorManipulator::Render(renderData);
}


InfiniteZoneManipulator::InfiniteZoneManipulator(InfiniteZone *infinite) : ZoneManipulator(infinite)
{
	SetManipulatorState(kManipulatorHidden);
	SetManipulatorFlags(kManipulatorLockedTransform);

	zoneRenderable.SetVertexCount(48);
	SetZonePositionArray(boxPosition);
	SetZoneTangentArray(&BoxZoneManipulator::outlineTangent[0]);
}

InfiniteZoneManipulator::~InfiniteZoneManipulator()
{
}

bool InfiniteZoneManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const Box3D& box = GetObject()->GetZoneBox();
	sphere->SetCenter(box.GetCenter());
	sphere->SetRadius(Magnitude(box.GetSize()) * 0.5F);
	return (true);
}

Box3D InfiniteZoneManipulator::CalculateNodeBoundingBox(void) const
{
	return (GetObject()->GetZoneBox());
}

void InfiniteZoneManipulator::HandleSizeUpdate(int32 count, const float *size)
{
	float	objectSize[6];

	for (machine a = 0; a < 3; a++)
	{
		float f = size[a];
		objectSize[a] = f;
		objectSize[a + 3] = Fmax(size[a + 3], f + 1.0F);
	}

	Node *node = GetTargetNode();
	node->GetObject()->SetObjectSize(objectSize);
	GetEditor()->InvalidateNode(node);
}

int32 InfiniteZoneManipulator::GetHandleTable(Point3D *handle) const
{
	const Box3D& box = GetObject()->GetZoneBox();
	Vector3D center = box.GetCenter();

	handle[0] = box.min;
	handle[1].Set(center.x, box.min.y, box.min.z);
	handle[2].Set(box.max.x, box.min.y, box.min.z);
	handle[3].Set(box.max.x, center.y, box.min.z);
	handle[4].Set(box.max.x, box.max.y, box.min.z);
	handle[5].Set(center.x, box.max.y, box.min.z);
	handle[6].Set(box.min.x, box.max.y, box.min.z);
	handle[7].Set(box.min.x, center.y, box.min.z);

	handle[8].Set(box.min.x, box.min.y, box.max.z);
	handle[9].Set(center.x, box.min.y, box.max.z);
	handle[10].Set(box.max.x, box.min.y, box.max.z);
	handle[11].Set(box.max.x, center.y, box.max.z);
	handle[12] = box.max;
	handle[13].Set(center.x, box.max.y, box.max.z);
	handle[14].Set(box.min.x, box.max.y, box.max.z);
	handle[15].Set(box.min.x, center.y, box.max.z);

	handle[16].Set(box.min.x, box.min.y, center.z);
	handle[17].Set(box.max.x, box.min.y, center.z);
	handle[18].Set(box.max.x, box.max.y, center.z);
	handle[19].Set(box.min.x, box.max.y, center.z);

	return (20);
}

void InfiniteZoneManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	BoxVolumeManipulator::GetHandleData(index, handleData);
}

void InfiniteZoneManipulator::BeginResize(const ManipulatorResizeData *resizeData)
{
	EditorManipulator::BeginResize(resizeData);
	originalZoneBox = GetObject()->GetZoneBox();
}

bool InfiniteZoneManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	Box3D box = originalZoneBox;

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	float dz = resizeData->resizeDelta.z;

	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (handleFlags & kManipulatorHandlePositiveX)
	{
		box.max.x = Fmax(box.max.x + dx, box.min.x + 1.0F);
	}
	else if (handleFlags & kManipulatorHandleNegativeX)
	{
		box.min.x = Fmin(box.min.x + dx, box.max.x - 1.0F);
	}

	if (handleFlags & kManipulatorHandlePositiveY)
	{
		box.max.y = Fmax(box.max.y + dy, box.min.y + 1.0F);
	}
	else if (handleFlags & kManipulatorHandleNegativeY)
	{
		box.min.y = Fmin(box.min.y + dy, box.max.y - 1.0F);
	}

	if (handleFlags & kManipulatorHandlePositiveZ)
	{
		box.max.z = Fmax(box.max.z + dz, box.min.z + 1.0F);
	}
	else if (handleFlags & kManipulatorHandleNegativeZ)
	{
		box.min.z = Fmin(box.min.z + dz, box.max.z - 1.0F);
	}

	GetObject()->SetZoneBox(box);
	return (false);
}

void InfiniteZoneManipulator::Update(void)
{
	unsigned_int32 state = GetManipulatorState();
	if (!(state & kManipulatorUpdated))
	{
		const Box3D& box = GetObject()->GetZoneBox();
		Vector3D size = box.GetSize();

		const Point3D *position = &BoxZoneManipulator::outlinePosition[0];
		for (machine a = 0; a < 48; a++)
		{
			boxPosition[a] = box.min + (position[a] & size);
		}
	}

	ZoneManipulator::Update();
}


BoxZoneManipulator::BoxZoneManipulator(BoxZone *box) : ZoneManipulator(box)
{
	zoneRenderable.SetVertexCount(48);
	SetZonePositionArray(&outlinePosition[0]);
	SetZoneTangentArray(&outlineTangent[0]);
}

BoxZoneManipulator::~BoxZoneManipulator()
{
}

bool BoxZoneManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	BoxVolumeManipulator::CalculateVolumeSphere(GetObject()->GetBoxSize(), sphere);
	return (true);
}

Box3D BoxZoneManipulator::CalculateNodeBoundingBox(void) const
{
	const Vector3D& size = GetObject()->GetBoxSize();
	return (Box3D(Zero3D, Zero3D + size));
}

int32 BoxZoneManipulator::GetHandleTable(Point3D *handle) const
{
	return (BoxVolumeManipulator::GetHandleTable(GetObject()->GetBoxSize(), handle));
}

void BoxZoneManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	BoxVolumeManipulator::GetHandleData(index, handleData);
}

bool BoxZoneManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	BoxZoneObject *object = GetObject();

	Vector3D newSize = object->GetBoxSize();
	const Vector3D& oldSize = *reinterpret_cast<const Vector3D *>(GetOriginalSize());
	bool move = BoxVolumeManipulator::Resize(resizeData, oldSize, newSize);

	object->SetBoxSize(newSize);
	return (move);
}

void BoxZoneManipulator::Update(void)
{
	const Vector3D& size = GetObject()->GetBoxSize();
	SetZoneSize(size.x, size.y, size.z);

	ZoneManipulator::Update();
}


CylinderZoneManipulator::CylinderZoneManipulator(CylinderZone *cylinder) : ZoneManipulator(cylinder)
{
	zoneRenderable.SetVertexCount(144);
	SetZonePositionArray(&outlinePosition[0]);
	SetZoneTangentArray(&outlineTangent[0]);
}

CylinderZoneManipulator::~CylinderZoneManipulator()
{
}

bool CylinderZoneManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const CylinderZoneObject *object = GetObject();
	CylinderVolumeManipulator::CalculateVolumeSphere(object->GetCylinderSize(), object->GetCylinderHeight(), sphere);
	return (true);
}

Box3D CylinderZoneManipulator::CalculateNodeBoundingBox(void) const
{
	const CylinderZoneObject *object = GetObject();
	const Vector2D& size = object->GetCylinderSize();
	float height = object->GetCylinderHeight();
	return (Box3D(Point3D(-size, 0.0F), Point3D(size, height)));
}

int32 CylinderZoneManipulator::GetHandleTable(Point3D *handle) const
{
	const CylinderZoneObject *object = GetObject();
	return (CylinderVolumeManipulator::GetHandleTable(GetObject()->GetCylinderSize(), object->GetCylinderHeight(), handle));
}

void CylinderZoneManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	CylinderVolumeManipulator::GetHandleData(index, handleData);
}

bool CylinderZoneManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	CylinderZoneObject *object = GetObject();

	Vector2D newSize = object->GetCylinderSize();
	float newHeight = object->GetCylinderHeight();
	const Vector2D& oldSize = *reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldHeight = GetOriginalSize()[2];
	bool move = CylinderVolumeManipulator::Resize(resizeData, oldSize, oldHeight, newSize, newHeight);

	object->SetCylinderSize(newSize);
	object->SetCylinderHeight(newHeight);
	return (move);
}

void CylinderZoneManipulator::Update(void)
{
	CylinderZoneObject *object = GetObject();
	const Vector2D& size = object->GetCylinderSize();
	SetZoneSize(size.x, size.y, object->GetCylinderHeight());

	ZoneManipulator::Update();
}


PolygonZoneManipulator::PolygonZoneManipulator(PolygonZone *polygon) : ZoneManipulator(polygon)
{
	SetZonePositionArray(polygonPosition);
	SetZoneTangentArray(polygonTangent);

	SetZoneSize(1.0F, 1.0F, 1.0F);
}

PolygonZoneManipulator::~PolygonZoneManipulator()
{
}

bool PolygonZoneManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const PolygonZoneObject *object = GetObject();

	const Point3D *vertex = object->GetVertexArray();
	float xmin = vertex->x;
	float ymin = vertex->y;
	float xmax = xmin;
	float ymax = ymin;

	int32 vertexCount = object->GetVertexCount();
	for (machine a = 1; a < vertexCount; a++)
	{
		const Point3D& p = vertex[a];
		float x = p.x;
		float y = p.y;

		xmin = Fmin(xmin, x);
		xmax = Fmax(xmax, x);
		ymin = Fmin(ymin, y);
		ymax = Fmax(ymax, y);
	}

	xmin *= 0.5F;
	ymin *= 0.5F;
	xmax *= 0.5F;
	ymax *= 0.5F;

	float h = object->GetPolygonHeight() * 0.5F;
	sphere->SetCenter(xmin + xmax, ymin + ymax, h);

	float x = xmax - xmin;
	float y = ymax - ymin;
	sphere->SetRadius(Sqrt(x * x + y * y + h * h));
	return (true);
}

Box3D PolygonZoneManipulator::CalculateNodeBoundingBox(void) const
{
	const PolygonZoneObject *object = GetObject();
	int32 vertexCount = object->GetVertexCount();
	const Point3D *vertex = object->GetVertexArray();

	Box3D box(vertex[0], vertex[0]);
	for (machine a = 1; a < vertexCount; a++)
	{
		box.Union(vertex[a]);
	}

	box.max.z = object->GetPolygonHeight();
	return (box);
}

int32 PolygonZoneManipulator::GetHandleTable(Point3D *handle) const
{
	const PolygonZoneObject *object = GetObject();
	int32 vertexCount = object->GetVertexCount();
	const Point3D *vertex = object->GetVertexArray();

	float x = 0.0F;
	float y = 0.0F;

	for (machine a = 0; a < vertexCount; a++)
	{
		handle[a] = vertex[a];
		x += vertex[a].x;
		y += vertex[a].y;
	}

	float f = 1.0F / (float) vertexCount;
	handle[vertexCount].Set(x * f, y * f, object->GetPolygonHeight());
	handle[vertexCount + 1].Set(x * f, y * f, 0.0F);

	return (vertexCount + 2);
}

void PolygonZoneManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	const PolygonZoneObject *object = GetObject();
	int32 vertexCount = object->GetVertexCount();
	if (index < vertexCount)
	{
		const Point3D *vertex = object->GetVertexArray();

		machine opposite = index;
		float distance = 0.0F;

		const Point3D& p = vertex[index];
		for (machine a = 0; a < vertexCount; a++) if (a != index)
		{
			float d = SquaredMag(vertex[a] - p);
			if (d > distance)
			{
				distance = d;
				opposite = a;
			}
		}

		handleData->handleFlags = 0;
		handleData->oppositeIndex = opposite;
	}
	else if (index == vertexCount)
	{
		handleData->handleFlags = kManipulatorHandlePositiveZ;
		handleData->oppositeIndex = vertexCount + 1;
	}
	else
	{
		handleData->handleFlags = kManipulatorHandleNegativeZ;
		handleData->oppositeIndex = vertexCount;
	}
}

void PolygonZoneManipulator::BeginResize(const ManipulatorResizeData *resizeData)
{
	EditorManipulator::BeginResize(resizeData);

	const PolygonZoneObject *object = GetObject();
	const Point3D *vertex = object->GetVertexArray();
	originalVertexPosition = vertex[resizeData->handleIndex];
}

Point3D PolygonZoneManipulator::ConstrainVertex(const Point3D& original, const Point3D& current, const Point3D& v1, const Point3D& v2)
{
	float x = current.x;
	float y = current.y;

	Vector2D dv(v1.x - v2.x, v1.y - v2.y);
	if ((x - v2.x) * dv.y - (y - v2.y) * dv.x < 0.0F)
	{
		float dx = x - original.x;
		float dy = y - original.y;

		Vector3D plane(dv.y, -dv.x, dv.x * v1.y - dv.y * v1.x);
		float t = -(plane.x * original.x + plane.y * original.y + plane.z) / (plane.x * dx + plane.y * dy);
		x = original.x + t * dx;
		y = original.y + t * dy;
	}

	return (Point3D(x, y, 0.0F));
}

bool PolygonZoneManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	PolygonZoneObject *object = GetObject();

	unsigned_int32 handleFlags = resizeData->handleFlags;
	if (handleFlags == 0)
	{
		Zone *zone = GetTargetNode();
		Point3D p = zone->GetWorldTransform() * (originalVertexPosition + resizeData->resizeDelta);
		p = zone->GetInverseWorldTransform() * GetEditor()->SnapToGrid(p);

		int32 count = object->GetVertexCount();
		Point3D *vertex = object->GetVertexArray();
		int32 index = resizeData->handleIndex;

		const Point3D *v1 = &vertex[(index != count - 1) ? index + 1 : 0];
		const Point3D *v2 = &vertex[(index != 0) ? index - 1 : count - 1];
		p = ConstrainVertex(originalVertexPosition, p, *v1, *v2);

		if (count > 3)
		{
			const Point3D *v3 = &vertex[(index < count - 2) ? index + 2 : index + 2 - count];
			p = ConstrainVertex(originalVertexPosition, p, *v1, *v3);

			v3 = &vertex[(index > 1) ? index - 2 : index - 2 + count];
			p = ConstrainVertex(originalVertexPosition, p, *v3, *v2);
		}

		vertex[index] = p;
	}
	else
	{
		float oldPolygonHeight = GetOriginalSize()[0];
		float dz = resizeData->resizeDelta.z;

		if (handleFlags & kManipulatorHandlePositiveZ)
		{
			object->SetPolygonHeight(Fmax(oldPolygonHeight + dz, kSizeEpsilon));
		}
		else
		{
			object->SetPolygonHeight(Fmax(oldPolygonHeight - dz, kSizeEpsilon));
			resizeData->positionOffset.z = dz;
			return (true);
		}
	}

	return (false);
}

bool PolygonZoneManipulator::Pick(const Ray *ray, PickData *data) const
{
	if (zoneRenderable.AttributeArrayEnabled(kArrayPosition))
	{
		int32 vertexCount = zoneRenderable.GetVertexCount();
		const Vector3D& size = GetZoneSize().GetVector3D();
		const Point3D *position = polygonPosition;

		float r = (ray->radius != 0.0F) ? ray->radius : Editor::kFrustumRenderScale;
		float r2 = r * r * 16.0F;

		for (machine a = 0; a < vertexCount; a += 4)
		{
			if (PickLineSegment(ray, position[0] & size, position[2] & size, r2, &data->rayParam))
			{
				a >>= 2;
				int32 index = -1;
				if (a % 3 != 2)
				{
					index = a / 3;
				}

				data->pickIndex[0] = index;
				data->pickPoint = ray->origin + data->rayParam * ray->direction;
				return (true);
			}

			position += 4;
		}
	}

	return (false);
}

void PolygonZoneManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		if (!zoneVertexBuffer.Active())
		{
			zoneVertexBuffer.Establish(sizeof(ZoneVertex) * kMaxZoneVertexCount * 12);
		}

		PolygonZoneObject *object = GetObject();

		int32 count = object->GetVertexCount();
		zoneRenderable.SetVertexCount(count * 12);

		const Point3D *vertex = object->GetVertexArray();
		float height = object->GetPolygonHeight();

		Point3D *position = polygonPosition;
		Vector4D *tangent = polygonTangent;

		const Point3D *p1 = &vertex[count - 1];
		for (machine a = 0; a < count; a++)
		{
			const Point3D *p2 = &vertex[a];
			Vector3D t = Normalize(*p2 - *p1);

			position[0].Set(p1->x, p1->y, 0.0F);
			position[1].Set(p1->x, p1->y, 0.0F);
			position[2].Set(p2->x, p2->y, 0.0F);
			position[3].Set(p2->x, p2->y, 0.0F);
			position[4].Set(p1->x, p1->y, height);
			position[5].Set(p1->x, p1->y, height);
			position[6].Set(p2->x, p2->y, height);
			position[7].Set(p2->x, p2->y, height);
			position[8].Set(p1->x, p1->y, 0.0F);
			position[9].Set(p1->x, p1->y, 0.0F);
			position[10].Set(p1->x, p1->y, height);
			position[11].Set(p1->x, p1->y, height);

			tangent[0].Set(t, -1.0F);
			tangent[1].Set(t, 1.0F);
			tangent[2].Set(t, 1.0F);
			tangent[3].Set(t, -1.0F);
			tangent[4].Set(t, -1.0F);
			tangent[5].Set(t, 1.0F);
			tangent[6].Set(t, 1.0F);
			tangent[7].Set(t, -1.0F);
			tangent[8].Set(0.0F, 0.0F, 1.0F, -1.0F);
			tangent[9].Set(0.0F, 0.0F, 1.0F, 1.0F);
			tangent[10].Set(0.0F, 0.0F, 1.0F, 1.0F);
			tangent[11].Set(0.0F, 0.0F, 1.0F, -1.0F);

			position += 12;
			tangent += 12;
			p1 = p2;
		}
	}

	ZoneManipulator::Update();
}

// ZYUQURM
