 

#include "C4Vector2D.h"


using namespace C4;


ConstPoint2D Zero2DType::zero = {0.0F, 0.0F};

const Zero2DType C4::Zero2D = {};


Vector2D& Vector2D::Rotate(float angle)
{
	Vector2D t = CosSin(angle);
	float nx = t.x * x - t.y * y;
	float ny = t.y * x + t.x * y;

	x = nx;
	y = ny;
	return (*this);
}

// ZYUQURM
