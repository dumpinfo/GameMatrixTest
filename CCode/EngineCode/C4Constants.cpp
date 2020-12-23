 

#include "C4Constants.h"


using namespace C4;


namespace
{
	unsigned_int32 hex_7F800000		= 0x7F800000;
	unsigned_int32 hex_FF800000		= 0xFF800000;
	unsigned_int32 hex_00800000		= 0x00800000;
	unsigned_int32 hex_7F7FFFFF		= 0x7F7FFFFF;
}


const float K::infinity =			*(float *) &hex_7F800000;
const float K::minus_infinity =		*(float *) &hex_FF800000;
const float K::min_float =			*(float *) &hex_00800000;
const float K::max_float =			*(float *) &hex_7F7FFFFF;

const float K::one_over_3 =			1.0F / 3.0F;
const float K::one_over_6 =			1.0F / 6.0F;
const float K::one_over_7 =			1.0F / 7.0F;
const float K::one_over_12 =		1.0F / 12.0F;
const float K::one_over_31 =		1.0F / 31.0F;
const float K::one_over_32 =		1.0F / 32.0F;
const float K::one_over_60 =		1.0F / 60.0F;
const float K::one_over_63 =		1.0F / 63.0F;
const float K::one_over_64 =		1.0F / 64.0F;
const float K::one_over_127 =		1.0F / 127.0F;
const float K::one_over_128 =		1.0F / 128.0F;
const float K::one_over_255 =		1.0F / 255.0F;
const float K::one_over_256 =		1.0F / 256.0F;
const float K::one_over_32767 =		1.0F / 32767.0F;
const float K::one_over_32768 =		1.0F / 32768.0F;
const float K::one_over_65535 =		1.0F / 65535.0F;
const float K::one_over_65536 =		1.0F / 65536.0F;

const float K::tau =				6.2831853071795864769252867665590F;
const float K::two_tau =			12.566370614359172953850573533118F;
const float K::three_tau_over_4 =	4.7123889803846898576939650749193F;
const float K::three_tau_over_8 =	2.3561944901923449288469825374596F;
const float K::tau_over_2 =			3.1415926535897932384626433832795F;
const float K::tau_over_3 =			2.0943951023931954923084289221863F;
const float K::two_tau_over_3 =		4.1887902047863909846168578443727F;
const float K::tau_over_4 =			1.5707963267948966192313216916398F;
const float K::tau_over_6 =			1.0471975511965977461542144610932F;
const float K::tau_over_8 =			0.78539816339744830961566084581988F;
const float K::tau_over_12 =		0.52359877559829887307710723054658F;
const float K::tau_over_16 =		0.39269908169872415480783042290994F;
const float K::tau_over_24 =		0.26179938779914943653855361527329F;
const float K::tau_over_40 =		0.15707963267948966192313216916398F;
const float K::one_over_tau =		1.0F / K::tau;
const float K::two_over_tau =		2.0F / K::tau;
const float K::four_over_tau =		4.0F / K::tau;
const float K::one_over_two_tau =	0.5F / K::tau;

const float K::pi =					3.1415926535897932384626433832795F;
const float K::two_pi =				6.2831853071795864769252867665590F;
const float K::four_pi =			12.566370614359172953850573533118F;
const float K::three_pi_over_2 =	4.7123889803846898576939650749193F;
const float K::three_pi_over_4 = 	2.3561944901923449288469825374596F;
const float K::two_pi_over_3 =		2.0943951023931954923084289221863F;
const float K::four_pi_over_3 = 	4.1887902047863909846168578443727F;
const float K::pi_over_2 =			1.5707963267948966192313216916398F;
const float K::pi_over_3 =			1.0471975511965977461542144610932F;
const float K::pi_over_4 =			0.78539816339744830961566084581988F;
const float K::pi_over_6 =			0.52359877559829887307710723054658F;
const float K::pi_over_8 =			0.39269908169872415480783042290994F;
const float K::pi_over_12 =			0.26179938779914943653855361527329F;
const float K::pi_over_20 =			0.15707963267948966192313216916398F;
const float K::one_over_pi =		1.0F / K::pi;
const float K::one_over_two_pi =	0.5F / K::pi;
const float K::one_over_four_pi =	0.25F / K::pi;

const float K::sqrt_2 =				1.4142135623730950488016887242097F;
const float K::sqrt_2_over_2 =		0.70710678118654752440084436210485F;
const float K::sqrt_2_over_3 =		0.47140452079103168293389624140323F;
const float K::sqrt_3 =				1.7320508075688772935274463415059F;
const float K::sqrt_3_over_2 =		0.86602540378443864676372317075294F;
const float K::sqrt_3_over_3 =		0.57735026918962576450914878050196F;

const float K::ln_2 =				0.69314718055994530941723212145818F;
const float K::one_over_ln_2 =		1.4426950408889634073599246810019F;
const float K::ln_10 =				2.3025850929940456840179914546844F;
const float K::ln_256 =				5.5451774444795624753378569716654F;

const float K::gravity =			-9.8e-6F;
const float K::half_gravity =		-4.9e-6F;
const float K::degrees =			360.0F * K::one_over_tau;
const float K::radians =			K::tau / 360.0F;

const ConstColorRGBA K::black = {0.0F, 0.0F, 0.0F, 1.0F};
const ConstColorRGBA K::white = {1.0F, 1.0F, 1.0F, 1.0F};
const ConstColorRGBA K::transparent = {0.0F, 0.0F, 0.0F, 0.0F};
const ConstColorRGBA K::red = {1.0F, 0.0F, 0.0F, 1.0F};
const ConstColorRGBA K::green = {0.0F, 1.0F, 0.0F, 1.0F};
const ConstColorRGBA K::blue = {0.0F, 0.0F, 1.0F, 1.0F};
const ConstColorRGBA K::yellow = {1.0F, 1.0F, 0.0F, 1.0F};
const ConstColorRGBA K::cyan = {0.0F, 1.0F, 1.0F, 1.0F};
const ConstColorRGBA K::magenta = {1.0F, 0.0F, 1.0F, 1.0F}; 

const ConstVector3D K::x_unit = {1.0F, 0.0F, 0.0F};
const ConstVector3D K::y_unit = {0.0F, 1.0F, 0.0F}; 
const ConstVector3D K::z_unit = {0.0F, 0.0F, 1.0F};
 
const ConstVector3D K::minus_x_unit = {-1.0F, 0.0F, 0.0F};
const ConstVector3D K::minus_y_unit = {0.0F, -1.0F, 0.0F};
const ConstVector3D K::minus_z_unit = {0.0F, 0.0F, -1.0F}; 

// ZYUQURM
