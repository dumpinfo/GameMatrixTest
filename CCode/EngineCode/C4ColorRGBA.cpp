 

#include "C4ColorRGBA.h"


using namespace C4;


namespace C4
{
	namespace K
	{
		extern const float one_over_255;
	}

	namespace Text
	{
		extern const char hexDigit[16];
	}
}


void ColorRGB::GetHexString(char *string) const
{
	int32 r = (int32) (red * 255.0F);
	int32 g = (int32) (green * 255.0F);
	int32 b = (int32) (blue * 255.0F);

	string[0] = Text::hexDigit[(r >> 4) & 15];
	string[1] = Text::hexDigit[r & 15];
	string[2] = Text::hexDigit[(g >> 4) & 15];
	string[3] = Text::hexDigit[g & 15];
	string[4] = Text::hexDigit[(b >> 4) & 15];
	string[5] = Text::hexDigit[b & 15];
	string[6] = 0;
}

ColorRGB& ColorRGB::SetHexString(const char *string)
{
	int32 rh = 15;
	int32 rl = 15;
	int32 gh = 15;
	int32 gl = 15;
	int32 bh = 15;
	int32 bl = 15;

	int32 k = string[0];
	if (k != 0)
	{
		rh = k - '0';
		if (rh > 9)
		{
			rh -= 7;
		}

		k = string[1];
		if (k != 0)
		{
			rl = k - '0';
			if (rl > 9)
			{
				rl -= 7;
			}

			k = string[2];
			if (k != 0)
			{
				gh = k - '0';
				if (gh > 9)
				{
					gh -= 7;
				}

				k = string[3];
				if (k != 0)
				{
					gl = k - '0';
					if (gl > 9)
					{
						gl -= 7;
					}

					k = string[4];
					if (k != 0)
					{
						bh = k - '0';
						if (bh > 9)
						{
							bh -= 7;
						}

						k = string[5];
						if (k != 0)
						{
							bl = k - '0';
							if (bl > 9)
							{
								bl -= 7;
							}
						}
					}
				}
			} 
		}
	}
 
	red = (float) (((rh << 4) | rl) * K::one_over_255);
	green = (float) (((gh << 4) | gl) * K::one_over_255); 
	blue = (float) (((bh << 4) | bl) * K::one_over_255);

	return (*this); 
}
 
 
void ColorRGBA::GetHexString(char *string) const
{
	int32 r = (int32) (red * 255.0F);
	int32 g = (int32) (green * 255.0F); 
	int32 b = (int32) (blue * 255.0F);
	int32 a = (int32) (alpha * 255.0F);

	string[0] = Text::hexDigit[(r >> 4) & 15];
	string[1] = Text::hexDigit[r & 15];
	string[2] = Text::hexDigit[(g >> 4) & 15];
	string[3] = Text::hexDigit[g & 15];
	string[4] = Text::hexDigit[(b >> 4) & 15];
	string[5] = Text::hexDigit[b & 15];
	string[6] = Text::hexDigit[(a >> 4) & 15];
	string[7] = Text::hexDigit[a & 15];
	string[8] = 0;
}

ColorRGBA& ColorRGBA::SetHexString(const char *string)
{
	int32 rh = 15;
	int32 rl = 15;
	int32 gh = 15;
	int32 gl = 15;
	int32 bh = 15;
	int32 bl = 15;
	int32 ah = 15;
	int32 al = 15;

	int32 k = string[0];
	if (k != 0)
	{
		rh = k - '0';
		if (rh > 9)
		{
			rh -= 7;
		}

		k = string[1];
		if (k != 0)
		{
			rl = k - '0';
			if (rl > 9)
			{
				rl -= 7;
			}

			k = string[2];
			if (k != 0)
			{
				gh = k - '0';
				if (gh > 9)
				{
					gh -= 7;
				}

				k = string[3];
				if (k != 0)
				{
					gl = k - '0';
					if (gl > 9)
					{
						gl -= 7;
					}

					k = string[4];
					if (k != 0)
					{
						bh = k - '0';
						if (bh > 9)
						{
							bh -= 7;
						}

						k = string[5];
						if (k != 0)
						{
							bl = k - '0';
							if (bl > 9)
							{
								bl -= 7;
							}

							k = string[6];
							if (k != 0)
							{
								ah = k - '0';
								if (ah > 9)
								{
									ah -= 7;
								}

								k = string[7];
								if (k != 0)
								{
									al = k - '0';
									if (al > 9)
									{
										al -= 7;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	red = (float) (((rh << 4) | rl) * K::one_over_255);
	green = (float) (((gh << 4) | gl) * K::one_over_255);
	blue = (float) (((bh << 4) | bl) * K::one_over_255);
	alpha = (float) (((ah << 4) | al) * K::one_over_255);

	return (*this);
}

// ZYUQURM
