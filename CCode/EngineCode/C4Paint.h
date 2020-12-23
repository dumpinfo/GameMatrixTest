 

#ifndef C4Paint_h
#define C4Paint_h


//# \component	Interface Manager
//# \prefix		InterfaceMgr/


#include "C4SpaceObjects.h"
#include "C4Processes.h"
#include "C4Panels.h"


namespace C4
{
	enum : ProcessType
	{
		kProcessPaintTexture	= 'PTEX',
		kProcessPaintTexcoord	= 'PTXC'
	};


	enum : WidgetType
	{
		kWidgetPaint			= 'PANT'
	};


	enum : FunctionType
	{
		kFunctionGetPaintWidgetBrushRadius		= 'GBRD',
		kFunctionSetPaintWidgetBrushRadius		= 'SBRD',
		kFunctionGetPaintWidgetBrushFuzziness	= 'GBFZ',
		kFunctionSetPaintWidgetBrushFuzziness	= 'SBFZ',
		kFunctionGetPaintWidgetBrushOpacity		= 'GBOP',
		kFunctionSetPaintWidgetBrushOpacity		= 'SBOP',
		kFunctionGetPaintWidgetBrushColor		= 'GBCL',
		kFunctionSetPaintWidgetBrushColor		= 'SBCL'
	};


	enum
	{
		kPaintMinResolution		= 16,
		kPaintMaxResolution		= 2048
	};


	class PaintState : public Packable
	{
		private:

			float				brushRadius;
			float				brushFuzziness;
			float				brushOpacity;
			ColorRGBA			brushColor;

			bool				channelMask[4];

		public:

			C4API PaintState();
			C4API PaintState(const PaintState& paintState);
			C4API ~PaintState();

			float GetBrushRadius(void) const
			{
				return (brushRadius);
			}

			void SetBrushRadius(float radius)
			{
				brushRadius = radius;
			}

			float GetBrushFuzziness(void) const
			{
				return (brushFuzziness);
			}

			void SetBrushFuzziness(float fuzziness)
			{
				brushFuzziness = fuzziness;
			}

			float GetBrushOpacity(void) const
			{
				return (brushOpacity);
			}

			void SetBrushOpacity(float opacity)
			{
				brushOpacity = opacity;
			}

			const ColorRGBA& GetBrushColor(void) const
			{
				return (brushColor);
			}

			void SetBrushColor(const ColorRGBA& color) 
			{
				brushColor = color;
			} 

			const bool *GetChannelMask(void) const 
			{
				return (channelMask);
			} 

			void SetChannelMask(bool red, bool green, bool blue, bool alpha) 
			{ 
				channelMask[0] = red;
				channelMask[1] = green;
				channelMask[2] = blue;
				channelMask[3] = alpha; 
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
	};


	class Painter
	{
		private:

			Integer2D			paintResolution;
			int32				channelCount;

			void				*paintImage;
			const PaintState	*paintState;

			unsigned_int8		*previousImage;
			unsigned_int8		*transferImage;

			Rect				paintBounds;

			void UpdateImage1(void);
			void UpdateImage2(void);
			void UpdateImage4(void);

		public:

			C4API Painter(const Integer2D& resolution, int32 count, void *image, const PaintState *state);
			C4API ~Painter();

			const Rect& GetPaintBounds(void) const
			{
				return (paintBounds);
			}

			C4API void BeginPainting(void);
			C4API void EndPainting(void);

			C4API bool UpdateImage(void);

			C4API const void *CreateUndoImage(const Rect& rect) const;
			C4API static void ApplyUndoImage(const PaintSpaceObject *object, const Rect& rect, const void *undoImage);
			C4API static void ReleaseUndoImage(const void *undoImage);

			C4API void DrawDot(const Point2D& p);
			C4API void DrawLine(const Point2D& p1, const Point2D& p2);
	};


	class PaintTexcoordProcess final : public InterpolantProcess
	{
		private:

			PaintTexcoordProcess(const PaintTexcoordProcess& paintTexcoordProcess);

			Process *Replicate(void) const override;

		public:

			PaintTexcoordProcess();
			~PaintTexcoordProcess();
	};


	class PaintTextureProcess final : public Process
	{
		private:

			PaintTextureProcess(const PaintTextureProcess& paintTextureProcess);

			Process *Replicate(void) const override;

			static const Texture *GetPaintTexture(const ShaderCompileData *compileData);

		public:

			PaintTextureProcess();
			~PaintTextureProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			#if C4MACOS

				int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;

			#endif

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	//# \class	PaintWidget		The interface widget that displays a painting canvas.
	//
	//# The $PaintWidget$ class represents an interface widget that displays a painting canvas.
	//
	//# \def	class PaintWidget final : public ImageWidget
	//
	//# \ctor	PaintWidget(const Vector2D& size, const Integer2D& resolution);
	//
	//# \param	size		The size of the quad in which the canvas is rendered, in pixels.
	//# \param	resolution	The internal resolution of the painting canvas, in pixels.
	//
	//# \desc
	//# The $PaintWidget$ class displays an interactive painting canvas.
	//
	//# \base	ImageWidget		An $PaintWidget$ is a specialized $ImageWidget$.


	class PaintWidget final : public ImageWidget
	{
		friend class WidgetReg<PaintWidget>;

		private:

			Integer2D			paintResolution;
			ColorRGBA			backgroundColor;
			PaintState			paintState;

			Point2D				brushPosition;

			Color4C				*paintImage;
			Painter				*painter;

			TextureHeader		textureHeader;

			PaintWidget();
			PaintWidget(const PaintWidget& paintWidget);

			Widget *Replicate(void) const override;

			void Initialize(void);

			void UpdateImage(void);

		public:

			C4API PaintWidget(const Vector2D& size, const Integer2D& resolution);
			C4API ~PaintWidget();

			PaintState *GetPaintState(void)
			{
				return (&paintState);
			}

			const PaintState *GetPaintState(void) const
			{
				return (&paintState);
			}

			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Build(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	class GetPaintWidgetBrushRadiusFunction final : public WidgetFunction
	{
		private:

			GetPaintWidgetBrushRadiusFunction(const GetPaintWidgetBrushRadiusFunction& getPaintWidgetBrushRadiusFunction);

			Function *Replicate(void) const override;

		public:

			GetPaintWidgetBrushRadiusFunction();
			~GetPaintWidgetBrushRadiusFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetPaintWidgetBrushRadiusFunction final : public WidgetFunction
	{
		private:

			float		brushRadius;

			SetPaintWidgetBrushRadiusFunction(const SetPaintWidgetBrushRadiusFunction& setPaintWidgetBrushRadiusFunction);

			Function *Replicate(void) const override;

		public:

			SetPaintWidgetBrushRadiusFunction();
			~SetPaintWidgetBrushRadiusFunction();

			float GetBrushRadius(void) const
			{
				return (brushRadius);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class GetPaintWidgetBrushFuzzinessFunction final : public WidgetFunction
	{
		private:

			GetPaintWidgetBrushFuzzinessFunction(const GetPaintWidgetBrushFuzzinessFunction& getPaintWidgetBrushFuzzinessFunction);

			Function *Replicate(void) const override;

		public:

			GetPaintWidgetBrushFuzzinessFunction();
			~GetPaintWidgetBrushFuzzinessFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetPaintWidgetBrushFuzzinessFunction final : public WidgetFunction
	{
		private:

			float		brushFuzziness;

			SetPaintWidgetBrushFuzzinessFunction(const SetPaintWidgetBrushFuzzinessFunction& setPaintWidgetBrushFuzzinessFunction);

			Function *Replicate(void) const override;

		public:

			SetPaintWidgetBrushFuzzinessFunction();
			~SetPaintWidgetBrushFuzzinessFunction();

			float GetBrushFuzziness(void) const
			{
				return (brushFuzziness);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class GetPaintWidgetBrushOpacityFunction final : public WidgetFunction
	{
		private:

			GetPaintWidgetBrushOpacityFunction(const GetPaintWidgetBrushOpacityFunction& getPaintWidgetBrushOpacityFunction);

			Function *Replicate(void) const override;

		public:

			GetPaintWidgetBrushOpacityFunction();
			~GetPaintWidgetBrushOpacityFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetPaintWidgetBrushOpacityFunction final : public WidgetFunction
	{
		private:

			float		brushOpacity;

			SetPaintWidgetBrushOpacityFunction(const SetPaintWidgetBrushOpacityFunction& setPaintWidgetBrushOpacityFunction);

			Function *Replicate(void) const override;

		public:

			SetPaintWidgetBrushOpacityFunction();
			~SetPaintWidgetBrushOpacityFunction();

			float GetBrushOpacity(void) const
			{
				return (brushOpacity);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class GetPaintWidgetBrushColorFunction final : public WidgetFunction
	{
		private:

			GetPaintWidgetBrushColorFunction(const GetPaintWidgetBrushColorFunction& getPaintWidgetBrushColorFunction);

			Function *Replicate(void) const override;

		public:

			GetPaintWidgetBrushColorFunction();
			~GetPaintWidgetBrushColorFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetPaintWidgetBrushColorFunction final : public WidgetFunction
	{
		private:

			ColorRGBA		brushColor;

			SetPaintWidgetBrushColorFunction(const SetPaintWidgetBrushColorFunction& setPaintWidgetBrushColorFunction);

			Function *Replicate(void) const override;

		public:

			SetPaintWidgetBrushColorFunction();
			~SetPaintWidgetBrushColorFunction();

			const ColorRGBA& GetBrushColor(void) const
			{
				return (brushColor);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};
}


#endif

// ZYUQURM
