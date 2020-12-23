 

#ifndef C4Panels_h
#define C4Panels_h


//# \component	Effect Manager
//# \prefix		EffectMgr/


#include "C4Effects.h"
#include "C4Portals.h"
#include "C4Cameras.h"
#include "C4Scripts.h"
#include "C4Input.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerPanel				= 'PANL'
	};


	enum : WidgetType
	{
		kWidgetCamera					= 'CAMR'
	};


	enum : FunctionType
	{
		kFunctionEnableWidget			= 'ENAB',
		kFunctionDisableWidget			= 'DSAB',
		kFunctionShowWidget				= 'SHOW',
		kFunctionHideWidget				= 'HIDE',
		kFunctionActivateWidget			= 'ACTV',
		kFunctionSetMutatorState		= 'MFLG',
		kFunctionGetWidgetColor			= 'GCLR',
		kFunctionSetWidgetColor			= 'COLR',
		kFunctionGetWidgetImage			= 'GIMG',
		kFunctionSetWidgetImage			= 'IMAG',
		kFunctionGetWidgetText			= 'GTXT',
		kFunctionSetWidgetText			= 'TEXT',
		kFunctionGetWidgetValue			= 'GVAL',
		kFunctionSetWidgetValue			= 'VALU',
		kFunctionSetCameraConnectorKey	= 'CKEY'
	};


	enum : Type
	{
		kSetWidgetTextReplace			= 'REPL',
		kSetWidgetTextAppend			= 'APND',
		kSetWidgetTextPrepend			= 'PPND'
	};


	//# \enum	PanelFlags

	enum
	{
		kPanelDepthOffset		= 1 << 0,		//## Apply depth offset to the panel.
		kPanelTwoSided			= 1 << 1,		//## Render panel two-sided.
		kPanelRenderFog			= 1 << 2		//## Allow panel to be rendered with fog.
	};


	enum
	{
		kMaxWidgetFunctionMessageSize	= kMaxMessageDataSize - kMaxWidgetKeyLength - 1
	};


	class CameraPortalObject : public PortalObject
	{
		friend class PortalObject;

		private:

			int32		viewportWidth;
			int32		viewportHeight;

			int32		minDetailLevel;
			float		detailLevelBias;

			CameraPortalObject();
			~CameraPortalObject();

		public:

			CameraPortalObject(const Vector2D& size, int32 width, int32 height);

			int32 GetViewportWidth(void) const
			{
				return (viewportWidth);
			}

			int32 GetViewportHeight(void) const
			{
				return (viewportHeight);
			} 

			int32 GetMinDetailLevel(void) const
			{ 
				return (minDetailLevel);
			} 

			void SetMinDetailLevel(int32 level)
			{ 
				minDetailLevel = level;
			} 
 
			float GetDetailLevelBias(void) const
			{
				return (detailLevelBias);
			} 

			void SetDetailLevelBias(float bias)
			{
				detailLevelBias = bias;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
	};


	class CameraPortal : public Portal
	{
		friend class Portal;

		public:

			typedef void RenderSizeProc(int32, int32, void *);

		private:

			Link<FrustumCamera>		targetCamera;
			Texture					*cameraTexture;

			RenderSizeProc			*renderSizeProc;
			void					*renderSizeCookie;

			CameraPortal(const CameraPortal& cameraPortal);

			Node *Replicate(void) const override;

		public:

			CameraPortal();
			CameraPortal(const Vector2D& size, int32 width, int32 height);
			~CameraPortal();

			CameraPortalObject *GetObject(void) const
			{
				return (static_cast<CameraPortalObject *>(Node::GetObject()));
			}

			FrustumCamera *GetTargetCamera(void) const
			{
				return (targetCamera);
			}

			void SetTargetCamera(FrustumCamera *camera)
			{
				targetCamera = camera;
			}

			Texture *GetCameraTexture(void) const
			{
				return (cameraTexture);
			}

			void SetRenderSizeProc(RenderSizeProc *proc, void *cookie)
			{
				renderSizeProc = proc;
				renderSizeCookie = cookie;
			}

			void CallRenderSizeProc(int32 width, int32 height) const
			{
				if (renderSizeProc)
				{
					(*renderSizeProc)(width, height, renderSizeCookie);
				}
			}

			void SetCameraTexture(Texture *texture);
	};


	//# \class	PanelController		Manages an interface panel effect in a world.
	//
	//# The $PanelController$ class manages an interface panel effect in a world.
	//
	//# \def	class PanelController final : public ScriptController
	//
	//# \ctor	PanelController();
	//
	//# \desc
	//# The $PanelController$ class manages an interface panel effect in a world. This type of controller is
	//# automatically assigned to panel effect nodes in the World Editor.
	//
	//# \base	Controller/ScriptController		Widgets contained in an interface panel can have scripts attached to them.
	//
	//# \also	$@PanelEffect@$
	//# \also	$@InterfaceMgr/Widget@$


	//# \function	PanelController::GetInternalPanelSize	Returns the internal panel size.
	//
	//# \proto	const Vector2D& GetInternalPanelSize(void) const;
	//
	//# \desc
	//# The $GetInternalPanelSize$ function returns the internal panel size. The <i>x</i> and <i>y</i> coordinates of the returned
	//# vector are the width and height of the panel's display area, in pixels.
	//
	//# \also	$@PanelController::SetInternalPanelSize@$


	//# \function	PanelController::SetInternalPanelSize	Sets the internal panel size.
	//
	//# \proto	void SetInternalPanelSize(const Vector2D& size);
	//
	//# \param	size	The new interface size.
	//
	//# \desc
	//# The $SetInternalPanelSize$ function sets the internal panel size. The <i>x</i> and <i>y</i> coordinates of the $size$
	//# parameter specify the width and height of the panel's display area, in pixels.
	//
	//# \also	$@PanelController::GetInternalPanelSize@$


	//# \function	PanelController::GetRootWidget		Returns the root widget for a panel controller.
	//
	//# \proto	Panel *GetRootWidget(void);
	//# \proto	const Panel *GetRootWidget(void) const;
	//
	//# \desc
	//# The $GetRootWidget$ function returns the root widget inside a panel controller. The member functions
	//# of the returned $@InterfaceMgr/Panel@$ object can be used to retrieve or modify the widgets belonging
	//# to the panel effect.
	//
	//# \also	$@PanelController::PurgeWidgets@$
	//# \also	$@InterfaceMgr/Panel@$


	//# \function	PanelController::PurgeWidgets		Purges all widgets owned by a panel controller.
	//
	//# \proto	void PurgeWidgets(void);
	//
	//# \desc
	//# The $PurgeWidgets$ function purges all widgets owned by a panel controller, leaving only the root widget.
	//
	//# \also	$@PanelController::GetRootWidget@$
	//# \also	$@InterfaceMgr/Widget@$


	class PanelController final : public ScriptController
	{
		private:

			unsigned_int32			panelState;
			int32					panelSleepTime;

			Cursor					panelCursor;

			Link<Widget>			hoverWidget;
			Link<Widget>			trackWidget;
			WidgetPart				trackPart;

			Widget					*keyboardWidget;
			KeyboardEventHandler	keyboardEventHandler;
			InputMode				savedInputMode;

			Panel					rootWidget;

			PanelController(const PanelController& panelController);

			Controller *Replicate(void) const override;

			Widget *DetectInteractiveWidget(const Point3D& panelPosition, PanelMouseEventData *eventData);

			static bool HandleKeyboardEvent(const KeyboardEventData *eventData, void *cookie);

		public:

			PanelController();
			~PanelController();

			unsigned_int32 GetPanelState(void) const
			{
				return (panelState);
			}

			const Vector2D& GetInternalPanelSize(void) const
			{
				return (rootWidget.GetWidgetSize());
			}

			void SetInternalPanelSize(const Vector2D& size)
			{
				rootWidget.SetWidgetSize(size);
			}

			int32 GetPanelSleepTime(void) const
			{
				return (panelSleepTime);
			}

			void SetPanelSleepTime(int32 time)
			{
				panelSleepTime = time;
			}

			Panel *GetRootWidget(void)
			{
				return (&rootWidget);
			}

			const Panel *GetRootWidget(void) const
			{
				return (&rootWidget);
			}

			void PurgeWidgets(void)
			{
				rootWidget.PurgeSubtree();
			}

			void ExecuteWidgetScript(ScriptObject *object, Node *initiator, EventType eventType)
			{
				ExecuteScript(object, initiator, nullptr, eventType);
			}

			static bool ValidNode(const Node *node);
			static void RegisterFunctions(ControllerRegistration *registration);

			C4API void Prepack(List<Object> *linkList) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			C4API void *BeginSettingsUnpack(void) override;

			C4API int32 GetSettingCount(void) const override;
			C4API Setting *GetSetting(int32 index) const override;
			C4API void SetSetting(const Setting *setting) override;

			C4API void SendInitialStateMessages(Player *player) const override;

			C4API void Preprocess(void) override;
			C4API void Move(void) override;

			C4API void HandleInteractionEvent(InteractionEventType type, const Point3D *position, Node *initiator) override;

			C4API void BeginKeyboardInteraction(Widget *widget);
			C4API void EndKeyboardInteraction(void);

			C4API void ExtendAnimationTime(float time);
	};


	//# \class	CameraWidget	The interface widget that displays a remote camera view.
	//
	//# The $CameraWidget$ class represents an interface widget that displays a remote camera view.
	//
	//# \def	class CameraWidget final : public ImageWidget
	//
	//# \ctor	CameraWidget(const Vector2D& size);
	//
	//# \param	size	The size of the camera widget, in pixels.
	//
	//# \desc
	//# The $CameraWidget$ class is used to display a remote camera view inside a panel effect. The camera used to
	//# render the source image is determined by a connector key stored in the camera widget. This key identifies a
	//# connector belonging to the panel effect that node, and it should be connected to a $@WorldMgr/FrustumCamera@$ node.
	//# The key can be specified by calling the $@CameraWidget::SetCameraConnectorKey@$ function.
	//
	//# \base	InterfaceMgr/ImageWidget	The image displayed in the camera view is based on the $ImageWidget$ class.


	//# \function	CameraWidget::GetCameraConnectorKey		Returns the connector key that identifies the camera node.
	//
	//# \proto	const ConnectorKey& GetCameraConnectorKey(void) const;
	//
	//# \desc
	//# The $GetCameraConnectorKey$ function returns the connector key that identifies the camera node used to
	//# render the camera widget's source image.
	//
	//# \also	$@CameraWidget::SetCameraConnectorKey@$


	//# \function	CameraWidget::SetCameraConnectorKey		Sets the connector key that identifies the camera node.
	//
	//# \proto	void SetCameraConnectorKey(const ConnectorKey& key);
	//
	//# \param	key		Thew new camera connector key.
	//
	//# \desc
	//# The $SetCameraConnectorKey$ function sets the connector key that identifies the camera node used to render
	//# the camera widget's source image. The $key$ parameter should identify a connector attached to the panel effect
	//# node containing the camera widget, and that connector should be connected to a $@WorldMgr/FrustumCamera@$ node.
	//# The connected camera must be a node that is part of the scene graph, and it cannot be the current camera that
	//# has been established with the $@WorldMgr/World::SetCamera@$ function.
	//#
	//# The camera connector key can be changed at any time without incurring a performance penalty specifically due
	//# to the change itself. It is not necessary to invalidate the widget after changing the camera connector key.
	//#
	//# The initial value of the camera connector key is the empty string.
	//
	//# \also	$@CameraWidget::SetCameraConnectorKey@$


	//# \function	CameraWidget::GetCameraMinDetailLevel	Returns the minimum detail level rendered through a camera widget.
	//
	//# \proto	int32 GetCameraMinDetailLevel(void) const;
	//
	//# \desc
	//# The $GetCameraMinDetailLevel$ function returns the minimum detail level that objects rendered through a camera
	//# widget are allowed to have. A value of zero represents the highest detail possible, and larger values represent
	//# successively lower levels of detail.
	//
	//# \also	$@CameraWidget::SetCameraMinDetailLevel@$
	//# \also	$@CameraWidget::GetCameraDetailLevelBias@$
	//# \also	$@CameraWidget::SetCameraDetailLevelBias@$


	//# \function	CameraWidget::SetCameraMinDetailLevel	Sets the minimum detail level rendered through a camera widget.
	//
	//# \proto	void SetCameraMinDetailLevel(int32 level);
	//
	//# \param	level	The minimum level of detail. This must be greater than or equal to zero.
	//
	//# \desc
	//# The $SetCameraMinDetailLevel$ function sets the minimum detail level that objects rendered through a camera
	//# widget are allowed to have. A value of zero represents the highest detail possible, and larger values represent
	//# successively lower levels of detail.
	//#
	//# The initial value for the minimum detail level is zero.
	//
	//# \also	$@CameraWidget::GetCameraMinDetailLevel@$
	//# \also	$@CameraWidget::GetCameraDetailLevelBias@$
	//# \also	$@CameraWidget::SetCameraDetailLevelBias@$


	//# \function	CameraWidget::GetCameraDetailLevelBias		Returns the detail level bias applied to a camera widget.
	//
	//# \proto	float GetCameraDetailLevelBias(void) const;
	//
	//# \desc
	//# The $GetCameraDetailLevelBias$ function returns the detail level bias that is applied to objects rendered through
	//# a camera widget. A value of 0.0 means no bias, positive values mean that the detail should be lower, and negative
	//# values mean the detail should be higher.
	//
	//# \also	$@CameraWidget::SetCameraDetailLevelBias@$
	//# \also	$@CameraWidget::GetCameraMinDetailLevel@$
	//# \also	$@CameraWidget::SetCameraMinDetailLevel@$


	//# \function	CameraWidget::SetCameraDetailLevelBias		Sets the detail level bias applied to a camera widget.
	//
	//# \proto	void SetCameraDetailLevelBias(float bias);
	//
	//# \param	bias	The detail level bias. This can be zero, positive, or negative.
	//
	//# \desc
	//# The $SetCameraDetailLevelBias$ function sets the detail level bias that is applied to objects rendered through
	//# a camera widget. A value of 0.0 means no bias, positive values mean that the detail should be lower, and negative
	//# values mean the detail should be higher. After the detail level bias is applied to each object, the result is
	//# clamped to the minimum detail level specified by the $@CameraWidget::SetCameraMinDetailLevel@$ function.
	//#
	//# The initial value for the detail level bias is 0.0.
	//
	//# \also	$@CameraWidget::GetCameraDetailLevelBias@$
	//# \also	$@CameraWidget::GetCameraMinDetailLevel@$
	//# \also	$@CameraWidget::SetCameraMinDetailLevel@$


	class CameraWidget final : public ImageWidget
	{
		friend class WidgetReg<CameraWidget>;

		private:

			ConnectorKey		cameraConnectorKey;

			int32				cameraMinDetailLevel;
			float				cameraDetailLevelBias;

			CameraPortal		*cameraPortal;
			TextureHeader		textureHeader;

			CameraWidget();
			CameraWidget(const CameraWidget& cameraWidget);

			Widget *Replicate(void) const override;

			void SetTargetCamera(void);
			static void SetRenderSize(int32 width, int32 height, void *cookie);

		public:

			C4API CameraWidget(const Vector2D& size);
			C4API ~CameraWidget();

			const ConnectorKey& GetCameraConnectorKey(void) const
			{
				return (cameraConnectorKey);
			}

			void SetCameraConnectorKey(const ConnectorKey& key)
			{
				cameraConnectorKey = key;
			}

			int32 GetCameraMinDetailLevel(void) const
			{
				return (cameraMinDetailLevel);
			}

			void SetCameraMinDetailLevel(int32 level)
			{
				cameraMinDetailLevel = level;
			}

			float GetCameraDetailLevelBias(void) const
			{
				return (cameraDetailLevelBias);
			}

			void SetCameraDetailLevelBias(float bias)
			{
				cameraDetailLevelBias = bias;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void SetWidgetState(unsigned_int32 state) override;

			void Preprocess(void) override;
			void Move(void) override;
			void Build(void) override;
	};


	class WidgetFunction : public Function
	{
		private:

			WidgetKey		widgetKey;

		protected:

			C4API WidgetFunction(FunctionType type);
			C4API WidgetFunction(const WidgetFunction& widgetFunction);

		public:

			C4API ~WidgetFunction();

			const WidgetKey& GetWidgetKey(void) const
			{
				return (widgetKey);
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void Compress(Compressor& data) const override;
			C4API bool Decompress(Decompressor& data) override;

			C4API int32 GetSettingCount(void) const override;
			C4API Setting *GetSetting(int32 index) const override;
			C4API void SetSetting(const Setting *setting) override;

			C4API bool OverridesFunction(const Function *function) const override;
	};


	class EnableWidgetFunction final : public WidgetFunction
	{
		private:

			EnableWidgetFunction(const EnableWidgetFunction& enableWidgetFunction);

			Function *Replicate(void) const override;

		public:

			EnableWidgetFunction();
			~EnableWidgetFunction();

			bool OverridesFunction(const Function *function) const override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class DisableWidgetFunction final : public WidgetFunction
	{
		private:

			DisableWidgetFunction(const DisableWidgetFunction& disableWidgetFunction);

			Function *Replicate(void) const override;

		public:

			DisableWidgetFunction();
			~DisableWidgetFunction();

			bool OverridesFunction(const Function *function) const override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class ShowWidgetFunction final : public WidgetFunction
	{
		private:

			ShowWidgetFunction(const ShowWidgetFunction& showWidgetFunction);

			Function *Replicate(void) const override;

		public:

			ShowWidgetFunction();
			~ShowWidgetFunction();

			bool OverridesFunction(const Function *function) const override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class HideWidgetFunction final : public WidgetFunction
	{
		private:

			HideWidgetFunction(const HideWidgetFunction& hideWidgetFunction);

			Function *Replicate(void) const override;

		public:

			HideWidgetFunction();
			~HideWidgetFunction();

			bool OverridesFunction(const Function *function) const override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class ActivateWidgetFunction final : public WidgetFunction
	{
		private:

			ActivateWidgetFunction(const ActivateWidgetFunction& activateWidgetFunction);

			Function *Replicate(void) const override;

		public:

			ActivateWidgetFunction();
			~ActivateWidgetFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetMutatorStateFunction final : public WidgetFunction
	{
		private:

			MutatorKey		mutatorKey;
			unsigned_int32	mutatorState;

			SetMutatorStateFunction(const SetMutatorStateFunction& setMutatorStateFunction);

			Function *Replicate(void) const override;

		public:

			SetMutatorStateFunction();
			~SetMutatorStateFunction();

			const MutatorKey& GetMutatorKey(void) const
			{
				return (mutatorKey);
			}

			unsigned_int32 GetMutatorState(void) const
			{
				return (mutatorState);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool OverridesFunction(const Function *function) const override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class GetWidgetColorFunction final : public WidgetFunction
	{
		private:

			WidgetColorType		colorType;

			GetWidgetColorFunction(const GetWidgetColorFunction& getWidgetColorFunction);

			Function *Replicate(void) const override;

		public:

			GetWidgetColorFunction();
			~GetWidgetColorFunction();

			WidgetColorType GetColorType(void) const
			{
				return (colorType);
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


	class SetWidgetColorFunction final : public WidgetFunction
	{
		private:

			ColorRGBA			widgetColor;
			WidgetColorType		colorType;

			SetWidgetColorFunction(const SetWidgetColorFunction& setWidgetColorFunction);

			Function *Replicate(void) const override;

		public:

			SetWidgetColorFunction();
			~SetWidgetColorFunction();

			const ColorRGBA& GetWidgetColor(void) const
			{
				return (widgetColor);
			}

			WidgetColorType GetColorType(void) const
			{
				return (colorType);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool OverridesFunction(const Function *function) const override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class GetWidgetImageFunction final : public WidgetFunction
	{
		private:

			GetWidgetImageFunction(const GetWidgetImageFunction& getWidgetImageFunction);

			Function *Replicate(void) const override;

		public:

			GetWidgetImageFunction();
			~GetWidgetImageFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetWidgetImageFunction final : public WidgetFunction
	{
		private:

			ResourceName		textureName;

			SetWidgetImageFunction(const SetWidgetImageFunction& setWidgetImageFunction);

			Function *Replicate(void) const override;

		public:

			SetWidgetImageFunction();
			~SetWidgetImageFunction();

			const char *GetTextureName(void) const
			{
				return (textureName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class GetWidgetTextFunction final : public WidgetFunction
	{
		private:

			GetWidgetTextFunction(const GetWidgetTextFunction& getWidgetTextFunction);

			Function *Replicate(void) const override;

		public:

			GetWidgetTextFunction();
			~GetWidgetTextFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetWidgetTextFunction final : public WidgetFunction
	{
		private:

			enum
			{
				kMaxFunctionStringLength = 63
			};

			Type		textCombineMode;
			int32		maxResultLength;

			String<kMaxFunctionStringLength>	widgetText;

			SetWidgetTextFunction(const SetWidgetTextFunction& setWidgetTextFunction);

			Function *Replicate(void) const override;

		public:

			SetWidgetTextFunction();
			~SetWidgetTextFunction();

			Type GetTextCombineMode(void) const
			{
				return (textCombineMode);
			}

			int32 GetMaxResultLength(void) const
			{
				return (maxResultLength);
			}

			const char *GetWidgetText(void) const
			{
				return (widgetText);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool OverridesFunction(const Function *function) const override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class GetWidgetValueFunction final : public WidgetFunction
	{
		private:

			GetWidgetValueFunction(const GetWidgetValueFunction& getWidgetValueFunction);

			Function *Replicate(void) const override;

		public:

			GetWidgetValueFunction();
			~GetWidgetValueFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetWidgetValueFunction final : public WidgetFunction
	{
		private:

			int32		widgetValue;

			SetWidgetValueFunction(const SetWidgetValueFunction& setWidgetValueFunction);

			Function *Replicate(void) const override;

		public:

			SetWidgetValueFunction();
			~SetWidgetValueFunction();

			int32 GetWidgetValue(void) const
			{
				return (widgetValue);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetCameraConnectorKeyFunction final : public WidgetFunction
	{
		private:

			ConnectorKey	cameraConnectorKey;

			SetCameraConnectorKeyFunction(const SetCameraConnectorKeyFunction& setCameraConnectorKeyFunction);

			Function *Replicate(void) const override;

		public:

			SetCameraConnectorKeyFunction();
			~SetCameraConnectorKeyFunction();

			const ConnectorKey& GetCameraConnectorKey(void) const
			{
				return (cameraConnectorKey);
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


	//# \class	PanelEffectObject		Encapsulates data pertaining to a panel effect.
	//
	//# The $PanelEffectObject$ class encapsulates data pertaining to a panel effect.
	//
	//# \def	class PanelEffectObject : public EffectObject
	//
	//# \ctor	PanelEffectObject(const Vector2D& size);
	//
	//# \param	size	The size of the panel.
	//
	//# \desc
	//# The $PanelEffectObject$ class encapsulates data pertaining to a panel effect such as the physical
	//# size of the panel. The contents of a panel effect are managed by the $@PanelController@$ class.
	//
	//# \base	EffectObject		A $PanelEffectObject$ is an object that can be shared by multiple panel effect nodes.
	//
	//# \also	$@PanelEffect@$
	//# \also	$@PanelController@$


	class PanelEffectObject final : public EffectObject
	{
		friend class EffectObject;

		private:

			unsigned_int32		panelFlags;

			Vector2D			externalPanelSize;
			float				interactionPadding;

			ResourceName		impostorName;
			float				impostorMinDistance;
			float				impostorMaxDistance;

			PanelEffectObject();
			~PanelEffectObject();

		public:

			PanelEffectObject(const Vector2D& size);

			unsigned_int32 GetPanelFlags(void) const
			{
				return (panelFlags);
			}

			void SetPanelFlags(unsigned_int32 flags)
			{
				panelFlags = flags;
			}

			const Vector2D& GetExternalPanelSize(void) const
			{
				return (externalPanelSize);
			}

			void SetExternalPanelSize(const Vector2D& size)
			{
				externalPanelSize = size;
			}

			float GetInteractionPadding(void) const
			{
				return (interactionPadding);
			}

			void SetInteractionPadding(float padding)
			{
				interactionPadding = padding;
			}

			const char *GetImpostorName(void) const
			{
				return (impostorName);
			}

			void SetImpostorName(const char *name)
			{
				impostorName = name;
			}

			float GetImpostorMinDistance(void) const
			{
				return (impostorMinDistance);
			}

			float GetImpostorMaxDistance(void) const
			{
				return (impostorMaxDistance);
			}

			void SetImpostorTransitionDistances(float minDistance, float maxDistance)
			{
				impostorMinDistance = minDistance;
				impostorMaxDistance = maxDistance;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool DetectCollision(const Point3D& p1, const Point3D& p2, float padding, CollisionPoint *collisionPoint) const;
	};


	//# \class	PanelEffect		Represents a panel effect node in a world.
	//
	//# The $PanelEffect$ class represents a panel effect node in a world.
	//
	//# \def	class PanelEffect : public Effect
	//
	//# \ctor	PanelEffect(const Vector2D& size);
	//
	//# \param	size	The size of the panel.
	//
	//# \desc
	//# The $PanelEffect$ class represents a panel effect node in a world. All panel effect nodes have a
	//# panel controller assigned to them automatically in the World Editor, and it's the $@PanelController@$
	//# class that manages the contents of the panel effect.
	//
	//# \base	Effect		A panel effect is a specific type of effect.
	//
	//# \also	$@PanelEffectObject@$
	//# \also	$@PanelController@$


	class PanelEffect final : public Effect
	{
		friend class Effect;
		friend class PanelController;

		private:

			Vector2D			panelScale;
			int32				panelInvisibleTime;

			unsigned_int32		renderState;
			unsigned_int32		materialState;

			volatile int32		queryThreadFlags;

			Cursor				*panelCursor;
			Point3D				cursorPosition;
			Box3D				cursorBox;

			ImageWidget			impostorWidget;

			VertexBuffer		vertexBuffer;
			List<Attribute>		attributeList;
			DiffuseAttribute	diffuseAttribute;

			PanelEffect();
			PanelEffect(const PanelEffect& panelEffect);

			Node *Replicate(void) const override;

			void HandleTransformUpdate(void) override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API PanelEffect(const Vector2D& size);
			C4API ~PanelEffect();

			PanelEffectObject *GetObject(void) const
			{
				return (static_cast<PanelEffectObject *>(Node::GetObject()));
			}

			volatile int32 *GetQueryThreadFlags(void)
			{
				return (&queryThreadFlags);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void ProcessObjectSettings(void) override;

			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;

			bool DetectCollision(const Point3D& p1, const Point3D& p2, CollisionPoint *collisionPoint) const;
	};
}


#endif

// ZYUQURM
