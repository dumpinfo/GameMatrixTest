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


#ifndef MGCrosshairs_h
#define MGCrosshairs_h


#include "MGBase.h"


namespace C4
{
	class Crosshairs : public Renderable
	{
		private:

			struct CrosshairVertex
			{
				Point3D		position;
				Point2D		texcoord;
			};

			bool						updateFlag;
			int32						crosshairType;
			float						crosshairSize;

			List<Renderable>			renderList;

			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;
			DiffuseTextureAttribute		textureAttribute;

		public:

			Crosshairs();
			~Crosshairs();

			int32 GetCrosshairType(void) const
			{
				return (crosshairType);
			}

			void HandleCrossTypeEvent(Variable *variable);
			void HandleCrossColorEvent(Variable *variable);
			void HandleCrossSizeEvent(Variable *variable);

			void Render(void);
	};
}


#endif

// ZYUQURM
