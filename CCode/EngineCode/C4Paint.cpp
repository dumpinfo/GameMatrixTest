 

#include "C4Paint.h"
#include "C4Graphics.h"


using namespace C4;


PaintState::PaintState()
{
	brushRadius = 8.0F;
	brushFuzziness = 0.5F;
	brushOpacity = 1.0F;
	brushColor.Set(1.0F, 1.0F, 1.0F, 1.0F);

	for (machine a = 0; a < 4; a++)
	{
		channelMask[a] = true;
	}
}

PaintState::PaintState(const PaintState& paintState)
{
	brushRadius = paintState.brushRadius;
	brushFuzziness = paintState.brushFuzziness;
	brushOpacity = paintState.brushOpacity;
	brushColor = paintState.brushColor;

	for (machine a = 0; a < 4; a++)
	{
		channelMask[a] = paintState.channelMask[a];
	}
}

PaintState::~PaintState()
{
}

void PaintState::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('BRAD', 4);
	data << brushRadius;

	data << ChunkHeader('BFUZ', 4);
	data << brushFuzziness;

	data << ChunkHeader('BOPC', 4);
	data << brushOpacity;

	data << ChunkHeader('BCOL', sizeof(ColorRGBA));
	data << brushColor;

	data << ChunkHeader('MASK', 16);
	for (machine a = 0; a < 4; a++)
	{
		data << channelMask[a];
	}

	data << TerminatorChunk;
}

void PaintState::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<PaintState>(data, unpackFlags);
}

bool PaintState::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'BRAD':

			data >> brushRadius;
			return (true);

		case 'BFUZ':

			data >> brushFuzziness;
			return (true);

		case 'BOPC':

			data >> brushOpacity;
			return (true);

		case 'BCOL':

			data >> brushColor;
			return (true);

		case 'MASK':

			for (machine a = 0; a < 4; a++)
			{
				data >> channelMask[a];
			}

			return (true);
	}

	return (false);
} 


Painter::Painter(const Integer2D& resolution, int32 count, void *image, const PaintState *state) 
{
	paintResolution = resolution; 
	channelCount = count;

	paintImage = image; 
	paintState = state;
 
	previousImage = nullptr; 
}

Painter::~Painter()
{ 
	delete[] previousImage;
}

void Painter::BeginPainting(void)
{
	int32 pixelCount = paintResolution.x * paintResolution.y;

	if (!previousImage)
	{
		previousImage = new unsigned_int8[pixelCount * (channelCount + 1)];
		transferImage = previousImage + pixelCount * channelCount;
		MemoryMgr::ClearMemory(transferImage, pixelCount);
	}

	MemoryMgr::CopyMemory(paintImage, previousImage, pixelCount * channelCount);
	paintBounds.Set(paintResolution.x, paintResolution.y, 0, 0);
}

void Painter::EndPainting(void)
{
	if (!paintBounds.Empty())
	{
		int32 left = paintBounds.left & ~3;
		int32 top = paintBounds.top;
		int32 width = (((paintBounds.right + 3) & ~3) - left) >> 2;
		int32 height = paintBounds.bottom - top;
		int32 row = paintResolution.x >> 2;

		unsigned_int32 *transfer = reinterpret_cast<unsigned_int32 *>(transferImage + (top * paintResolution.x + left));
		for (machine j = 0; j < height; j++)
		{
			for (machine i = 0; i < width; i++)
			{
				transfer[i] = 0;
			}

			transfer += row;
		}
	}
}

void Painter::UpdateImage1(void)
{
	const ColorRGBA& brushColor = paintState->GetBrushColor();
	unsigned_int32 brushLum = Max(Max((unsigned_int32) (brushColor.red * 255.0F), (unsigned_int32) (brushColor.green * 255.0F)), (unsigned_int32) (brushColor.blue * 255.0F));

	int32 left = paintBounds.left;
	int32 top = paintBounds.top;
	int32 width = paintBounds.right - left;
	int32 height = paintBounds.bottom - top;

	int32 offset = top * paintResolution.x + left;
	Color1C *paint = static_cast<Color1C *>(paintImage) + offset;
	const Color1C *previous = reinterpret_cast<Color1C *>(previousImage) + offset;
	const unsigned_int8 *transfer = transferImage + offset;

	for (machine j = 0; j < height; j++)
	{
		for (machine i = 0; i < width; i++)
		{
			unsigned_int32 a = transfer[i];
			if (a != 0)
			{
				a += a >> 7;
				unsigned_int32 b = 256 - a;
				paint[i] = (Color1C) ((brushLum * a + previous[i] * b) >> 8);
			}
		}

		paint += paintResolution.x;
		previous += paintResolution.x;
		transfer += paintResolution.x;
	}
}

void Painter::UpdateImage2(void)
{
	const ColorRGBA& brushColor = paintState->GetBrushColor();
	unsigned_int32 brushLum = Max(Max((unsigned_int32) (brushColor.red * 255.0F), (unsigned_int32) (brushColor.green * 255.0F)), (unsigned_int32) (brushColor.blue * 255.0F));
	unsigned_int32 brushAlpha = (unsigned_int32) (brushColor.alpha * 255.0F);

	int32 left = paintBounds.left;
	int32 top = paintBounds.top;
	int32 width = paintBounds.right - left;
	int32 height = paintBounds.bottom - top;

	int32 offset = top * paintResolution.x + left;
	Color2C *paint = static_cast<Color2C *>(paintImage) + offset;
	const Color2C *previous = reinterpret_cast<Color2C *>(previousImage) + offset;
	const unsigned_int8 *transfer = transferImage + offset;

	const bool *channelMask = paintState->GetChannelMask();
	bool colorMask = channelMask[0] | channelMask[1] | channelMask[2];
	if (colorMask & channelMask[3])
	{
		for (machine j = 0; j < height; j++)
		{
			for (machine i = 0; i < width; i++)
			{
				unsigned_int32 a = transfer[i];
				if (a != 0)
				{
					a += a >> 7;
					unsigned_int32 b = 256 - a;

					const Color2C& p = previous[i];
					UnsignedFixed lum = brushLum * a + p.GetLum() * b;
					UnsignedFixed alpha = brushAlpha * a + p.GetAlpha() * b;

					paint[i].Set(lum >> 8, alpha >> 8);
				}
			}

			paint += paintResolution.x;
			previous += paintResolution.x;
			transfer += paintResolution.x;
		}
	}
	else
	{
		unsigned_int16 mask = Color2C(-colorMask & 0xFF, -channelMask[3] & 0xFF).GetPackedColor();

		for (machine j = 0; j < height; j++)
		{
			for (machine i = 0; i < width; i++)
			{
				unsigned_int32 a = transfer[i];
				if (a != 0)
				{
					a += a >> 7;
					unsigned_int32 b = 256 - a;

					const Color2C& p = previous[i];
					UnsignedFixed lum = brushLum * a + p.GetLum() * b;
					UnsignedFixed alpha = brushAlpha * a + p.GetAlpha() * b;

					Color2C color(lum >> 8, alpha >> 8);
					paint[i].SetPackedColor((color.GetPackedColor() & mask) | (p.GetPackedColor() & ~mask));
				}
			}

			paint += paintResolution.x;
			previous += paintResolution.x;
			transfer += paintResolution.x;
		}
	}
}

void Painter::UpdateImage4(void)
{
	const ColorRGBA& brushColor = paintState->GetBrushColor();
	unsigned_int32 brushRed = (unsigned_int32) (brushColor.red * 255.0F);
	unsigned_int32 brushGreen = (unsigned_int32) (brushColor.green * 255.0F);
	unsigned_int32 brushBlue = (unsigned_int32) (brushColor.blue * 255.0F);
	unsigned_int32 brushAlpha = (unsigned_int32) (brushColor.alpha * 255.0F);

	int32 left = paintBounds.left;
	int32 top = paintBounds.top;
	int32 width = paintBounds.right - left;
	int32 height = paintBounds.bottom - top;

	int32 offset = top * paintResolution.x + left;
	Color4C *paint = static_cast<Color4C *>(paintImage) + offset;
	const Color4C *previous = reinterpret_cast<Color4C *>(previousImage) + offset;
	const unsigned_int8 *transfer = transferImage + offset;

	const bool *channelMask = paintState->GetChannelMask();
	if (channelMask[0] & channelMask[1] & channelMask[2] & channelMask[3])
	{
		for (machine j = 0; j < height; j++)
		{
			for (machine i = 0; i < width; i++)
			{
				unsigned_int32 a = transfer[i];
				if (a != 0)
				{
					a += a >> 7;
					unsigned_int32 b = 256 - a;

					const Color4C& p = previous[i];
					UnsignedFixed red = brushRed * a + p.GetRed() * b;
					UnsignedFixed green = brushGreen * a + p.GetGreen() * b;
					UnsignedFixed blue = brushBlue * a + p.GetBlue() * b;
					UnsignedFixed alpha = brushAlpha * a + p.GetAlpha() * b;

					paint[i].Set(red >> 8, green >> 8, blue >> 8, alpha >> 8);
				}
			}

			paint += paintResolution.x;
			previous += paintResolution.x;
			transfer += paintResolution.x;
		}
	}
	else
	{
		unsigned_int32 mask = Color4C(-channelMask[0] & 0xFF, -channelMask[1] & 0xFF, -channelMask[2] & 0xFF, -channelMask[3] & 0xFF).GetPackedColor();

		for (machine j = 0; j < height; j++)
		{
			for (machine i = 0; i < width; i++)
			{
				unsigned_int32 a = transfer[i];
				if (a != 0)
				{
					a += a >> 7;
					unsigned_int32 b = 256 - a;

					const Color4C& p = previous[i];
					UnsignedFixed red = brushRed * a + p.GetRed() * b;
					UnsignedFixed green = brushGreen * a + p.GetGreen() * b;
					UnsignedFixed blue = brushBlue * a + p.GetBlue() * b;
					UnsignedFixed alpha = brushAlpha * a + p.GetAlpha() * b;

					Color4C color(red >> 8, green >> 8, blue >> 8, alpha >> 8);
					paint[i].SetPackedColor((color.GetPackedColor() & mask) | (p.GetPackedColor() & ~mask));
				}
			}

			paint += paintResolution.x;
			previous += paintResolution.x;
			transfer += paintResolution.x;
		}
	}
}

bool Painter::UpdateImage(void)
{
	if (!paintBounds.Empty())
	{
		int32 count = channelCount;
		if (count == 4)
		{
			UpdateImage4();
		}
		else if (count == 2)
		{
			UpdateImage2();
		}
		else
		{
			UpdateImage1();
		}

		return (true);
	}

	return (false);
}

const void *Painter::CreateUndoImage(const Rect& rect) const
{
	unsigned_int8	*undoImage;

	int32 left = rect.left;
	int32 top = rect.top;
	int32 width = (rect.right - left) * channelCount;
	int32 height = rect.bottom - top;

	unsigned_int32 imageSize = width * height;
	unsigned_int8 *deltaImage = new unsigned_int8[imageSize * 2];

	int32 offset = (top * paintResolution.x + left) * channelCount;
	const unsigned_int8 *paint = static_cast<unsigned_int8 *>(paintImage) + offset;
	const unsigned_int8 *previous = previousImage + offset;
	unsigned_int8 *delta = deltaImage;

	int32 rowSkip = paintResolution.x * channelCount;
	for (machine j = 0; j < height; j++)
	{
		for (machine i = 0; i < width; i++)
		{
			delta[i] = paint[i] ^ previous[i];
		}

		paint += rowSkip;
		previous += rowSkip;
		delta += width;
	}

	unsigned_int8 *compressedImage = deltaImage + imageSize;
	unsigned_int32 compressedSize = Comp::CompressData(deltaImage, imageSize, compressedImage);
	if (compressedSize != 0)
	{
		undoImage = new unsigned_int8[compressedSize + 4];
		reinterpret_cast<unsigned_int32 *>(undoImage)[0] = compressedSize;
		MemoryMgr::CopyMemory(compressedImage, undoImage + 4, compressedSize);
	}
	else
	{
		undoImage = new unsigned_int8[imageSize + 4];
		reinterpret_cast<unsigned_int32 *>(undoImage)[0] = 0;
		MemoryMgr::CopyMemory(deltaImage, undoImage + 4, imageSize);
	}

	delete[] deltaImage;
	return (undoImage);
}

void Painter::ApplyUndoImage(const PaintSpaceObject *object, const Rect& rect, const void *undoImage)
{
	const unsigned_int8		*deltaImage;

	const Integer2D& paintResolution = object->GetPaintResolution();
	int32 channelCount = object->GetChannelCount();

	int32 left = rect.left;
	int32 top = rect.top;
	int32 width = (rect.right - left) * channelCount;
	int32 height = rect.bottom - top;

	unsigned_int32 compressedSize = static_cast<const unsigned_int32 *>(undoImage)[0];
	if (compressedSize != 0)
	{
		unsigned_int8 *delta = new unsigned_int8[width * height];
		Comp::DecompressData(static_cast<const unsigned_int8 *>(undoImage) + 4, compressedSize, delta);
		deltaImage = delta;
	}
	else
	{
		deltaImage = static_cast<const unsigned_int8 *>(undoImage) + 4;
	}

	int32 offset = (top * paintResolution.x + left) * channelCount;
	unsigned_int8 *paint = static_cast<unsigned_int8 *>(object->GetPaintImage()) + offset;
	const unsigned_int8 *delta = deltaImage;

	int32 rowSkip = paintResolution.x * channelCount;
	for (machine j = 0; j < height; j++)
	{
		for (machine i = 0; i < width; i++)
		{
			paint[i] ^= delta[i];
		}

		paint += rowSkip;
		delta += width;
	}

	if (compressedSize != 0)
	{
		delete[] deltaImage;
	}
}

void Painter::ReleaseUndoImage(const void *undoImage)
{
	delete[] static_cast<const unsigned_int8 *>(undoImage);
}

void Painter::DrawDot(const Point2D& p)
{
	float radius = paintState->GetBrushRadius();
	int32 size = (int32) radius;

	int32 left = MaxZero((int32) Floor(p.x) - size);
	int32 top = MaxZero((int32) Floor(p.y) - size);
	int32 right = Min((int32) Ceil(p.x) + size + 1, paintResolution.x);
	int32 bottom = Min((int32) Ceil(p.y) + size + 1, paintResolution.y);
	paintBounds |= Rect(left, top, right, bottom);

	float fuzziness = 1.0F / ((radius - 1.0F) * paintState->GetBrushFuzziness() + 1.0F);
	float opacity = paintState->GetBrushOpacity() * 255.0F;

	unsigned_int8 *transfer = transferImage + paintResolution.x * top;
	for (machine j = top; j < bottom; j++)
	{
		float k = (float) j - p.y;
		k = k * k;

		for (machine i = left; i < right; i++)
		{
			float m = (float) i - p.x;
			float r = Sqrt(k + m * m);
			if (r < radius)
			{
				int32 a = MaxZero((int32) (Fmin((radius - r) * fuzziness, 1.0F) * opacity + 0.5F));
				transfer[i] = (unsigned_int8) Max(transfer[i], a);
			}
		}

		transfer += paintResolution.x;
	}
}

void Painter::DrawLine(const Point2D& p1, const Point2D& p2)
{
	DrawDot(p1);
	DrawDot(p2);

	float x1 = p1.x;
	float y1 = p1.y;
	float x2 = p2.x;
	float y2 = p2.y;

	float dx = x2 - x1;
	float dy = y2 - y1;

	float radius = paintState->GetBrushRadius();
	float fuzziness = 1.0F / ((radius - 1.0F) * paintState->GetBrushFuzziness() + 1.0F);
	float opacity = paintState->GetBrushOpacity() * 255.0F;

	float ax = Fabs(dx);
	float ay = Fabs(dy);

	if (ax < ay)
	{
		if (ay > 1.0F)
		{
			if (y1 > y2)
			{
				Exchange(x1, x2);
				Exchange(y1, y2);
				dx = -dx;
				dy = -dy;
			}

			float s = (float) dx / (float) dy;
			float t = 1.0F + s * s;
			float f = InverseSqrt(t);
			int32 size = (int32) (f * t * radius);

			int32 left = MaxZero((int32) Floor(Fmin(x1, x2)) - size);
			int32 top = MaxZero((int32) Floor(y1) - size);
			int32 right = Min((int32) Ceil(Fmax(x1, x2)) + size + 2, paintResolution.x);
			int32 bottom = Min((int32) Ceil(y2) + size + 2, paintResolution.y);
			paintBounds |= Rect(left, top, right, bottom);

			float w1 = -dx * x1 - dy * y1;
			float w2 = -dx * x2 - dy * y2;

			float x = x1 + ((float) top - y1) * s;
			for (machine j = top; j < bottom; j++)
			{
				left = MaxZero((int32) Floor(x) - size);
				right = Min((int32) Ceil(x) + size + 2, paintResolution.x);
				float k = (float) j * dy;

				unsigned_int8 *transfer = transferImage + paintResolution.x * j;
				for (machine i = left; i < right; i++)
				{
					float m = (float) i;
					float d = m * dx + k;
					if ((d + w1 > 0.0F) && (d + w2 < 0.0F))
					{
						float r = (m - x) * f;
						int32 a = MaxZero((int32) (Fmin((radius - Fabs(r)) * fuzziness, 1.0F) * opacity + 0.5F));
						transfer[i] = (unsigned_int8) Max(transfer[i], a);
					}
				}

				x += s;
			}
		}
	}
	else if (ax > 1.0F)
	{
		if (x1 > x2)
		{
			Exchange(x1, x2);
			Exchange(y1, y2);
			dx = -dx;
			dy = -dy;
		}

		float s = (float) dy / (float) dx;
		float t = 1.0F + s * s;
		float f = InverseSqrt(t);
		int32 size = (int32) (f * t * radius);

		int32 left = MaxZero((int32) Floor(x1) - size);
		int32 top = MaxZero((int32) Floor(Fmin(y1, y2)) - size);
		int32 right = Min((int32) Ceil(x2) + size + 2, paintResolution.x);
		int32 bottom = Min((int32) Ceil(Fmax(y1, y2)) + size + 2, paintResolution.y);
		paintBounds |= Rect(left, top, right, bottom);

		float w1 = -dx * x1 - dy * y1;
		float w2 = -dx * x2 - dy * y2;

		float y = y1 + ((float) left - x1) * s;
		for (machine i = left; i < right; i++)
		{
			top = MaxZero((int32) Floor(y) - size);
			bottom = Min((int32) Ceil(y) + size + 2, paintResolution.y);
			float k = (float) i * dx;

			unsigned_int8 *transfer = transferImage + paintResolution.x * top + i;
			for (machine j = top; j < bottom; j++)
			{
				float m = (float) j;
				float d = k + m * dy;
				if ((d + w1 > 0.0F) && (d + w2 < 0.0F))
				{
					float r = (m - y) * f;
					int32 a = MaxZero((int32) (Fmin((radius - Fabs(r)) * fuzziness, 1.0F) * opacity + 0.5F));
					*transfer = (unsigned_int8) Max(*transfer, a);
				}

				transfer += paintResolution.x;
			}

			y += s;
		}
	}
}


PaintTexcoordProcess::PaintTexcoordProcess() : InterpolantProcess(kProcessPaintTexcoord)
{
}

PaintTexcoordProcess::PaintTexcoordProcess(const PaintTexcoordProcess& paintTexcoordProcess) : InterpolantProcess(paintTexcoordProcess)
{
}

PaintTexcoordProcess::~PaintTexcoordProcess()
{
}

Process *PaintTexcoordProcess::Replicate(void) const
{
	return (new PaintTexcoordProcess(*this));
}


PaintTextureProcess::PaintTextureProcess() : Process(kProcessPaintTexture)
{
}

PaintTextureProcess::PaintTextureProcess(const PaintTextureProcess& paintTextureProcess) : Process(paintTextureProcess)
{
}

PaintTextureProcess::~PaintTextureProcess()
{
}

Process *PaintTextureProcess::Replicate(void) const
{
	return (new PaintTextureProcess(*this));
}

int32 PaintTextureProcess::GetPortCount(void) const
{
	return (1);
}

const char *PaintTextureProcess::GetPortName(int32 index) const
{
	return ("TEXC");
}

const Texture *PaintTextureProcess::GetPaintTexture(const ShaderCompileData *compileData)
{
	const Texture *const *texture = compileData->renderable->GetPaintEnvironment()->paintTexture;
	return ((texture) ? *texture : TheGraphicsMgr->GetNullTexture());
}

#if C4MACOS

	int32 PaintTextureProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
	{
		int32 count = Process::GenerateProcessSignature(compileData, signature);

		TextureFormat format = GetPaintTexture(compileData)->GetImageFormat();
		if ((format == kTextureI8) || (format == kTextureLA8))
		{
			signature[count] = format;
			count++;
		}

		return (count);
	}

#endif

void PaintTextureProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->textureCount = 1;
	data->outputSize = 4;
	data->inputSize[0] = 2;
	data->textureObject[0] = GetPaintTexture(compileData);
}

int32 PaintTextureProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = %TRG0(%IMG0, %0);\n"
	};

	#if !C4MACOS

		shaderCode[0] = code;

	#else

		static const char intensityCode[] =
		{
			"# = %TRG0(%IMG0, %0).xxxw;\n"
		};

		static const char luminanceAlphaCode[] =
		{
			"# = %TRG0(%IMG0, %0).xxxy;\n"
		};

		TextureFormat format = GetPaintTexture(compileData)->GetImageFormat();
		if (format == kTextureI8)
		{
			shaderCode[0] = intensityCode;
		}
		else if (format == kTextureLA8)
		{
			shaderCode[0] = luminanceAlphaCode;
		}
		else
		{
			shaderCode[0] = code;
		}

	#endif

	return (1);
}


PaintWidget::PaintWidget() : ImageWidget(kWidgetPaint)
{
	paintResolution.Set(128, 128);
	Initialize();
}

PaintWidget::PaintWidget(const Vector2D& size, const Integer2D& resolution) : ImageWidget(kWidgetPaint, size)
{
	paintResolution.Set(Max((resolution.x + 3) & ~3, kPaintMinResolution), Max((resolution.y + 3) & ~3, kPaintMinResolution));
	Initialize();
}

PaintWidget::PaintWidget(const PaintWidget& paintWidget) :
		ImageWidget(paintWidget),
		paintState(paintWidget.paintState)
{
	paintResolution = paintWidget.paintResolution;
	backgroundColor = paintWidget.backgroundColor;

	paintImage = nullptr;
	painter = nullptr;
}

PaintWidget::~PaintWidget()
{
	delete painter;
	delete[] paintImage;
}

Widget *PaintWidget::Replicate(void) const
{
	return (new PaintWidget(*this));
}

void PaintWidget::Initialize(void)
{
	backgroundColor.Set(0.0F, 0.0F, 0.0F, 0.0F);

	paintImage = nullptr;
	painter = nullptr;

	SetWidgetUsage(kWidgetGeneratedImage);
}

void PaintWidget::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static FunctionReg<GetPaintWidgetBrushRadiusFunction> getPaintWidgetBrushRadiusRegistration(registration, kFunctionGetPaintWidgetBrushRadius, table->GetString(StringID('CTRL', kControllerPanel, kFunctionGetPaintWidgetBrushRadius)), kFunctionOutputValue);
	static FunctionReg<SetPaintWidgetBrushRadiusFunction> setPaintWidgetBrushRadiusRegistration(registration, kFunctionSetPaintWidgetBrushRadius, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetPaintWidgetBrushRadius)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<GetPaintWidgetBrushFuzzinessFunction> getPaintWidgetBrushFuzzinessRegistration(registration, kFunctionGetPaintWidgetBrushFuzziness, table->GetString(StringID('CTRL', kControllerPanel, kFunctionGetPaintWidgetBrushFuzziness)), kFunctionOutputValue);
	static FunctionReg<SetPaintWidgetBrushFuzzinessFunction> setPaintWidgetBrushFuzzinessRegistration(registration, kFunctionSetPaintWidgetBrushFuzziness, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetPaintWidgetBrushFuzziness)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<GetPaintWidgetBrushOpacityFunction> getPaintWidgetBrushOpacityRegistration(registration, kFunctionGetPaintWidgetBrushOpacity, table->GetString(StringID('CTRL', kControllerPanel, kFunctionGetPaintWidgetBrushOpacity)), kFunctionOutputValue);
	static FunctionReg<SetPaintWidgetBrushOpacityFunction> setPaintWidgetBrushOpacityRegistration(registration, kFunctionSetPaintWidgetBrushOpacity, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetPaintWidgetBrushOpacity)), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<GetPaintWidgetBrushColorFunction> getPaintWidgetBrushColorRegistration(registration, kFunctionGetPaintWidgetBrushColor, table->GetString(StringID('CTRL', kControllerPanel, kFunctionGetPaintWidgetBrushColor)), kFunctionOutputValue);
	static FunctionReg<SetPaintWidgetBrushColorFunction> setPaintWidgetBrushColorRegistration(registration, kFunctionSetPaintWidgetBrushColor, table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetPaintWidgetBrushColor)), kFunctionRemote | kFunctionJournaled);
}

void PaintWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ImageWidget::Pack(data, packFlags);

	data << ChunkHeader('RESO', sizeof(Integer2D));
	data << paintResolution;

	data << ChunkHeader('BACK', sizeof(ColorRGBA));
	data << backgroundColor;

	PackHandle handle = data.BeginChunk('STAT');
	paintState.Pack(data, packFlags);
	data.EndChunk(handle);

	if (paintImage)
	{
		int32 dataSize = paintResolution.x * paintResolution.y * sizeof(Color4C);
		unsigned_int8 *compressedData = new unsigned_int8[dataSize];

		unsigned_int32 compressedSize = Comp::CompressData(paintImage, dataSize, compressedData);
		if (compressedSize != 0)
		{
			data << ChunkHeader('ICMP', 4 + compressedSize);
			data << compressedSize;

			data.WriteData(compressedData, compressedSize);
		}
		else
		{
			data << ChunkHeader('IRAW', dataSize);
			data.WriteData(paintImage, dataSize);
		}

		delete[] compressedData;
	}

	data << TerminatorChunk;
}

void PaintWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ImageWidget::Unpack(data, unpackFlags);
	UnpackChunkList<PaintWidget>(data, unpackFlags);
}

bool PaintWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'RESO':

			data >> paintResolution;
			return (true);

		case 'BACK':

			data >> backgroundColor;
			return (true);

		case 'STAT':

			paintState.Unpack(data, unpackFlags);
			return (true);

		case 'ICMP':
		{
			unsigned_int32	compressedSize;

			data >> compressedSize;
			unsigned_int8 *compressedData = new unsigned_int8[compressedSize];
			data.ReadData(compressedData, compressedSize);

			int32 pixelCount = paintResolution.x * paintResolution.y;
			paintImage = new Color4C[pixelCount];
			Comp::DecompressData(compressedData, compressedSize, paintImage);

			delete[] compressedData;
			return (true);
		}

		case 'IRAW':
		{
			int32 pixelCount = paintResolution.x * paintResolution.y;
			paintImage = new Color4C[pixelCount];
			data.ReadData(paintImage, pixelCount * sizeof(Color4C));
			return (true);
		}
	}

	return (false);
}

void *PaintWidget::BeginSettingsUnpack(void)
{
	delete[] paintImage;
	paintImage = nullptr;

	return (ImageWidget::BeginSettingsUnpack());
}

int32 PaintWidget::GetSettingCount(void) const
{
	return (ImageWidget::GetSettingCount() + 8);
}

Setting *PaintWidget::GetSetting(int32 index) const
{
	int32 count = ImageWidget::GetSettingCount();
	if (index < count)
	{
		return (ImageWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPaint, 'SETT'));
		return (new HeadingSetting(kWidgetPaint, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPaint, 'XRES'));
		return (new IntegerSetting('XRES', paintResolution.x, title, kPaintMinResolution, kPaintMaxResolution, 16));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPaint, 'YRES'));
		return (new IntegerSetting('YRES', paintResolution.y, title, kPaintMinResolution, kPaintMaxResolution, 16));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPaint, 'BACK'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetPaint, 'PICK'));
		return (new ColorSetting('BACK', backgroundColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPaint, 'BRAD'));
		return (new TextSetting('BRAD', paintState.GetBrushRadius(), title));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPaint, 'BFUZ'));
		return (new FloatSetting('BFUZ', paintState.GetBrushFuzziness(), title, 0.0F, 1.0F, 0.05F));
	}

	if (index == count + 6)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPaint, 'BOPC'));
		return (new IntegerSetting('BOPC', (int32) (paintState.GetBrushOpacity() * 100.0F + 0.5F), title, 1, 100, 1));
	}

	if (index == count + 7)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetPaint, 'BCOL'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetPaint, 'PCOL'));
		return (new ColorSetting('BCOL', paintState.GetBrushColor(), title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void PaintWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'XRES')
	{
		paintResolution.x = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (identifier == 'YRES')
	{
		paintResolution.y = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (identifier == 'BACK')
	{
		backgroundColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'BRAD')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		paintState.SetBrushRadius(Fmax(Text::StringToFloat(text), 1.0F));
	}
	else if (identifier == 'BFUZ')
	{
		paintState.SetBrushFuzziness(static_cast<const FloatSetting *>(setting)->GetFloatValue());
	}
	else if (identifier == 'BOPC')
	{
		paintState.SetBrushOpacity((float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F);
	}
	else if (identifier == 'BCOL')
	{
		paintState.SetBrushColor(static_cast<const ColorSetting *>(setting)->GetColor());
	}
	else
	{
		ImageWidget::SetSetting(setting);
	}
}

void PaintWidget::Preprocess(void)
{
	ImageWidget::Preprocess();

	delete painter;
	painter = nullptr;

	if (!GetManipulator())
	{
		const PanelController *controller = GetPanelController();
		if ((!controller) || (!controller->GetTargetNode()->GetManipulator()))
		{
			if (!paintImage)
			{
				int32 pixelCount = paintResolution.x * paintResolution.y;
				paintImage = new Color4C[pixelCount];

				unsigned_int32 red = (unsigned_int32) (backgroundColor.red * 255.0F + 0.5F);
				unsigned_int32 green = (unsigned_int32) (backgroundColor.green * 255.0F + 0.5F);
				unsigned_int32 blue = (unsigned_int32) (backgroundColor.blue * 255.0F + 0.5F);
				unsigned_int32 alpha = (unsigned_int32) (backgroundColor.alpha * 255.0F + 0.5F);
				Color4C color(red, green, blue, alpha);

				Color4C *image = paintImage;
				for (machine a = 0; a < pixelCount; a += 4)
				{
					image[0] = color;
					image[1] = color;
					image[2] = color;
					image[3] = color;
					image += 4;
				}
			}

			textureHeader.textureType = kTextureRectangle;
			textureHeader.textureFlags = 0;
			textureHeader.colorSemantic = kTextureSemanticNone;
			textureHeader.alphaSemantic = kTextureSemanticNone;
			textureHeader.imageFormat = kTextureRGBA8;
			textureHeader.imageWidth = paintResolution.x;
			textureHeader.imageHeight = paintResolution.y;
			textureHeader.imageDepth = 1;
			textureHeader.wrapMode[0] = kTextureClamp;
			textureHeader.wrapMode[1] = kTextureClamp;
			textureHeader.wrapMode[2] = kTextureClamp;
			textureHeader.mipmapCount = 1;
			textureHeader.mipmapDataOffset = 0;
			textureHeader.auxiliaryDataSize = 0;
			textureHeader.auxiliaryDataOffset = 0;

			SetTexture(0, &textureHeader, paintImage);
			SetImageScale(Vector2D(1.0F, 1.0F));
		}
	}

	if (paintImage)
	{
		painter = new Painter(paintResolution, 4, paintImage, &paintState);
	}
	else
	{
		SetTexture(0, "C4/checker");
	}
}

void PaintWidget::Build(void)
{
	if (!paintImage)
	{
		const Vector2D& size = GetWidgetSize();
		SetImageScale(Vector2D(size.x * 0.03125F, size.y * 0.03125F));
	}

	ImageWidget::Build();
}

void PaintWidget::UpdateImage(void)
{
	if (painter->UpdateImage())
	{
		Texture *texture = GetTexture();
		texture->UpdateRect(painter->GetPaintBounds());
	}
}

void PaintWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;

	const Vector2D& size = GetWidgetSize();
	float x = eventData->mousePosition.x * (float) paintResolution.x / size.x;
	float y = (size.y - 1.0F - eventData->mousePosition.y) * (float) paintResolution.y / size.y;
	Point2D position(x, y);

	if (eventType == kEventMouseDown)
	{
		painter->BeginPainting();
		painter->DrawDot(position);
		brushPosition = position;

		UpdateImage();
	}
	else if (eventType == kEventMouseMoved)
	{
		painter->DrawLine(brushPosition, position);
		brushPosition = position;

		UpdateImage();
	}
	else if (eventType == kEventMouseUp)
	{
		painter->DrawLine(brushPosition, position);
		brushPosition = position;

		UpdateImage();
		painter->EndPainting();
	}
}


GetPaintWidgetBrushRadiusFunction::GetPaintWidgetBrushRadiusFunction() : WidgetFunction(kFunctionGetPaintWidgetBrushRadius)
{
}

GetPaintWidgetBrushRadiusFunction::GetPaintWidgetBrushRadiusFunction(const GetPaintWidgetBrushRadiusFunction& getPaintWidgetBrushRadiusFunction) : WidgetFunction(getPaintWidgetBrushRadiusFunction)
{
}

GetPaintWidgetBrushRadiusFunction::~GetPaintWidgetBrushRadiusFunction()
{
}

Function *GetPaintWidgetBrushRadiusFunction::Replicate(void) const
{
	return (new GetPaintWidgetBrushRadiusFunction(*this));
}

void GetPaintWidgetBrushRadiusFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	const Widget *widget = root->FindWidget(GetWidgetKey());
	if ((widget) && (widget->GetWidgetType() == kWidgetPaint))
	{
		method->SetOutputValue(state, static_cast<const PaintWidget *>(widget)->GetPaintState()->GetBrushRadius());
	}

	CallCompletionProc();
}


SetPaintWidgetBrushRadiusFunction::SetPaintWidgetBrushRadiusFunction() : WidgetFunction(kFunctionSetPaintWidgetBrushRadius)
{
	brushRadius = 1.0F;
}

SetPaintWidgetBrushRadiusFunction::SetPaintWidgetBrushRadiusFunction(const SetPaintWidgetBrushRadiusFunction& setPaintWidgetBrushRadiusFunction) : WidgetFunction(setPaintWidgetBrushRadiusFunction)
{
	brushRadius = setPaintWidgetBrushRadiusFunction.brushRadius;
}

SetPaintWidgetBrushRadiusFunction::~SetPaintWidgetBrushRadiusFunction()
{
}

Function *SetPaintWidgetBrushRadiusFunction::Replicate(void) const
{
	return (new SetPaintWidgetBrushRadiusFunction(*this));
}

void SetPaintWidgetBrushRadiusFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	data << brushRadius;
}

void SetPaintWidgetBrushRadiusFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);

	data >> brushRadius;
}

void SetPaintWidgetBrushRadiusFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << brushRadius;
}

bool SetPaintWidgetBrushRadiusFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		data >> brushRadius;
		return (true);
	}

	return (false);
}

int32 SetPaintWidgetBrushRadiusFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 1);
}

Setting *SetPaintWidgetBrushRadiusFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetPaintWidgetBrushRadius, 'RADI'));
		return (new FloatSetting('BRAD', brushRadius, title, 1.0F, 64.0F, 1.0F));
	}

	return (nullptr);
}

void SetPaintWidgetBrushRadiusFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'BRAD')
	{
		brushRadius = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

void SetPaintWidgetBrushRadiusFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		static_cast<PaintWidget *>(widget)->GetPaintState()->SetBrushRadius(brushRadius);
		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


GetPaintWidgetBrushFuzzinessFunction::GetPaintWidgetBrushFuzzinessFunction() : WidgetFunction(kFunctionGetPaintWidgetBrushFuzziness)
{
}

GetPaintWidgetBrushFuzzinessFunction::GetPaintWidgetBrushFuzzinessFunction(const GetPaintWidgetBrushFuzzinessFunction& getPaintWidgetBrushFuzzinessFunction) : WidgetFunction(getPaintWidgetBrushFuzzinessFunction)
{
}

GetPaintWidgetBrushFuzzinessFunction::~GetPaintWidgetBrushFuzzinessFunction()
{
}

Function *GetPaintWidgetBrushFuzzinessFunction::Replicate(void) const
{
	return (new GetPaintWidgetBrushFuzzinessFunction(*this));
}

void GetPaintWidgetBrushFuzzinessFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	const Widget *widget = root->FindWidget(GetWidgetKey());
	if ((widget) && (widget->GetWidgetType() == kWidgetPaint))
	{
		method->SetOutputValue(state, static_cast<const PaintWidget *>(widget)->GetPaintState()->GetBrushFuzziness());
	}

	CallCompletionProc();
}


SetPaintWidgetBrushFuzzinessFunction::SetPaintWidgetBrushFuzzinessFunction() : WidgetFunction(kFunctionSetPaintWidgetBrushFuzziness)
{
	brushFuzziness = 0.0F;
}

SetPaintWidgetBrushFuzzinessFunction::SetPaintWidgetBrushFuzzinessFunction(const SetPaintWidgetBrushFuzzinessFunction& setPaintWidgetBrushFuzzinessFunction) : WidgetFunction(setPaintWidgetBrushFuzzinessFunction)
{
	brushFuzziness = setPaintWidgetBrushFuzzinessFunction.brushFuzziness;
}

SetPaintWidgetBrushFuzzinessFunction::~SetPaintWidgetBrushFuzzinessFunction()
{
}

Function *SetPaintWidgetBrushFuzzinessFunction::Replicate(void) const
{
	return (new SetPaintWidgetBrushFuzzinessFunction(*this));
}

void SetPaintWidgetBrushFuzzinessFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	data << brushFuzziness;
}

void SetPaintWidgetBrushFuzzinessFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);

	data >> brushFuzziness;
}

void SetPaintWidgetBrushFuzzinessFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << brushFuzziness;
}

bool SetPaintWidgetBrushFuzzinessFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		data >> brushFuzziness;
		return (true);
	}

	return (false);
}

int32 SetPaintWidgetBrushFuzzinessFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 1);
}

Setting *SetPaintWidgetBrushFuzzinessFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetPaintWidgetBrushFuzziness, 'FUZZ'));
		return (new FloatSetting('BFUZ', brushFuzziness, title, 0.0F, 1.0F, 0.05F));
	}

	return (nullptr);
}

void SetPaintWidgetBrushFuzzinessFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'BFUZ')
	{
		brushFuzziness = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

void SetPaintWidgetBrushFuzzinessFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		static_cast<PaintWidget *>(widget)->GetPaintState()->SetBrushFuzziness(brushFuzziness);
		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


GetPaintWidgetBrushOpacityFunction::GetPaintWidgetBrushOpacityFunction() : WidgetFunction(kFunctionGetPaintWidgetBrushOpacity)
{
}

GetPaintWidgetBrushOpacityFunction::GetPaintWidgetBrushOpacityFunction(const GetPaintWidgetBrushOpacityFunction& getPaintWidgetBrushOpacityFunction) : WidgetFunction(getPaintWidgetBrushOpacityFunction)
{
}

GetPaintWidgetBrushOpacityFunction::~GetPaintWidgetBrushOpacityFunction()
{
}

Function *GetPaintWidgetBrushOpacityFunction::Replicate(void) const
{
	return (new GetPaintWidgetBrushOpacityFunction(*this));
}

void GetPaintWidgetBrushOpacityFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	const Widget *widget = root->FindWidget(GetWidgetKey());
	if ((widget) && (widget->GetWidgetType() == kWidgetPaint))
	{
		method->SetOutputValue(state, static_cast<const PaintWidget *>(widget)->GetPaintState()->GetBrushOpacity());
	}

	CallCompletionProc();
}


SetPaintWidgetBrushOpacityFunction::SetPaintWidgetBrushOpacityFunction() : WidgetFunction(kFunctionSetPaintWidgetBrushOpacity)
{
	brushOpacity = 1.0F;
}

SetPaintWidgetBrushOpacityFunction::SetPaintWidgetBrushOpacityFunction(const SetPaintWidgetBrushOpacityFunction& setPaintWidgetBrushOpacityFunction) : WidgetFunction(setPaintWidgetBrushOpacityFunction)
{
	brushOpacity = setPaintWidgetBrushOpacityFunction.brushOpacity;
}

SetPaintWidgetBrushOpacityFunction::~SetPaintWidgetBrushOpacityFunction()
{
}

Function *SetPaintWidgetBrushOpacityFunction::Replicate(void) const
{
	return (new SetPaintWidgetBrushOpacityFunction(*this));
}

void SetPaintWidgetBrushOpacityFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	data << brushOpacity;
}

void SetPaintWidgetBrushOpacityFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);

	data >> brushOpacity;
}

void SetPaintWidgetBrushOpacityFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << brushOpacity;
}

bool SetPaintWidgetBrushOpacityFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		data >> brushOpacity;
		return (true);
	}

	return (false);
}

int32 SetPaintWidgetBrushOpacityFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 1);
}

Setting *SetPaintWidgetBrushOpacityFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetPaintWidgetBrushOpacity, 'OPAC'));
		return (new IntegerSetting('BFOP', (int32) (brushOpacity * 100.0F + 0.5F), title, 1, 100, 1));
	}

	return (nullptr);
}

void SetPaintWidgetBrushOpacityFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'BFOP')
	{
		brushOpacity = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

void SetPaintWidgetBrushOpacityFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		static_cast<PaintWidget *>(widget)->GetPaintState()->SetBrushOpacity(brushOpacity);
		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}


GetPaintWidgetBrushColorFunction::GetPaintWidgetBrushColorFunction() : WidgetFunction(kFunctionGetPaintWidgetBrushColor)
{
}

GetPaintWidgetBrushColorFunction::GetPaintWidgetBrushColorFunction(const GetPaintWidgetBrushColorFunction& getPaintWidgetBrushColorFunction) : WidgetFunction(getPaintWidgetBrushColorFunction)
{
}

GetPaintWidgetBrushColorFunction::~GetPaintWidgetBrushColorFunction()
{
}

Function *GetPaintWidgetBrushColorFunction::Replicate(void) const
{
	return (new GetPaintWidgetBrushColorFunction(*this));
}

void GetPaintWidgetBrushColorFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	const Widget *widget = root->FindWidget(GetWidgetKey());
	if ((widget) && (widget->GetWidgetType() == kWidgetPaint))
	{
		method->SetOutputValue(state, static_cast<const PaintWidget *>(widget)->GetPaintState()->GetBrushColor());
	}

	CallCompletionProc();
}


SetPaintWidgetBrushColorFunction::SetPaintWidgetBrushColorFunction() : WidgetFunction(kFunctionSetPaintWidgetBrushColor)
{
	brushColor.Set(1.0F, 1.0F, 1.0F, 1.0F);
}

SetPaintWidgetBrushColorFunction::SetPaintWidgetBrushColorFunction(const SetPaintWidgetBrushColorFunction& setPaintWidgetBrushColorFunction) : WidgetFunction(setPaintWidgetBrushColorFunction)
{
	brushColor = setPaintWidgetBrushColorFunction.brushColor;
}

SetPaintWidgetBrushColorFunction::~SetPaintWidgetBrushColorFunction()
{
}

Function *SetPaintWidgetBrushColorFunction::Replicate(void) const
{
	return (new SetPaintWidgetBrushColorFunction(*this));
}

void SetPaintWidgetBrushColorFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	WidgetFunction::Pack(data, packFlags);

	data << brushColor;
}

void SetPaintWidgetBrushColorFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	WidgetFunction::Unpack(data, unpackFlags);

	data >> brushColor;
}

void SetPaintWidgetBrushColorFunction::Compress(Compressor& data) const
{
	WidgetFunction::Compress(data);

	data << brushColor;
}

bool SetPaintWidgetBrushColorFunction::Decompress(Decompressor& data)
{
	if (WidgetFunction::Decompress(data))
	{
		data >> brushColor;
		return (true);
	}

	return (false);
}

int32 SetPaintWidgetBrushColorFunction::GetSettingCount(void) const
{
	return (WidgetFunction::GetSettingCount() + 1);
}

Setting *SetPaintWidgetBrushColorFunction::GetSetting(int32 index) const
{
	int32 count = WidgetFunction::GetSettingCount();
	if (index < count)
	{
		return (WidgetFunction::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		const char *title = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetPaintWidgetBrushColor, 'COLR'));
		const char *picker = table->GetString(StringID('CTRL', kControllerPanel, kFunctionSetPaintWidgetBrushColor, 'PICK'));
		return (new ColorSetting('BCOL', brushColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void SetPaintWidgetBrushColorFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'BCOL')
	{
		brushColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else
	{
		WidgetFunction::SetSetting(setting);
	}
}

void SetPaintWidgetBrushColorFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	const Panel *root = static_cast<PanelController *>(controller)->GetRootWidget();
	Widget *widget = root->FindWidget(GetWidgetKey());
	while (widget)
	{
		static_cast<PaintWidget *>(widget)->GetPaintState()->SetBrushColor(brushColor);
		widget = widget->GetNextWidgetWithSameKey();
	}

	CallCompletionProc();
}

// ZYUQURM
